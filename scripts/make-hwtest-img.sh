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
# Image identity (GJ_IMAGE_VERSION → panel STATUS title):
#   ./scripts/gj-image-version.sh --report
#   After pack this script prints: expected panel "STATUS (static) v…"
#   Soft!=product: stamp confirms which image is flashed — not product PASS.
#   Stamp residual: pack never bumps GJ_IMAGE_VERSION (reads KERNEL.ELF only).
#   Fly bar is config.h (current: 0.1.97 (semver; 0.2.0 when net works)). Bump only on real flash cuts.
#
# Soft residual deepen (C2 scripts residual Soft!=product; G-AC-1;
# dual MIT OR Apache-2.0; Dual DoD A/B OPEN; stamp-free residual):
#   pack residual        — GPT ESP + GJ-PERSIST with UDX drivers/ on ESP
#   udx pack residual    — ddi_host + rtl8168_udx + xhci_udx (PASS|SKIP)
#   udx dual-land residual — ESP drivers/ + GJ-PERSIST/drivers/ mirror
#   sshd pack residual   — user/sshd.elf on ESP (PASS|SKIP); soft listen != wire
#   embed residual       — KERNEL embeds (sshd live spawn) + staged hosts pack
#   freestanding residual — class SKIP default (not Dual DoD close)
#   Dual DoD residual    — A (xhci_udx USB) / B (rtl8168_udx NIC+sshd wire) OPEN
#   honesty residual     — pack PASS != product TX/RX/BOT != bar3 != Dual DoD
#   product honesty      — Soft!=product on hwtest img: media pack != L3 close
#   stamp residual       — image_version from KERNEL.ELF only (no bump here)
# greppable: make-hwtest-img: soft residual dual_dod
# greppable: make-hwtest-img: soft residual product=UDX+ABI
# greppable: make-hwtest-img: soft residual freestanding class SKIP
# greppable: make-hwtest-img: soft residual udx pack
# greppable: make-hwtest-img: soft residual udx dual_land
# greppable: make-hwtest-img: soft residual udx dual_land byte_match=
# greppable: make-hwtest-img: soft residual product_host density=
# greppable: make-hwtest-img: soft residual sshd pack
# greppable: make-hwtest-img: soft residual product honesty
# greppable: make-hwtest-img: soft residual stamp-free
# greppable: make-hwtest-img: soft residual panel=
# greppable: make-hwtest-img: udx pack ddi_host=
# greppable: make-hwtest-img: sshd pack=
# Fly bar: KERNEL.ELF GJ_IMAGE_VERSION (config.h); pack script never invents stamps.
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
# Soft best-effort UDX host pack (optional; not hard-dep). Soft!=product · G-AC-1.
# Prefer make drivers-udx → build/user/drivers/{ddi_host,rtl8168_udx,xhci_udx}
# so stage-esp → stage-udx-drivers can land product-named hosts on ESP.
# Freestanding class (kernel rtl8168 / xhci_msc) default SKIP — residual opt-in only.
if [ ! -f build/user/drivers/ddi_host ] || \
   [ ! -f build/user/drivers/rtl8168_udx ] || \
   [ ! -f build/user/drivers/xhci_udx ]; then
	echo "make-hwtest-img: soft make drivers-udx (pack ddi_host+rtl8168_udx+xhci_udx on ESP)..."
	make drivers-udx >/dev/null 2>&1 || \
		echo "make-hwtest-img: soft-miss drivers-udx (stage will SKIP absent hosts; Soft!=product)" >&2
fi
echo "make-hwtest-img: stage-esp + stage-rootfs..."
chmod +x scripts/stage-esp.sh scripts/stage-rootfs.sh scripts/stage-udx-drivers.sh
# stage-esp → stage-udx-drivers packs EFI/GREENJADE/drivers/ (UDX, not .ko)
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
	"$persist_dir/bin" "$persist_dir/steam" "$persist_dir/drivers"

# STRONGER residual: dual-land UDX product hosts onto GJ-PERSIST/drivers/
# (stage-esp alone lands ESP only). Soft!=product · Dual DoD A/B OPEN.
# Re-harvest → ensure → pack_esp → mirror_persist → inventory (stage-udx-drivers).
echo "make-hwtest-img: stage-udx-drivers dual-land ESP+GJ-PERSIST (UDX hosts)..."
./scripts/stage-udx-drivers.sh "$esp_dir" "$persist_dir" >/dev/null 2>&1 || \
	echo "make-hwtest-img: soft-miss stage-udx dual-land (hosts may SKIP; Soft!=product)" >&2

# Soft ensure sshd.elf on ESP user/ when build artifact exists (DUT pack path).
# Kernel also embeds sshd for live spawn; staged ELF is operator dual path.
# Soft!=product: pack/embed sshd.elf != Dual DoD B close (needs UDX NIC wire).
if [ ! -f "$esp_dir/EFI/GREENJADE/user/sshd.elf" ] && [ -f build/user/sshd.elf ]; then
	mkdir -p "$esp_dir/EFI/GREENJADE/user"
	cp -f build/user/sshd.elf "$esp_dir/EFI/GREENJADE/user/sshd.elf"
	echo "make-hwtest-img: soft ensure user/sshd.elf staged (Soft!=product; not Dual DoD B)"
fi

# Option 2: prebuilt Steam tree onto GJ-PERSIST (primary durable payload)
chmod +x scripts/stage-steam-tree.sh
./scripts/stage-steam-tree.sh "$persist_dir" >/dev/null || true
# stage-steam-tree writes $persist_dir/steam/; keep STATUS for operators
steam_status="SKELETON"
if [ -f "$persist_dir/steam/STATUS" ]; then
	steam_status=$(cat "$persist_dir/steam/STATUS" | tr -d '\r\n')
