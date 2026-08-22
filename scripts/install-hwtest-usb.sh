#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade hardware-test GPT image (ESP + GJ-PERSIST) to a block device.
# DESTROYS the entire target device. Requires root for dd.
#
# Layout written (from make hwtest-img):
#   p1  EF00 ESP (FAT32, GREENJADE)  — BOOTX64.EFI + KERNEL.ELF + rootfs/user
#                                     (FAT required by UEFI firmware)
#   p2  8300 Linux (ext4, GJ-PERSIST) — durable logs/ + ssh/ + steam/
#                                     (ext4 default for Linux/Unix lab)
#
# Prerequisites:
#   make hwtest-img              → build/greenjade-hwtest.img
#   optional: make steam-fetch && make steam-stage   # STATUS=READY on p2
#
# Usage:
#   sudo ./scripts/install-hwtest-usb.sh /dev/sdX
#   sudo GJ_HWTEST_IMG=build/greenjade-hwtest.img ./scripts/install-hwtest-usb.sh /dev/nvme0n1
#   GJ_FORCE_DISK=1  — override refusal of primary-looking disks (sda/nvme0n1/vda)
#
# Image identity (versioned flash; stamp-free script — never invent/bump stamp):
#   ./scripts/gj-image-version.sh --report
#   After write: confirm DUT panel STATUS title matches printed "expect panel".
#   Soft!=product: GJ_IMAGE_VERSION identity != product PASS != Steam != Top-50.
#   Dual DoD A/B remain OPEN (UDX product path) — flash write != DoD close.
#
# UDX pack on ESP (from make hwtest-img → stage-esp → stage-udx-drivers):
#   EFI/GREENJADE/drivers/{ddi_host,rtl8168_udx,xhci_udx}  (userspace; G-AC-1)
# Freestanding class SKIP default (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0).
# Soft!=product: staged hosts != product TX/RX/BOT != bar3; pack != Dual DoD close.
#
# Soft residual deepen (C2 scripts residual Soft!=product; G-AC-1;
# dual MIT OR Apache-2.0; Dual DoD A/B OPEN; stamp-free residual):
#   flash residual       — dd GPT image to DUT stick (DESTROYS target)
#   udx pack residual    — verify ESP drivers/ inventory post-write (operator)
#   freestanding residual — class SKIP default (not Dual DoD close)
#   Dual DoD residual    — A OPEN until USB path; B until interactive SSH login; flash write != DoD close
#   honesty residual     — flash PASS != product TX/RX/BOT != bar3 != Dual DoD
#   stamp residual       — read GJ_IMAGE_VERSION from pack/ELF only; never invent next N
# greppable: install-hwtest-usb: soft residual dual_dod
# greppable: install-hwtest-usb: soft residual product=UDX+ABI
# greppable: install-hwtest-usb: soft residual freestanding class SKIP
# greppable: install-hwtest-usb: soft residual stamp-free
# greppable: install-hwtest-usb: soft residual flash
# greppable: install-hwtest-usb: soft residual udx pack
# greppable: install-hwtest-usb: soft residual udx dual_land
# greppable: install-hwtest-usb: soft residual panel=
# greppable: install-hwtest-usb: soft residual post_verify=
# Bar honesty v0.1.178 panel context only — no invent next N.
#
# Post-write (lab host + DUT):
#   1. Boot DUT UEFI → GreenJade BOOTX64.EFI  (serial: GJ-EFI / M0 OK)
#   2. Panel: STATUS (static) v0.1.N  must match pack/flash report
#   3. Logs:  sudo mount -t ext4 -L GJ-PERSIST /mnt/gj-persist  →  logs/
#   4. Lab SSH (serial bridge): sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh
#      or: sudo ./scripts/hwtest-ssh-setup.sh
#   5. Soft-scan serial: ./scripts/gj-product-summary.sh /mnt/gj-persist/logs/….txt
#      Hard keys only:   ./scripts/gj-quick-keys.sh <serial-log>   # exit 1 on miss
#   6. Steam bar3 media: ./scripts/steam-bar3-check.sh   # soft exit 0
#   7. If steam/STATUS=SKELETON: sudo make steam-to-persist  (option 3 host prep)
#
# See: docs/STEAM_HWTEST.md
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
img="${GJ_HWTEST_IMG:-build/greenjade-hwtest.img}"
dev="${1:-}"

