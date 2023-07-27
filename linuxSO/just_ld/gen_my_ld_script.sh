#!/bin/bash

echo "INCLUDE default_ld_script.ld"
echo ""

awk '
BEGIN { duplicate_count = 0; }
{
  if (seen[$3]++) {
    duplicate_count++;
  } else {
    print $3 " = 0x" $1 ";"
  }
}
END { print "Warning: " duplicate_count " duplicate symbols found, only the first occurrence was used." > "/dev/stderr"; }
' /proc/kallsyms

