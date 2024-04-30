#!/bin/bash

for i in {0..10}; do make run_xput_sc 2>/dev/null | grep Throughput | cut -d ' ' -f 2; done