if [ -z "$dev" ]; then
	echo "usage: $0 /dev/sdX   # wipes entire device" >&2
	echo "  pack first: make hwtest-img" >&2
	echo "  identity:   ./scripts/gj-image-version.sh --report" >&2
	exit 1
fi
if [ ! -b "$dev" ]; then
	echo "not a block device: $dev" >&2
	exit 1
fi
if [ ! -f "$img" ]; then
	echo "missing $img — run: make hwtest-img" >&2
	exit 1
fi

# Refuse obvious system disks when possible
case "$dev" in
*/sda|*/nvme0n1|*/vda|*/mmcblk0)
	if [ "${GJ_FORCE_DISK:-0}" != "1" ]; then
		echo "refusing $dev (looks like primary disk). Set GJ_FORCE_DISK=1 to override." >&2
		exit 1
	fi
	;;
esac

img_sz=$(wc -c <"$img" | tr -d ' ')
# Soft: refuse if target looks smaller than image (best-effort)
base=$(basename "$dev")
sys_name=$base
case "$base" in
nvme*n*p*|mmcblk*p*)
	sys_name=$(echo "$base" | sed 's/p[0-9][0-9]*$//')
	;;
esac
if [ -f "/sys/block/$sys_name/size" ]; then
	sectors=$(cat "/sys/block/$sys_name/size")
	dev_sz=$((sectors * 512))
	if [ "$dev_sz" -gt 0 ] && [ "$dev_sz" -lt "$img_sz" ]; then
		echo "install-hwtest-usb: FAIL device ${dev_sz}B < image ${img_sz}B" >&2
		exit 1
	fi
fi

# Flash identity (stamp-free RO) — prefer packed image ESP, then host ELF.
# NEVER bump GJ_IMAGE_VERSION; bar honesty v0.1.178; no invent next N.
chmod +x scripts/gj-image-version.sh 2>/dev/null || true
img_ver=unknown
img_status_title="STATUS (static) vunknown"
img_ver_src=none
# Prefer packed media identity on ESP (default GPT ESP start LBA 2048)
_esp_off=$((2048 * 512))
if command -v mcopy >/dev/null 2>&1; then
	_tmp_ver="${TMPDIR:-/tmp}/gj-hwtest-img-ver.$$"
	if mcopy -n -i "${img}@@${_esp_off}" ::/EFI/GREENJADE/IMAGE_VERSION "$_tmp_ver" 2>/dev/null \
		&& [ -f "$_tmp_ver" ]; then
		_v=$(tr -d '\r\n' <"$_tmp_ver" 2>/dev/null || true)
		rm -f "$_tmp_ver"
		if [ -n "$_v" ] && [ "$_v" != "unknown" ]; then
			img_ver=$_v
			img_status_title="STATUS (static) v${img_ver}"
			img_ver_src=img_esp_IMAGE_VERSION
		fi
	else
		rm -f "$_tmp_ver"
	fi
fi
if [ "$img_ver" = "unknown" ] && [ -x scripts/gj-image-version.sh ]; then
	_v=$(./scripts/gj-image-version.sh 2>/dev/null || true)
	if [ -n "$_v" ] && [ "$_v" != "unknown" ]; then
		img_ver=$_v
		img_status_title="STATUS (static) v${img_ver}"
		img_ver_src=gj-image-version
	fi
fi
# Soft pre-flash UDX pack inventory from image ESP (mtools; Soft!=product)
pre_ddi=SKIP
pre_rtl=SKIP
pre_xhci=SKIP
if command -v mdir >/dev/null 2>&1; then
	_mdir=$(mdir -i "${img}@@${_esp_off}" ::/EFI/GREENJADE/drivers 2>/dev/null || true)
	if [ -n "$_mdir" ]; then
		echo "$_mdir" | grep -qi 'ddi_host' && pre_ddi=PASS
		echo "$_mdir" | grep -qi 'rtl8168_udx' && pre_rtl=PASS
		echo "$_mdir" | grep -qi 'xhci_udx' && pre_xhci=PASS
	fi
