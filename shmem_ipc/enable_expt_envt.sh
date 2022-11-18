#!/bin/bash

sudo grubby --remove-args="mitigations=off isolcpus=0,1" --args="nosmt single" --update-kernel /boot/vmlinuz-$(uname -r)
