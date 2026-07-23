#!/bin/sh
# Temporary precompile helper for continuum 14371-14380 (not a Makefile).
set -eu
cd "$(dirname "$0")"
CFLAGS="-std=c11 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -msse2 -Wall -Wextra -Werror -c"
pass=0
fail=0
for n in 14371 14372 14373 14374 14375 14376 14377 14378 14379 14380; do
  f="graph_batch${n}.c"
  o="/tmp/graph_batch${n}.o"
  if cc $CFLAGS -o "$o" "$f" 2>/tmp/precompile_${n}.err; then
    echo "PASS graph_batch${n}.c"
    pass=$((pass + 1))
    rm -f "$o"
  else
    echo "FAIL graph_batch${n}.c"
    cat /tmp/precompile_${n}.err
    fail=$((fail + 1))
  fi
done
echo "RESULT: ${pass} PASS, ${fail} FAIL"
exit "$fail"
