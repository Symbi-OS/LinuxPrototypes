#!/bin/bash

sudo grubby --remove-args="nosmt single" --args="mitigations=off isolcpus=0,1" --update-kernel /boot/vmlinuz-$(uname -r)
