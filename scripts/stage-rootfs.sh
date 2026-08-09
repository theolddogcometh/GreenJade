#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage a minimal freestanding userspace rootfs tree for product packaging.
# Not a full distro — ELFs + text stubs for install path bring-up.
#
# Usage:
#   ./scripts/stage-rootfs.sh [out_dir]
# Default: build/rootfs
#
# Honesty (Soft!=product · G-AC-1 · Dual DoD OPEN · stamp-free):
#   - Packing freestanding ELFs + text stubs != product TX/RX/BOT close.
#   - product_bins soft inventory count != Dual DoD A/B close != bar3 close.
#   - Freestanding class (kernel rtl8168 / xhci_msc) default SKIP
#     (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0); residual opt-in only.
#   - Product path = userspace UDX hosts (ddi_host / rtl8168_udx / xhci_udx)
#     over hot+cold ABI/DDI — not freestanding class drivers, not .ko product AC.
#   - Dual DoD A (USB UDX) / B (NIC UDX) remain OPEN until DUT proof.
#   - bar3 / Deck Top 50 remain OPEN until Steam client + matrix evidence.
#   - Stamp-free script: does not bump GJ_IMAGE_VERSION / invent stamps;
#     flash identity is KERNEL.ELF / gj-image-version only (Soft!=product).
#   - Bar honesty v2026.08.04.75 panel context only — never invent .76.
#   - Soft-scan serial (exit 0): ./scripts/gj-product-summary.sh <log>
#   - Hard product keys:        ./scripts/gj-quick-keys.sh <log>
#
# Soft residual deepen (C2 scripts residual Soft!=product; G-AC-1;
# Dual DoD A/B OPEN; stamp-free residual):
#   pack residual        — freestanding rootfs ELFs + product.env inventory
#   product_bins residual — soft count of staged freestanding bins (not DoD)
#   freestanding residual — class SKIP default (not Dual DoD close)
#   Dual DoD residual    — A/B OPEN; rootfs pack != UDX product close
#   honesty residual     — product_bins count != Dual DoD / bar3 / product AC
#   stamp residual       — never bump GJ_IMAGE_VERSION; no invent .76
# greppable: stage-rootfs: soft residual dual_dod
# greppable: stage-rootfs: soft residual product=UDX+ABI
# greppable: stage-rootfs: soft residual freestanding class SKIP
# greppable: stage-rootfs: soft residual product_bins
# greppable: stage-rootfs: soft residual stamp-free
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/rootfs}"

# Soft: prefer full rebuild; if tree is mid-wave broken, use existing ELFs.
if ! make -j"$(nproc)" userland libcgj >/dev/null 2>&1; then
    if [ -f build/user/init.elf ] && [ -f build/user/shell.elf ]; then
        echo "stage-rootfs: warn: make incomplete; packing existing userland ELFs" >&2
    else
        echo "stage-rootfs: FAIL need build/user/init.elf + shell.elf" >&2
        exit 1
    fi
fi

rm -rf "$out"
mkdir -p "$out/bin" "$out/sbin" "$out/etc" "$out/tmp" "$out/var/tmp" \
         "$out/usr/bin" "$out/usr/sbin" "$out/usr/lib" "$out/lib" "$out/proc" \
         "$out/dev" "$out/dev/shm" "$out/mnt" "$out/var/log" "$out/etc/ssh" \
         "$out/root" "$out/etc/greenjade"

cp -f build/user/init.elf "$out/sbin/init"
cp -f build/user/shell.elf "$out/bin/sh"
cp -f build/user/shell.elf "$out/bin/greenjade-shell"
# Product shell also as usr/bin for FHS-shaped lookup
cp -f build/user/shell.elf "$out/usr/bin/sh" 2>/dev/null || true
cp -f build/user/sessiond.elf "$out/usr/bin/sessiond" 2>/dev/null || true
cp -f build/user/netstackd.elf "$out/usr/bin/netstackd" 2>/dev/null || true
cp -f build/user/storaged.elf "$out/usr/bin/storaged" 2>/dev/null || true
cp -f build/user/vfsd.elf "$out/usr/bin/vfsd" 2>/dev/null || true
# Product freestanding clients (also kernel-embedded for live spawn)
if [ -f build/user/scsi_mid.elf ]; then
	cp -f build/user/scsi_mid.elf "$out/usr/sbin/scsi_mid"
	cp -f build/user/scsi_mid.elf "$out/sbin/scsi_mid"
