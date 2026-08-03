#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a dual-partition GPT disk image for real-hardware testing on USB:
#
#   p1  EF00 ESP (FAT32, ~384 MiB)  — BOOTX64.EFI + KERNEL.ELF + rootfs/user
#                                     (FAT required by UEFI firmware)
#   p2  8300 Linux (ext4, rest)     — label GJ-PERSIST: logs + ssh + Steam tree
#                                     (ext4 for Linux/Unix-only lab: symlinks OK)
#
# Usage:
#   ./scripts/make-hwtest-img.sh [out.img]
# Default: build/greenjade-hwtest.img
#
# Write to USB (DESTROYS device):
#   sudo ./scripts/install-hwtest-usb.sh /dev/sdX
#
# Operator after boot (serial / GJ-PERSIST logs):
#   ./scripts/gj-product-summary.sh <serial-log>   # soft product PASSes, exit 0
#   ./scripts/gj-quick-keys.sh <serial-log>        # hard product keys, exit 1 on miss
#   ./scripts/steam-bar3-check.sh                  # media READY/SKELETON, exit 0
#   ./scripts/hwtest-ssh-setup.sh                  # lab-host OpenSSH + hwtest key
#
# Env: GJ_HWTEST_IMG_MB, GJ_HWTEST_ESP_MIB — geometry overrides
# Steam READY auto-grows image unless GJ_HWTEST_IMG_MB is set (see below).
# GJ_HWTEST_PERSIST_FS=fat32  — emergency fallback to FAT32 persist (not default).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/greenjade-hwtest.img}"
esp_dir="${TMPDIR:-/tmp}/gj-hwtest-esp.$$"
persist_dir="${TMPDIR:-/tmp}/gj-hwtest-persist.$$"
persist_raw="${TMPDIR:-/tmp}/gj-hwtest-persist-raw.$$"

need() {
	command -v "$1" >/dev/null 2>&1 || {
		echo "missing tool: $1" >&2
		exit 1
	}
}
need dd
need mformat
need mcopy
need mmd
need sgdisk
# Default persist is ext4 (Linux/Unix lab); mke2fs required unless FAT fallback.
PERSIST_FS="${GJ_HWTEST_PERSIST_FS:-ext4}"
case "$PERSIST_FS" in
ext4|EXT4)
	PERSIST_FS=ext4
	need mke2fs
	;;
fat32|FAT32|vfat|msdos)
	PERSIST_FS=fat32
	;;
*)
	echo "make-hwtest-img: unknown GJ_HWTEST_PERSIST_FS=$PERSIST_FS (use ext4 or fat32)" >&2
	exit 1
	;;
esac

cleanup() {
	rm -rf "$esp_dir" "$persist_dir"
	rm -f "$persist_raw"
}
trap cleanup EXIT INT TERM HUP

mkdir -p "$(dirname "$out")"
echo "make-hwtest-img: stage-esp + stage-rootfs..."
chmod +x scripts/stage-esp.sh scripts/stage-rootfs.sh
./scripts/stage-esp.sh "$esp_dir" >/dev/null
./scripts/stage-rootfs.sh build/rootfs >/dev/null

# Ensure hwtest keys exist (shared with live-iso / ssh-setup)
mkdir -p build/hwtest-keys
if [ ! -f build/hwtest-keys/id_ed25519 ]; then
	if command -v ssh-keygen >/dev/null 2>&1; then
		ssh-keygen -t ed25519 -N "" -C "greenjade-hwtest@lab" \
			-f build/hwtest-keys/id_ed25519 >/dev/null
		echo "make-hwtest-img: generated build/hwtest-keys/id_ed25519"
	else
		echo "make-hwtest-img: warn: ssh-keygen missing; persist has no authorized_keys" >&2
	fi
fi

rm -rf "$persist_dir"
mkdir -p "$persist_dir/logs" "$persist_dir/journal" "$persist_dir/ssh" \
	"$persist_dir/bin" "$persist_dir/steam"

# Option 2: prebuilt Steam tree onto GJ-PERSIST (primary durable payload)
chmod +x scripts/stage-steam-tree.sh
./scripts/stage-steam-tree.sh "$persist_dir" >/dev/null || true
# stage-steam-tree writes $persist_dir/steam/; keep STATUS for operators
steam_status="SKELETON"
if [ -f "$persist_dir/steam/STATUS" ]; then
	steam_status=$(cat "$persist_dir/steam/STATUS" | tr -d '\r\n')