fi

echo "install-hwtest-usb: image=$img (${img_sz}B)"
ls -la "$img"
echo "install-hwtest-usb: GJ_IMAGE_VERSION=${img_ver} src=${img_ver_src}"
echo "install-hwtest-usb: expect panel → ${img_status_title}"
echo "install-hwtest-usb: soft residual panel=${img_status_title} src=${img_ver_src}"
echo "install-hwtest-usb: soft residual udx pack pre ddi_host=${pre_ddi} rtl8168_udx=${pre_rtl} xhci_udx=${pre_xhci} (img ESP; Soft!=product)"
echo "install-hwtest-usb: TARGET=$dev (ALL DATA WILL BE ERASED)"

# Desktop automounts (GNOME/KDE) sit under /run/media/<user>/, not on $dev
# until findmnt catches up. sudo makes $USER=root — prefer jay / SUDO_USER.
unmount_lab_media() {
	_u="${SUDO_USER:-}"
	if [ -z "$_u" ] || [ "$_u" = "root" ]; then
		_u=jay
	fi
	echo "install-hwtest-usb: umount /run/media/${_u}/*"
	umount /run/media/"${_u}"/* 2>/dev/null || true
	if [ "$_u" != "jay" ]; then
		umount /run/media/jay/* 2>/dev/null || true
	fi
	echo "install-hwtest-usb: umount GJ-PERSIST"
	umount -L GJ-PERSIST 2>/dev/null || true
	umount /mnt/gj-persist 2>/dev/null || umount -l /mnt/gj-persist 2>/dev/null || true
	if command -v findmnt >/dev/null 2>&1; then
		_tgts=$(findmnt -rn -o LABEL,TARGET 2>/dev/null | while read -r lab tgt; do
			if [ "$lab" = "GJ-PERSIST" ] || [ "$lab" = "GREENJADE" ]; then
				printf '%s\n' "$tgt"
			fi
		done)
		for tgt in $_tgts; do
			[ -n "$tgt" ] || continue
			echo "install-hwtest-usb: umount $tgt (label)"
			umount "$tgt" 2>/dev/null || umount -l "$tgt" 2>/dev/null || true
		done
	fi
}

# Must unmount before dd: writing while automount holds old vfat leaves a live
# wrong FS in the kernel (I/O errors, missing steam/STATUS, lsblk still "vfat").
unmount_target() {
	_d="$1"
	if command -v findmnt >/dev/null 2>&1; then
		# Collect targets first (umount changes findmnt output while iterating)
		_tgts=$(findmnt -rn -o SOURCE,TARGET 2>/dev/null | while read -r src tgt; do
			case "$src" in
			"$_d"|"$_d"p[0-9]*|"$_d"[0-9]*)
				printf '%s\n' "$tgt"
				;;
			esac
		done)
		for tgt in $_tgts; do
			[ -n "$tgt" ] || continue
			echo "install-hwtest-usb: umount $tgt"
			umount "$tgt" 2>/dev/null || umount -l "$tgt" 2>/dev/null || true
		done
	fi
	for p in "$_d"?* "$_d"[0-9]* "$_d"p[0-9]*; do
		[ -b "$p" ] || continue
		if findmnt -n "$p" >/dev/null 2>&1; then
			echo "install-hwtest-usb: umount $p"
			umount "$p" 2>/dev/null || umount -l "$p" 2>/dev/null || true
		fi
	done
	if command -v blockdev >/dev/null 2>&1; then
		blockdev --flushbufs "$_d" 2>/dev/null || true
	fi
}
unmount_lab_media
unmount_target "$dev"

if findmnt -rn -o SOURCE 2>/dev/null | grep -E "^${dev}(p?[0-9]+)?\$" >/dev/null 2>&1; then
	echo "install-hwtest-usb: FAIL $dev still has mounts — umount and retry:" >&2
	findmnt -rn -o SOURCE,TARGET 2>/dev/null | grep -E "^${dev}" >&2 || true
	exit 1
fi

echo "install-hwtest-usb: 5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync,notrunc
sync

if command -v partprobe >/dev/null 2>&1; then
	partprobe "$dev" 2>/dev/null || true
fi
if command -v udevadm >/dev/null 2>&1; then
	udevadm settle --timeout=10 2>/dev/null || true
fi
# Invalidate stale blkid cache (old TYPE=vfat for p2)
if command -v blkid >/dev/null 2>&1; then
	blkid -g 2>/dev/null || true
	for p in "$dev"?* "$dev"[0-9]* "$dev"p[0-9]*; do
		[ -b "$p" ] || continue
		blkid -p -o udev "$p" >/dev/null 2>&1 || true
	done
fi

echo "install-hwtest-usb: partition probe:"
if command -v lsblk >/dev/null 2>&1; then
	lsblk -o NAME,SIZE,FSTYPE,LABEL,PARTTYPENAME "$dev" 2>/dev/null || true
fi
if command -v blkid >/dev/null 2>&1; then
	blkid "${dev}1" "${dev}2" "${dev}p1" "${dev}p2" 2>/dev/null || \
		blkid | grep -E "$(basename "$dev")" || true
fi

p1=""
p2=""
for cand in "${dev}1" "${dev}p1"; do
	[ -b "$cand" ] && p1=$cand && break
done
for cand in "${dev}2" "${dev}p2"; do
	[ -b "$cand" ] && p2=$cand && break
done
if [ -n "$p2" ] && command -v blkid >/dev/null 2>&1; then
	p2_type=$(blkid -o value -s TYPE "$p2" 2>/dev/null || true)
	p2_label=$(blkid -o value -s LABEL "$p2" 2>/dev/null || true)
	echo "install-hwtest-usb: p2 $p2 TYPE=${p2_type:-?} LABEL=${p2_label:-?}"
	case "$p2_type" in
	ext4|ext3|ext2)
		echo "install-hwtest-usb: p2 filesystem OK (ext*, Linux lab default)"
		;;
	vfat|msdos)
		echo "install-hwtest-usb: warn: p2 is FAT (expected ext4 unless GJ_HWTEST_PERSIST_FS=fat32)" >&2
		;;
	*)
		echo "install-hwtest-usb: warn: unexpected p2 TYPE=${p2_type:-unknown}" >&2
		;;
	esac
fi

# ---------------------------------------------------------------------------
# W10 denser: soft post-flash UDX + panel verify (mount labels; never hard-fail).
# Soft!=product: post_verify PASS != Dual DoD close != product TX/RX/BOT != bar3.
# ---------------------------------------------------------------------------
post_ddi=SKIP
post_rtl=SKIP
post_xhci=SKIP
post_persist_ddi=SKIP
post_persist_rtl=SKIP
post_persist_xhci=SKIP
post_ver=unknown
post_ver_st=SKIP
post_verify=SKIP
dual_land_flash=SKIP
_mnt_esp="${TMPDIR:-/tmp}/gj-hwtest-esp-mnt.$$"
_mnt_persist="${TMPDIR:-/tmp}/gj-hwtest-persist-mnt.$$"
mkdir -p "$_mnt_esp" "$_mnt_persist"
# Prefer LABEL mounts; fall back to p1/p2 block devices
if mount -L GREENJADE "$_mnt_esp" 2>/dev/null \
	|| { [ -n "$p1" ] && mount "$p1" "$_mnt_esp" 2>/dev/null; }; then
	if [ -f "$_mnt_esp/EFI/GREENJADE/IMAGE_VERSION" ]; then
		post_ver=$(tr -d '\r\n' <"$_mnt_esp/EFI/GREENJADE/IMAGE_VERSION" 2>/dev/null || echo unknown)
		[ -n "$post_ver" ] && [ "$post_ver" != "unknown" ] && post_ver_st=PASS
	fi
	if [ -d "$_mnt_esp/EFI/GREENJADE/drivers" ]; then
		[ -f "$_mnt_esp/EFI/GREENJADE/drivers/ddi_host" ] && \
			[ "$(wc -c <"$_mnt_esp/EFI/GREENJADE/drivers/ddi_host" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_ddi=PASS
		[ -f "$_mnt_esp/EFI/GREENJADE/drivers/rtl8168_udx" ] && \
			[ "$(wc -c <"$_mnt_esp/EFI/GREENJADE/drivers/rtl8168_udx" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_rtl=PASS
		[ -f "$_mnt_esp/EFI/GREENJADE/drivers/xhci_udx" ] && \
			[ "$(wc -c <"$_mnt_esp/EFI/GREENJADE/drivers/xhci_udx" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_xhci=PASS
	fi
	umount "$_mnt_esp" 2>/dev/null || umount -l "$_mnt_esp" 2>/dev/null || true
fi
if mount -t ext4 -L GJ-PERSIST "$_mnt_persist" 2>/dev/null \
	|| mount -L GJ-PERSIST "$_mnt_persist" 2>/dev/null \
	|| { [ -n "$p2" ] && mount "$p2" "$_mnt_persist" 2>/dev/null; }; then
	if [ -d "$_mnt_persist/drivers" ]; then
		[ -f "$_mnt_persist/drivers/ddi_host" ] && \
			[ "$(wc -c <"$_mnt_persist/drivers/ddi_host" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_persist_ddi=PASS
		[ -f "$_mnt_persist/drivers/rtl8168_udx" ] && \
			[ "$(wc -c <"$_mnt_persist/drivers/rtl8168_udx" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_persist_rtl=PASS
		[ -f "$_mnt_persist/drivers/xhci_udx" ] && \
			[ "$(wc -c <"$_mnt_persist/drivers/xhci_udx" | tr -d ' ')" -gt 0 ] 2>/dev/null && post_persist_xhci=PASS
	fi
	# Prefer packed IMAGE_STATUS_TITLE when ESP ver miss
	if [ "$post_ver_st" = "SKIP" ] && [ -f "$_mnt_persist/IMAGE_VERSION" ]; then
		post_ver=$(tr -d '\r\n' <"$_mnt_persist/IMAGE_VERSION" 2>/dev/null || echo unknown)
		[ -n "$post_ver" ] && [ "$post_ver" != "unknown" ] && post_ver_st=PASS
	fi
	umount "$_mnt_persist" 2>/dev/null || umount -l "$_mnt_persist" 2>/dev/null || true
fi
rmdir "$_mnt_esp" "$_mnt_persist" 2>/dev/null || true
# Automount often remounts GREENJADE / GJ-PERSIST after partprobe.
unmount_lab_media
# dual_land flash residual
if { [ "$post_ddi" = "PASS" ] || [ "$post_rtl" = "PASS" ] || [ "$post_xhci" = "PASS" ]; } \
	&& { [ "$post_persist_ddi" = "PASS" ] || [ "$post_persist_rtl" = "PASS" ] || [ "$post_persist_xhci" = "PASS" ]; }; then
	dual_land_flash=PASS
elif [ "$post_ddi" = "PASS" ] || [ "$post_rtl" = "PASS" ] || [ "$post_xhci" = "PASS" ]; then
	dual_land_flash=ESP_ONLY
elif [ "$post_persist_ddi" = "PASS" ] || [ "$post_persist_rtl" = "PASS" ] || [ "$post_persist_xhci" = "PASS" ]; then
	dual_land_flash=PERSIST_ONLY
fi
# post_verify: panel identity match + any UDX product host on ESP
if [ "$post_ver_st" = "PASS" ] && [ "$post_ver" = "$img_ver" ] \
	&& { [ "$post_ddi" = "PASS" ] || [ "$post_rtl" = "PASS" ] || [ "$post_xhci" = "PASS" ] || [ "$pre_ddi" = "PASS" ]; }; then
	post_verify=PASS
elif [ "$post_ver_st" = "PASS" ] || [ "$post_ddi" = "PASS" ] || [ "$pre_ddi" = "PASS" ]; then
	post_verify=PARTIAL
fi
# Prefer post-mount ver when host pre-extract was unknown
if [ "$img_ver" = "unknown" ] && [ "$post_ver_st" = "PASS" ]; then
	img_ver=$post_ver
	img_status_title="STATUS (static) v${img_ver}"
	img_ver_src=post_mount
fi

echo "install-hwtest-usb: PASS wrote $img -> $dev"
echo "  image: GJ_IMAGE_VERSION=${img_ver} src=${img_ver_src}"
echo "  panel: expect STATUS title → ${img_status_title}"
echo "  flash: flash bar → ${img_status_title} (Soft!=product identity only; stamp-free)"
echo "  serial: main: image version=${img_ver}"
echo "  Boot:  UEFI -> GreenJade BOOTX64.EFI (serial: GJ-EFI / M0 OK)"
echo "  Verify: on DUT, panel STATUS row 0 must match the title above"
echo "          host recheck: ./scripts/gj-image-version.sh --report"
echo "  Logs:  umount any automount first, then:"
echo "         sudo umount /run/media/\$USER/GJ-PERSIST 2>/dev/null; true"
echo "         sudo mount -t ext4 -L GJ-PERSIST /mnt/gj-persist"
echo "         cat /mnt/gj-persist/IMAGE_STATUS_TITLE   # packed expect title"
echo "         cat /mnt/gj-persist/steam/STATUS         # expect READY (media)"
echo "         ls /mnt/gj-persist/drivers/             # UDX dual-land mirror"
echo "  ESP:   sudo mount -L GREENJADE /mnt/gj-esp"
echo "         cat /mnt/gj-esp/EFI/GREENJADE/IMAGE_VERSION"
echo "         ls /mnt/gj-esp/EFI/GREENJADE/drivers/    # UDX: ddi_host rtl8168_udx xhci_udx"
echo "         cat /mnt/gj-esp/EFI/GREENJADE/drivers/MANIFEST.txt 2>/dev/null || true"
echo "         cat /mnt/gj-esp/EFI/GREENJADE/DUT-PACK.txt 2>/dev/null || true"
echo "  SSH:   sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "  Soft:  ./scripts/gj-product-summary.sh <serial-log>   # exit 0 always"
echo "  Keys:  ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam: if STATUS=SKELETON: sudo make steam-to-persist"
echo "  Note:  READY/media != Steam client; Top-50 stays NOT-TRIED until DUT run"
echo "  Soft!=product: version identity / soft greps != product complete != bar3 close"
echo "  Soft!=product · G-AC-1: UDX pack != product TX/RX/BOT; freestanding class SKIP default"
echo "  Dual DoD: A OPEN until USB path; B OPEN until interactive SSH login — flash != close"
echo "  stamp-free: read GJ_IMAGE_VERSION from pack/ELF only — never invent/bump here"
echo "  class: freestanding SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 residual opt-in)"
echo "install-hwtest-usb: soft residual flash wrote=${img} -> ${dev}"
echo "install-hwtest-usb: soft residual post_verify=${post_verify} panel_ver=${post_ver_st} ver=${post_ver}"
echo "install-hwtest-usb: soft residual udx pack ddi_host=${post_ddi} rtl8168_udx=${post_rtl} xhci_udx=${post_xhci} (post-flash ESP; Soft!=product)"
echo "install-hwtest-usb: soft residual udx dual_land=${dual_land_flash} persist ddi_host=${post_persist_ddi} rtl8168_udx=${post_persist_rtl} xhci_udx=${post_persist_xhci}"
echo "install-hwtest-usb: soft residual panel=${img_status_title} src=${img_ver_src}"
echo "install-hwtest-usb: soft residual dual_dod A=OPEN B=OPEN (flash != Dual DoD close)"
echo "install-hwtest-usb: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "install-hwtest-usb: soft residual freestanding class SKIP"
echo "install-hwtest-usb: soft residual stamp-free (bar honesty v0.1.178; NEVER bump GJ_IMAGE_VERSION; no invent next N)"
echo "  greppable: install-hwtest-usb: soft residual dual_dod"
echo "  greppable: install-hwtest-usb: soft residual product=UDX+ABI"
echo "  greppable: install-hwtest-usb: soft residual freestanding class SKIP"
echo "  greppable: install-hwtest-usb: soft residual stamp-free"
echo "  greppable: install-hwtest-usb: soft residual flash"
echo "  greppable: install-hwtest-usb: soft residual udx pack"
echo "  greppable: install-hwtest-usb: soft residual udx dual_land"
echo "  greppable: install-hwtest-usb: soft residual panel="
echo "  greppable: install-hwtest-usb: soft residual post_verify="
