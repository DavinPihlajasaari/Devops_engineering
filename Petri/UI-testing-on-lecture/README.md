# Create a new venv
python3 -m venv selenium-env

# Activate it
source selenium-env/bin/activate   # Linux / macOS

selenium-env\Scripts\activate      # Windows PowerShell

# Install dependencies
pip install --upgrade pip

pip install selenium webdriver-manager pytest

# Run the test
pytest -s test_example.py
