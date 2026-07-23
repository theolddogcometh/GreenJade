#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage a prebuilt Steam tree (option 2) into a destination directory.
#
# Layout written:
#   <dest>/steam/          — client tree (from build/steam-tree/steam if present)
#   <dest>/steam/README.txt
#   <dest>/steam/STATUS    — READY | SKELETON
#   <dest>/opt/steam       — same tree when dest looks like a rootfs (has /bin)
#
# Usage:
#   ./scripts/stage-steam-tree.sh [dest]
# Default dest: build/steam-stage
#
# Env:
#   GJ_STEAM_TREE   host tree (default build/steam-tree)
#   GJ_STEAM_FORCE_SKELETON=1  never copy client; skeleton only
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

dest="${1:-build/steam-stage}"
src_tree="${GJ_STEAM_TREE:-build/steam-tree}"
src_steam="$src_tree/steam"

mkdir -p "$dest/steam"

cat >"$dest/steam/README.txt" <<'EOF'
GreenJade hardware-test — staged Steam tree (option 2)
======================================================

Policy: prebuilt Steam payload on media. No dpkg/apt on GreenJade.
Steam proprietary binaries are NOT in the GreenJade git tree; the lab
host fetches them (`make steam-fetch`) and this directory is packed onto
GJ-PERSIST (and optionally rootfs /opt/steam).

Paths on media
  GJ-PERSIST/steam/     durable client tree (primary for USB hwtest)
  /opt/steam/           rootfs copy when staged into build/rootfs

Lab host prep
  make steam-fetch      # download+extract Valve bootstrap → build/steam-tree
  make steam-stage      # stage into build/steam-stage + rootfs/opt/steam
  make hwtest-img       # pack onto p2 GJ-PERSIST/steam

DUT / operator
  Mount GJ-PERSIST after boot (or from lab host) and inspect steam/STATUS.
  Full client launch still requires Linux ABI + GPU/audio/net (bar #3 open).

STATUS file
  READY     — bootstrap files present under steam/usr or steam/bin
  SKELETON  — placeholders only; run make steam-fetch on lab host
EOF

has_client=0
if [ "${GJ_STEAM_FORCE_SKELETON:-0}" != "1" ] && [ -d "$src_steam" ]; then
	if [ -e "$src_steam/usr/bin/steam" ] || [ -e "$src_steam/bin/steam" ] ||
		[ -d "$src_steam/usr/lib/steam" ] || [ -d "$src_steam/usr" ]; then
		has_client=1
	fi
fi

if [ "$has_client" -eq 1 ]; then
	echo "stage-steam-tree: copying client from $src_steam → $dest/steam"
	# Preserve tree; exclude host-only download dir if nested
	if command -v rsync >/dev/null 2>&1; then
		rsync -a --delete \
			--exclude download \
			--exclude READY \
			"$src_steam"/ "$dest/steam/"
	else
		# cp -a into clean subdir then merge README back
		rm -rf "$dest/steam.body"
		mkdir -p "$dest/steam.body"
		cp -a "$src_steam"/. "$dest/steam.body"/
		# keep README we wrote; overlay client
		cp -a "$dest/steam.body"/. "$dest/steam"/
		rm -rf "$dest/steam.body"
	fi
	# restore README if overwritten
	if [ ! -f "$dest/steam/README.txt" ]; then
		:
	fi
	# re-write README (always ours)
	cat >"$dest/steam/README.txt" <<'EOF'
GreenJade hardware-test — staged Steam tree (option 2)
======================================================

STATUS: READY (bootstrap tree present)

Fetched on lab host via `make steam-fetch` (Valve steam.deb extracted; not
installed with dpkg). Packed onto GJ-PERSIST for hardware test.

Launch (when ABI ready)
  /opt/steam/usr/bin/steam
  # or from persist mount:
  /mnt/gj-persist/steam/usr/bin/steam

Bar #3 still requires display/input/audio/net + libcgj ABI growth.
EOF
	echo READY >"$dest/steam/STATUS"
	if [ -f "$src_tree/MANIFEST.txt" ]; then
		cp -f "$src_tree/MANIFEST.txt" "$dest/steam/MANIFEST.txt"
	fi
	echo "stage-steam-tree: STATUS=READY"
else
	mkdir -p "$dest/steam/bin" "$dest/steam/usr/bin"
	cat >"$dest/steam/bin/steam.placeholder" <<'EOF'
#!/bin/sh
echo "GreenJade: Steam bootstrap not staged. On lab host: make steam-fetch && make steam-stage && make hwtest-img" >&2
exit 127
EOF
	chmod +x "$dest/steam/bin/steam.placeholder"
	echo SKELETON >"$dest/steam/STATUS"
	echo "stage-steam-tree: STATUS=SKELETON (no build/steam-tree yet)"
fi

# Rootfs-shaped destinations also get /opt/steam and a thin /usr/bin/steam hint
if [ -d "$dest/bin" ] || [ -d "$dest/sbin" ] || [ "${GJ_STEAM_AS_ROOTFS:-0}" = "1" ]; then
	mkdir -p "$dest/opt"
	rm -rf "$dest/opt/steam"
	cp -a "$dest/steam" "$dest/opt/steam"
	mkdir -p "$dest/usr/bin" "$dest/etc/greenjade"
	cat >"$dest/usr/bin/steam" <<'EOF'
#!/bin/sh
# GreenJade thin launcher — prefers /opt/steam, then GJ-PERSIST
for p in /opt/steam/usr/bin/steam /opt/steam/bin/steam \
	/mnt/gj-persist/steam/usr/bin/steam /mnt/gj-persist/steam/bin/steam; do
	if [ -x "$p" ]; then
		exec "$p" "$@"
	fi
done
echo "steam: bootstrap not found (stage with make steam-fetch && make steam-stage)" >&2
exit 127
EOF
	chmod +x "$dest/usr/bin/steam"
	cat >"$dest/etc/greenjade/steam.env" <<'EOF'
# GreenJade Steam staging (option 2 — prebuilt tree on media)
GJ_STEAM_ROOT=/opt/steam
GJ_STEAM_PERSIST=/mnt/gj-persist/steam
GJ_STEAM_POLICY=prebuilt-tree-no-dpkg
EOF
fi

echo "stage-steam-tree: PASS dest=$dest"
