#!/bin/sh
cd /home/jay/Documents/knano || exit 1
CFLAGS="-std=c11 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -msse2 -Wall -Wextra -Werror -Iuser/libcgj/include -g -O2 -fPIC"
OUT=/tmp/gj_precompile_14281_14290
mkdir -p "$OUT"
pass=0
fail=0
for n in 14281 14282 14283 14284 14285 14286 14287 14288 14289 14290; do
  if cc $CFLAGS -c -o "$OUT/graph_batch${n}.o" "user/libcgj/src/graph_batch${n}.c"; then
    echo "PASS graph_batch${n}.c"
    pass=$((pass+1))
  else
    echo "FAIL graph_batch${n}.c"
    fail=$((fail+1))
  fi
done
echo "RESULT: ${pass} PASS, ${fail} FAIL"
exit "$fail"
