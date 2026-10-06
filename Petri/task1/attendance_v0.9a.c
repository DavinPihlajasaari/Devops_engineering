#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "xxhash.h"
#include "xxhash.c"
#include <dirent.h>
#include <unistd.h>

enum
{
    COL_ID = 0,
    COL_NAME,
    COL_STATUS,
    COL_COLOR,
    NUM_COLS
};

GtkListStore *global_store;
GtkWidget *class_menu_shell;
char *current_class_name = NULL;

// Hash helper function.
char *hash_id(const char *input)
{
    if (!input)
        return NULL;

    char *clean = g_strdup(input);
    // 1. Remove carriage returns and newlines (\r and \n)
    clean[strcspn(clean, "\r\n")] = 0;
    // 2. Strip leading/trailing spaces
    g_strstrip(clean);

    XXH64_hash_t hash = XXH3_64bits(clean, strlen(clean));
    char *result = g_strdup_printf("%016llx", (unsigned long long)hash);

    g_free(clean);
    return result;
}

void load_csv_into_temp_store(const char *filename, GtkListStore *temp_store)
{
    gtk_list_store_clear(temp_store);
    FILE *f = fopen(filename, "r");
    if (!f)
        return;

    char line[256];
    fgets(line, sizeof(line), f); // Skip header
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = 0;
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        if (id && name)
        {
            GtkTreeIter iter;
            gtk_list_store_append(temp_store, &iter);
            gtk_list_store_set(temp_store, &iter, 0, id, 1, name, -1);
        }
    }
    fclose(f);
}

void on_create_combo_changed(GtkComboBoxText *combo, gpointer user_data)
{
    GtkListStore *temp_store = GTK_LIST_STORE(user_data);
    char *selected = gtk_combo_box_text_get_active_text(combo);

    if (selected)
    {
        char filename[256];
        snprintf(filename, sizeof(filename), "%s_students.csv", selected);
        load_csv_into_temp_store(filename, temp_store);
        g_free(selected);
    }
}

void on_class_file_selected(GtkMenuItem *item, gpointer user_data)
{
    const char *filename = (const char *)user_data;
    if (current_class_name)
        g_free(current_class_name);

    // Store just the base name (e.g., "IoT_Spring" from "IoT_Spring_students.csv")
    current_class_name = g_strdup(filename);
    char *ext = strstr(current_class_name, "_students.csv");
    if (ext)
        *ext = '\0';

    g_print("Switching to class list: %s\n", filename);

    // Clear current list and load the selected one
    gtk_list_store_clear(global_store);

    FILE *file = fopen(filename, "r");
    if (!file)
        return;
    char line[256];
    fgets(line, sizeof(line), file); // Skip header
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        if (id && name)
        {
            GtkTreeIter iter;
            gtk_list_store_append(global_store, &iter);
            gtk_list_store_set(global_store, &iter, 0, id, 1, name, 2, "Absent", 3, "white", -1);
        }
    }
    fclose(file);
}

GtkWidget *create_class_menu()
{
    GtkWidget *menu = gtk_menu_new();
    DIR *d = opendir("."); // Scan current directory
    struct dirent *dir;

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            // Only look for student CSV files (e.g., IoT_Spring_students.csv)
            if (strstr(dir->d_name, "_students.csv"))
            {
                GtkWidget *item = gtk_menu_item_new_with_label(dir->d_name);
                g_signal_connect(item, "activate", G_CALLBACK(on_class_file_selected), g_strdup(dir->d_name));
                gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
            }
        }
        closedir(d);
    }
    return menu;
}

char *lookup_student_name(const char *hashed_id)
{
    FILE *file = fopen("master_list.csv", "r");
    if (!file)
    {
        g_print("Error: Could not open master_students.csv\n");
        return NULL;
    }

    char line[256];
    char *found_name = NULL;
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0; // Strip newline characters

        char *csv_hash = strtok(line, ",");
        char *csv_name = strtok(NULL, ",");

        if (csv_hash && csv_name)
        {
            g_strstrip(csv_hash); // Clean hash column
            g_strstrip(csv_name); // Clean name column

            if (g_strcmp0(csv_hash, hashed_id) == 0)
            {
                found_name = g_strdup(csv_name);
                g_print("Match found! ID: %s -> Name: %s\n", csv_hash, found_name);
                break;
            }
        }
    }

    if (!found_name)
    {
        g_print("No match in master list for hash: %s\n", hashed_id);
    }

    fclose(file);
    return found_name;
}

