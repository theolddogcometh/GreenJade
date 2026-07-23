#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage a GreenJade ESP tree for UEFI install / real-hardware copy.
# Layout:
#   EFI/BOOT/BOOTX64.EFI       — GreenJade PE32+ loader
#   EFI/GREENJADE/KERNEL.ELF   — Multiboot/product kernel
#   EFI/GREENJADE/INSTALL.txt  — checklist for real-hw + Steam path
#
# Usage:
#   ./scripts/stage-esp.sh [out_dir]
# Default out_dir: build/esp
#
# After staging: copy the tree to a FAT32 ESP partition, or pack with
# mtools as run-ovmf.sh does. Product bar: installable on real hardware
# then install Steam and fill matrix/deck-top50-*.md.
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/esp}"

# Soft: prefer full rebuild; if tree is mid-wave broken, use existing artifacts.
if ! make -j"$(nproc)" build/greenjade.elf greenjade.efi userland libcgj >/dev/null 2>&1; then
    if [ -f build/greenjade.elf ] && [ -f build/GreenJade.efi ]; then
        echo "stage-esp: warn: make incomplete; packing existing build artifacts" >&2
    else
        echo "stage-esp: FAIL need build/greenjade.elf + build/GreenJade.efi" >&2
        exit 1
    fi
fi

mkdir -p "$out/EFI/BOOT" "$out/EFI/GREENJADE" "$out/EFI/GREENJADE/user"
cp -f build/GreenJade.efi "$out/EFI/BOOT/BOOTX64.EFI"
cp -f build/greenjade.elf "$out/EFI/GREENJADE/KERNEL.ELF"

# Freestanding userspace ELFs for product tree (ESP payload; kernel embeds
# still provide boot live-spawn for init/shell/sshd/scsi_mid/hda_client/…)
user_n=0
for f in init.elf shell.elf sessiond.elf netstackd.elf sshd.elf storaged.elf \
         vfsd.elf scsi_mid.elf hda_client.elf; do
    if [ -f "build/user/$f" ]; then
        cp -f "build/user/$f" "$out/EFI/GREENJADE/user/$f"
        user_n=$((user_n + 1))
    fi
done
# Dynlinker + SO names for product INTERP / DT_NEEDED path
mkdir -p "$out/EFI/GREENJADE/lib"
if [ -f build/user/ld-gj.so.1 ]; then
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/lib/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/user/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/lib/ld-linux-x86-64.so.2" 2>/dev/null || true
fi
if [ -f build/user/libgj-so.so.1 ]; then
    cp -f build/user/libgj-so.so.1 "$out/EFI/GREENJADE/lib/libgj-so.so.1"
else
    printf '%s\n' '# GreenJade SysV-hash SO placeholder' >"$out/EFI/GREENJADE/lib/libgj-so.so.1"
fi
if [ -f build/user/libgj-gnu.so.1 ]; then
    cp -f build/user/libgj-gnu.so.1 "$out/EFI/GREENJADE/lib/libgj-gnu.so.1"
else
    printf '%s\n' '# GreenJade GNU-hash SO placeholder' >"$out/EFI/GREENJADE/lib/libgj-gnu.so.1"
fi
if [ -f build/user/libc.so.6 ]; then
    cp -f build/user/libc.so.6 "$out/EFI/GREENJADE/lib/libc.so.6"
else
    printf '%s\n' '# GreenJade libc placeholder — make libcgj' >"$out/EFI/GREENJADE/lib/libc.so.6"
fi

# Also stage host rootfs tree when available (make stage-rootfs)
if [ ! -d build/rootfs/sbin ]; then
    chmod +x scripts/stage-rootfs.sh
    ./scripts/stage-rootfs.sh build/rootfs >/dev/null || true
fi
if [ -d build/rootfs ]; then
    mkdir -p "$out/EFI/GREENJADE/rootfs"
    # Copy small text stubs only (ELFs already under user/)
    cp -f build/rootfs/etc/os-release "$out/EFI/GREENJADE/rootfs/os-release" 2>/dev/null || true
    cp -f build/rootfs/etc/hostname "$out/EFI/GREENJADE/rootfs/hostname" 2>/dev/null || true
    cp -f build/rootfs/README.txt "$out/EFI/GREENJADE/rootfs/README.txt" 2>/dev/null || true
fi

cat >"$out/EFI/GREENJADE/VERSION" <<EOF
GreenJade product stage
kernel=$(wc -c <build/greenjade.elf | tr -d ' ')B
date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)
EOF

cat >"$out/EFI/GREENJADE/INSTALL.txt" <<'EOF'
GreenJade — real-hardware install path (bring-up)

Quick path (wipes target device):
  make install-img
  sudo ./scripts/install-usb.sh /dev/sdX

