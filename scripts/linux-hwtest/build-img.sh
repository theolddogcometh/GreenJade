#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a bootable Linux hardware-test disk image (Alpine x86_64, UEFI):
#   p1  EF00  FAT32  label GJ-LNX-ESP  — GRUB + kernel + initramfs + reports/
#   p2  8300  ext4   label GJ-LNX-ROOT — Alpine root with auto hwtest loop
#
# Boot → OpenRC local.d → /opt/gj-hwtest/run-all.sh → write report to ESP → poweroff
#
# Usage:
#   ./scripts/linux-hwtest/build-img.sh [out.img]
# Env:
#   GJ_LINUX_HWTEST_MB=2048   image size MiB (default 2048)
#   GJ_ALPINE_VERSION=3.20   Alpine branch
#   GJ_SKIP_DOWNLOAD=1       reuse cached rootfs under build/linux-hwtest-cache/
#
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)"
cd "$root"

out="${1:-build/linux-hwtest.img}"
IMG_MB="${GJ_LINUX_HWTEST_MB:-2048}"
ALPINE_VER="${GJ_ALPINE_VERSION:-3.20}"
ARCH=x86_64
CACHE="$root/build/linux-hwtest-cache"
WORK="$root/build/linux-hwtest-work.$$"
MNT_ROOT="$WORK/mnt-root"
MNT_ESP="$WORK/mnt-esp"
OVERLAY="$(CDPATH= cd -- "$(dirname "$0")" && pwd)/overlay"

need() {
	command -v "$1" >/dev/null 2>&1 || {
		echo "missing tool: $1" >&2
		exit 1
	}
}
need dd
need sgdisk
need mkfs.ext4
need mkfs.vfat
need tar
need curl
need mount
need umount

# Prefer mtools-free host mount for packing
if [ "$(id -u)" -ne 0 ]; then
	echo "build-img: need root for loop mount (sudo $0 ...)" >&2
	exit 1
fi

cleanup() {
	umount "$MNT_ROOT/boot/efi" 2>/dev/null || true
	umount "$MNT_ESP" 2>/dev/null || true
	umount "$MNT_ROOT/boot" 2>/dev/null || true
	umount "$MNT_ROOT/proc" 2>/dev/null || true
	umount "$MNT_ROOT/sys" 2>/dev/null || true
	umount "$MNT_ROOT/dev" 2>/dev/null || true
	umount "$MNT_ROOT" 2>/dev/null || true
	# detach loops
	if [ -n "${LOOP:-}" ]; then
		losetup -d "$LOOP" 2>/dev/null || true
	fi
	rm -rf "$WORK"
}
trap cleanup EXIT INT TERM HUP

mkdir -p "$CACHE" "$(dirname "$out")" "$WORK" "$MNT_ROOT" "$MNT_ESP"

# --- fetch Alpine minirootfs ---
MINI_VER_FILE="$CACHE/alpine-version.txt"
# Resolve latest patch for branch via static known-good pattern; allow override
MINI_NAME="${GJ_ALPINE_MINIROOTFS:-alpine-minirootfs-${ALPINE_VER}.3-${ARCH}.tar.gz}"
# Try a few patch levels
fetch_minirootfs() {
	if [ "${GJ_SKIP_DOWNLOAD:-0}" = "1" ] && [ -f "$CACHE/minirootfs.tar.gz" ]; then
		echo "build-img: using cached minirootfs"
		return 0
	fi
	for patch in 3 2 1 0 4 5; do
		name="alpine-minirootfs-${ALPINE_VER}.${patch}-${ARCH}.tar.gz"
		url="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VER}/releases/${ARCH}/${name}"
		echo "build-img: try $url"
		if curl -fsSL --connect-timeout 15 -o "$CACHE/minirootfs.tar.gz.part" "$url"; then
			mv "$CACHE/minirootfs.tar.gz.part" "$CACHE/minirootfs.tar.gz"
			echo "$name" >"$MINI_VER_FILE"
			echo "build-img: downloaded $name"
			return 0
		fi
	done
	# fallback: latest stable symlink style
	url="https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/${ARCH}/"
	echo "build-img: listing $url for minirootfs..." >&2
	return 1
}

if ! fetch_minirootfs; then
	echo "build-img: FAIL could not download Alpine minirootfs (network?)" >&2
	echo "  Set GJ_SKIP_DOWNLOAD=1 with cache, or GJ_ALPINE_MINIROOTFS=name" >&2
	exit 1
fi