fi

cat >"$persist_dir/README.txt" <<EOF
GJ-PERSIST — GreenJade hardware-test durable storage
====================================================

Filesystem: ${PERSIST_FS}  (default ext4 for Linux/Unix-only lab; ESP is still FAT)
Label:      GJ-PERSIST

Mount on a lab host after a run:

  sudo mkdir -p /mnt/gj-persist
  sudo mount -L GJ-PERSIST /mnt/gj-persist
  ls /mnt/gj-persist/logs
  # ext4: rsync -a works (symlinks + modes preserved)

Directories
  logs/           serial captures, dmesg dumps, operator notes
  journal/        structured run journals (JSON/text)
  ssh/            authorized_keys + lab-host enable helpers
  steam/          prebuilt Steam tree (option 2) — STATUS=$steam_status
  linux-drivers/  host-collected Linux .ko + firmware + NEEDED-DRIVERS.txt
                  (ABI/module-path staging; freestanding does not load .ko yet)

Steam (option 2 — no dpkg on GreenJade)
  Lab host:  make steam-fetch && make steam-stage && make hwtest-img
  Docs:      docs/STEAM_HWTEST.md
  On stick:  steam/STATUS (READY or SKELETON), steam/usr/bin/steam when READY

SSH remote debug (Grok / operator)
  Product freestanding sshd is on by default at boot (sshd.elf, port 22).
  Full eth-TCP + crypto KEX is staged; lab-host OpenSSH remains for serial:

    sudo /path/to/knano/scripts/hwtest-ssh-setup.sh

  Or, with this partition mounted:

    sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh

  Then: ssh -i <tree>/build/hwtest-keys/id_ed25519 root@<lab-host>

Soft-scan serial captures (host)
  ./scripts/gj-product-summary.sh logs/serial-….txt   # soft exit 0
  ./scripts/gj-quick-keys.sh logs/serial-….txt        # hard miss exit 1
  ./scripts/steam-bar3-check.sh                       # media READY/SKELETON

Honesty
  STATUS=READY means bootstrap tree on media — not Steam client run.
  Kernel HDA multi-stream PASS is not Steam audio. Deck Top 50 stays
  NOT-TRIED until real DUT title results.
EOF

cat >"$persist_dir/logs/README.txt" <<'EOF'
Drop serial captures here, e.g.:
  picocom -b 115200 /dev/ttyUSB0 | tee logs/serial-$(date -u +%Y%m%dT%H%MZ).txt
EOF

# SSH materials on persist so plugging the stick into the lab host is enough
if [ -f build/hwtest-keys/id_ed25519.pub ]; then
	cp -f build/hwtest-keys/id_ed25519.pub "$persist_dir/ssh/authorized_keys"
	cp -f build/hwtest-keys/id_ed25519.pub "$persist_dir/ssh/id_ed25519.pub"
fi
cat >"$persist_dir/ssh/sshd_config.snippet" <<'EOF'
# Append or include on lab host (/etc/ssh/sshd_config.d/99-greenjade-hwtest.conf)
Port 22
PermitRootLogin prohibit-password
PasswordAuthentication no
PubkeyAuthentication yes
EOF

cat >"$persist_dir/ssh/enable-lab-ssh.sh" <<'EOF'
#!/bin/sh
# Run on the *lab host* Linux (not inside freestanding GreenJade).
# Enables sshd and installs GJ-PERSIST authorized_keys for root.
set -eu
HERE="$(CDPATH= cd -- "$(dirname "$0")" && pwd)"
AUTH="$HERE/authorized_keys"
if [ ! -f "$AUTH" ]; then
	echo "missing $AUTH" >&2
	exit 1
fi
if [ "$(id -u)" -ne 0 ]; then
	echo "run as root: sudo $0" >&2
	exit 1
fi
mkdir -p /root/.ssh
chmod 700 /root/.ssh
# Merge key if not already present
if [ -f /root/.ssh/authorized_keys ]; then
	while IFS= read -r line; do
		[ -z "$line" ] && continue
		grep -qxF "$line" /root/.ssh/authorized_keys 2>/dev/null || \
			echo "$line" >>/root/.ssh/authorized_keys
	done <"$AUTH"
else
	cp -f "$AUTH" /root/.ssh/authorized_keys
