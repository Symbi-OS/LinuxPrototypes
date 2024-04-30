#!/bin/bash

for i in {0..10}; do make run_xput_manual_sc | grep Throughput | cut -d ' ' -f 2; done