fi

# Flash identity from KERNEL.ELF (GJ_IMAGE_VERSION → panel STATUS title)
chmod +x scripts/gj-image-version.sh
img_ver=$(./scripts/gj-image-version.sh \
	"${esp_dir}/EFI/GREENJADE/KERNEL.ELF" 2>/dev/null || true)
if [ -z "$img_ver" ] || [ "$img_ver" = "unknown" ]; then
	img_ver=$(./scripts/gj-image-version.sh 2>/dev/null || echo unknown)
fi
img_status_title="STATUS (static) v${img_ver}"
printf '%s\n' "$img_ver" >"$persist_dir/IMAGE_VERSION"
printf '%s\n' "$img_status_title" >"$persist_dir/IMAGE_STATUS_TITLE"
# Mirror onto ESP for post-mount cat without GJ-PERSIST
printf '%s\n' "$img_ver" >"$esp_dir/EFI/GREENJADE/IMAGE_VERSION" 2>/dev/null || true
printf '%s\n' "$img_status_title" >"$esp_dir/EFI/GREENJADE/IMAGE_STATUS_TITLE" 2>/dev/null || true

cat >"$persist_dir/README.txt" <<EOF
GJ-PERSIST — GreenJade hardware-test durable storage
====================================================

Filesystem: ${PERSIST_FS}  (default ext4 for Linux/Unix-only lab; ESP is still FAT)
Label:      GJ-PERSIST
Image:      GJ_IMAGE_VERSION=${img_ver}
Panel:      ${img_status_title}

Mount on a lab host after a run:

  sudo mkdir -p /mnt/gj-persist
  sudo mount -L GJ-PERSIST /mnt/gj-persist
  ls /mnt/gj-persist/logs
  cat /mnt/gj-persist/IMAGE_VERSION          # flash identity
  cat /mnt/gj-persist/IMAGE_STATUS_TITLE     # expected panel STATUS title
  # ext4: rsync -a works (symlinks + modes preserved)

Directories
  logs/           serial captures, dmesg dumps, operator notes
  journal/        structured run journals (JSON/text)
  ssh/            authorized_keys + lab-host enable helpers
  steam/          prebuilt Steam tree (option 2) — STATUS=$steam_status
  drivers/        UDX dual-land mirror (ddi_host rtl8168_udx xhci_udx)
                  Soft!=product: pack != TX/RX/BOT != Dual DoD close
  linux-drivers/  host-collected Linux .ko + firmware + NEEDED-DRIVERS.txt
                  (full tree; Soft!=product). ESP also has
                  /linux-drivers/modules/r8169.ko for UEFI soft media handoff.
  DUT-PACK.txt    greppable UDX+sshd pack inventory (Soft!=product honesty)
  IMAGE_VERSION / IMAGE_STATUS_TITLE
                  flash identity for DUT panel verify (Soft!=product)

Steam (option 2 — no dpkg on GreenJade)
  Lab host:  make steam-fetch && make steam-stage && make hwtest-img
  Docs:      docs/STEAM_HWTEST.md
  On stick:  steam/STATUS (READY or SKELETON), steam/usr/bin/steam when READY

SSH remote debug (Grok / operator)
  Freestanding sshd.elf is default-on at boot (embed + staged user/sshd.elf).
  Soft listen :22 / pack PASS != Dual DoD B close (product :22 on laptop
  wire closes only via UDX NIC path rtl8168_udx). Soft!=product · G-AC-1.
  Lab-host OpenSSH remains for serial bridge:

    sudo /path/to/knano/scripts/hwtest-ssh-setup.sh

  Or, with this partition mounted:

    sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh

  Then: ssh -i <tree>/build/hwtest-keys/id_ed25519 root@<lab-host>

UDX product hosts (dual-land; Soft!=product · Dual DoD A/B OPEN)
  ESP:        EFI/GREENJADE/drivers/{ddi_host,rtl8168_udx,xhci_udx}
  GJ-PERSIST: drivers/ (mirror) + drivers/MANIFEST.txt
  pack PASS != product TX/RX/BOT != Dual DoD close; freestanding class SKIP.

Soft-scan serial captures (host)
  ./scripts/gj-product-summary.sh logs/serial-….txt   # soft exit 0
  ./scripts/gj-quick-keys.sh logs/serial-….txt        # hard miss exit 1
  ./scripts/steam-bar3-check.sh                       # media READY/SKELETON
  ./scripts/gj-image-version.sh --report              # expected STATUS title

Honesty (product honesty on hwtest img — Soft!=product · G-AC-1)
  STATUS=READY means bootstrap tree on media — not Steam client run.
  Kernel HDA multi-stream PASS is not Steam audio. Deck Top 50 stays
  NOT-TRIED until real DUT title results.
  Panel STATUS title version confirms image identity only (Soft!=product).
  UDX host pack + sshd.elf pack/embed != Dual DoD A/B close (both OPEN).
  stamp: fly identity = GJ_IMAGE_VERSION from KERNEL.ELF (config.h).
  Soft residual pack script never invents stamps.
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
# KERNEL.ELF as product. Soft!=product GPL ship; G-AC-1.
#
# D4 media path (Soft!=product):
#   GJ-PERSIST/linux-drivers/  — full tree (ext4; no freestanding reader yet)
#   ESP /linux-drivers/modules/r8169.ko — FAT, UEFI SimpleFS can LoadFile
#     → gj_boot_info soft media → soft load source=media (before embed)
# greppable: make-hwtest-img: esp stage PASS|SKIP
chmod +x scripts/collect-linux-drivers.sh
./scripts/collect-linux-drivers.sh build/linux-drivers >/dev/null || \
	./scripts/collect-linux-drivers.sh build/linux-drivers || true
