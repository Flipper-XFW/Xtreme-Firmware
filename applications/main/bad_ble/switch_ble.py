import os
import re

def analyze_and_replace(directory):
  # Recursively search for .c and .h files in the given directory
  for root, dirs, files in os.walk(directory):
    for file in files:
      if file.endswith(".c") or file.endswith(".h"):
        # Read the contents of the file
        with open(os.path.join(root, file), "r") as f:
          contents = f.read()
        
        # Replace all occurrences of "BadUsb" and "bad_usb" with "BadBle" and "bad_ble"
        contents = contents.replace("usb", "ble")
        contents = contents.replace("USB", "BLE")
        contents = contents.replace("Usb", "Ble")

        
        # Write the modified contents back to the file
        with open(os.path.join(root, file), "w") as f:
          f.write(contents)

# Test the function with a sample directory
analyze_and_replace(".")
