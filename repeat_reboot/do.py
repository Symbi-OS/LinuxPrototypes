import os
import subprocess

# Variables
SCRIPT_NAME = "experiment.py"
CURRENT_DIR = os.getcwd()
CRONTAB_ENTRY = f"@reboot {CURRENT_DIR}/{SCRIPT_NAME}"

def run_command(command):
    expanded_command = command.split()
    subprocess.run(expanded_command, check=True)

def install():
    # Install the cron package
    run_command("sudo dnf install -y cronie")

    # Start and enable the cron service
    run_command("sudo systemctl start crond")
    run_command("sudo systemctl enable crond")

    # Make the script executable
    run_command(f"chmod +x {SCRIPT_NAME}")

    # Add the script to the crontab
    crontab = subprocess.run("crontab -l", capture_output=True, text=True, shell=True).stdout
    if CRONTAB_ENTRY not in crontab:
        crontab += f"\n{CRONTAB_ENTRY}\n"
        subprocess.run(f"echo '{crontab}' | crontab -", shell=True, check=True)

def uninstall():
    # Remove the script from the crontab
    crontab = subprocess.run("crontab -l", capture_output=True, text=True, shell=True).stdout
    crontab = crontab.replace(CRONTAB_ENTRY + "\n", "")
    subprocess.run(f"echo '{crontab}' | crontab -", shell=True, check=True)

def help():
    print("Available commands:")
    print("  install   - Install and configure the experiment automation")
    print("  uninstall - Remove the experiment automation")
    print("  help      - Show this help message")

if __name__ == "__main__":
    if len(os.sys.argv) != 2:
        help()
    else:
        command = os.sys.argv[1]
        if command == "install":
            install()
        elif command == "uninstall":
            uninstall()
        elif command == "help":
            help()
        else:
            print(f"Unknown command: {command}")
            help()