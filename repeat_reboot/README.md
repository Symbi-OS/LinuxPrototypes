Getting close. Need to find a way to get this to run without logging in. Systemd would be the obvious way, but if we're doing this on single user mode, it's not going to work. Tried /etc/rc.local which didn't persist.

# Fedora Experiment Automation

This Python script automates running an experiment on a Fedora machine with reboots between each run. It allows you to specify the number of times to run the experiment and handles the rebooting process.

## Prerequisites

- Fedora operating system
- Python 3 installed

## Installation

1. Clone the repository or download the script file.

2. Install the cron package if it's not already installed:
   ```bash
   sudo dnf install cronie
   ```

3. Start and enable the cron service (enable is necessary if you want it to start on system boot):
   ```bash
   sudo systemctl start crond
   sudo systemctl enable crond
   ```

## Usage

1. Open the script file and modify the following variables according to your requirements:
   - `num_runs`: Set the desired number of times to run the experiment.
   - `run_file`: Specify the path to the file that will store the current run number.
   - `your_experiment_command`: Replace this with the actual command or set of commands to run your experiment.

2. Make the script executable:
   ```bash
   chmod +x experiment.py
   ```

3. Schedule the script to run on system startup using crontab:
   ```bash
   crontab -e
   ```

   Add the following line at the end of the crontab file:
   ```
   @reboot /path/to/your/experiment.py
   ```

   Replace `/path/to/your/experiment.py` with the actual path to the script file.

4. Save the crontab file and exit the editor.

5. Reboot your Fedora machine to start the automated experiment.

The script will run the experiment the specified number of times, rebooting the system between each run. After completing all the runs, the system will boot up normally without running the experiment again.

## Note

If your experiment requires running commands with `sudo`, make sure to configure sudo permissions appropriately. 

Maybe look into this if you need to run the script with sudo on a real system:

Use visudo to grant specific command execution without a password:

Edit the /etc/sudoers file using the command sudo visudo.
Add the following line at the end of the file, replacing your_username with your actual username and /path/to/your/script.py with the actual path to your Python script:

Copy code
your_username ALL=(ALL) NOPASSWD: <executable>