# --- apk.static for installing packages into rootfs ---
APK_STATIC="$CACHE/apk.static"
if [ ! -x "$APK_STATIC" ] || [ "${GJ_SKIP_DOWNLOAD:-0}" != "1" ]; then
	# Official static builds mirror
	apk_url="https://gitlab.alpinelinux.org/api/v4/projects/5/packages/generic/v2.14.4/${ARCH}/apk.static"
	echo "build-img: fetch apk.static"
	if ! curl -fsSL --connect-timeout 15 -o "$APK_STATIC.part" "$apk_url"; then
		# mirror via alpine package (extract from apk-tools-static)
		echo "build-img: primary apk.static URL failed; trying dl-cdn apk-tools-static..."
		for patch in 3 2 1 0; do
			aurl="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VER}/main/${ARCH}/apk-tools-static-2.14.${patch}-r0.apk"
			if curl -fsSL -o "$CACHE/apk-tools-static.apk" "$aurl"; then
				# .apk is tar.gz
				tar -xzf "$CACHE/apk-tools-static.apk" -C "$CACHE" sbin/apk.static 2>/dev/null \
					|| tar -xzf "$CACHE/apk-tools-static.apk" -C "$CACHE"
				if [ -f "$CACHE/sbin/apk.static" ]; then
					cp "$CACHE/sbin/apk.static" "$APK_STATIC"
					break
				fi
			fi
		done
	else
		mv "$APK_STATIC.part" "$APK_STATIC"
	fi
	chmod +x "$APK_STATIC" 2>/dev/null || true
fi
if [ ! -x "$APK_STATIC" ]; then
	echo "build-img: FAIL no apk.static — cannot install packages" >&2
	exit 1
fi

# --- create blank image + GPT ---
echo "build-img: create $out (${IMG_MB} MiB)"
rm -f "$out"
dd if=/dev/zero of="$out" bs=1M count="$IMG_MB" status=none 2>/dev/null \
	|| dd if=/dev/zero of="$out" bs=1048576 count="$IMG_MB"

# p1 ESP 512 MiB, p2 rest
ESP_MIB=512
ESP_START=2048
ESP_SECTS=$((ESP_MIB * 1024 * 1024 / 512))
ESP_END=$((ESP_START + ESP_SECTS - 1))
ROOT_START=$(( (ESP_END + 1 + 2047) / 2048 * 2048 ))

sgdisk -o \
	-n 1:${ESP_START}:${ESP_END} -t 1:ef00 -c 1:GJ-LNX-ESP \
	-n 2:${ROOT_START}:0 -t 2:8300 -c 2:GJ-LNX-ROOT \
	"$out" >/dev/null

LOOP=$(losetup -f --show -P "$out")
echo "build-img: loop $LOOP"
# wait for partitions
i=0
while [ ! -b "${LOOP}p1" ] && [ "$i" -lt 50 ]; do
	sleep 0.1
	i=$((i + 1))
done
if [ ! -b "${LOOP}p1" ]; then
	# some kernels use LOOP p1 as LOOP1
	if [ -b "${LOOP}p1" ]; then
		:
	elif ls "${LOOP}p"* >/dev/null 2>&1; then
		:
	else
		partprobe "$LOOP" 2>/dev/null || true
		sleep 0.5
	fi
fi

P1="${LOOP}p1"
P2="${LOOP}p2"
if [ ! -b "$P1" ]; then
	# util-linux losetup -P naming
	P1="${LOOP}p1"
	P2="${LOOP}p2"
fi
if [ ! -b "$P1" ]; then
	echo "build-img: FAIL no partition nodes for $LOOP" >&2
	ls -la "$LOOP"* >&2 || true
	exit 1
fi

mkfs.vfat -F 32 -n GJ-LNX-ESP "$P1" >/dev/null
mkfs.ext4 -F -L GJ-LNX-ROOT "$P2" >/dev/null

mount "$P2" "$MNT_ROOT"
mkdir -p "$MNT_ROOT/boot"
mount "$P1" "$MNT_ESP"

# --- extract minirootfs ---
echo "build-img: extract minirootfs"
tar -xzf "$CACHE/minirootfs.tar.gz" -C "$MNT_ROOT"

# DNS for apk
cp -L /etc/resolv.conf "$MNT_ROOT/etc/resolv.conf" 2>/dev/null \
	|| echo "nameserver 1.1.1.1" >"$MNT_ROOT/etc/resolv.conf"

mount --bind /proc "$MNT_ROOT/proc"
mount --bind /sys "$MNT_ROOT/sys"
mount --bind /dev "$MNT_ROOT/dev"

# --- install packages ---
echo "build-img: apk add packages (this needs network)"
APK_REPO="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VER}/main"
APK_REPO_C="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VER}/community"
cat >"$MNT_ROOT/etc/apk/repositories" <<EOF
$APK_REPO
$APK_REPO_C
EOF

