#!/bin/bash
# Temporary precompile helper for continuum wave 14231-14240 (dbus soft).
# CREATE-ONLY; not a Makefile. Safe to delete after PASS.
set -uo pipefail
cd /home/jay/Documents/knano
CFLAGS=(-std=c11 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -msse2 -Wall -Wextra -Werror -Iuser/libcgj/include -g -O2 -fPIC)
OUT=/tmp/gj_precompile_14231_14240
mkdir -p "$OUT"
pass=0
fail=0
for n in 14231 14232 14233 14234 14235 14236 14237 14238 14239 14240; do
  src="user/libcgj/src/graph_batch${n}.c"
  obj="${OUT}/graph_batch${n}.o"
  if gcc "${CFLAGS[@]}" -c -o "$obj" "$src" 2>"${OUT}/graph_batch${n}.err"; then
    echo "PASS graph_batch${n}.c"
    rm -f "${OUT}/graph_batch${n}.err"
    pass=$((pass+1))
  else
    echo "FAIL graph_batch${n}.c"
    cat "${OUT}/graph_batch${n}.err" || true
    fail=$((fail+1))
  fi
done
echo "SUMMARY pass=${pass} fail=${fail}"
exit "$fail"
