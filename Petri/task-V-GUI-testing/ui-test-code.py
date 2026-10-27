
import pytest
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

BASE_URL = "http://localhost:3000"
TIMEOUT = 10


@pytest.fixture
def driver():
    """Create a Chrome browser and close it after each test."""
    options = webdriver.ChromeOptions()
    options.add_argument("--window-size=1920,1080")
    browser = webdriver.Chrome(options=options)
    browser.implicitly_wait(2)
    yield browser
    browser.quit()


def accept_cookies(driver):
    """Accept W3Schools consent through the actual form control."""
    try:
        accept_button = WebDriverWait(driver, TIMEOUT).until(
            EC.element_to_be_clickable((
                By.CSS_SELECTOR,
                "form#fast-cmp-form "
                "article#fast-cmp-home "
                "button.fast-cmp-button-primary[value='all']"
            ))
        )

        accept_button.click()

        WebDriverWait(driver, TIMEOUT).until(
            EC.invisibility_of_element_located(
                (By.ID, "fast-cmp-home")
            )
        )

        print("Cookie consent accepted.")

    except Exception as error:
        dialog = driver.find_elements(By.ID, "fast-cmp-home")

        if not dialog or not dialog[0].is_displayed():
            print("Cookie consent dialog not present.")
        else:
            raise RuntimeError(
                "Could not accept W3Schools cookie consent."
            ) from error


@pytest.fixture
def page(driver):
    """Open the website and handle its cookie banner before testing."""
    driver.get(BASE_URL)
    accept_cookies(driver)
    return driver


def test_page_loads(page):
    """Check that the page loads with a valid URL and non-empty title."""
    assert page.current_url.startswith("http")
    assert page.title.strip() != ""


def test_page_has_body(page):
    """Check that the page body becomes visible."""
    body = WebDriverWait(page, TIMEOUT).until(
        EC.visibility_of_element_located((By.TAG_NAME, "body"))
    )
    assert body.is_displayed()


def test_page_has_no_horizontal_overflow(page):
    """Check that the page does not overflow horizontally."""
    has_overflow = page.execute_script(
        "return document.documentElement.scrollWidth > "
        "document.documentElement.clientWidth;"
    )
    assert not has_overflow, "Page has horizontal overflow"


def test_images_load(page):
    """Check that every visible image has a source and loads successfully."""
    images = page.find_elements(By.TAG_NAME, "img")

    for image in images:
        if image.is_displayed():
            src = image.get_attribute("src")
            assert src, "Image has no source"
            assert image.get_property("complete") is True, (
                f"Image not fully loaded: {src}"
            )
            assert image.get_property("naturalWidth") > 0, (
                f"Image broken or 0px wide: {src}"
            )


def test_links_have_destinations(page):
    """Check that every visible link has a destination URL."""
    links = page.find_elements(By.TAG_NAME, "a")

    for link in links:
        if link.is_displayed():
            assert link.get_attribute("href"), "Link has no destination"


def test_buttons_are_interactable(page):
    """Check that visible buttons and submit controls are enabled."""
    buttons = page.find_elements(
        By.CSS_SELECTOR,
        "button, input[type='button'], input[type='submit']"
    )

    for button in buttons:
        if button.is_displayed():
            assert button.is_enabled(), (
                f"Button is unexpectedly disabled: {button.text}"
            )


def test_no_broken_internal_links(page):
    """Visit internal links and verify that each page reaches a ready state."""
    base_origin = page.execute_script("return window.location.origin")
    links = page.find_elements(By.TAG_NAME, "a")

    internal_urls = {
        link.get_attribute("href")
        for link in links
        if link.get_attribute("href")
        and link.get_attribute("href").startswith(base_origin)
    }

    for url in internal_urls:
        page.get(url)
        WebDriverWait(page, TIMEOUT).until(
            lambda browser: browser.execute_script(
                "return document.readyState"
            ) in ("interactive", "complete")
        )

    # Restore the starting page for consistency.
    page.get(BASE_URL)
    accept_cookies(page)


def test_page_has_no_horizontal_overflow_after_resize(page):
    """Check horizontal overflow at desktop, tablet, and mobile widths."""
    for width in (1920, 1366, 768, 375):
        page.set_window_size(width, 900)

        WebDriverWait(page, TIMEOUT).until(
            EC.visibility_of_element_located((By.TAG_NAME, "body"))
        )

        has_overflow = page.execute_script(
            "return document.documentElement.scrollWidth > "
            "document.documentElement.clientWidth;"
        )

        assert not has_overflow, (
            f"Horizontal overflow at width {width}px"
        )