Hardware-test media (ESP + GJ-PERSIST logs/ssh/steam) — preferred on DUT:
  make hwtest-img
  sudo ./scripts/install-hwtest-usb.sh /dev/sdX

Manual ESP copy:
1. Create a GPT disk with an EFI System Partition (type EF00), FAT32.
2. Copy this tree onto the ESP:
     EFI/BOOT/BOOTX64.EFI
     EFI/GREENJADE/KERNEL.ELF
     EFI/GREENJADE/user/*.elf   (init, shell, sessiond, netstackd, sshd,
                                 storaged, vfsd, scsi_mid, hda_client)
     EFI/GREENJADE/lib/*        (ld-gj, libcgj libc.so.6, libgj-so/gnu)
3. Firmware boot: select "EFI Boot" / BOOTX64.EFI (or set BootOrder).
4. Expect serial: GJ-EFI, KERNEL.ELF loaded, M0 OK; soft product markers
   (sshd/scsi_mid/hda_client live spawn) when embeds run.
5. Soft-scan a serial capture (always exit 0):
     ./scripts/gj-product-summary.sh logs/serial-….txt
   Hard product keys (exit 1 on miss):
     ./scripts/gj-quick-keys.sh logs/serial-….txt
6. Storage/display/input/net must be enough for a Steam client install.
7. Then: prebuilt Steam on media (docs/STEAM_HWTEST.md); fill
   matrix/deck-top50-*.md only after real DUT client/title runs.
   Never claim Top-50 from media STATUS=READY alone.

Bring-up status (product surface — honest):
  - Multiboot SMP + OVMF UEFI + stage-esp + install-img / hwtest-img
  - PE32/CS32 int80 Wine surface + Hybrid Linux x86_64 ABI (Option C)
  - ELF dynlinker: INTERP-first, ld-gj, multi-SO SysV/GNU hash+bloom
  - Product embeds: sshd (default-on :22), scsi_mid, hda_client (kernel
    multi-stream PASS ≠ Steam audio)
  - ESP packages freestanding user ELFs + lib/ under EFI/GREENJADE/
  - Deck Top 50 rows remain NOT-TRIED until real-hw + Steam client run

Policy: pure C, MIT OR Apache-2.0, no GPL source, clean-room Linux ABI OK.
Priorities: Security → Performance → Portability → Readability.
EOF

# Manifest for smoke / operators
{
    echo "stage-esp: root=$out"
    echo "  EFI/BOOT/BOOTX64.EFI"
    ls -la "$out/EFI/BOOT/BOOTX64.EFI"
    echo "  EFI/GREENJADE/KERNEL.ELF"
    ls -la "$out/EFI/GREENJADE/KERNEL.ELF"
    echo "  EFI/GREENJADE/INSTALL.txt"
    echo "  EFI/GREENJADE/user/"
    ls -la "$out/EFI/GREENJADE/user/" 2>/dev/null || true
}

# Fingerprint sizes (install smoke gate string)
sz_efi=$(wc -c <"$out/EFI/BOOT/BOOTX64.EFI" | tr -d ' ')
sz_k=$(wc -c <"$out/EFI/GREENJADE/KERNEL.ELF" | tr -d ' ')
if [ "$sz_efi" -lt 1000 ] || [ "$sz_k" -lt 10000 ]; then
    echo "stage-esp: FAIL sizes efi=$sz_efi kernel=$sz_k" >&2
    exit 1
fi
if [ ! -f "$out/EFI/GREENJADE/user/init.elf" ]; then
    echo "stage-esp: FAIL missing user/init.elf" >&2
    exit 1
fi
# Soft inventory: product ELFs present vs missing (do not hard-fail optional)
soft_miss=""
for f in shell.elf sshd.elf scsi_mid.elf hda_client.elf sessiond.elf \
         netstackd.elf storaged.elf vfsd.elf; do
    if [ ! -f "$out/EFI/GREENJADE/user/$f" ]; then
        soft_miss="${soft_miss} $f"
    fi
done
# Soft: real libcgj ELF should be multi-KB (placeholder text is tiny)
sz_libc=$(wc -c <"$out/EFI/GREENJADE/lib/libc.so.6" 2>/dev/null | tr -d ' ' || echo 0)
libc_note="libc=${sz_libc}B"
if [ "${sz_libc:-0}" -lt 1000 ]; then
    libc_note="libc=${sz_libc}B(placeholder)"
fi
echo "stage-esp: PASS efi=${sz_efi}B kernel=${sz_k}B user_elfs=${user_n} ${libc_note} path=$out"
if [ -n "$soft_miss" ]; then
    echo "stage-esp: soft-miss user:${soft_miss}" >&2
fi
