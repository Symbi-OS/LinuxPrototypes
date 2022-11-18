#!/bin/bash

sudo grubby --remove-args="nosmt single" --args="mitigations=off" --update-kernel /boot/vmlinuz-$(uname -r)
