#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a dual-partition GPT disk image for real-hardware testing on USB:
#
#   p1  EF00 ESP (FAT32, ~384 MiB)  — BOOTX64.EFI + KERNEL.ELF + rootfs/user
#   p2  0700 data (FAT32, ~256 MiB) — label GJ-PERSIST for durable logs + ssh keys
#                                     + optional prebuilt Steam tree (option 2)
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
# Env: GJ_HWTEST_IMG_MB, GJ_HWTEST_ESP_END_MB — geometry overrides
# Steam READY auto-grows image unless GJ_HWTEST_IMG_MB is set (see below).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/greenjade-hwtest.img}"
esp_dir="${TMPDIR:-/tmp}/gj-hwtest-esp.$$"
persist_dir="${TMPDIR:-/tmp}/gj-hwtest-persist.$$"

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

cleanup() {
	rm -rf "$esp_dir" "$persist_dir"
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

Mount this FAT32 partition on a lab host (label GJ-PERSIST) after a run:

  sudo mkdir -p /mnt/gj-persist
  sudo mount -L GJ-PERSIST /mnt/gj-persist
  ls /mnt/gj-persist/logs

Directories
  logs/      serial captures, dmesg dumps, operator notes
  journal/   structured run journals (JSON/text)
  ssh/       authorized_keys + lab-host enable helpers
  steam/     prebuilt Steam tree (option 2) — STATUS=$steam_status

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
  p1 ESP (this partition) — boot GreenJade + user/ + lib/ + rootfs-full/
  p2 GJ-PERSIST — durable logs + lab SSH enable + steam/

Boot: UEFI → BOOTX64.EFI → serial GJ-EFI / M0 OK
      soft markers: sshd/scsi_mid/hda_client live spawn when embeds run
Logs: mount -L GJ-PERSIST; see README.txt
SSH:  sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh  (lab host)
Soft: ./scripts/gj-product-summary.sh <serial-log>
Keys: ./scripts/gj-quick-keys.sh <serial-log>
EOF

# Image geometry (defaults): 640 MiB total
# p1: 1 MiB .. 385 MiB  (ESP ~384 MiB)
# p2: 385 MiB .. end    (persist ~255 MiB)
# With staged Steam READY, enlarge persist unless operator overrides.
IMG_MB="${GJ_HWTEST_IMG_MB:-640}"
ESP_END_MB="${GJ_HWTEST_ESP_END_MB:-385}"
if [ "$steam_status" = "READY" ] && [ -z "${GJ_HWTEST_IMG_MB:-}" ]; then
	# Headroom for bootstrap tree + logs (override with GJ_HWTEST_IMG_MB)
	IMG_MB=2048
	echo "make-hwtest-img: Steam READY → IMG_MB=$IMG_MB (set GJ_HWTEST_IMG_MB to override)"
fi
dd if=/dev/zero of="$out" bs=1M count="$IMG_MB" status=none 2>/dev/null || \
	dd if=/dev/zero of="$out" bs=1048576 count="$IMG_MB" 2>/dev/null

sgdisk -o \
	-n 1:2048:$((ESP_END_MB * 2048)) -t 1:ef00 -c 1:EFI \
	-n 2:$((ESP_END_MB * 2048 + 1)):0 -t 2:0700 -c 2:GJ-PERSIST \
	"$out" >/dev/null

ESP_OFF=$((2048 * 512))
# persist starts at sector ESP_END_MB * 2048
PERSIST_OFF=$((ESP_END_MB * 1024 * 1024))

# Format ESP
mformat -i "$out@@$ESP_OFF" -F -v GREENJADE ::
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

# Format persist + copy tree (mcopy -s recursive when available)
mformat -i "$out@@$PERSIST_OFF" -F -v GJ-PERSIST ::
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
# Steam tree (option 2) — recursive copy when mcopy -s available
if [ -d "$persist_dir/steam" ]; then
	if mcopy -s -o -i "$out@@$PERSIST_OFF" "$persist_dir/steam" ::/steam 2>/dev/null; then
		:
	else
		# Fallback: top-level text + known files
		for f in README.txt STATUS MANIFEST.txt; do
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
		# Best-effort usr tree (bootstrap can be large)
		if [ -d "$persist_dir/steam/usr" ] && command -v mcopy >/dev/null 2>&1; then
			mcopy -s -o -i "$out@@$PERSIST_OFF" "$persist_dir/steam/usr" ::/steam/usr 2>/dev/null || \
				echo "make-hwtest-img: warn: full steam/usr copy failed (size/mtools); STATUS still on media" >&2
		fi
	fi
fi

sz=$(wc -c <"$out" | tr -d ' ')
echo "make-hwtest-img: PASS img=$out size=${sz}B steam=$steam_status user_elfs=${user_n} libs=${lib_n} rootfs_files=${rootfs_n}"
echo "  layout: p1 ESP(GREENJADE) + p2 FAT32(GJ-PERSIST) logs+ssh+steam"
echo "  write:  sudo ./scripts/install-hwtest-usb.sh /dev/sdX"
echo "  SSH:    after plug-in on lab host:"
echo "          sudo mount -L GJ-PERSIST /mnt/gj-persist"
echo "          sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "  Soft:   ./scripts/gj-product-summary.sh <serial-log>   # exit 0"
echo "  Keys:   ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam:  docs/STEAM_HWTEST.md  (make steam-fetch for READY tree)"
echo "  Note:   READY/media ≠ Steam client run; Top-50 remains NOT-TRIED"