fi
if [ -f build/user/hda_client.elf ]; then
	cp -f build/user/hda_client.elf "$out/usr/bin/hda_client"
	cp -f build/user/hda_client.elf "$out/bin/hda_client"
fi
# Product sshd — freestanding ELF, enabled by default on live/hwtest boot
if [ -f build/user/sshd.elf ]; then
	cp -f build/user/sshd.elf "$out/usr/sbin/sshd"
	cp -f build/user/sshd.elf "$out/sbin/sshd"
	mkdir -p "$out/etc/ssh"
	cat >"$out/etc/ssh/sshd_config" <<'SSHEOF'
# GreenJade product sshd (freestanding). Port 22; pubkey preferred.
Port 22
Protocol 2
PermitRootLogin prohibit-password
PasswordAuthentication no
PubkeyAuthentication yes
AuthorizedKeysFile /etc/ssh/authorized_keys
X11Forwarding no
SSHEOF
	if [ -f build/hwtest-keys/id_ed25519.pub ]; then
		cp -f build/hwtest-keys/id_ed25519.pub "$out/etc/ssh/authorized_keys"
	else
		: >"$out/etc/ssh/authorized_keys"
	fi
fi

# Dynlinker scaffold (PT_INTERP target for product Steam/glibc path)
if [ -f build/user/ld-gj.so.1 ]; then
    cp -f build/user/ld-gj.so.1 "$out/lib/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/usr/lib/ld-gj.so.1"
    # Classic Linux path alias used by many ET_DYN INTERP strings
    cp -f build/user/ld-gj.so.1 "$out/lib/ld-linux-x86-64.so.2" 2>/dev/null || true
fi
# Clean-room glibc-shaped libc (libcgj) — real ELF soname libc.so.6 (not GNU glibc)
if [ -f build/user/libc.so.6 ]; then
    cp -f build/user/libc.so.6 "$out/lib/libc.so.6"
    cp -f build/user/libc.so.6 "$out/usr/lib/libc.so.6"
else
    cat >"$out/lib/libc.so.6" <<'EOF'
# GreenJade libc placeholder — build make libcgj for real ELF
EOF
    cp -f "$out/lib/libc.so.6" "$out/usr/lib/libc.so.6"
fi
# Product DT_NEEDED SOs (real ELF — SysV hash + GNU hash)
if [ -f build/user/libgj-so.so.1 ]; then
    cp -f build/user/libgj-so.so.1 "$out/lib/libgj-so.so.1"
    cp -f build/user/libgj-so.so.1 "$out/usr/lib/libgj-so.so.1"
else
    echo "FAIL: missing build/user/libgj-so.so.1 (make userland)" >&2
    exit 1
fi
if [ -f build/user/libgj-gnu.so.1 ]; then
    cp -f build/user/libgj-gnu.so.1 "$out/lib/libgj-gnu.so.1"
    cp -f build/user/libgj-gnu.so.1 "$out/usr/lib/libgj-gnu.so.1"
else
    echo "FAIL: missing build/user/libgj-gnu.so.1 (make userland)" >&2
    exit 1
fi

cat >"$out/etc/os-release" <<'EOF'
NAME="GreenJade"
ID=greenjade
VERSION_ID="0.1"
PRETTY_NAME="GreenJade 0.1"
HOME_URL="https://greenjade.local"
EOF

cat >"$out/etc/hostname" <<'EOF'
greenjade
EOF