# Run apk.static with root
"$APK_STATIC" --root "$MNT_ROOT" --arch "$ARCH" --repository "$APK_REPO" \
	--repository "$APK_REPO_C" --keys-dir "$MNT_ROOT/etc/apk/keys" \
	--initdb add alpine-base 2>/dev/null || true

"$APK_STATIC" --root "$MNT_ROOT" --arch "$ARCH" --repository "$APK_REPO" \
	--repository "$APK_REPO_C" --keys-dir "$MNT_ROOT/etc/apk/keys" \
	add \
	openrc \
	linux-lts \
	linux-firmware-none \
	grub \
	grub-efi \
	efibootmgr \
	eudev \
	eudev-openrc \
	util-linux \
	e2fsprogs \
	dosfstools \
	blkid \
	pciutils \
	usbutils \
	dmidecode \
	coreutils \
	findutils \
	grep \
	bash \
	lsblk \
	mount \
	kbd-bkeymaps \
	|| {
		echo "build-img: WARN some packages failed; retry base set" >&2
		"$APK_STATIC" --root "$MNT_ROOT" --arch "$ARCH" \
			--repository "$APK_REPO" --repository "$APK_REPO_C" \
			--keys-dir "$MNT_ROOT/etc/apk/keys" \
			add openrc linux-lts grub grub-efi eudev util-linux \
			pciutils usbutils blkid dosfstools e2fsprogs
	}

# --- overlay scripts ---
echo "build-img: install overlay"
cp -a "$OVERLAY/." "$MNT_ROOT/"
chmod +x "$MNT_ROOT/opt/gj-hwtest/"*.sh
chmod +x "$MNT_ROOT/etc/local.d/gj-hwtest.start"

# OpenRC: boot + default services (chroot + direct runlevel links)
echo "gj-linux-hwtest" >"$MNT_ROOT/etc/hostname"
mkdir -p "$MNT_ROOT/etc/runlevels/sysinit" \
	"$MNT_ROOT/etc/runlevels/boot" \
	"$MNT_ROOT/etc/runlevels/default"
# Essential OpenRC links (names match Alpine openrc scripts when packages install)
for s in devfs dmesg udev udev-trigger udev-settle hwdrivers; do
	[ -f "$MNT_ROOT/etc/init.d/$s" ] && ln -sf "/etc/init.d/$s" "$MNT_ROOT/etc/runlevels/sysinit/$s"
done
for s in modules sysctl hostname bootmisc syslog; do
	[ -f "$MNT_ROOT/etc/init.d/$s" ] && ln -sf "/etc/init.d/$s" "$MNT_ROOT/etc/runlevels/boot/$s"
done
# local runs /etc/local.d/*.start — this is the auto hwtest hook
if [ -f "$MNT_ROOT/etc/init.d/local" ]; then
	ln -sf /etc/init.d/local "$MNT_ROOT/etc/runlevels/default/local"
fi
chroot "$MNT_ROOT" /bin/sh -c '
	rc-update add local default 2>/dev/null || true
	rc-update add udev sysinit 2>/dev/null || true
	rc-update add udev-trigger sysinit 2>/dev/null || true
	rc-update add udev-settle sysinit 2>/dev/null || true
' 2>/dev/null || true

# Disable interactive OpenRC prompts
if [ -f "$MNT_ROOT/etc/rc.conf" ]; then
	grep -q 'rc_interactive=' "$MNT_ROOT/etc/rc.conf" 2>/dev/null \
		|| echo 'rc_interactive=NO' >>"$MNT_ROOT/etc/rc.conf"
fi

# fstab
cat >"$MNT_ROOT/etc/fstab" <<'EOF'
LABEL=GJ-LNX-ROOT  /     ext4  rw,noatime 0 1
LABEL=GJ-LNX-ESP   /boot vfat  rw,noatime 0 2
proc                 /proc proc  defaults   0 0
sysfs                /sys  sysfs defaults   0 0
devtmpfs             /dev  devtmpfs defaults 0 0
EOF

# secure tty / permit local without password for debug (optional console)
echo "root::0:0:root:/root:/bin/sh" >"$MNT_ROOT/etc/passwd.bak" 2>/dev/null || true
# Keep root locked for auto loop; console still shows logs on tty1

# --- install GRUB EFI to ESP ---
echo "build-img: GRUB EFI"
mkdir -p "$MNT_ESP/EFI/BOOT" "$MNT_ESP/GJ-HWTEST/reports" "$MNT_ESP/grub"
# kernel + initramfs into ESP (and keep in /boot for package paths)
KVER=$(ls "$MNT_ROOT/lib/modules" 2>/dev/null | head -1 || true)
if [ -z "$KVER" ]; then
	echo "build-img: FAIL no kernel modules after apk add linux-lts" >&2
	exit 1
