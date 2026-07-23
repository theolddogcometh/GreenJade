#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Build GreenJade.efi as PE32+ EFI_APPLICATION (subsystem 10).
# Uses PIC + shared ELF → objcopy pei-x86-64 (gnu-efi style, no mingw).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
mkdir -p build/kernel/boot

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

# Patch Characteristics: EXECUTABLE | LARGE_ADDRESS_AWARE | DLL
python3 - <<'PY'
import struct
path = "build/GreenJade.efi"
d = bytearray(open(path, "rb").read())
e = struct.unpack_from("<I", d, 0x3C)[0]
struct.pack_into("<H", d, e + 22, 0x2022)
open(path, "wb").write(d)
PY

echo "build-efi: build/GreenJade.efi (PIC shared → pei-x86-64)"
file build/GreenJade.efi