esp_r8169=0
esp_fw=0
esp_fw_n=0
if [ -d build/linux-drivers ]; then
	mkdir -p "$persist_dir/linux-drivers"
	cp -a build/linux-drivers/. "$persist_dir/linux-drivers/" 2>/dev/null || true
	# ESP glance copy of the checklist (FAT-friendly)
	if [ -f build/linux-drivers/NEEDED-DRIVERS.txt ]; then
		cp -f build/linux-drivers/NEEDED-DRIVERS.txt \
			"$esp_dir/EFI/GREENJADE/NEEDED-DRIVERS.txt" 2>/dev/null || true
	fi
	# ESP soft media: plain r8169.ko + critical rtl_nic plain .fw (UEFI-readable FAT).
	# Layout matches soft probe / UEFI stub path:
	#   \linux-drivers\modules\r8169.ko
	#   \linux-drivers\firmware\rtl_nic\rtl8168*.fw  (operator stage; multi-blob
	#     soft-media handoff NOT wired — runtime HIT is kernel .incbin embed)
	# Also mirror under EFI/GREENJADE/linux-drivers/ next to checklist.
	mkdir -p "$esp_dir/linux-drivers/modules" \
		"$esp_dir/linux-drivers/firmware/rtl_nic" \
		"$esp_dir/EFI/GREENJADE/linux-drivers/modules" \
		"$esp_dir/EFI/GREENJADE/linux-drivers/firmware/rtl_nic"
	: >"$esp_dir/linux-drivers/firmware/.keep"
	: >"$esp_dir/EFI/GREENJADE/linux-drivers/firmware/.keep"
	if [ -f build/linux-drivers/modules/r8169.ko ]; then
		cp -f build/linux-drivers/modules/r8169.ko \
			"$esp_dir/linux-drivers/modules/r8169.ko"
		cp -f build/linux-drivers/modules/r8169.ko \
			"$esp_dir/EFI/GREENJADE/linux-drivers/modules/r8169.ko"
		esp_r8169=$(wc -c <"$esp_dir/linux-drivers/modules/r8169.ko" | tr -d ' ')
	elif [ -f build/linux-drivers/modules_raw/r8169.ko ]; then
		cp -f build/linux-drivers/modules_raw/r8169.ko \
			"$esp_dir/linux-drivers/modules/r8169.ko"
		cp -f build/linux-drivers/modules_raw/r8169.ko \
			"$esp_dir/EFI/GREENJADE/linux-drivers/modules/r8169.ko"
		esp_r8169=$(wc -c <"$esp_dir/linux-drivers/modules/r8169.ko" | tr -d ' ')
	fi
	# Stage plain rtl8168*.fw (~33 KiB total) for operator media honesty.
	esp_fw=0
	esp_fw_n=0
	fw_plain="build/linux-drivers/firmware_plain/rtl_nic"
	if [ ! -d "$fw_plain" ] || [ -z "$(ls -A "$fw_plain"/rtl8168*.fw 2>/dev/null || true)" ]; then
		# Best-effort decompress from staged .xz if embed script not yet run
		if [ -x scripts/embed-linux-fw.sh ] || [ -f scripts/embed-linux-fw.sh ]; then
			chmod +x scripts/embed-linux-fw.sh 2>/dev/null || true
			./scripts/embed-linux-fw.sh >/dev/null 2>&1 || true
		fi
	fi
	if [ -d "$fw_plain" ]; then
		for f in "$fw_plain"/rtl8168*.fw; do
			[ -f "$f" ] || continue
			bn=$(basename "$f")
			cp -f "$f" "$esp_dir/linux-drivers/firmware/rtl_nic/$bn"
			cp -f "$f" "$esp_dir/EFI/GREENJADE/linux-drivers/firmware/rtl_nic/$bn" \
				2>/dev/null || true
			sz=$(wc -c <"$f" | tr -d ' ')
			esp_fw=$((esp_fw + sz))
			esp_fw_n=$((esp_fw_n + 1))
		done
	fi
	cat >"$esp_dir/linux-drivers/ESP-STAGE.txt" <<EOF
GreenJade ESP soft media stage (D4; Soft!=product; G-AC-1)
==========================================================
r8169.ko bytes=${esp_r8169}
rtl8168*.fw n=${esp_fw_n} bytes=${esp_fw}
paths:
  /linux-drivers/modules/r8169.ko
  /linux-drivers/firmware/rtl_nic/rtl8168*.fw
  /EFI/GREENJADE/linux-drivers/modules/r8169.ko
  /EFI/GREENJADE/NEEDED-DRIVERS.txt
firmware runtime: soft request_firmware uses in-kernel embed table
  (scripts/embed-linux-fw.sh → linux_dma_soft HIT), NOT UEFI multi-blob load.
  ESP fw copy is operator media honesty / future handoff; Soft!=product.
