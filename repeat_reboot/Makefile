# Makefile for Fedora Experiment Automation

# Variables
SCRIPT_NAME = rep_reboot.py
CURRENT_DIR = $(shell pwd)
CRONTAB_ENTRY = "@reboot $(CURRENT_DIR)/$(SCRIPT_NAME)"

# Default target
all: install

# Install target
install:
    # Install the cron package
    sudo dnf install -y cronie

    # Start and enable the cron service
    sudo systemctl start crond
    sudo systemctl enable crond

    # Make the script executable
    chmod +x $(SCRIPT_NAME)

    # Add the script to the crontab
    (crontab -l 2>/dev/null; echo "$(CRONTAB_ENTRY)") | crontab -

# Uninstall target
uninstall:
    # Remove the script from the crontab
    crontab -l | grep -v "$(SCRIPT_NAME)" | crontab -

# Help target
help:
    @echo "Available targets:"
    @echo "  install   - Install and configure the experiment automation"
    @echo "  uninstall - Remove the experiment automation"
    @echo "  help      - Show this help message"

# Phony targets
.PHONY: all install uninstall help