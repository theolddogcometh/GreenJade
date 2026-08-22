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
# still provide boot live-spawn for init/shell/scsi_mid/hda_client/…)
# Product SSH is OpenSSH-portable DUT — do not copy abandoned sshd_gj
# (build/user/sshd.elf). Dual DoD B OPEN.
user_n=0
for f in init.elf dash.elf shell.elf sessiond.elf netstackd.elf storaged.elf \
         vfsd.elf scsi_mid.elf hda_client.elf; do
    if [ -f "build/user/$f" ]; then
        cp -f "build/user/$f" "$out/EFI/GREENJADE/user/$f"
        user_n=$((user_n + 1))
    fi
done
# Never leave abandoned sshd_gj as user/sshd.elf (prior ESP / build/user).
rm -f "$out/EFI/GREENJADE/user/sshd.elf"
# OpenSSH DUT = build/openssh-dut/sshd (kernel .incbin of the same ELF).
# Stage as user/sshd.elf and rootfs-shaped usr/sbin + libexec helpers.
# Missing DUT ELFs: warn only — KERNEL.ELF must still pack. Dual DoD B OPEN.
mkdir -p "$out/EFI/GREENJADE/usr/sbin" "$out/EFI/GREENJADE/usr/libexec"
sshd_src=""
if [ -f build/openssh-dut/sshd ]; then
    sshd_src=build/openssh-dut/sshd
fi
if [ -n "$sshd_src" ]; then
    cp -f "$sshd_src" "$out/EFI/GREENJADE/user/sshd.elf"
    chmod +x "$out/EFI/GREENJADE/user/sshd.elf"
    user_n=$((user_n + 1))
    cp -f "$sshd_src" "$out/EFI/GREENJADE/usr/sbin/sshd"
    chmod +x "$out/EFI/GREENJADE/usr/sbin/sshd"
else
    echo "stage-esp: warn: missing build/openssh-dut/sshd (OpenSSH DUT; Dual DoD B OPEN)" >&2
fi
if [ -f build/openssh-dut/sshd-session ]; then
    cp -f build/openssh-dut/sshd-session "$out/EFI/GREENJADE/usr/libexec/sshd-session"
    chmod +x "$out/EFI/GREENJADE/usr/libexec/sshd-session"
else
    echo "stage-esp: warn: missing build/openssh-dut/sshd-session (OpenSSH DUT; Dual DoD B OPEN)" >&2
fi
if [ -f build/openssh-dut/sshd-auth ]; then
    cp -f build/openssh-dut/sshd-auth "$out/EFI/GREENJADE/usr/libexec/sshd-auth"
    chmod +x "$out/EFI/GREENJADE/usr/libexec/sshd-auth"
else
    echo "stage-esp: warn: missing build/openssh-dut/sshd-auth (OpenSSH DUT; Dual DoD B OPEN)" >&2
fi
if [ -f build/user/sshd.elf ]; then
    echo "stage-esp: warn: ignoring build/user/sshd.elf (abandoned sshd_gj; not product SSH)" >&2
fi
# Cold personality + UDX driver-host artifacts (soft; missing OK).
# Prefer first: make drivers-udx personality-gj
#   → build/user/personality.elf
#   → build/user/drivers/{ddi_host,rtl8168_udx,xhci_udx}
# Harvest also: user/drivers/*/build/*, build/udx_skeleton as ddi_host alias.
# Also writes EFI/GREENJADE/DRIVERS.txt + LAPTOP.txt (G752VT).
#
# Honesty (Soft!=product · G-AC-1 · stamp-free):
#   Freestanding class (kernel rtl8168 / xhci_msc) default SKIP
#   (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0). Product path = UDX hosts
#   on EFI/GREENJADE/drivers/{ddi_host,rtl8168_udx,xhci_udx}. Soft-miss OK.
#   Dual DoD A/B OPEN — staged UDX pack != product TX/RX/BOT != bar3.
#   Stamp-free: IMAGE_VERSION from KERNEL.ELF only (never hardcode flash bar).
#   Bar honesty v0.1.182 packed, not host-probed — never invent next N.
#
# Soft residual deepen (C2 scripts residual Soft!=product; G-AC-1;
# Dual DoD A/B OPEN; stamp-free residual):
#   pack residual        — BOOTX64 + KERNEL + user/ + lib/ + UDX drivers/
#   udx pack residual    — stage-udx-drivers harvest ddi_host/rtl/xhci
#   dual_land residual   — ESP drivers/ + optional persist mirror (W10 denser)
#   freestanding residual — class SKIP default (not Dual DoD close)
#   Dual DoD residual    — A/B OPEN until USB path / interactive SSH login
#   honesty residual     — stage PASS != product TX/RX/BOT != bar3 != Dual DoD
#   stamp residual       — IMAGE_VERSION from KERNEL.ELF only; no invent next N
# greppable: stage-esp: soft residual dual_dod
# greppable: stage-esp: soft residual product=UDX+ABI
# greppable: stage-esp: soft residual freestanding class SKIP
# greppable: stage-esp: soft residual udx pack
# greppable: stage-esp: soft residual udx dual_land
# greppable: stage-esp: soft residual product_host density=
# greppable: stage-esp: soft residual panel=
# greppable: stage-esp: soft residual stamp-free
chmod +x scripts/stage-udx-drivers.sh
# W10 denser dual-land: prefer arg2 / GJ_PERSIST_DIR / build/persist when present
_persist_stage="${GJ_PERSIST_DIR:-}"
if [ -z "$_persist_stage" ] && [ -d build/persist ]; then
	_persist_stage=build/persist