// Function to save attendance to csv file format with appends for multiple lessons.
void on_save_clicked(GtkWidget *button, gpointer data)
{
    if (!current_class_name)
        return;

    GtkListStore *store = GTK_LIST_STORE(data);
    char attendance_file[256];
    snprintf(attendance_file, sizeof(attendance_file), "%s_attendance.csv", current_class_name);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[11];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", t);

    FILE *exist_f = fopen(attendance_file, "r");
    FILE *temp_f = fopen("temp_attn.csv", "w");

    if (!temp_f)
    {
        g_print("Error: Could not create temporary file.\n");
        if (exist_f)
            fclose(exist_f);
        return;
    }

    if (!exist_f)
    {
        // CASE 1: New File
        fprintf(temp_f, "ID,Name,%s\n", date_str);
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
        while (valid)
        {
            gchar *id, *name, *status;
            gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, COL_ID, &id, COL_NAME, &name, COL_STATUS, &status, -1);
            fprintf(temp_f, "%s,%s,%s\n", id, name, status);
            g_free(id);
            g_free(name);
            g_free(status);
            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
    }
    else
    {
        // CASE 2: Merge with existing
        char line[1024];
        GSList *processed_ids = NULL;
        int prior_date_count = 0;

        if (fgets(line, sizeof(line), exist_f))
        {
            line[strcspn(line, "\r\n")] = 0;
            for (char *p = line; *p; p++)
                if (*p == ',')
                    prior_date_count++;
            prior_date_count -= 1;
            fprintf(temp_f, "%s,%s\n", line, date_str);
        }

        while (fgets(line, sizeof(line), exist_f))
        {
            line[strcspn(line, "\r\n")] = 0;
            char *line_copy = g_strdup(line);
            char *id = strtok(line_copy, ",");
            const char *status = "Absent";

            GtkTreeIter iter;
            if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
            {
                do
                {
                    gchar *list_id, *list_status;
                    gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, COL_ID, &list_id, COL_STATUS, &list_status, -1);
                    if (g_strcmp0(id, list_id) == 0)
                    {
                        status = list_status;
                        processed_ids = g_slist_append(processed_ids, g_strdup(list_id));
                        g_free(list_id);
                        g_free(list_status);
                        break;
                    }
                    g_free(list_id);
                    g_free(list_status);
                } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
            }
            fprintf(temp_f, "%s,%s\n", line, status);
            g_free(line_copy);
        }

        // Append late-joiners
        GtkTreeIter iter;
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
        {
            do
            {
                gchar *list_id, *list_name, *list_status;
                gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, COL_ID, &list_id, COL_NAME, &list_name, COL_STATUS, &list_status, -1);

                gboolean is_new = TRUE;
                for (GSList *l = processed_ids; l != NULL; l = l->next)
                {
                    if (g_strcmp0((char *)l->data, list_id) == 0)
                    {
                        is_new = FALSE;
                        break;
                    }
                }

                if (is_new)
                {
                    fprintf(temp_f, "%s,%s", list_id, list_name);
                    for (int i = 0; i < prior_date_count; i++)
                        fprintf(temp_f, ",Absent");
                    fprintf(temp_f, ",%s\n", list_status);
                }
                g_free(list_id);
                g_free(list_name);
                g_free(list_status);
            } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
        }
        g_slist_free_full(processed_ids, g_free);
        fclose(exist_f);
    }

    // CRITICAL: Close temp_f BEFORE rename
    fclose(temp_f);

    if (remove(attendance_file) != 0)
    {
        // If file is locked, we can't remove it.
        g_print("Warning: Could not remove old file. It might be open in Excel.\n");
    }

    if (rename("temp_attn.csv", attendance_file) == 0)
    {
        g_print("Attendance saved to %s\n", attendance_file);
    }
    else
    {
        g_print("Error: Could not rename temp file. Check file permissions.\n");
    }
}

void load_students_into_list(GtkListStore *store)
{
    FILE *file = fopen("students.csv", "r");
    if (!file)
        return;
    char line[256];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = 0;
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");

        if (id && name)
        {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               COL_ID, id, // Use the ID directly from file
                               COL_NAME, name,
                               COL_STATUS, "Absent",
                               COL_COLOR, "white", -1);
        }
    }
    fclose(file);
}

void on_barcode_scanned(GtkEntry *entry, gpointer data)
{
    GtkListStore *store = GTK_LIST_STORE(data);
    const char *text = gtk_entry_get_text(entry);
    if (strlen(text) == 0)
        return;

    // 1. Hash the incoming scan
    char *scanned_hash = hash_id(text);
    GtkTreeIter iter;
    GtkTreeModel *model = GTK_TREE_MODEL(store);
    gboolean found = FALSE;

    // 2. Compare the new hash against hashes in the table
    if (gtk_tree_model_get_iter_first(model, &iter))
    {
        do
        {
            gchar *list_hash;
            gtk_tree_model_get(model, &iter, COL_ID, &list_hash, -1);
            if (g_strcmp0(list_hash, scanned_hash) == 0)
            {
                gtk_list_store_set(store, &iter, COL_STATUS, "Present", COL_COLOR, "#90EE90", -1);
                found = TRUE;
                g_free(list_hash);
                break;
            }
            g_free(list_hash);
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    // ... handle !found logic ...
    g_free(scanned_hash);
    gtk_entry_set_text(entry, "");
}

void on_name_edited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data)
{
    GtkListStore *store = GTK_LIST_STORE(data);
    GtkTreeIter iter;

    // Convert the visible row number (path) to a data pointer (iter)
    if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path))
    {
        // Update Column 1 (the Name column) with the new text
        gtk_list_store_set(store, &iter, 1, new_text, -1);
    }
}

