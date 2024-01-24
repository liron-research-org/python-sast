# Mock user data
user_data = {
    'Authenticating': {
        'Mother\'s Maiden Name': 'Smith',
        'Passwords': 'Password123',
        'PIN': '1234'
    },
    'Behavioral Information': {
        'Browsing Behavior': 'Visited https://www.example.com',
        'Communication': 'Email sent to john@example.com, content "Hello John"'
    },
    'Computer Device': {
        'Browser Fingerprint': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)',
        'Device identifier': 'Android-12345',
        'IP address': '192.168.1.1',
        'Mac address': '00:0a:95:9d:68:16'
    },
    # More data...
}

# Simulating a function that might accidentally print sensitive data
def debug_print_user_data():
    print("Debug: User data: ", user_data)

# Calling the function
debug_print_user_data()