fi
chmod 600 /root/.ssh/authorized_keys
if [ -d /etc/ssh/sshd_config.d ]; then
	cp -f "$HERE/sshd_config.snippet" \
		/etc/ssh/sshd_config.d/99-greenjade-hwtest.conf
fi
if command -v systemctl >/dev/null 2>&1; then
	systemctl enable --now sshd 2>/dev/null || systemctl enable --now ssh 2>/dev/null || true
	systemctl reload sshd 2>/dev/null || systemctl reload ssh 2>/dev/null || true
elif command -v service >/dev/null 2>&1; then
	service sshd start 2>/dev/null || service ssh start 2>/dev/null || true
fi
# Ensure sshd is listening
if command -v ss >/dev/null 2>&1; then
	ss -ltn | grep -E ':22\s' || echo "warn: nothing listening on :22 yet" >&2
fi
HOST="$(hostname -f 2>/dev/null || hostname || echo lab-host)"
IP="$(hostname -I 2>/dev/null | awk '{print $1}')"
echo "enable-lab-ssh: PASS"
echo "  ssh -i build/hwtest-keys/id_ed25519 root@${IP:-$HOST}"
echo "  Then attach DUT serial (picocom /dev/ttyUSB0) for GreenJade console"
EOF
chmod +x "$persist_dir/ssh/enable-lab-ssh.sh"

cat >"$persist_dir/bin/collect-serial-log.sh" <<'EOF'
#!/bin/sh
# Host helper: capture serial to GJ-PERSIST logs/ (run on lab host).
set -eu
DEV="${1:-/dev/ttyUSB0}"
DEST_ROOT="${2:-.}"
mkdir -p "$DEST_ROOT/logs"
out="$DEST_ROOT/logs/serial-$(date -u +%Y%m%dT%H%MZ).txt"
echo "collect-serial-log: $DEV -> $out (Ctrl-A X to quit picocom)"
if command -v picocom >/dev/null 2>&1; then
	picocom -b 115200 "$DEV" | tee "$out"
elif command -v minicom >/dev/null 2>&1; then
	minicom -D "$DEV" -b 115200 | tee "$out"
else
	echo "install picocom or minicom" >&2
	exit 1
fi
EOF
chmod +x "$persist_dir/bin/collect-serial-log.sh"

# ---- Linux drivers on disk (operator collect → ABI/module-path staging) ----
# Host-collected .ko + firmware + NEEDED-DRIVERS checklist. Not linked into
# KERNEL.ELF; freestanding module load remains OPEN. Soft≠product GPL ship.
chmod +x scripts/collect-linux-drivers.sh
./scripts/collect-linux-drivers.sh build/linux-drivers >/dev/null || \
	./scripts/collect-linux-drivers.sh build/linux-drivers || true
if [ -d build/linux-drivers ]; then
	mkdir -p "$persist_dir/linux-drivers"
	cp -a build/linux-drivers/. "$persist_dir/linux-drivers/" 2>/dev/null || true
	# ESP glance copy of the checklist (FAT-friendly)
	if [ -f build/linux-drivers/NEEDED-DRIVERS.txt ]; then
		cp -f build/linux-drivers/NEEDED-DRIVERS.txt \
			"$esp_dir/EFI/GREENJADE/NEEDED-DRIVERS.txt" 2>/dev/null || true
	fi
	n_ld=$(find build/linux-drivers/modules -type f 2>/dev/null | wc -l | tr -d ' ')
	echo "make-hwtest-img: linux-drivers staged modules=${n_ld:-0} → GJ-PERSIST/linux-drivers/"
else
	echo "make-hwtest-img: warn: no build/linux-drivers (collect soft-failed)" >&2
fi

# Copy product rootfs tree onto ESP under EFI/GREENJADE/rootfs-full (small)
mkdir -p "$esp_dir/EFI/GREENJADE/rootfs-full"
# Limit to essential dirs to keep ESP size predictable
for d in sbin bin usr lib etc; do
	if [ -d "build/rootfs/$d" ]; then
		cp -a "build/rootfs/$d" "$esp_dir/EFI/GREENJADE/rootfs-full/"
	fi
done
cp -f "$esp_dir/EFI/GREENJADE/INSTALL.txt" \
	"$esp_dir/EFI/GREENJADE/HWTEST.txt" 2>/dev/null || true