void on_id_entry_activated(GtkEntry *entry, gpointer user_data)
{
    GtkListStore *store = GTK_LIST_STORE(user_data);
    const char *raw_id = gtk_entry_get_text(entry);

    if (strlen(raw_id) > 0)
    {
        // 1. Convert to hash immediately
        char *h_id = hash_id(raw_id);

        // 2. Lookup name using that hash
        char *name = lookup_student_name(h_id);

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        // 3. Store ONLY the hash in the ID column (COL_ID)
        gtk_list_store_set(store, &iter, 0, h_id, 1, name ? name : "Unknown", -1);

        g_free(h_id);
        if (name)
            g_free(name);
        gtk_entry_set_text(entry, "");
    }
}

void on_remove_id_clicked(GtkWidget *button, gpointer user_data)
{
    GtkTreeView *tree_view = GTK_TREE_VIEW(user_data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;

    // Check if a row is actually selected
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void on_make_master_clicked(GtkWidget *button, gpointer user_data)
{
    GtkListStore *temp_store = GTK_LIST_STORE(user_data);

    // 1. Load existing IDs from master_list.csv into a GSList for easy checking
    GSList *existing_hashes = NULL;
    FILE *read_f = fopen("master_list.csv", "r");
    if (read_f)
    {
        char line[512];
        fgets(line, sizeof(line), read_f); // Skip header
        while (fgets(line, sizeof(line), read_f))
        {
            char *comma = strchr(line, ',');
            if (comma)
            {
                *comma = '\0'; // Just get the hash
                existing_hashes = g_slist_append(existing_hashes, g_strdup(line));
            }
        }
        fclose(read_f);
    }

    // 2. Open for appending (if file doesn't exist, "a" creates it)
    gboolean is_new_file = (access("master_list.csv", F_OK) == -1);
    FILE *f = fopen("master_list.csv", "a");
    if (!f)
        return;

    if (is_new_file)
    {
        fprintf(f, "ID,Name\n");
    }

    // 3. Iterate through current UI list and append only new IDs
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(temp_store), &iter);
    int added_count = 0;

    while (valid)
    {
        gchar *h_id, *name;
        gtk_tree_model_get(GTK_TREE_MODEL(temp_store), &iter, 0, &h_id, 1, &name, -1);

        // Check if hash already exists in master_list
        gboolean already_exists = FALSE;
        for (GSList *l = existing_hashes; l != NULL; l = l->next)
        {
            if (g_strcmp0((char *)l->data, h_id) == 0)
            {
                already_exists = TRUE;
                break;
            }
        }

        if (!already_exists)
        {
            fprintf(f, "%s,%s\n", h_id, (name && strlen(name) > 0) ? name : "Unknown");
            added_count++;
        }

        g_free(h_id);
        g_free(name);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(temp_store), &iter);
    }

    fclose(f);

    // Clean up memory
    g_slist_free_full(existing_hashes, g_free);

    g_print("Master list updated: Added %d new students.\n", added_count);
}

