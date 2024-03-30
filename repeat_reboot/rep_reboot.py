#!/usr/bin/env python3

import os
import subprocess

# Set the number of times to run the experiment
num_runs = 4

# File to store the current run number
run_file = "./run_file.txt"

def initialize_run_file():
    if not os.path.exists(run_file):
        with open(run_file, "w") as file:
            file.write("1")

def read_current_run():
    with open(run_file, "r") as file:
        return int(file.read())

def write_current_run(current_run):
    with open(run_file, "w") as file:
        file.write(str(current_run))

def run_experiment(current_run):
    # Run your experiment command here
    subprocess.run(["touch", f"file_{current_run}"])

    # Run an executable
    # run_command(f"./{EXECUTABLE_NAME}")

    # Run a separate Python script
    # run_command(f"python {PYTHON_SCRIPT_NAME}")

    # Run a command with sudo
    # subprocess.run(["sudo", "-S", "echo", "hi"], input=b"kele\n")

def reboot_system():
    # subprocess.run(["sudo", "-S", "reboot"], input=b"kele\n")
    subprocess.run(["sudo", "-S", "echo", "reboot"], input=b"kele\n")

def main():
    initialize_run_file()
    current_run = read_current_run()

    while current_run <= num_runs:
        if current_run <= num_runs:
            reboot_system()

        run_experiment(current_run)
        current_run += 1
        write_current_run(current_run)

    # rm this counter file
    os.remove(run_file)

if __name__ == "__main__":
    main()