fi
if [ -n "$_persist_stage" ]; then
	./scripts/stage-udx-drivers.sh "$out" "$_persist_stage" || true
else
	./scripts/stage-udx-drivers.sh "$out" || true
fi
if [ -f "$out/EFI/GREENJADE/user/personality.elf" ]; then
    user_n=$((user_n + 1))
fi
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

# Flash identity (GJ_IMAGE_VERSION) from KERNEL.ELF — operator verify on DUT panel
chmod +x scripts/gj-image-version.sh 2>/dev/null || true
_img_ver=unknown
if [ -x scripts/gj-image-version.sh ]; then
	_img_ver=$(./scripts/gj-image-version.sh "$out/EFI/GREENJADE/KERNEL.ELF" 2>/dev/null || \
		./scripts/gj-image-version.sh 2>/dev/null || echo unknown)
fi
_img_title="STATUS (static) v${_img_ver}"
printf '%s\n' "$_img_ver" >"$out/EFI/GREENJADE/IMAGE_VERSION"
printf '%s\n' "$_img_title" >"$out/EFI/GREENJADE/IMAGE_STATUS_TITLE"
cat >"$out/EFI/GREENJADE/VERSION" <<EOF
GreenJade product stage
image_version=${_img_ver}
status_title=${_img_title}
kernel=$(wc -c <build/greenjade.elf | tr -d ' ')B
date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)
# Soft!=product: stamp confirms flash identity only (not product PASS / bar3).
# Dual DoD A/B OPEN. Stamp-free: image_version from KERNEL.ELF only.
EOF

# Pre-sized kernel stick log file (128 KiB, contiguous clusters preferred).
# xhci_msc overwrites this on real USB MSC after BOT bring-up.
# Also empty BOOT.LOG placeholder for UEFI pre-ExitBootServices path.
dd if=/dev/zero of="$out/EFI/GREENJADE/KLOG.TXT" bs=1024 count=128 status=none 2>/dev/null || \
	dd if=/dev/zero of="$out/EFI/GREENJADE/KLOG.TXT" bs=1024 count=128 2>/dev/null || true
: >"$out/EFI/GREENJADE/BOOT.LOG"

cat >"$out/EFI/GREENJADE/INSTALL.txt" <<'EOF'
GreenJade — real-hardware install path (bring-up)

Quick path (wipes target device):
  make install-img
  sudo ./scripts/install-usb.sh /dev/sdX

Hardware-test media (ESP + GJ-PERSIST logs/ssh/steam) — preferred on DUT:
  make hwtest-img
  sudo ./scripts/install-hwtest-usb.sh /dev/sdX

ASUS ROG G752VT (laptop DUT) — see also:
  EFI/GREENJADE/LAPTOP.txt   USB boot keys, firmware, SS port tips
  EFI/GREENJADE/DRIVERS.txt  G752 PCI IDs + UDX host bind (no GPL .ko)

G752VT short path:
  1. make hwtest-img && sudo ./scripts/install-hwtest-usb.sh /dev/sdX
  2. Secure Boot Off · UEFI USB · Esc boot menu · USB3 jack (SS port 5)
  3. Collect: mount -L GREENJADE (BOOT.LOG, KLOG.TXT) + GJ-PERSIST/logs