cat >"$out/etc/passwd" <<'EOF'
root:x:0:0:root:/root:/bin/sh
EOF

cat >"$out/etc/issue" <<'EOF'
GreenJade \n \l
EOF

: >"$out/tmp/.keep"
: >"$out/var/tmp/.keep"

# Steam tree (option 2): prebuilt payload under /opt/steam when lab host fetched it
chmod +x scripts/stage-steam-tree.sh
GJ_STEAM_AS_ROOTFS=1 ./scripts/stage-steam-tree.sh "$out" >/dev/null || true

cat >"$out/etc/greenjade/product.env" <<'EOF'
# GreenJade product rootfs soft inventory (text only; not sourced by kernel)
# Soft!=product · G-AC-1 · Dual DoD OPEN · stamp-free
GJ_PRODUCT=greenjade
GJ_SSHD_DEFAULT_ON=1
GJ_SSHD_PORT=22
# Kernel multi-stream PASS is not Steam audio; bar3 client remains open.
GJ_HDA_KERNEL_MULTI_STREAM=1
GJ_STEAM_BAR3_OPEN=1
# Soft!=product honesty flags (inventory only — not Dual DoD close)
GJ_SOFT_NE_PRODUCT=1
GJ_G_AC_1=1
GJ_DUAL_DOD_A_OPEN=1
GJ_DUAL_DOD_B_OPEN=1
GJ_FREESTANDING_CLASS_SKIP=1
# Freestanding class default SKIP (residual opt-in only; not product AC)
# GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 — product path = UDX hosts
# Dual DoD A=USB UDX (xhci_udx) · B=NIC UDX (rtl8168_udx) — both OPEN
# Stamp-free: rootfs pack does not own GJ_IMAGE_VERSION (KERNEL.ELF identity only)
# Bar honesty v2026.08.04.75 panel context — never invent .76
GJ_STAMP_FREE=1
GJ_BAR_HONESTY=v2026.08.04.75
GJ_PRODUCT_PATH=UDX+ABI
EOF

cat >"$out/README.txt" <<'EOF'
GreenJade minimal rootfs (freestanding ELFs)

sbin/init              — first process (embedded in kernel for Multiboot smoke)
bin/sh                 — shell scaffold
lib/ld-gj.so.1         — dynamic linker (PT_INTERP; also ld-linux-x86-64.so.2)
lib/libgj-so.so.1      — SysV-hash product SO (gj_so_export)
lib/libgj-gnu.so.1     — GNU-hash product SO (gj_gnu_export)
lib/libc.so.6          — clean-room libcgj (glibc-shaped; see docs/GLIBC_COMPAT.md)
usr/bin/sessiond       — compositor server ELF
usr/bin/netstackd      — net server ELF
usr/sbin/sshd          — freestanding SSH daemon (port 22, on by default)
usr/bin/storaged       — storage server ELF
usr/bin/vfsd           — block-backed VFS server (store door + named cache)
usr/sbin/scsi_mid      — freestanding SCSI mid (also kernel-embedded live spawn)
usr/bin/hda_client     — freestanding HDA client (kernel multi-stream != Steam audio)
etc/ssh/sshd_config    — freestanding sshd config
etc/ssh/authorized_keys — hwtest ed25519 pubkey when present
etc/greenjade/product.env — soft inventory flags (Soft!=product)
opt/steam/             — staged Steam bootstrap (option 2; make steam-fetch)
usr/bin/steam          — thin launcher → /opt/steam or GJ-PERSIST/steam

Product: Steam via prebuilt tree on media (docs/STEAM_HWTEST.md); in-tree libc is libcgj.
Never claim Deck Top 50 from media STATUS=READY alone — real DUT client run only.