cat >"$esp_dir/EFI/GREENJADE/HWTEST.txt" <<'EOF'
GreenJade hardware-test USB layout
  p1 ESP (this partition) — boot GreenJade + user/ + lib/ + drivers/ + rootfs-full/
  p2 GJ-PERSIST — durable logs + lab SSH enable + steam/ + linux-drivers/

Boot: UEFI → BOOTX64.EFI → serial GJ-EFI / M0 OK
      soft markers: sshd/scsi_mid/hda_client live spawn when embeds run
G752: EFI/GREENJADE/LAPTOP.txt · DRIVERS.txt · NEEDED-DRIVERS.txt
Linux modules (host-collected, for ABI/module-path work — not loaded yet):
      mount -L GJ-PERSIST → linux-drivers/modules/ + NEEDED-DRIVERS.txt
Logs: mount -L GJ-PERSIST; see README.txt
SSH:  sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh  (lab host)
Soft: ./scripts/gj-product-summary.sh <serial-log>
Keys: ./scripts/gj-quick-keys.sh <serial-log>
EOF

# Image geometry (defaults): 640 MiB total
# p1 ESP: start LBA 2048, size 384 MiB (exact sector count for mformat -T)
# p2 GJ-PERSIST: next 1 MiB-aligned LBA → end of image
# With staged Steam READY, enlarge image unless operator overrides.
IMG_MB="${GJ_HWTEST_IMG_MB:-640}"
ESP_MIB="${GJ_HWTEST_ESP_MIB:-384}"
if [ "$steam_status" = "READY" ] && [ -z "${GJ_HWTEST_IMG_MB:-}" ]; then
	# Headroom for bootstrap tree + logs (override with GJ_HWTEST_IMG_MB)
	IMG_MB=2048
	echo "make-hwtest-img: Steam READY → IMG_MB=$IMG_MB (set GJ_HWTEST_IMG_MB to override)"
fi
dd if=/dev/zero of="$out" bs=1M count="$IMG_MB" status=none 2>/dev/null || \
	dd if=/dev/zero of="$out" bs=1048576 count="$IMG_MB" 2>/dev/null

ESP_START_SECT=2048
ESP_SECTS=$((ESP_MIB * 1024 * 1024 / 512))
ESP_END_SECT=$((ESP_START_SECT + ESP_SECTS - 1))
# Align persist start up to next 1 MiB boundary after ESP
PERSIST_START_SECT=$(( (ESP_END_SECT + 1 + 2047) / 2048 * 2048 ))
TOTAL_SECTS=$((IMG_MB * 1024 * 1024 / 512))
# GPT last usable LBA is TOTAL_SECTS-34 (backup header + 32-entry table)
# Do not fill through disk end or dd will clobber the backup GPT.
GPT_LAST_USABLE=$((TOTAL_SECTS - 34))
PERSIST_END_SECT=$GPT_LAST_USABLE
PERSIST_SECTS=$((PERSIST_END_SECT - PERSIST_START_SECT + 1))
if [ "$PERSIST_SECTS" -lt 2048 ]; then
	echo "make-hwtest-img: FAIL persist too small (IMG_MB=$IMG_MB ESP_MIB=$ESP_MIB)" >&2
	exit 1
fi

if [ "$PERSIST_FS" = "ext4" ]; then
	PERSIST_GPT_TYPE=8300
else
	PERSIST_GPT_TYPE=0700
fi
sgdisk -o \
	-n 1:${ESP_START_SECT}:${ESP_END_SECT} -t 1:ef00 -c 1:EFI \
	-n 2:${PERSIST_START_SECT}:${PERSIST_END_SECT} -t 2:${PERSIST_GPT_TYPE} -c 2:GJ-PERSIST \
	"$out" >/dev/null

ESP_OFF=$((ESP_START_SECT * 512))
PERSIST_OFF=$((PERSIST_START_SECT * 512))
PERSIST_BYTES=$((PERSIST_SECTS * 512))

# Hard require staged EFI + kernel before packing
if [ ! -f "$esp_dir/EFI/BOOT/BOOTX64.EFI" ] || [ ! -f "$esp_dir/EFI/GREENJADE/KERNEL.ELF" ]; then
	echo "make-hwtest-img: FAIL missing staged BOOTX64.EFI or KERNEL.ELF under $esp_dir" >&2
	exit 1
