"""
test_lab_fi_site.py

Automated UI tests for the LAB.fi website using Selenium WebDriver and pytest.

This test suite verifies basic page functionality and content, including:
- Page title verification
- Meta description correctness
- Page navigation and interaction with elements such as links and cookie banners
- Saves screenshot of the front-page for audit purposes.

Requirements:
- Python 3.x
- Selenium
- pytest
- webdriver_manager

The tests are designed to be run with Chrome, using ChromeDriverManager to manage the driver.
Some `time.sleep()` calls are used for demo purposes to allow visual confirmation during test runs.
"""

import pytest
import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

@pytest.fixture
def driver():
    """
    Pytest fixture to initialize and yield a Selenium WebDriver instance for Chrome.

    The driver is configured to:
    - Use ChromeDriverManager to install and manage ChromeDriver automatically
    - Set device scale factor (zoom) to 0.5 for demonstration purposes
    - Maximize the browser window

    Yields:
        webdriver.Chrome: A configured Chrome WebDriver instance.

    The driver is automatically quit after the test using the fixture completes.
    """
    service = Service(ChromeDriverManager().install())
    
    options = webdriver.ChromeOptions()
    options.add_argument("--force-device-scale-factor=0.5")  # Zoom to make sure the page fits in the Chrome Window 1.0 = 100%, 1.5 = 150%, 0.5 = 50%
    # options.add_argument("--window-size=1920,1080")  # we could also set the initial window size, width, height in pixels
    
    driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=options)
    driver.maximize_window()  # maximize the window
    yield driver
    driver.quit()

@pytest.mark.skip(reason="Skipping this test for demo")  # comment to enable the test
def test_lab_fi_title(driver):
    """
    Verify that the LAB.fi homepage has the correct page title.

    Steps:
    1. Open the LAB.fi homepage.
    2. Optionally wait to visually confirm the page load.
    3. Assert that the page title matches the expected string.

    Args:
        driver (webdriver.Chrome): Selenium WebDriver fixture.
    """
    print("Checking for correct page title")
    
    driver.get("https://lab.fi/en")
    time.sleep(2)  # not required, but pause so you can see the page
    
    assert "LAB University of Applied Sciences | LAB.fi" in driver.title # use the Selenium's built-in property
        
    #title_element = driver.find_element(By.TAG_NAME, "title")                      # this might also work,
    #assert "LAB University of Applied Sciences | LAB.fi" in title_element.text     # if the title is not dynamically updated
    
    time.sleep(2)  # not required, but pause so you can see the page

@pytest.mark.skip(reason="Skipping this test for demo")  # comment to enable the test
def test_lab_fi_meta_description(driver):
    """
    Verify that the LAB.fi homepage meta description is correct.

    Steps:
    1. Open the LAB.fi homepage.
    2. Optionally wait to visually confirm the page load.
    3. Locate the meta description using multiple methods (XPath or CSS selector).
    4. Assert that the content matches the expected description.

    Args:
        driver (webdriver.Chrome): Selenium WebDriver fixture.
    """
    print("Checking for correct meta description")
    
    driver.get("https://lab.fi/en")
    time.sleep(2)  # not required, but pause so you can see the page
    
    ############# there are multiple ways of doing this #############
    meta_desc = driver.find_element(By.XPATH, "//head/meta[@name='description']") # use XPATH
    assert meta_desc.get_attribute("content") == "LAB is a higher education institution focusing on innovation, business and industry. It operates in Lahti and Lappeenranta and also provides education online."
    # OR
    meta_desc = WebDriverWait(driver, 5).until( # "same" as above, but wait up to 5 seconds for the element to appear in DOM in case it is not immediately populated
        EC.presence_of_element_located((By.XPATH, "//head/meta[@name='description']"))
    )
    assert meta_desc.get_attribute("content") == "LAB is a higher education institution focusing on innovation, business and industry. It operates in Lahti and Lappeenranta and also provides education online."
    # OR
    meta_desc = driver.find_element(By.CSS_SELECTOR, "head > meta[name='description']") # use CSS Selector
    assert meta_desc.get_attribute("content") == "LAB is a higher education institution focusing on innovation, business and industry. It operates in Lahti and Lappeenranta and also provides education online."
    
    time.sleep(2)  # not required, but pause so you can see the page

@pytest.mark.skip(reason="Skipping this test for demo")  # comment to enable the test
def test_page_navigation(driver):
    """
    Verify navigation from the LAB.fi homepage to a specific news page.

    Steps:
    1. Open the LAB.fi homepage.
    2. Handle the cookie banner if present.
    3. Locate and click a specific link to navigate to the "News and Stories" page.
    4. Wait until navigation is complete and verify the URL contains the expected path.

    Args:
        driver (webdriver.Chrome): Selenium WebDriver fixture.
    """
    print("Checking for correct meta description")
    
    driver.get("https://lab.fi/en")
    time.sleep(2)  # not required, but pause so you can see the page
    
    try:    # let's see if a cookie banner appears, and try to close it.
        cookie_button = WebDriverWait(driver, 5).until(
            EC.element_to_be_clickable((By.ID, "ppms_cm_reject-all")) # there is no universal, standard naming for the reject button, the id/class needs be checked from the page sources
        )
        cookie_button.click()
        print("Cookie banner accepted")
    except:
        print("No cookie banner found, continuing...")

    time.sleep(2)  # not required, but pause so you can see the page
    
    #button = WebDriverWait(driver, 5).until(
    #    EC.element_to_be_clickable((By.ID, "submit-button")) # finding a specific element to click on a dynamically generated page can be a bit challenging. Something like this might work.
    #)
    #button.click()
    
    link = driver.find_element(By.CSS_SELECTOR, 'a[data-drupal-link-system-path="node/5"]') # but in this case, let's try to navigate by finding a specific Drupal node.
    link.click()
    
    WebDriverWait(driver, 10).until(EC.url_contains("/news-and-stories")) # wait until page navigation has completed
    
    assert "/news-and-stories" in driver.current_url # check we arrived to the correct page, note: this only works if a real URL transition happened, if the contents were only dynamically updated, something else that has "changed" should be found on the page
    
    time.sleep(2)  # not required, but pause so you can see the page

@pytest.mark.skip(reason="Skipping this test for demo")  # comment to enable the test
def test_front_page(driver):
    """
    Take screenshot of the LAB.fi front page for audit purposes.

    Steps:
    1. Open the LAB.fi homepage.
    2. Take a screenshot of the front page.

    Args:
        driver (webdriver.Chrome): Selenium WebDriver fixture.
    """
    print("Checking that the front page looks OK")
    
    driver.get("https://lab.fi/en")
    time.sleep(2)  # not required, but pause so you can see the page
    
    # Take a screenshot for debugging/reporting
    timestamp = int(time.time())
    screenshot_file = f"screenshot_{timestamp}.png"
    driver.save_screenshot(screenshot_file) # take a screenshot to verify how the front page looked like
    print(f"Screenshot saved to {screenshot_file}")
    
    time.sleep(2)  # not required, but pause so you can see the page
    