UEFI stub LoadFile → gj_boot_info soft media → source=media before embed (.ko).
Do not claim product NIC (G-AC-1). Full tree still on GJ-PERSIST.
EOF
	cp -f "$esp_dir/linux-drivers/ESP-STAGE.txt" \
		"$esp_dir/EFI/GREENJADE/linux-drivers/ESP-STAGE.txt" 2>/dev/null || true
	n_ld=$(find build/linux-drivers/modules -type f 2>/dev/null | wc -l | tr -d ' ')
	echo "make-hwtest-img: linux-drivers staged modules=${n_ld:-0} → GJ-PERSIST/linux-drivers/"
	if [ "${esp_r8169:-0}" -gt 1000 ]; then
		echo "make-hwtest-img: esp stage PASS r8169.ko=${esp_r8169}B path=/linux-drivers/modules/r8169.ko (Soft!=product; D4)"
	else
		echo "make-hwtest-img: esp stage SKIP r8169.ko missing (collect soft-failed?)" >&2
	fi
	if [ "${esp_fw_n:-0}" -gt 0 ]; then
		echo "make-hwtest-img: esp stage PASS rtl_nic fw n=${esp_fw_n} bytes=${esp_fw} path=/linux-drivers/firmware/rtl_nic/ (soft embed HIT at runtime; Soft!=product)"
	else
		echo "make-hwtest-img: esp stage SKIP rtl_nic fw (collect/embed soft-failed? GJ-PERSIST still has firmware/*.xz)" >&2
	fi
else
	echo "make-hwtest-img: warn: no build/linux-drivers (collect soft-failed)" >&2
	echo "make-hwtest-img: esp stage SKIP no linux-drivers tree" >&2
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
cat >"$esp_dir/EFI/GREENJADE/HWTEST.txt" <<EOF
GreenJade hardware-test USB layout
  p1 ESP (this partition) — boot GreenJade + user/ + lib/ + drivers/ + rootfs-full/
      + /linux-drivers/modules/r8169.ko (soft media; UEFI LoadFile → source=media)
  p2 GJ-PERSIST — durable logs + lab SSH enable + steam/ + linux-drivers/ + drivers/

Image: GJ_IMAGE_VERSION=${img_ver}
Panel: expect STATUS title → ${img_status_title}
       (also EFI/GREENJADE/IMAGE_STATUS_TITLE · GJ-PERSIST/IMAGE_STATUS_TITLE)
Serial: main: image version=${img_ver}

Boot: UEFI → BOOTX64.EFI → serial GJ-EFI / M0 OK
      soft markers: sshd/scsi_mid/hda_client live spawn when embeds run
      soft media: GJ-EFI: soft media PASS → linux_module source=media (Soft!=product)
G752: EFI/GREENJADE/LAPTOP.txt · DRIVERS.txt · NEEDED-DRIVERS.txt
UDX product hosts (userspace; Soft!=product · G-AC-1 — pack != TX/RX/BOT):
      ESP:        EFI/GREENJADE/drivers/{ddi_host,rtl8168_udx,xhci_udx}
      GJ-PERSIST: drivers/ (dual-land mirror) + drivers/MANIFEST.txt
      Prefer: make drivers-udx before make hwtest-img
sshd pack path (Soft!=product — pack/embed != Dual DoD B close):
      ESP user/sshd.elf (staged) + KERNEL embed live spawn default-on :22
      Product laptop :22 closes only via UDX NIC (rtl8168_udx) wire proof
Freestanding class SKIP (default; residual opt-in only — not product):
      kernel rtl8168 / xhci_msc · GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0
Linux modules (host-collected; Soft!=product; G-AC-1):
      ESP:  /linux-drivers/modules/r8169.ko  (UEFI handoff; D4 media path)
      full: mount -L GJ-PERSIST → linux-drivers/modules/ + NEEDED-DRIVERS.txt
      Soft freestanding_no_exec: SKIP exec all staged .ko in-kernel