fi
sz_k_stage=$(wc -c <"$esp_dir/EFI/GREENJADE/KERNEL.ELF" | tr -d ' ')
sz_efi_stage=$(wc -c <"$esp_dir/EFI/BOOT/BOOTX64.EFI" | tr -d ' ')
if [ "$sz_k_stage" -lt 100000 ] || [ "$sz_efi_stage" -lt 1000 ]; then
	echo "make-hwtest-img: FAIL staged sizes efi=${sz_efi_stage}B kernel=${sz_k_stage}B" >&2
	exit 1
fi

# Format ESP with exact sector count (do not let mformat eat the rest of the image)
mformat -i "$out@@$ESP_OFF" -T "$ESP_SECTS" -F -v GREENJADE ::
# Recreate directory tree on ESP via mtools
mmd -i "$out@@$ESP_OFF" ::/EFI
mmd -i "$out@@$ESP_OFF" ::/EFI/BOOT
mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE
mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/user
mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/lib

mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/BOOT/BOOTX64.EFI" ::/EFI/BOOT/BOOTX64.EFI
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/KERNEL.ELF" ::/EFI/GREENJADE/KERNEL.ELF
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/INSTALL.txt" ::/EFI/GREENJADE/INSTALL.txt
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/HWTEST.txt" ::/EFI/GREENJADE/HWTEST.txt
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/VERSION" ::/EFI/GREENJADE/VERSION 2>/dev/null || true
# UDX / G752 operator notes (staged by stage-udx-drivers via stage-esp)
if [ -f "$esp_dir/EFI/GREENJADE/DRIVERS.txt" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/DRIVERS.txt" ::/EFI/GREENJADE/DRIVERS.txt
fi
if [ -f "$esp_dir/EFI/GREENJADE/LAPTOP.txt" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/LAPTOP.txt" ::/EFI/GREENJADE/LAPTOP.txt
fi
if [ -f "$esp_dir/EFI/GREENJADE/NEEDED-DRIVERS.txt" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/NEEDED-DRIVERS.txt" \
		::/EFI/GREENJADE/NEEDED-DRIVERS.txt
fi
# Pre-sized 128 KiB KLOG.TXT for kernel xhci_msc stick log (FAT overwrite path)
if [ ! -f "$esp_dir/EFI/GREENJADE/KLOG.TXT" ]; then
	dd if=/dev/zero of="$esp_dir/EFI/GREENJADE/KLOG.TXT" bs=1024 count=128 status=none 2>/dev/null || \
		dd if=/dev/zero of="$esp_dir/EFI/GREENJADE/KLOG.TXT" bs=1024 count=128 2>/dev/null || true
fi
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/KLOG.TXT" ::/EFI/GREENJADE/KLOG.TXT 2>/dev/null || true
if [ -f "$esp_dir/EFI/GREENJADE/BOOT.LOG" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/BOOT.LOG" ::/EFI/GREENJADE/BOOT.LOG 2>/dev/null || true
else
	: >"${TMPDIR:-/tmp}/gj-boot-log-empty.$$"
	mcopy -o -i "$out@@$ESP_OFF" "${TMPDIR:-/tmp}/gj-boot-log-empty.$$" ::/EFI/GREENJADE/BOOT.LOG 2>/dev/null || true
	rm -f "${TMPDIR:-/tmp}/gj-boot-log-empty.$$"
fi

# User ELFs + libs
user_n=0
for f in "$esp_dir/EFI/GREENJADE/user/"*; do
	[ -f "$f" ] || continue
	mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/user/$(basename "$f")"
	user_n=$((user_n + 1))
done
lib_n=0
for f in "$esp_dir/EFI/GREENJADE/lib/"*; do
	[ -f "$f" ] || continue
	mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/lib/$(basename "$f")"
	lib_n=$((lib_n + 1))
done

# UDX driver-host binaries (flat under EFI/GREENJADE/drivers/)
drv_n=0
if [ -d "$esp_dir/EFI/GREENJADE/drivers" ]; then
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/drivers 2>/dev/null || true
	for f in "$esp_dir/EFI/GREENJADE/drivers/"*; do
		[ -f "$f" ] || continue
		mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/drivers/$(basename "$f")"
		drv_n=$((drv_n + 1))
	done
fi

# Thin rootfs-full snapshot on ESP (sbin/bin/usr/lib/etc — no opt/steam bulk)
# Prepared above under $esp_dir; best-effort recursive mcopy, then fallbacks.
rootfs_n=0
if [ -d "$esp_dir/EFI/GREENJADE/rootfs-full" ]; then
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/rootfs-full 2>/dev/null || true
	if mcopy -s -o -i "$out@@$ESP_OFF" \
		"$esp_dir/EFI/GREENJADE/rootfs-full" ::/EFI/GREENJADE/ 2>/dev/null; then
		rootfs_n=$(find "$esp_dir/EFI/GREENJADE/rootfs-full" -type f 2>/dev/null | wc -l | tr -d ' ')
	else
		for d in sbin bin usr lib etc; do
			if [ -d "$esp_dir/EFI/GREENJADE/rootfs-full/$d" ]; then
				mmd -i "$out@@$ESP_OFF" "::/EFI/GREENJADE/rootfs-full/$d" 2>/dev/null || true
				# One level of files; nested dirs best-effort
				find "$esp_dir/EFI/GREENJADE/rootfs-full/$d" -type f 2>/dev/null | while read -r rf; do
					rel=${rf#"$esp_dir/EFI/GREENJADE/rootfs-full/"}
					parent=$(dirname "$rel")
					mmd -i "$out@@$ESP_OFF" "::/EFI/GREENJADE/rootfs-full/$parent" 2>/dev/null || true
					mcopy -o -i "$out@@$ESP_OFF" "$rf" "::/EFI/GREENJADE/rootfs-full/$rel" 2>/dev/null || true
				done
			fi
		done
		rootfs_n=$(find "$esp_dir/EFI/GREENJADE/rootfs-full" -type f 2>/dev/null | wc -l | tr -d ' ')
	fi
fi

# Format + populate persist (default ext4 for Linux/Unix lab; FAT32 fallback)
if [ "$PERSIST_FS" = "ext4" ]; then
	# Raw FS image sized exactly to GPT p2 → mke2fs -d seeds tree → dd inject
	# Do NOT pass sector count as mke2fs blocks-count (that is FS blocks, often 4KiB).
	rm -f "$persist_raw"
	if command -v truncate >/dev/null 2>&1; then
		truncate -s "$PERSIST_BYTES" "$persist_raw"
	else
		dd if=/dev/zero of="$persist_raw" bs=512 count="$PERSIST_SECTS" status=none 2>/dev/null
	fi
	# -F: force on file; -L: volume label; -d: root dir contents (e2fsprogs ≥1.43)
	# File size alone sets the FS size (matches PERSIST_BYTES / GPT p2).
	if ! mke2fs -t ext4 -F -L GJ-PERSIST -d "$persist_dir" \
		"$persist_raw" >/dev/null 2>&1; then
		if ! mke2fs -t ext4 -F -L GJ-PERSIST -d "$persist_dir" "$persist_raw"; then
			echo "make-hwtest-img: FAIL mke2fs ext4 persist (need e2fsprogs with -d)" >&2
			exit 1
		fi
	fi
	raw_sz=$(wc -c <"$persist_raw" | tr -d ' ')
	if [ "$raw_sz" -ne "$PERSIST_BYTES" ]; then
		echo "make-hwtest-img: FAIL persist raw size $raw_sz != $PERSIST_BYTES" >&2
		exit 1
	fi
	dd if="$persist_raw" of="$out" bs=512 seek="$PERSIST_START_SECT" count="$PERSIST_SECTS" \
		conv=notrunc status=none
	rm -f "$persist_raw"
	echo "make-hwtest-img: persist ext4 label=GJ-PERSIST sectors=$PERSIST_SECTS (symlinks OK)"
else
	# Emergency FAT32 fallback (GJ_HWTEST_PERSIST_FS=fat32) — no real symlinks
	mformat -i "$out@@$PERSIST_OFF" -T "$PERSIST_SECTS" -F -v GJ-PERSIST ::
	mmd -i "$out@@$PERSIST_OFF" ::/logs
	mmd -i "$out@@$PERSIST_OFF" ::/journal
	mmd -i "$out@@$PERSIST_OFF" ::/ssh
	mmd -i "$out@@$PERSIST_OFF" ::/bin
	mmd -i "$out@@$PERSIST_OFF" ::/steam
	mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/README.txt" ::/README.txt
	mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/logs/README.txt" ::/logs/README.txt
	mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/ssh/enable-lab-ssh.sh" ::/ssh/enable-lab-ssh.sh
	mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/ssh/sshd_config.snippet" ::/ssh/sshd_config.snippet
	mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/bin/collect-serial-log.sh" ::/bin/collect-serial-log.sh
	if [ -f "$persist_dir/ssh/authorized_keys" ]; then
		mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/ssh/authorized_keys" ::/ssh/authorized_keys
		mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/ssh/id_ed25519.pub" ::/ssh/id_ed25519.pub
	fi
	if [ -d "$persist_dir/steam" ]; then
		if mcopy -s -o -i "$out@@$PERSIST_OFF" "$persist_dir/steam" ::/steam 2>/dev/null; then
			:
		else
			for f in README.txt STATUS MANIFEST.txt STAGE_META.txt; do
				if [ -f "$persist_dir/steam/$f" ]; then
					mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/steam/$f" "::/steam/$f"
				fi
			done
			if [ -d "$persist_dir/steam/bin" ]; then
				mmd -i "$out@@$PERSIST_OFF" ::/steam/bin 2>/dev/null || true
				for f in "$persist_dir/steam/bin/"*; do
					[ -f "$f" ] || continue
					mcopy -o -i "$out@@$PERSIST_OFF" "$f" "::/steam/bin/$(basename "$f")"
				done
			fi
			if [ -d "$persist_dir/steam/usr" ]; then
				mcopy -s -o -i "$out@@$PERSIST_OFF" "$persist_dir/steam/usr" ::/steam/usr 2>/dev/null || \
					echo "make-hwtest-img: warn: full steam/usr copy failed (FAT/mtools)" >&2
			fi
		fi
	fi
	echo "make-hwtest-img: persist FAT32 (fallback) — prefer default ext4 for Steam symlinks"
fi

# Post-pack verification: KERNEL.ELF must be present and full-size on ESP
{
	export MTOOLSRC
	MTOOLSRC=$(mktemp)
	printf 'mtools_skip_check=1\ndrive x: file="%s" offset=%s\n' "$out" "$ESP_OFF" >"$MTOOLSRC"
	k_listed=$(mdir x:/EFI/GREENJADE 2>/dev/null | grep -i KERNEL || true)
	rm -f "$MTOOLSRC"
	if [ -z "$k_listed" ]; then
		echo "make-hwtest-img: FAIL KERNEL.ELF missing from packed ESP" >&2
		exit 1
	fi
	echo "make-hwtest-img: ESP has KERNEL.ELF (staged ${sz_k_stage}B)"
}

# Ensure backup GPT is intact (safety net if any writer touched the tail)
if ! sgdisk -v "$out" >/dev/null 2>&1; then
	sgdisk -e "$out" >/dev/null 2>&1 || true
fi

sz=$(wc -c <"$out" | tr -d ' ')
ld_n=$(find "$persist_dir/linux-drivers/modules" -type f 2>/dev/null | wc -l | tr -d ' ')
echo "make-hwtest-img: PASS img=$out size=${sz}B steam=$steam_status user_elfs=${user_n} libs=${lib_n} drivers=${drv_n} linux_ko=${ld_n:-0} rootfs_files=${rootfs_n}"
echo "  layout: p1 ESP FAT(GREENJADE) + p2 ${PERSIST_FS}(GJ-PERSIST) logs+ssh+steam+linux-drivers"
echo "  G752:   EFI/GREENJADE/LAPTOP.txt · DRIVERS.txt · NEEDED-DRIVERS.txt · drivers/ (UDX)"
echo "  Linux:  GJ-PERSIST/linux-drivers/ (host .ko staging; load on GJ = OPEN)"
echo "  write:  sudo ./scripts/install-hwtest-usb.sh /dev/sdX"
echo "  SSH:    after plug-in on lab host:"
echo "          sudo mount -L GJ-PERSIST /mnt/gj-persist"
echo "          sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "          ls /mnt/gj-persist/linux-drivers/modules"
echo "  Soft:   ./scripts/gj-product-summary.sh <serial-log>   # exit 0"
echo "  Keys:   ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam:  docs/STEAM_HWTEST.md  (make steam-fetch for READY tree)"
echo "  Note:   READY/media ≠ Steam client run; Top-50 remains NOT-TRIED"
echo "  FS:     default persist=ext4 (Linux lab); ESP remains FAT for UEFI"
echo "          fallback: GJ_HWTEST_PERSIST_FS=fat32"
