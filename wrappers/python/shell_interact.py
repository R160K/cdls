#TODO: Get the enter command to come in after the CDLS string
#i.e. read from the output buffer before the line is finished.
#
#invoke (pythonically) any expression entered that returns an error code (1)
#:-;
#
#Exit if no command entered.
#Goodnight.

import subprocess
import os

def launch_it(command = ""):
    result = subprocess.run(['C:\\bin\\cdls.exe'], input=command, capture_output=True)

    output = result.stdout.decode()
    lines = output.split("\n")
    
    i = 0
    num_lines = len(lines) - 1
    while i < num_lines:
        print(lines[i])
        i = i+1
    
    last_line = lines[num_lines]
    
    parts = last_line.split(" ")
    uri = parts[-1].split(">")[0]
    expression = ""
    if not uri:
        not_last_line = lines[num_lines - 1]
        uri = not_last_line.split(" ")[2].split(">")[0]
        expression = not_last_line.split(">")[1]
    
    return uri, result.returncode, expression

def run_it(command=""):
    if not command:
        command = input("CDLS (Python) %s> " % os.getcwd()).encode()
    
    uri, return_code, expression = launch_it(command)
    
    os.chdir(uri)
    
    print("\nReturn code:", return_code)


#Initialise
#run_it(b".")

while True:
    run_it()