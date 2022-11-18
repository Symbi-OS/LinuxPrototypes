#!/bin/bash

sudo grubby --remove-args="mitigations=off" --args="nosmt single" --update-kernel /boot/vmlinuz-$(uname -r)
