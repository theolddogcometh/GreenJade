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

make -j"$(nproc)" build/greenjade.elf greenjade.efi userland libcgj >/dev/null

mkdir -p "$out/EFI/BOOT" "$out/EFI/GREENJADE" "$out/EFI/GREENJADE/user"
cp -f build/GreenJade.efi "$out/EFI/BOOT/BOOTX64.EFI"
cp -f build/greenjade.elf "$out/EFI/GREENJADE/KERNEL.ELF"

# Freestanding userspace ELFs for product tree (not yet auto-spawned from ESP)
for f in init.elf shell.elf sessiond.elf netstackd.elf sshd.elf storaged.elf vfsd.elf; do
    if [ -f "build/user/$f" ]; then
        cp -f "build/user/$f" "$out/EFI/GREENJADE/user/$f"
    fi
done
# Dynlinker + SO name stubs for product INTERP / DT_NEEDED path
if [ -f build/user/ld-gj.so.1 ]; then
    mkdir -p "$out/EFI/GREENJADE/lib"
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/lib/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/user/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/EFI/GREENJADE/lib/ld-linux-x86-64.so.2" 2>/dev/null || true
fi
mkdir -p "$out/EFI/GREENJADE/lib"
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
if [ -f build/user/vfsd.elf ]; then
    cp -f build/user/vfsd.elf "$out/EFI/GREENJADE/user/vfsd.elf"
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

Manual ESP copy:
1. Create a GPT disk with an EFI System Partition (type EF00), FAT32.
2. Copy this tree onto the ESP:
     EFI/BOOT/BOOTX64.EFI
     EFI/GREENJADE/KERNEL.ELF
     EFI/GREENJADE/user/*.elf   (init, shell, sessiond, netstackd, storaged)
3. Firmware boot: select "EFI Boot" / BOOTX64.EFI (or set BootOrder).
4. Expect serial (or early FB later): GJ-EFI, KERNEL.ELF loaded, M0 OK.
5. Storage/display/input/net must be enough for a Steam client install.
6. Then: install Steam (host userspace, not a SteamOS distro port) and
   run Deck Top 50 titles via Proton; fill matrix/deck-top50-*.md.

Bring-up status (smoke-all green):
  - Multiboot SMP + OVMF UEFI + stage-esp + install-img
  - PE32/CS32 int80 Wine surface (pipe/eventfd/epoll/timerfd/inotify/FS/execve)
  - Hybrid Linux x86_64 ABI (Option C) + init.elf + shell.elf
  - ELF dynlinker: INTERP-first, packaged ld-gj, multi-SO SysV/GNU hash+bloom
  - Fork COW (share/break/refcounts) + private leaf reclaim on as_destroy
  - ESP packages freestanding user ELFs + lib/ under EFI/GREENJADE/
  - Deck Top 50 rows remain NOT-TRIED until real-hw + Steam

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
echo "stage-esp: PASS efi=${sz_efi}B kernel=${sz_k}B path=$out"