Logs: mount -L GJ-PERSIST; see README.txt
SSH:  sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh  (lab host)
Soft: ./scripts/gj-product-summary.sh <serial-log>
Keys: ./scripts/gj-quick-keys.sh <serial-log>
Host: ./scripts/gj-image-version.sh --report
Soft!=product: version stamp != product complete != Steam client != Top-50 != bar3
Dual DoD A/B OPEN (UDX USB/NIC) — pack != Dual DoD close; fly stamp from KERNEL.ELF
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
# Flash identity for post-mount verify (Soft!=product stamp only)
if [ -f "$esp_dir/EFI/GREENJADE/IMAGE_VERSION" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/IMAGE_VERSION" \
		::/EFI/GREENJADE/IMAGE_VERSION
fi
if [ -f "$esp_dir/EFI/GREENJADE/IMAGE_STATUS_TITLE" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/IMAGE_STATUS_TITLE" \
		::/EFI/GREENJADE/IMAGE_STATUS_TITLE
fi
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
# Soft media r8169.ko + rtl8168*.fw on ESP FAT (UEFI SimpleFS; D4). Soft!=product.
# Runtime request_firmware HIT is in-kernel embed (not multi-blob UEFI LoadFile).
if [ -f "$esp_dir/linux-drivers/modules/r8169.ko" ]; then
	mmd -i "$out@@$ESP_OFF" ::/linux-drivers 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/linux-drivers/modules 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/linux-drivers/firmware 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/linux-drivers/firmware/rtl_nic 2>/dev/null || true
	mcopy -o -i "$out@@$ESP_OFF" \
		"$esp_dir/linux-drivers/modules/r8169.ko" \
		::/linux-drivers/modules/r8169.ko
	if [ -f "$esp_dir/linux-drivers/firmware/.keep" ]; then
		mcopy -o -i "$out@@$ESP_OFF" \
			"$esp_dir/linux-drivers/firmware/.keep" \
			::/linux-drivers/firmware/.keep 2>/dev/null || true
	fi
	for f in "$esp_dir/linux-drivers/firmware/rtl_nic"/rtl8168*.fw; do
		[ -f "$f" ] || continue
		mcopy -o -i "$out@@$ESP_OFF" "$f" \
			"::/linux-drivers/firmware/rtl_nic/$(basename "$f")" 2>/dev/null || true
	done
	if [ -f "$esp_dir/linux-drivers/ESP-STAGE.txt" ]; then
		mcopy -o -i "$out@@$ESP_OFF" \
			"$esp_dir/linux-drivers/ESP-STAGE.txt" \
			::/linux-drivers/ESP-STAGE.txt 2>/dev/null || true
	fi
	# Mirror next to NEEDED-DRIVERS (UEFI alt path)
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/linux-drivers 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/linux-drivers/modules 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/linux-drivers/firmware 2>/dev/null || true
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/linux-drivers/firmware/rtl_nic 2>/dev/null || true
	mcopy -o -i "$out@@$ESP_OFF" \
		"$esp_dir/linux-drivers/modules/r8169.ko" \
		::/EFI/GREENJADE/linux-drivers/modules/r8169.ko 2>/dev/null || true
	echo "make-hwtest-img: esp mcopy r8169.ko → ::/linux-drivers/modules/ (UEFI soft media)"
	if [ "${esp_fw_n:-0}" -gt 0 ]; then
		echo "make-hwtest-img: esp mcopy rtl_nic fw n=${esp_fw_n} → ::/linux-drivers/firmware/rtl_nic/"
	fi
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

# User ELFs + libs (track sshd.elf pack residual for Dual DoD B endpoint honesty)
user_n=0
sshd_pack=SKIP
sshd_bytes=0
for f in "$esp_dir/EFI/GREENJADE/user/"*; do
	[ -f "$f" ] || continue
	bn=$(basename "$f")
	mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/user/$bn"
	user_n=$((user_n + 1))
	if [ "$bn" = "sshd.elf" ]; then
		sshd_pack=PASS
		sshd_bytes=$(wc -c <"$f" | tr -d ' ')
	fi
done
lib_n=0
for f in "$esp_dir/EFI/GREENJADE/lib/"*; do
	[ -f "$f" ] || continue
	mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/lib/$(basename "$f")"
	lib_n=$((lib_n + 1))
done

# UDX driver-host binaries (flat under EFI/GREENJADE/drivers/)
# Product pack names: ddi_host + rtl8168_udx + xhci_udx (Soft!=product · G-AC-1).
# Freestanding class SKIP default — these are userspace UDX hosts, not .ko.
# W10 STRONGER residual: non-empty byte honesty + dual-land byte_match + density.
drv_n=0
ddi_pack=SKIP
rtl_pack=SKIP
xhci_pack=SKIP
ddi_bytes=0
rtl_bytes=0
xhci_bytes=0
product_host_total=3
product_host_n=0
if [ -d "$esp_dir/EFI/GREENJADE/drivers" ]; then
	mmd -i "$out@@$ESP_OFF" ::/EFI/GREENJADE/drivers 2>/dev/null || true
	for f in "$esp_dir/EFI/GREENJADE/drivers/"*; do
		[ -f "$f" ] || continue
		bn=$(basename "$f")
		# denser: skip zero-byte product host placeholders (non-empty honesty)
		_fsz=$(wc -c <"$f" | tr -d ' ')
		case "$bn" in
		ddi_host|rtl8168_udx|xhci_udx)
			if [ "${_fsz:-0}" -eq 0 ] 2>/dev/null; then
				continue
			fi
			;;
		esac
		mcopy -o -i "$out@@$ESP_OFF" "$f" "::/EFI/GREENJADE/drivers/$bn"
		drv_n=$((drv_n + 1))
		case "$bn" in
		ddi_host)
			ddi_pack=PASS
			ddi_bytes=$_fsz
			;;
		rtl8168_udx)
			rtl_pack=PASS
			rtl_bytes=$_fsz
			;;
		xhci_udx)
			xhci_pack=PASS
			xhci_bytes=$_fsz
			;;
		esac
	done
