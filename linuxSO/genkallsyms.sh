#!/bin/bash
echo '.section .data'

awk '
BEGIN { duplicate_count = 0; }
{
  if ($3 in symbol_count) {
    ++symbol_count[$3];
    ++duplicate_count;
  } else {
    symbol_count[$3] = 1;
    print_symbol($1, $2, $3);
  }
}
END { print "Warning: " duplicate_count " duplicate symbols found, only the first occurrence was used." > "/dev/stderr"; }
function print_symbol(addr, type, symbol) {
  if (type ~ /^[tTrRdDbB]$/) {
    print ".global " symbol;
    print symbol ":";
    print "  jmp *" symbol "_my_unique_suffix(%rip)";
    print "  nop";
    print symbol "_my_unique_suffix:";
    print "  .quad 0x" addr;
  }
}
' /proc/kallsyms