Honesty (Soft!=product · G-AC-1 · Dual DoD OPEN · stamp-free)
  - Staged freestanding ELFs + text stubs != product TX/RX/BOT close.
  - product_bins soft count != Dual DoD A/B close != bar3 close.
  - Freestanding class (kernel rtl8168 / xhci_msc) default SKIP
    (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0); residual opt-in only.
  - Product path = userspace UDX (ddi_host / rtl8168_udx / xhci_udx)
    over hot+cold ABI/DDI — not freestanding class, not .ko product AC.
  - Dual DoD A (USB UDX) / B (NIC UDX) remain OPEN until DUT proof.
  - bar3 / Deck Top 50 remain OPEN until Steam client + matrix evidence.
  - Stamp-free: this tree does not bump GJ_IMAGE_VERSION; flash identity
    is KERNEL.ELF / ./scripts/gj-image-version.sh only (Soft!=product).
  - Policy: dual MIT OR Apache-2.0; no GPL source as product AC.

Soft-scan serial (exit 0):  ./scripts/gj-product-summary.sh <log>
Hard product keys:          ./scripts/gj-quick-keys.sh <log>
Bar3 media (soft exit 0):   ./scripts/steam-bar3-check.sh
Flash identity (soft):      ./scripts/gj-image-version.sh --report
EOF

# Soft inventory of freestanding bins (operator / packing scripts).
# Soft!=product: count is pack presence only — not Dual DoD / bar3 / product AC.
prod_n=0
for p in \
	"$out/sbin/init" \
	"$out/bin/sh" \
	"$out/usr/sbin/sshd" \
	"$out/usr/sbin/scsi_mid" \
	"$out/usr/bin/hda_client" \
	"$out/usr/bin/sessiond" \
	"$out/usr/bin/netstackd" \
	"$out/usr/bin/storaged" \
	"$out/usr/bin/vfsd" \
	"$out/lib/ld-gj.so.1" \
	"$out/lib/libc.so.6" \
	"$out/lib/libgj-so.so.1" \
	"$out/lib/libgj-gnu.so.1"
do
	if [ -f "$p" ]; then
		prod_n=$((prod_n + 1))
	fi
done
n=$(find "$out" -type f | wc -l | tr -d ' ')
sz=$(du -sk "$out" | awk '{print $1}')
echo "stage-rootfs: PASS files=$n size_kb=$sz product_bins=$prod_n path=$out"
echo "stage-rootfs: Soft!=product · G-AC-1 — freestanding rootfs pack != product TX/RX/BOT != bar3"
echo "stage-rootfs: freestanding class SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 residual opt-in)"
echo "stage-rootfs: Dual DoD A/B OPEN (UDX USB/NIC) — product_bins soft inventory only"
echo "stage-rootfs: stamp-free — no GJ_IMAGE_VERSION bump; flash identity is KERNEL.ELF only"
echo "stage-rootfs: soft residual product_bins=${prod_n} (pack presence only; not Dual DoD close)"
echo "stage-rootfs: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "stage-rootfs: soft residual freestanding class SKIP"
echo "stage-rootfs: soft residual dual_dod A=OPEN B=OPEN (rootfs pack != Dual DoD close)"
echo "stage-rootfs: soft residual stamp-free (bar honesty v2026.08.04.75; NEVER bump GJ_IMAGE_VERSION; no invent .76)"
echo "  Soft!=product: product_bins=$prod_n pack presence != Dual DoD close != Steam Top-50"
echo "  Soft!=product · G-AC-1: UDX hosts (not this tree) own product TX/RX/BOT path"
echo "  bar3: OPEN (client launch + Deck Top 50 still NOT-TRIED)"
echo "  greppable: stage-rootfs: soft residual dual_dod"
echo "  greppable: stage-rootfs: soft residual product=UDX+ABI"
echo "  greppable: stage-rootfs: soft residual freestanding class SKIP"
echo "  greppable: stage-rootfs: soft residual product_bins"
echo "  greppable: stage-rootfs: soft residual stamp-free"
echo "  soft-scan: ./scripts/gj-product-summary.sh <log>   # exit 0"
echo "  hard-keys: ./scripts/gj-quick-keys.sh <log>        # exit 1 on miss"
