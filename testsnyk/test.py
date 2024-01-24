import os
import subprocess

x = 'abc'

domain = input("Enter the Domain: ")
output = subprocess.check_output(f"nslookup {domain}", shell=True, encoding='UTF-8')
print(output)

print(os.popen("ls -l").read())