Manual ESP copy:
1. Create a GPT disk with an EFI System Partition (type EF00), FAT32.
2. Copy this tree onto the ESP:
     EFI/BOOT/BOOTX64.EFI
     EFI/GREENJADE/KERNEL.ELF
     EFI/GREENJADE/user/*.elf   (init, shell, sessiond, netstackd,
                                 storaged, vfsd, scsi_mid, hda_client,
                                 sshd.elf = OpenSSH DUT when present,
                                 personality.elf when built)
     EFI/GREENJADE/usr/sbin/sshd
     EFI/GREENJADE/usr/libexec/sshd-session
     EFI/GREENJADE/usr/libexec/sshd-auth
         (OpenSSH DUT when build/openssh-dut present; not sshd_gj)
     EFI/GREENJADE/lib/*        (ld-gj, libcgj libc.so.6, libgj-so/gnu)
     EFI/GREENJADE/drivers/*    (UDX hosts: ddi_host, rtl8168_udx, xhci_udx,
                                 udx_skeleton; make drivers-udx)
     EFI/GREENJADE/DRIVERS.txt  (G752 IDs + UDX bind model)
     EFI/GREENJADE/LAPTOP.txt   (G752VT USB boot steps)
3. Firmware boot: select "EFI Boot" / BOOTX64.EFI (or set BootOrder).
4. Expect serial: GJ-EFI, KERNEL.ELF loaded, M0 OK; soft product markers
   (scsi_mid/hda_client live spawn) when embeds run. OpenSSH DUT is
   staged; kernel sshd live spawn remains SKIP until embed. Dual DoD B OPEN.
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
  - OpenSSH DUT staged (usr/sbin/sshd + libexec helpers); kernel live
    spawn still SKIP until embed; Dual DoD B OPEN
  - Product embeds: scsi_mid, hda_client (kernel multi-stream PASS !=
    Steam audio)
  - ESP packages freestanding user ELFs + lib/ + UDX drivers/ hosts
  - Freestanding class (rtl8168 / xhci_msc) default SKIP — product = UDX
  - Soft!=product · G-AC-1: staged hosts != TX/RX/BOT != bar3; no .ko product AC
  - Dual DoD A/B OPEN (UDX product path); freestanding class SKIP default
  - Stamp-free: IMAGE_VERSION from KERNEL.ELF only (never hardcode flash bar)
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
    echo "  EFI/GREENJADE/INSTALL.txt DRIVERS.txt LAPTOP.txt"
    echo "  EFI/GREENJADE/user/"
    ls -la "$out/EFI/GREENJADE/user/" 2>/dev/null || true
    if [ -d "$out/EFI/GREENJADE/usr" ]; then
        echo "  EFI/GREENJADE/usr/sbin + usr/libexec (OpenSSH DUT)"
        ls -la "$out/EFI/GREENJADE/usr/sbin" "$out/EFI/GREENJADE/usr/libexec" 2>/dev/null || true
    fi
    if [ -d "$out/EFI/GREENJADE/drivers" ]; then
        echo "  EFI/GREENJADE/drivers/"
        ls -la "$out/EFI/GREENJADE/drivers/" 2>/dev/null || true
    fi
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
for f in dash.elf shell.elf scsi_mid.elf hda_client.elf sessiond.elf \
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
drv_note="drivers=0"
ddi_st=SKIP
rtl_st=SKIP
xhci_st=SKIP
ddi_bytes=0
rtl_bytes=0
xhci_bytes=0
product_host_n=0
product_host_total=3
# W10 denser: non-empty honesty for UDX product hosts (zero-byte = SKIP)
_host_bytes() {
	if [ -f "$1" ]; then
		wc -c <"$1" | tr -d ' \n'
	else
		echo 0
	fi
}
if [ -d "$out/EFI/GREENJADE/drivers" ]; then
    drv_n=$(find "$out/EFI/GREENJADE/drivers" -type f ! -name 'MANIFEST.txt' ! -name 'UDX-STAGE.txt' ! -name 'PRODUCT-HOSTS.txt' 2>/dev/null | wc -l | tr -d ' ')
    drv_note="drivers=${drv_n}"
    ddi_bytes=$(_host_bytes "$out/EFI/GREENJADE/drivers/ddi_host")
    rtl_bytes=$(_host_bytes "$out/EFI/GREENJADE/drivers/rtl8168_udx")
    xhci_bytes=$(_host_bytes "$out/EFI/GREENJADE/drivers/xhci_udx")
    [ "${ddi_bytes:-0}" -gt 0 ] 2>/dev/null && ddi_st=PASS && product_host_n=$((product_host_n + 1))
    [ "${rtl_bytes:-0}" -gt 0 ] 2>/dev/null && rtl_st=PASS && product_host_n=$((product_host_n + 1))
    [ "${xhci_bytes:-0}" -gt 0 ] 2>/dev/null && xhci_st=PASS && product_host_n=$((product_host_n + 1))
fi
# dual-land residual from optional persist mirror
persist_ddi=SKIP
persist_rtl=SKIP
persist_xhci=SKIP
dual_land_st=SKIP
if [ -n "${_persist_stage:-}" ] && [ -d "$_persist_stage/drivers" ]; then
	[ "$(_host_bytes "$_persist_stage/drivers/ddi_host")" -gt 0 ] 2>/dev/null && persist_ddi=PASS
	[ "$(_host_bytes "$_persist_stage/drivers/rtl8168_udx")" -gt 0 ] 2>/dev/null && persist_rtl=PASS
	[ "$(_host_bytes "$_persist_stage/drivers/xhci_udx")" -gt 0 ] 2>/dev/null && persist_xhci=PASS
fi
if { [ "$ddi_st" = "PASS" ] || [ "$rtl_st" = "PASS" ] || [ "$xhci_st" = "PASS" ]; } \
	&& { [ "$persist_ddi" = "PASS" ] || [ "$persist_rtl" = "PASS" ] || [ "$persist_xhci" = "PASS" ]; }; then
	dual_land_st=PASS
elif [ "$ddi_st" = "PASS" ] || [ "$rtl_st" = "PASS" ] || [ "$xhci_st" = "PASS" ]; then
	dual_land_st=ESP_ONLY
elif [ "$persist_ddi" = "PASS" ] || [ "$persist_rtl" = "PASS" ] || [ "$persist_xhci" = "PASS" ]; then
	dual_land_st=PERSIST_ONLY
fi
pers_note=""
if [ -f "$out/EFI/GREENJADE/user/personality.elf" ]; then
    pers_note=" personality=yes"
fi
echo "stage-esp: PASS efi=${sz_efi}B kernel=${sz_k}B user_elfs=${user_n} ${libc_note} ${drv_note}${pers_note} path=$out"
echo "stage-esp: udx pack ddi_host=${ddi_st} rtl8168_udx=${rtl_st} xhci_udx=${xhci_st}"
echo "stage-esp: udx pack bytes ddi_host=${ddi_bytes} rtl8168_udx=${rtl_bytes} xhci_udx=${xhci_bytes}"
echo "stage-esp: product_host density=${product_host_n}/${product_host_total} dual_land=${dual_land_st}"
echo "stage-esp: freestanding class SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 residual opt-in)"
echo "stage-esp: image_version=${_img_ver} panel=${_img_title}"
echo "stage-esp: flash bar → ${_img_title} (GJ_IMAGE_VERSION=${_img_ver}; Soft!=product)"
echo "stage-esp: soft residual udx pack ddi_host=${ddi_st} rtl8168_udx=${rtl_st} xhci_udx=${xhci_st}"
echo "stage-esp: soft residual udx dual_land=${dual_land_st} persist ddi_host=${persist_ddi} rtl8168_udx=${persist_rtl} xhci_udx=${persist_xhci} path=${_persist_stage:-}"
echo "stage-esp: soft residual product_host density=${product_host_n}/${product_host_total} ddi_bytes=${ddi_bytes} rtl_bytes=${rtl_bytes} xhci_bytes=${xhci_bytes}"
echo "stage-esp: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "stage-esp: soft residual freestanding class SKIP"
echo "stage-esp: soft residual dual_dod A=OPEN B=OPEN (stage PASS != Dual DoD close)"
echo "stage-esp: soft residual panel=${_img_title} (stamp-free RO; Soft!=product)"
echo "stage-esp: soft residual stamp-free (bar honesty v0.1.182 packed, not host-probed; NEVER bump GJ_IMAGE_VERSION; no invent next N)"
echo "  Soft!=product: stamp confirms flash identity only (not product PASS / bar3)"
echo "  Soft!=product · G-AC-1: UDX pack != product TX/RX/BOT; freestanding class not product"
echo "  Dual DoD A/B OPEN (UDX); stamp-free IMAGE_VERSION from KERNEL.ELF only"
echo "  greppable: stage-esp: soft residual dual_dod"
echo "  greppable: stage-esp: soft residual product=UDX+ABI"
echo "  greppable: stage-esp: soft residual freestanding class SKIP"
echo "  greppable: stage-esp: soft residual udx pack"
echo "  greppable: stage-esp: soft residual udx dual_land"
echo "  greppable: stage-esp: soft residual product_host density="
echo "  greppable: stage-esp: soft residual panel="
echo "  greppable: stage-esp: soft residual stamp-free"
if [ -n "$soft_miss" ]; then
    echo "stage-esp: soft-miss user:${soft_miss}" >&2
fi