fi
# Alpine places vmlinuz-lts / initramfs-lts in /boot
VMLINUZ=$(ls "$MNT_ROOT/boot"/vmlinuz* 2>/dev/null | head -1)
INITRD=$(ls "$MNT_ROOT/boot"/initramfs* 2>/dev/null | head -1)
if [ -z "$VMLINUZ" ] || [ -z "$INITRD" ]; then
	echo "build-img: FAIL missing vmlinuz/initramfs in /boot" >&2
	ls -la "$MNT_ROOT/boot" >&2 || true
	exit 1
fi
case "$VMLINUZ" in
*/vmlinuz-lts) ;;
*) cp -a "$VMLINUZ" "$MNT_ROOT/boot/vmlinuz-lts" ;;
esac
case "$INITRD" in
*/initramfs-lts) ;;
*) cp -a "$INITRD" "$MNT_ROOT/boot/initramfs-lts" ;;
esac

# Remount ESP at /boot/efi, then place kernel + GRUB there (single mount)
mkdir -p "$MNT_ROOT/boot/efi"
umount "$MNT_ESP" 2>/dev/null || true
mount "$P1" "$MNT_ROOT/boot/efi"
MNT_ESP="$MNT_ROOT/boot/efi"
mkdir -p "$MNT_ESP/EFI/BOOT" "$MNT_ESP/grub" "$MNT_ESP/GJ-HWTEST/reports"
cp -a "$MNT_ROOT/boot/vmlinuz-lts" "$MNT_ESP/vmlinuz-lts"
cp -a "$MNT_ROOT/boot/initramfs-lts" "$MNT_ESP/initramfs-lts"

#
# GRUB EFI: build a self-contained BOOTX64.EFI with prefix /EFI/BOOT so it
# loads /EFI/BOOT/grub.cfg next to itself. (grub-install --removable often
# sets prefix to grub-boot/ and drops to "grub>" when that path is empty.)
#
echo "build-img: grub-mkimage (prefix=/EFI/BOOT, modules embedded)"
GRUB_MODS="fat part_gpt part_msdos ext2 iso9660 linux normal configfile
	search search_label search_fs_file search_fs_uuid gzio echo ls cat
	test true boot chain reboot halt all_video efi_gop efi_uga
	video videoinfo loadenv regexp probe"
# shellcheck disable=SC2086
GJ_MKIMAGE=""
for c in /usr/bin/grub-mkimage /usr/sbin/grub-mkimage; do
	if [ -x "$MNT_ROOT$c" ]; then
		GJ_MKIMAGE=$c
		break
	fi
done
if [ -n "$GJ_MKIMAGE" ] && chroot "$MNT_ROOT" "$GJ_MKIMAGE" -O x86_64-efi \
	-o /boot/efi/EFI/BOOT/BOOTX64.EFI \
	-p /EFI/BOOT \
	$GRUB_MODS 2>"$WORK/grub-mkimage.err"; then
	echo "build-img: grub-mkimage OK ($GJ_MKIMAGE)"
else
	echo "build-img: grub-mkimage failed:" >&2
	cat "$WORK/grub-mkimage.err" 2>/dev/null >&2 || true
	# Last resort: grub-install (may need cfg mirrors; we write them below)
	chroot "$MNT_ROOT" /usr/sbin/grub-install \
		--target=x86_64-efi \
		--efi-directory=/boot/efi \
		--boot-directory=/boot/efi \
		--removable \
		--recheck 2>/dev/null || true
fi

# Robust grub.cfg: find kernel by file, then boot Alpine disk root.
# modules= list is Alpine initramfs (nlplug-findfs) — need usb+ext4 to see stick.
GJ_GRUB_CFG='set default=0
set timeout=3
set gfxpayload=keep

# Prefer the partition that holds our kernel
if search --file --no-floppy --set=root /vmlinuz-lts ; then
  true
elif search --label --no-floppy --set=root GJ-LNX-ESP ; then
  true
elif search --file --no-floppy --set=root /EFI/BOOT/BOOTX64.EFI ; then
  true
fi

