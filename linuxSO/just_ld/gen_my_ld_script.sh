#!/bin/bash

# Check if the user provided a file as an argument
if [[ -z "$1" ]]; then
    echo "Usage: ./gen_my_ld_script.sh <path to kallsyms>" >&2
    echo "Please provide a file from where to get the symbols." >&2
    echo "It might be the SystemMap file produced at kernel build time or perhaps /proc/kallsyms." >&2
    exit 1
fi

# Use the provided file or fallback to /proc/kallsyms
FILE="$1"
if [[ ! -f "$FILE" ]]; then
    echo "Error: File $FILE does not exist." >&2
    exit 2
fi

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
' "$FILE"