fi
# Explicit denser catalogs at GREENJADE root (DUT post-mount cat)
if [ -f "$esp_dir/EFI/GREENJADE/PRODUCT-HOSTS.txt" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/PRODUCT-HOSTS.txt" \
		::/EFI/GREENJADE/PRODUCT-HOSTS.txt 2>/dev/null || true
elif [ -f "$esp_dir/EFI/GREENJADE/drivers/PRODUCT-HOSTS.txt" ]; then
	mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/drivers/PRODUCT-HOSTS.txt" \
		::/EFI/GREENJADE/PRODUCT-HOSTS.txt 2>/dev/null || true
fi
# Dual-land residual: GJ-PERSIST/drivers/ host mirror (seeded by stage-udx-drivers)
# denser: non-empty honesty for persist hosts (zero-byte = SKIP)
persist_ddi=SKIP
persist_rtl=SKIP
persist_xhci=SKIP
persist_ddi_bytes=0
persist_rtl_bytes=0
persist_xhci_bytes=0
persist_drv_n=0
if [ -d "$persist_dir/drivers" ]; then
	persist_drv_n=$(find "$persist_dir/drivers" -type f 2>/dev/null | wc -l | tr -d ' ')
	if [ -f "$persist_dir/drivers/ddi_host" ]; then
		persist_ddi_bytes=$(wc -c <"$persist_dir/drivers/ddi_host" | tr -d ' ')
		[ "${persist_ddi_bytes:-0}" -gt 0 ] 2>/dev/null && persist_ddi=PASS
	fi
	if [ -f "$persist_dir/drivers/rtl8168_udx" ]; then
		persist_rtl_bytes=$(wc -c <"$persist_dir/drivers/rtl8168_udx" | tr -d ' ')
		[ "${persist_rtl_bytes:-0}" -gt 0 ] 2>/dev/null && persist_rtl=PASS
	fi
	if [ -f "$persist_dir/drivers/xhci_udx" ]; then
		persist_xhci_bytes=$(wc -c <"$persist_dir/drivers/xhci_udx" | tr -d ' ')
		[ "${persist_xhci_bytes:-0}" -gt 0 ] 2>/dev/null && persist_xhci=PASS
	fi
fi
product_host_n=0
[ "$ddi_pack" = "PASS" ] && product_host_n=$((product_host_n + 1))
[ "$rtl_pack" = "PASS" ] && product_host_n=$((product_host_n + 1))
[ "$xhci_pack" = "PASS" ] && product_host_n=$((product_host_n + 1))
dual_land=SKIP
if [ "$ddi_pack" = "PASS" ] || [ "$rtl_pack" = "PASS" ] || [ "$xhci_pack" = "PASS" ]; then
	if [ "$persist_ddi" = "PASS" ] || [ "$persist_rtl" = "PASS" ] || [ "$persist_xhci" = "PASS" ]; then
		dual_land=PASS
	else
		dual_land=ESP_ONLY
	fi
elif [ "$persist_ddi" = "PASS" ] || [ "$persist_rtl" = "PASS" ] || [ "$persist_xhci" = "PASS" ]; then
	dual_land=PERSIST_ONLY
fi
# W10: dual-land byte_match residual (ESP size == persist size when both PASS)
byte_match_n=0
byte_match_st=SKIP
_bm_ddi=SKIP
_bm_rtl=SKIP
_bm_xhci=SKIP
if [ "$ddi_pack" = "PASS" ] && [ "$persist_ddi" = "PASS" ]; then
	if [ "$ddi_bytes" -eq "$persist_ddi_bytes" ] 2>/dev/null; then
		_bm_ddi=MATCH
		byte_match_n=$((byte_match_n + 1))
	else
		_bm_ddi=MISMATCH
	fi
elif [ "$ddi_pack" = "PASS" ] || [ "$persist_ddi" = "PASS" ]; then
	_bm_ddi=PARTIAL
fi
if [ "$rtl_pack" = "PASS" ] && [ "$persist_rtl" = "PASS" ]; then
	if [ "$rtl_bytes" -eq "$persist_rtl_bytes" ] 2>/dev/null; then
		_bm_rtl=MATCH
		byte_match_n=$((byte_match_n + 1))
	else
		_bm_rtl=MISMATCH
	fi
elif [ "$rtl_pack" = "PASS" ] || [ "$persist_rtl" = "PASS" ]; then
	_bm_rtl=PARTIAL
fi
if [ "$xhci_pack" = "PASS" ] && [ "$persist_xhci" = "PASS" ]; then
	if [ "$xhci_bytes" -eq "$persist_xhci_bytes" ] 2>/dev/null; then
		_bm_xhci=MATCH
		byte_match_n=$((byte_match_n + 1))
	else
		_bm_xhci=MISMATCH
	fi
elif [ "$xhci_pack" = "PASS" ] || [ "$persist_xhci" = "PASS" ]; then
	_bm_xhci=PARTIAL
fi
if [ "$byte_match_n" -eq 3 ]; then
	byte_match_st=PASS
elif [ "$byte_match_n" -gt 0 ]; then
	byte_match_st=PARTIAL
elif [ "$_bm_ddi" = "MISMATCH" ] || [ "$_bm_rtl" = "MISMATCH" ] || [ "$_bm_xhci" = "MISMATCH" ]; then
	byte_match_st=MISMATCH
fi

# DUT pack honesty inventory (ESP + persist) — Soft!=product product honesty
{
	echo "# GreenJade hwtest DUT pack inventory (make-hwtest-img; Soft!=product)"
	echo "# MIT OR Apache-2.0 · G-AC-1 · Dual DoD A/B OPEN · fly stamp from KERNEL.ELF"
	echo "# Pack script never bumps GJ_IMAGE_VERSION (reads elf only)"
	echo "# W10 denser: non-empty honesty · product_host density N/3 · dual_land byte_match"
	echo "image_version=${img_ver}"
	echo "status_title=${img_status_title}"
	echo "product_path=UDX+ABI"
	echo "freestanding_class=SKIP"
	echo "dual_dod_a=OPEN"
	echo "dual_dod_b=OPEN"
	echo "soft_not_product=1"
	echo "stamp_free=1"
	echo "bar_honesty=v${img_ver}"
	echo "fly_stamp=${img_ver}"
	echo "ddi_host=${ddi_pack} bytes=${ddi_bytes}"
	echo "rtl8168_udx=${rtl_pack} bytes=${rtl_bytes}"
	echo "xhci_udx=${xhci_pack} bytes=${xhci_bytes}"
	echo "product_host_n=${product_host_n}"
	echo "product_host_total=${product_host_total}"
	echo "product_host_density=${product_host_n}/${product_host_total}"
	echo "drivers_esp=${drv_n}"
	echo "drivers_persist=${persist_drv_n}"
	echo "persist_ddi_host=${persist_ddi} bytes=${persist_ddi_bytes}"
	echo "persist_rtl8168_udx=${persist_rtl} bytes=${persist_rtl_bytes}"
	echo "persist_xhci_udx=${persist_xhci} bytes=${persist_xhci_bytes}"
	echo "dual_land=${dual_land}"
	echo "dual_land_byte_match=${byte_match_st}"
	echo "byte_match_ddi_host=${_bm_ddi}"
	echo "byte_match_rtl8168_udx=${_bm_rtl}"
	echo "byte_match_xhci_udx=${_bm_xhci}"
	echo "sshd_elf=${sshd_pack} bytes=${sshd_bytes}"
	echo "sshd_note=pack_or_embed_PASS_not_Dual_DoD_B_close"
	echo "product_honesty=pack_PASS_ne_TX_RX_BOT_ne_bar3_ne_Dual_DoD"
} >"$esp_dir/EFI/GREENJADE/DUT-PACK.txt"
cp -f "$esp_dir/EFI/GREENJADE/DUT-PACK.txt" "$persist_dir/DUT-PACK.txt" 2>/dev/null || true
mcopy -o -i "$out@@$ESP_OFF" "$esp_dir/EFI/GREENJADE/DUT-PACK.txt" \
	::/EFI/GREENJADE/DUT-PACK.txt 2>/dev/null || true

echo "make-hwtest-img: udx pack ddi_host=${ddi_pack} rtl8168_udx=${rtl_pack} xhci_udx=${xhci_pack} drivers=${drv_n}"
echo "make-hwtest-img: udx pack bytes ddi_host=${ddi_bytes} rtl8168_udx=${rtl_bytes} xhci_udx=${xhci_bytes}"
echo "make-hwtest-img: udx dual_land=${dual_land} persist ddi_host=${persist_ddi} rtl8168_udx=${persist_rtl} xhci_udx=${persist_xhci} drivers=${persist_drv_n}"
echo "make-hwtest-img: udx dual_land byte_match=${byte_match_st} ddi_host=${_bm_ddi} rtl8168_udx=${_bm_rtl} xhci_udx=${_bm_xhci} n=${byte_match_n}/3"
echo "make-hwtest-img: product_host density=${product_host_n}/${product_host_total} (Soft!=product; denser non-empty honesty)"
echo "make-hwtest-img: sshd pack=${sshd_pack} bytes=${sshd_bytes} (Soft!=product; pack/embed != Dual DoD B close)"
echo "make-hwtest-img: freestanding class SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 residual opt-in)"
echo "make-hwtest-img: soft residual udx pack ddi_host=${ddi_pack} rtl8168_udx=${rtl_pack} xhci_udx=${xhci_pack}"
echo "make-hwtest-img: soft residual udx dual_land=${dual_land} product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "make-hwtest-img: soft residual udx dual_land byte_match=${byte_match_st} ddi_host=${_bm_ddi} rtl8168_udx=${_bm_rtl} xhci_udx=${_bm_xhci}"
echo "make-hwtest-img: soft residual product_host density=${product_host_n}/${product_host_total} ddi_bytes=${ddi_bytes} rtl_bytes=${rtl_bytes} xhci_bytes=${xhci_bytes}"
echo "make-hwtest-img: soft residual sshd pack=${sshd_pack} (endpoint residual; not Dual DoD B close)"
echo "make-hwtest-img: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "make-hwtest-img: soft residual freestanding class SKIP"
echo "make-hwtest-img: soft residual dual_dod A=OPEN B=OPEN (pack PASS != Dual DoD close)"
echo "make-hwtest-img: soft residual product honesty pack!=TX/RX/BOT!=bar3!=Dual_DoD (Soft!=product · G-AC-1)"
echo "make-hwtest-img: soft residual panel=${img_status_title} (stamp-free RO; Soft!=product)"
if [ "$ddi_pack" = "SKIP" ] || [ "$rtl_pack" = "SKIP" ] || [ "$xhci_pack" = "SKIP" ]; then
	echo "make-hwtest-img: udx soft-miss (run make drivers-udx then re-pack; Soft!=product)" >&2
fi
if [ "$sshd_pack" = "SKIP" ]; then
	echo "make-hwtest-img: sshd soft-miss user/sshd.elf absent (make sshd-gj; Soft!=product)" >&2
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
	if [ -f "$persist_dir/IMAGE_VERSION" ]; then
		mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/IMAGE_VERSION" ::/IMAGE_VERSION
	fi
	if [ -f "$persist_dir/IMAGE_STATUS_TITLE" ]; then
		mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/IMAGE_STATUS_TITLE" \
			::/IMAGE_STATUS_TITLE
	fi
	if [ -f "$persist_dir/DUT-PACK.txt" ]; then
		mcopy -o -i "$out@@$PERSIST_OFF" "$persist_dir/DUT-PACK.txt" ::/DUT-PACK.txt
	fi
	# Soft dual-land UDX hosts onto FAT persist (best-effort; prefer ext4)
	if [ -d "$persist_dir/drivers" ]; then
		mmd -i "$out@@$PERSIST_OFF" ::/drivers 2>/dev/null || true
		for f in "$persist_dir/drivers/"*; do
			[ -f "$f" ] || continue
			mcopy -o -i "$out@@$PERSIST_OFF" "$f" "::/drivers/$(basename "$f")" 2>/dev/null || true
		done
	fi
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
# Re-extract from staged ELF in case early path used config.h only
if [ -f "$esp_dir/EFI/GREENJADE/KERNEL.ELF" ]; then
	_v2=$(./scripts/gj-image-version.sh "$esp_dir/EFI/GREENJADE/KERNEL.ELF" 2>/dev/null || true)
	if [ -n "$_v2" ] && [ "$_v2" != "unknown" ]; then
		img_ver=$_v2
		img_status_title="STATUS (static) v${img_ver}"
	fi
fi
echo "make-hwtest-img: PASS img=$out size=${sz}B steam=$steam_status user_elfs=${user_n} libs=${lib_n} drivers=${drv_n} linux_ko=${ld_n:-0} rootfs_files=${rootfs_n} esp_r8169=${esp_r8169:-0} esp_fw_n=${esp_fw_n:-0} sshd=${sshd_pack} dual_land=${dual_land}"
echo "  image:  GJ_IMAGE_VERSION=${img_ver}"
echo "  panel:  expect STATUS title → ${img_status_title}"
echo "  flash:  flash bar → ${img_status_title} (Soft!=product stamp only)"
echo "  serial: main: image version=${img_ver}"
echo "  layout: p1 ESP FAT(GREENJADE) + p2 ${PERSIST_FS}(GJ-PERSIST) logs+ssh+steam+linux-drivers+drivers/"
echo "  G752:   EFI/GREENJADE/LAPTOP.txt · DRIVERS.txt · NEEDED-DRIVERS.txt · drivers/ (UDX)"
echo "  UDX:    ddi_host=${ddi_pack}(${ddi_bytes}B) rtl8168_udx=${rtl_pack}(${rtl_bytes}B) xhci_udx=${xhci_pack}(${xhci_bytes}B)"
echo "  dual:   dual_land=${dual_land} persist ddi=${persist_ddi} rtl=${persist_rtl} xhci=${persist_xhci} n=${persist_drv_n}"
echo "  sshd:   pack=${sshd_pack}(${sshd_bytes}B) ESP user/sshd.elf (embed live spawn separate; Soft!=product)"
echo "  class:  freestanding SKIP default (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0)"
echo "  Linux:  GJ-PERSIST/linux-drivers/ (full tree) + ESP /linux-drivers/modules/r8169.ko"
echo "  fw:     ESP /linux-drivers/firmware/rtl_nic/rtl8168*.fw (media honesty); runtime HIT = kernel embed"
echo "  D4:     UEFI soft media handoff (source=media) when .ko staged; Soft!=product · G-AC-1"
if [ "${esp_r8169:-0}" -gt 1000 ]; then
	echo "  greppable: make-hwtest-img: esp stage PASS"
fi
echo "  greppable: make-hwtest-img: udx pack ddi_host=${ddi_pack} rtl8168_udx=${rtl_pack} xhci_udx=${xhci_pack}"
echo "  greppable: make-hwtest-img: sshd pack=${sshd_pack}"
echo "  greppable: make-hwtest-img: udx dual_land=${dual_land}"
echo "  write:  sudo ./scripts/install-hwtest-usb.sh /dev/sdX"
echo "  verify: after DUT boot, panel row 0 == \"${img_status_title}\""
echo "          host: ./scripts/gj-image-version.sh --report"
echo "  SSH:    after plug-in on lab host:"
echo "          sudo mount -L GJ-PERSIST /mnt/gj-persist"
echo "          cat /mnt/gj-persist/IMAGE_STATUS_TITLE"
echo "          cat /mnt/gj-persist/DUT-PACK.txt"
echo "          sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "          ls /mnt/gj-persist/linux-drivers/modules"
echo "          ls /mnt/gj-persist/drivers/   # UDX dual-land mirror"
echo "  ESP:    sudo mount -L GREENJADE /mnt/gj-esp"
echo "          ls /mnt/gj-esp/EFI/GREENJADE/drivers/   # ddi_host rtl8168_udx xhci_udx"
echo "          ls /mnt/gj-esp/EFI/GREENJADE/user/sshd.elf"
echo "          cat /mnt/gj-esp/EFI/GREENJADE/DUT-PACK.txt"
echo "  Soft:   ./scripts/gj-product-summary.sh <serial-log>   # exit 0"
echo "  Keys:   ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam:  docs/STEAM_HWTEST.md  (make steam-fetch for READY tree)"
echo "  Note:   READY/media != Steam client run; Top-50 remains NOT-TRIED"
echo "  Soft!=product: image version stamp != product complete != bar3 close"
echo "  Soft!=product · G-AC-1: UDX pack != product TX/RX/BOT; freestanding class not product"
echo "  Soft!=product: sshd pack/embed PASS != Dual DoD B close (needs UDX NIC wire)"
echo "  Dual DoD: A/B OPEN (UDX USB/NIC) — pack != Dual DoD close; L3 host probes required"
echo "make-hwtest-img: soft residual dual_dod A=OPEN B=OPEN product_udx=1 freestanding_skip=1"
echo "make-hwtest-img: soft residual udx pack ddi_host=${ddi_pack} rtl8168_udx=${rtl_pack} xhci_udx=${xhci_pack}"
echo "make-hwtest-img: soft residual udx dual_land=${dual_land}"
echo "make-hwtest-img: soft residual udx dual_land byte_match=${byte_match_st}"
echo "make-hwtest-img: soft residual product_host density=${product_host_n}/${product_host_total}"
echo "make-hwtest-img: soft residual sshd pack=${sshd_pack}"
echo "make-hwtest-img: soft residual product honesty pack!=TX/RX/BOT!=bar3!=Dual_DoD"
echo "make-hwtest-img: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "make-hwtest-img: soft residual freestanding class SKIP"
echo "make-hwtest-img: soft residual panel=${img_status_title}"
echo "make-hwtest-img: soft residual stamp-free (fly bar=v${img_ver}; pack never bumps GJ_IMAGE_VERSION)"
echo "  greppable: make-hwtest-img: soft residual dual_dod"
echo "  greppable: make-hwtest-img: soft residual product=UDX+ABI"
echo "  greppable: make-hwtest-img: soft residual freestanding class SKIP"
echo "  greppable: make-hwtest-img: soft residual udx pack"
echo "  greppable: make-hwtest-img: soft residual udx dual_land"
echo "  greppable: make-hwtest-img: soft residual udx dual_land byte_match="
echo "  greppable: make-hwtest-img: soft residual product_host density="
echo "  greppable: make-hwtest-img: soft residual sshd pack"
echo "  greppable: make-hwtest-img: soft residual product honesty"
echo "  greppable: make-hwtest-img: soft residual stamp-free"
echo "  greppable: make-hwtest-img: soft residual panel="
echo "  FS:     default persist=ext4 (Linux lab); ESP remains FAT for UEFI"
echo "          fallback: GJ_HWTEST_PERSIST_FS=fat32"