// Callback to handle the "Create Student List" button
void on_create_list_clicked(GtkWidget *button, gpointer data)
{
    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_toplevel(button));
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create Student List", parent,
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_Save CSV", GTK_RESPONSE_ACCEPT,
                                                    "_Cancel", GTK_RESPONSE_REJECT, NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 15);

    // --- 1: INITIALIZE THE STORE FIRST ---
    // We do this early so the combo box "changed" signal has a valid pointer to use
    GtkListStore *temp_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    // --- 2: CLASS NAME INPUT ---
    GtkWidget *class_combo = gtk_combo_box_text_new_with_entry();
    GtkWidget *class_entry = gtk_bin_get_child(GTK_BIN(class_combo));
    gtk_entry_set_placeholder_text(GTK_ENTRY(class_entry), "Select or Type Class Name");

    // Populate dropdown
    DIR *d = opendir(".");
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strstr(dir->d_name, "_students.csv"))
            {
                char *display_name = g_strdup(dir->d_name);
                char *ext = strstr(display_name, "_students.csv");
                if (ext)
                    *ext = '\0';
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(class_combo), display_name);
                g_free(display_name);
            }
        }
        closedir(d);
    }

    // Connect the "changed" signal so it loads the list when you pick a class
    g_signal_connect(class_combo, "changed", G_CALLBACK(on_create_combo_changed), temp_store);

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Class Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), class_combo, FALSE, FALSE, 0);

    // --- 3: ID ENTRY ---
    GtkWidget *add_id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(add_id_entry), "Scan or Type ID and press Enter");
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Add New Student by ID:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), add_id_entry, FALSE, FALSE, 0);

    // --- 4: SETUP THE TABLE VIEW ---
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(temp_store));
    g_signal_connect(add_id_entry, "activate", G_CALLBACK(on_id_entry_activated), temp_store);

    GtkCellRenderer *id_renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", id_renderer, "text", 0, NULL);

    GtkCellRenderer *name_renderer = gtk_cell_renderer_text_new();
    g_object_set(name_renderer, "editable", TRUE, NULL);
    g_signal_connect(name_renderer, "edited", G_CALLBACK(on_name_edited), temp_store);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Student Name", name_renderer, "text", 1, NULL);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll, 400, 300);
    gtk_container_add(GTK_CONTAINER(scroll), tree_view);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    // --- 5: BUTTONS ---
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
    gtk_box_set_spacing(GTK_BOX(button_box), 10);

    GtkWidget *master_btn = gtk_button_new_with_label("Save as Master List");
    g_signal_connect(master_btn, "clicked", G_CALLBACK(on_make_master_clicked), temp_store);

    GtkWidget *remove_btn = gtk_button_new_with_label("Remove Selected ID");
    g_signal_connect(remove_btn, "clicked", G_CALLBACK(on_remove_id_clicked), tree_view);

    gtk_container_add(GTK_CONTAINER(button_box), remove_btn);
    gtk_container_add(GTK_CONTAINER(button_box), master_btn);
    gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        const char *class_name = gtk_entry_get_text(GTK_ENTRY(class_entry));
        if (strlen(class_name) > 0)
        {
            char filename[256];
            snprintf(filename, sizeof(filename), "%s_students.csv", class_name);

            // Open in Write mode ("w") because temp_store now contains the FULL list (loaded + added)
            FILE *f = fopen(filename, "w");
            if (f)
            {
                fprintf(f, "ID,Name\n");
                GtkTreeIter iter;
                gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(temp_store), &iter);
                while (valid)
                {
                    gchar *h_id, *name;
                    gtk_tree_model_get(GTK_TREE_MODEL(temp_store), &iter, 0, &h_id, 1, &name, -1);
                    fprintf(f, "%s,%s\n", h_id, (name && strlen(name) > 0) ? name : "Unknown");
                    g_free(h_id);
                    g_free(name);
                    valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(temp_store), &iter);
                }
                fclose(f);
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void refresh_class_list_menu(GtkWidget *menu, gpointer user_data)
{
    GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
    for (GList *l = children; l != NULL; l = l->next)
    {
        gtk_widget_destroy(GTK_WIDGET(l->data));
    }
    g_list_free(children);

    DIR *d = opendir(".");
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strstr(dir->d_name, "_students.csv"))
            {
                GtkWidget *item = gtk_menu_item_new_with_label(dir->d_name);
                g_signal_connect(item, "activate", G_CALLBACK(on_class_file_selected), g_strdup(dir->d_name));
                gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
            }
        }
        closedir(d);
    }
    gtk_widget_show_all(menu);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Attendance System v0.6a");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Input area
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Scan ID...");

    // Data store
    GtkListStore *store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    load_students_into_list(store);

    // View
    GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    char *titles[] = {"ID", "Name", "Status"};
    for (int i = 0; i < 3; i++)
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        // This line tells GTK to look at COL_COLOR to decide the background color
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, "cell-background", COL_COLOR, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
    }

    // Save Button
    GtkWidget *save_btn = gtk_button_new_with_label("Save Attendance to CSV");
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_clicked), store);

    // Create new list button
    GtkWidget *create_list_btn = gtk_button_new_with_label("Create New Class List");
    g_signal_connect(create_list_btn, "clicked", G_CALLBACK(on_create_list_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(vbox), create_list_btn, FALSE, FALSE, 0);

    // menu bar for opening student lists
    global_store = store;

    GtkWidget *menu_bar = gtk_menu_bar_new();
    GtkWidget *file_root = gtk_menu_item_new_with_label("Select Class List");
    GtkWidget *class_menu = gtk_menu_new();

    g_signal_connect(class_menu, "show", G_CALLBACK(refresh_class_list_menu), NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_root), class_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_root);

    // Pack it at the start (index 0) of the vbox
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_reorder_child(GTK_BOX(vbox), menu_bar, 0);

    // Layout
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), treeview);

    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), save_btn, FALSE, FALSE, 0);

    g_signal_connect(entry, "activate", G_CALLBACK(on_barcode_scanned), store);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(entry);

    gtk_main();
    return 0;
}