menuentry "GreenJade Linux HWTEST (auto report + poweroff)" {
	if search --file --no-floppy --set=root /vmlinuz-lts ; then true ; fi
	echo "Loading Linux..."
	linux /vmlinuz-lts root=LABEL=GJ-LNX-ROOT rootfstype=ext4 rw \
		modules=sd-mod,usb-storage,uas,xhci-pci,xhci-hcd,ext4,crc32c \
		rootdelay=10 console=tty0 gj_hwtest=1
	echo "Loading initrd..."
	initrd /initramfs-lts
}
menuentry "GreenJade Linux HWTEST (verbose)" {
	if search --file --no-floppy --set=root /vmlinuz-lts ; then true ; fi
	linux /vmlinuz-lts root=LABEL=GJ-LNX-ROOT rootfstype=ext4 rw \
		modules=sd-mod,usb-storage,uas,xhci-pci,xhci-hcd,ext4,crc32c \
		rootdelay=15 console=tty0 debug
	initrd /initramfs-lts
}
menuentry "GRUB shell (debug)" {
	echo "type: ls /  and  cat /EFI/BOOT/grub.cfg"
	read
}
'
# Write cfg where GRUB looks (prefix /EFI/BOOT) and mirrors
printf '%s\n' "$GJ_GRUB_CFG" >"$MNT_ESP/EFI/BOOT/grub.cfg"
printf '%s\n' "$GJ_GRUB_CFG" >"$MNT_ESP/grub/grub.cfg"
# Also under boot/grub if grub-install left that layout
mkdir -p "$MNT_ESP/boot/grub" 2>/dev/null || true
printf '%s\n' "$GJ_GRUB_CFG" >"$MNT_ESP/boot/grub/grub.cfg" 2>/dev/null || true

if [ ! -f "$MNT_ESP/EFI/BOOT/BOOTX64.EFI" ]; then
	echo "build-img: FAIL no BOOTX64.EFI after grub packaging" >&2
	find "$MNT_ESP" -iname '*.efi' 2>/dev/null | head -20 >&2 || true
	exit 1
fi
echo "build-img: ESP boot files:"
ls -la "$MNT_ESP/EFI/BOOT/" "$MNT_ESP/"/*.lts 2>/dev/null | head -30 || true

# README on ESP for operators
cat >"$MNT_ESP/GJ-HWTEST/README.txt" <<'EOF'
GreenJade Linux hardware test media
===================================

Boot this USB in UEFI mode (Secure Boot OFF).

Automatic loop:
  1. Alpine Linux boots
  2. /etc/local.d/gj-hwtest.start runs
  3. Probes system/PCI/USB/xHCI/storage/dmesg
  4. Writes report under GJ-HWTEST/reports/
  5. poweroff

After the machine shuts down, plug the stick into a lab host:

  sudo mkdir -p /mnt/esp
  sudo mount -L GJ-LNX-ESP /mnt/esp
  ls /mnt/esp/GJ-HWTEST/reports/
  cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt
  cat /mnt/esp/GJ-HWTEST/reports/KEY-FACTS.txt   # greppable xhci_bdf / stick_bus_port
  cat /mnt/esp/GJ-HWTEST-RESULT.txt
  sudo umount /mnt/esp

Labels: this image uses GJ-LNX-ESP (FAT reports) + GJ-LNX-ROOT (Alpine),
not product GREENJADE / GJ-PERSIST.

Honesty: this is a Linux inventory helper for DUT bring-up.
It does not claim GreenJade freestanding bar3 / Top50 / product USB.
EOF

# unmount bind mounts before finish
umount "$MNT_ROOT/dev" 2>/dev/null || true
umount "$MNT_ROOT/sys" 2>/dev/null || true
umount "$MNT_ROOT/proc" 2>/dev/null || true
umount "$MNT_ROOT/boot/efi" 2>/dev/null || true
umount "$MNT_ESP" 2>/dev/null || true
umount "$MNT_ROOT" 2>/dev/null || true
sync
losetup -d "$LOOP" 2>/dev/null || true
LOOP=

sz=$(wc -c <"$out" | tr -d ' ')
echo "build-img: PASS $out size=${sz}B"
echo "  write:  sudo ./scripts/linux-hwtest/install-usb.sh /dev/sdX"
echo "  boot:   UEFI → GreenJade Linux HWTEST (auto)"
echo "  report: mount -L GJ-LNX-ESP → GJ-HWTEST/reports/{LATEST,KEY-FACTS}.txt"
# Verify EFI binary exists on image
if command -v mdir >/dev/null 2>&1; then
	ESP_OFF=$((ESP_START * 512))
	if mdir -i "$out@@$ESP_OFF" ::/EFI/BOOT 2>/dev/null | grep -qi BOOTX64; then
		echo "  efi:    BOOTX64.EFI present on ESP"
	else
		echo "  efi:    WARN BOOTX64.EFI may be missing — check grub-install logs" >&2
	fi
fi
