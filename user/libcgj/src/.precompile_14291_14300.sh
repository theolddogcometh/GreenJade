#!/bin/bash
# Temporary precompile helper for continuum wave 14291-14300 (MILESTONE 14300).
# CREATE-ONLY; not a Makefile. Safe to delete after PASS.
set -uo pipefail
cd /home/jay/Documents/knano
CFLAGS=(-std=c11 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -msse2 -Wall -Wextra -Werror -Iuser/libcgj/include -g -O2 -fPIC)
OUT=/tmp/gj_precompile_14291_14300
mkdir -p "$OUT"
pass=0
fail=0
for n in 14291 14292 14293 14294 14295 14296 14297 14298 14299 14300; do
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
