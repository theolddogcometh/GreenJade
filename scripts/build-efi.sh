#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Build GreenJade.efi as PE32+ EFI_APPLICATION (subsystem 10).
# Uses PIC + shared ELF → objcopy pei-x86-64 (gnu-efi style, no mingw).
#
# Soft deepen (report-only; never softens a failed compile/link/objcopy):
#   soft_need_tool   hard-fail if a required host tool is missing
#   soft_pe_report   dump PE machine/subsystem/characteristics after build
#   soft_pe_check    verify Characteristics patch (EXECUTABLE|LAA|DLL)
#
# Hard gates (exit 1): missing gcc/ld/objcopy/python3, compile/link/objcopy
# failure, or PE characteristics not matching the product patch (0x2022).
#
# Product consumers: make greenjade.efi, scripts/run-ovmf.sh, stage-esp,
# scripts/smoke-all.sh (hard GJ-EFI on serial).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
mkdir -p build/kernel/boot

# soft_need_tool <name> — hard gate wrapped with a clear product message.
soft_need_tool() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "build-efi: missing tool: $1" >&2
        exit 1
    fi
}

soft_need_tool gcc
soft_need_tool ld
soft_need_tool objcopy
soft_need_tool python3
soft_need_tool file

CFLAGS_EFI="-std=c11 -ffreestanding -fno-stack-protector -fno-builtin \
  -fshort-wchar -mno-red-zone -fPIC -fno-asynchronous-unwind-tables \
  -mno-sse -mno-sse2 -mgeneral-regs-only \
  -Wall -Wextra -Ikernel/include -O2 -g0"

# shellcheck disable=SC2086
gcc $CFLAGS_EFI -c -o build/kernel/boot/uefi_stub.o kernel/boot/uefi_stub.c

# Shared ELF with dynamic relocations (UEFI relocates the PE)
ld -nostdlib -shared -Bsymbolic -T kernel/boot/efi_link.lds \
    -o build/kernel/boot/uefi_stub.so \
    build/kernel/boot/uefi_stub.o

objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
    -j .rel -j .rela -j .reloc -j .rodata -j .bss \
    -O pei-x86-64 --subsystem=10 \
    build/kernel/boot/uefi_stub.so build/GreenJade.efi

# Patch Characteristics: EXECUTABLE | LARGE_ADDRESS_AWARE | DLL (0x2022)
python3 - <<'PY'
import struct
import sys

path = "build/GreenJade.efi"
d = bytearray(open(path, "rb").read())
if len(d) < 0x40 or d[0:2] != b"MZ":
    print("build-efi: REJECT not MZ PE", file=sys.stderr)
    sys.exit(1)
e = struct.unpack_from("<I", d, 0x3C)[0]
if e + 24 > len(d) or d[e : e + 4] != b"PE\0\0":
    print("build-efi: REJECT missing PE signature", file=sys.stderr)
    sys.exit(1)
# COFF Characteristics at PE+22
struct.pack_into("<H", d, e + 22, 0x2022)
open(path, "wb").write(d)

# Soft PE report + hard characteristics check
machine = struct.unpack_from("<H", d, e + 4)[0]
nsects = struct.unpack_from("<H", d, e + 6)[0]
chars = struct.unpack_from("<H", d, e + 22)[0]
opt_magic = struct.unpack_from("<H", d, e + 24)[0]
# PE32+ optional header: subsystem at +68 from optional header start (e+24)
subsystem = struct.unpack_from("<H", d, e + 24 + 68)[0] if opt_magic == 0x20B else -1
size = len(d)
print(
    f"build-efi: soft PE report size={size} machine=0x{machine:04x} "
    f"sections={nsects} chars=0x{chars:04x} opt=0x{opt_magic:04x} "
    f"subsystem={subsystem}"
)
# Hard gates for product PE shape
if machine != 0x8664:
    print(f"build-efi: REJECT machine 0x{machine:04x} (want AMD64 0x8664)", file=sys.stderr)
    sys.exit(1)
if chars != 0x2022:
    print(f"build-efi: REJECT characteristics 0x{chars:04x} (want 0x2022)", file=sys.stderr)
    sys.exit(1)
if opt_magic != 0x20B:
    print(f"build-efi: REJECT optional magic 0x{opt_magic:04x} (want PE32+ 0x20B)", file=sys.stderr)
    sys.exit(1)
if subsystem != 10:
    print(f"build-efi: REJECT subsystem {subsystem} (want 10 EFI_APPLICATION)", file=sys.stderr)
    sys.exit(1)
print("build-efi: soft PE check PASS (AMD64 PE32+ EFI_APPLICATION chars=0x2022)")
PY

echo "build-efi: build/GreenJade.efi (PIC shared → pei-x86-64)"
file build/GreenJade.efi
# Soft size line for agents (never fails)
if [ -f build/GreenJade.efi ]; then
    _sz=$(wc -c <build/GreenJade.efi | tr -d ' ')
    echo "build-efi: soft size=${_sz} bytes"
fi
