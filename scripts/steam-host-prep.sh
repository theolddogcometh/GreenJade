#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Lab-host Steam prep for GreenJade hardware test.
#
# Combines:
#   Option 2 — prebuilt Steam tree on media (fetch + stage + pack)
#   Option 3 — host-side prep: fetch/extract, then copy onto an existing
#              image, USB partition, or mount (no dpkg on the DUT)
#
# Soft deepen / honesty:
#   host-prep PASS  =  fetch/stage/copy succeeded on lab host
#   Does NOT launch Steam, download Runtime apps, or try Deck titles
#   Bar3 remains OPEN until real-DUT client run + matrix fill
#   Pair: ./scripts/steam-bar3-check.sh  (media READY | SKELETON | MISSING)
#
# Usage:
#   ./scripts/steam-host-prep.sh                  # fetch+stage only
#   ./scripts/steam-host-prep.sh --to-stage        # same, explicit
#   ./scripts/steam-host-prep.sh --to-rootfs       # → build/rootfs/opt/steam
#   ./scripts/steam-host-prep.sh --to-img          # loop-mount p2 of hwtest img
#   ./scripts/steam-host-prep.sh --to-img PATH.img
#   ./scripts/steam-host-prep.sh --to-label        # mount -L GJ-PERSIST, copy
#   ./scripts/steam-host-prep.sh --to-mount /mnt/gj-persist
#   ./scripts/steam-host-prep.sh --to-dev /dev/sdb2
#   ./scripts/steam-host-prep.sh --all-img         # fetch+stage+hwtest-img rebuild
#   ./scripts/steam-host-prep.sh --fetch-only
#   ./scripts/steam-host-prep.sh --skip-fetch --to-stage
#
# Env:
#   GJ_STEAM_TREE, GJ_STEAM_URL, GJ_STEAM_FORCE=1
#   GJ_HWTEST_IMG (default build/greenjade-hwtest.img)
#   GJ_SKIP_FETCH=1   reuse existing build/steam-tree
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

mode="stage"
target=""
do_fetch=1
if [ "${GJ_SKIP_FETCH:-0}" = "1" ]; then
	do_fetch=0
fi

while [ "$#" -gt 0 ]; do
	case "$1" in
	--to-stage | --stage)
		mode="stage"
		;;
	--to-rootfs)
		mode="rootfs"
		;;
	--to-img)
		mode="img"
		if [ "${2:-}" != "" ] && [ "${2#--}" = "$2" ]; then
			target=$2
			shift
		fi
		;;
	--to-label)
		mode="label"
		;;
	--to-mount)
		mode="mount"
		target=${2:-}
		shift
		;;
	--to-dev)
		mode="dev"
		target=${2:-}
		shift
		;;
	--all-img)
		mode="all-img"
		;;
	--skip-fetch)
		do_fetch=0
		;;
	--fetch-only)
		mode="fetch-only"
		;;
	-h | --help)
		sed -n '2,40p' "$0" | sed 's/^# \{0,1\}//'
		exit 0
		;;
	*)
		echo "unknown arg: $1 (try --help)" >&2
		exit 1
		;;
	esac
	shift
done

chmod +x scripts/fetch-steam-bootstrap.sh scripts/stage-steam-tree.sh scripts/steam-bar3-check.sh

need_root() {
	if [ "$(id -u)" -ne 0 ]; then
		echo "need root for this step: $1" >&2
		echo "  re-run: sudo $0 ..." >&2
		exit 1
	fi
}

soft_count() {
	_d=$1
	if [ ! -d "$_d" ]; then
		echo 0
		return 0
	fi
	find "$_d" -type f 2>/dev/null | head -n 5000 | wc -l | tr -d ' ' || echo 0
}

honesty_footer() {
	# $1 = short pass context
	_ctx=$1
	_st=$(cat build/steam-stage/steam/STATUS 2>/dev/null || echo ?)
	echo "steam-host-prep: honesty — ${_ctx}"
	echo "  stage STATUS=${_st} (media bootstrap only)"
	echo "  open: bar3 client run + Deck Top 50 still NOT-TRIED"
	echo "  note: host-prep PASS ≠ smoke-all PASS ≠ client/title PASS"
	# Soft bar3 media check (always exit 0; never fails parent)
	if [ -x scripts/steam-bar3-check.sh ]; then
		echo "steam-host-prep: soft bar3 media check →"
		./scripts/steam-bar3-check.sh || true
	fi
}

copy_tree_to() {
	dest=$1
	src="${GJ_STEAM_TREE:-build/steam-tree}/steam"
	if [ ! -d "$src" ]; then
		echo "missing $src — run fetch first" >&2
		exit 1
	fi
	mkdir -p "$dest/steam"
	echo "steam-host-prep: copy $src → $dest/steam"
	if command -v rsync >/dev/null 2>&1; then
		rsync -a --delete "$src"/ "$dest/steam/"
	else
		rm -rf "$dest/steam"
		mkdir -p "$dest/steam"
		cp -a "$src"/. "$dest/steam"/
	fi
	# Ensure STATUS/README from stage policy
	if [ ! -f "$dest/steam/STATUS" ]; then
		echo READY >"$dest/steam/STATUS"
	fi
	if [ -f build/steam-tree/MANIFEST.txt ]; then
		cp -f build/steam-tree/MANIFEST.txt "$dest/steam/MANIFEST.txt"
	fi
	# Soft honesty meta on destination (agent-readable)
	{
		echo "host_prep_date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)"
		echo "status=$(cat "$dest/steam/STATUS" 2>/dev/null || echo ?)"
		echo "src=$src"
		echo "dest=$dest/steam"
		echo "files≈$(soft_count "$dest/steam") (capped sample; soft)"
		echo "honesty=option-3-host-copy; bar3-client-run=OPEN; deck-top50=NOT-TRIED"
	} >"$dest/steam/HOST_PREP_META.txt"
	# Rootfs-shaped: also opt/steam + launcher
	if [ -d "$dest/bin" ] || [ -d "$dest/sbin" ] || [ -d "$dest/lib" ]; then
		mkdir -p "$dest/opt"
		rm -rf "$dest/opt/steam"
		cp -a "$dest/steam" "$dest/opt/steam"
		mkdir -p "$dest/usr/bin" "$dest/etc/greenjade"
		cat >"$dest/usr/bin/steam" <<'EOF'
#!/bin/sh
# GreenJade thin launcher — prefers /opt/steam, then GJ-PERSIST
# Honesty: wrapper presence ≠ Steam client run (bar3 still open).
for p in /opt/steam/usr/bin/steam /opt/steam/bin/steam \
	/mnt/gj-persist/steam/usr/bin/steam /mnt/gj-persist/steam/bin/steam; do
	if [ -x "$p" ]; then
		exec "$p" "$@"
	fi
done
echo "steam: bootstrap not found" >&2
exit 127
EOF
		chmod +x "$dest/usr/bin/steam"
		cat >"$dest/etc/greenjade/steam.env" <<'EOF'
GJ_STEAM_ROOT=/opt/steam
GJ_STEAM_PERSIST=/mnt/gj-persist/steam
GJ_STEAM_POLICY=prebuilt-tree-no-dpkg
GJ_STEAM_HOST_PREP=1
GJ_STEAM_BAR3=OPEN
EOF
	fi
	_st=$(cat "$dest/steam/STATUS" 2>/dev/null || echo ?)
	_n=$(soft_count "$dest/steam")
	echo "steam-host-prep: copy PASS → $dest (STATUS=${_st} files≈${_n}; media only)"
	echo "  open: bar3 client run + Deck Top 50 still NOT-TRIED"
}

# --- fetch (option 2 host cache) ---
if [ "$do_fetch" -eq 1 ]; then
	echo "steam-host-prep: [1/n] fetch bootstrap (option 2 host cache)"
	./scripts/fetch-steam-bootstrap.sh
else
	echo "steam-host-prep: skip fetch (GJ_SKIP_FETCH or --skip-fetch)"
	if [ ! -f build/steam-tree/READY ] && [ ! -d build/steam-tree/steam/usr ]; then
		echo "no steam tree; refusing skip" >&2
		exit 1
	fi
fi

if [ "$mode" = "fetch-only" ]; then
	echo "steam-host-prep: PASS (fetch-only; host extract, not client run)"
	honesty_footer "fetch-only complete"
	exit 0
fi

# --- stage into build/steam-stage ---
echo "steam-host-prep: [2/n] stage → build/steam-stage"
./scripts/stage-steam-tree.sh build/steam-stage

case "$mode" in
stage)
	echo "steam-host-prep: PASS (staged only; media bootstrap, not client run)"
	echo "  next: $0 --to-img | --to-label | --to-mount DIR | --all-img"
	honesty_footer "staged only"
	exit 0
	;;
rootfs)
	echo "steam-host-prep: [3/n] rootfs build/rootfs"
	if [ ! -d build/rootfs/sbin ] && [ ! -d build/rootfs/bin ]; then
		chmod +x scripts/stage-rootfs.sh
		./scripts/stage-rootfs.sh build/rootfs
	else
		GJ_STEAM_AS_ROOTFS=1 ./scripts/stage-steam-tree.sh build/rootfs
	fi
	echo "steam-host-prep: PASS rootfs/opt/steam (media layout; not client run)"
	honesty_footer "rootfs staged"
	exit 0
	;;
all-img)
	echo "steam-host-prep: [3/n] rebuild hwtest-img with steam (option 2 pack)"
	make -j"$(nproc)" hwtest-img
	echo "steam-host-prep: PASS img=build/greenjade-hwtest.img (pack only; not client run)"
	echo "  write USB: sudo make install-hwtest-usb DEV=/dev/sdX"
	honesty_footer "hwtest-img rebuilt"
	exit 0
	;;
mount)
	if [ -z "$target" ] || [ ! -d "$target" ]; then
		echo "usage: $0 --to-mount /mnt/gj-persist" >&2
		exit 1
	fi
	echo "steam-host-prep: [3/n] copy onto mount $target (option 3)"
	copy_tree_to "$target"
	echo "steam-host-prep: PASS (mount copy; media only)"
	honesty_footer "option-3 mount"
	exit 0
	;;
label)
	need_root "--to-label"
	mnt="${TMPDIR:-/tmp}/gj-steam-persist.$$"
	mkdir -p "$mnt"
	echo "steam-host-prep: [3/n] mount -L GJ-PERSIST (option 3)"
	if ! mount -L GJ-PERSIST "$mnt"; then
		echo "could not mount label GJ-PERSIST — is the stick plugged in?" >&2
		rmdir "$mnt" 2>/dev/null || true
		exit 1
	fi
	copy_tree_to "$mnt"
	sync
	umount "$mnt"
	rmdir "$mnt" 2>/dev/null || true
	echo "steam-host-prep: PASS (label GJ-PERSIST; media only)"
	honesty_footer "option-3 label"
	exit 0
	;;
dev)
	need_root "--to-dev"
	if [ -z "$target" ] || [ ! -b "$target" ]; then
		echo "usage: sudo $0 --to-dev /dev/sdX2   # GJ-PERSIST partition" >&2
		exit 1
	fi
	mnt="${TMPDIR:-/tmp}/gj-steam-dev.$$"
	mkdir -p "$mnt"
	echo "steam-host-prep: [3/n] mount $target (option 3)"
	mount "$target" "$mnt"
	copy_tree_to "$mnt"
	sync
	umount "$mnt"
	rmdir "$mnt" 2>/dev/null || true
	echo "steam-host-prep: PASS (dev $target; media only)"
	honesty_footer "option-3 dev"
	exit 0
	;;
img)
	img=${target:-${GJ_HWTEST_IMG:-build/greenjade-hwtest.img}}
	if [ ! -f "$img" ]; then
		echo "missing $img — building hwtest-img first (includes steam stage)"
		make -j"$(nproc)" hwtest-img
		img=${GJ_HWTEST_IMG:-build/greenjade-hwtest.img}
	fi
	need_root "--to-img (loop mount)"
	# Partition 2 of GPT image: start at ESP_END_MB (default 385 MiB)
	esp_end_mb=${GJ_HWTEST_ESP_END_MB:-385}
	off=$((esp_end_mb * 1024 * 1024))
	mnt="${TMPDIR:-/tmp}/gj-steam-img.$$"
	mkdir -p "$mnt"
	echo "steam-host-prep: [3/n] loop-mount $img p2 @ ${esp_end_mb}MiB (option 3)"
	if ! mount -o loop,offset="$off" "$img" "$mnt" 2>/dev/null; then
		# try losetup + partscan
		if command -v losetup >/dev/null 2>&1; then
			loop=$(losetup -f --show -P "$img")
			# prefer *p2
			part=""
			for c in "${loop}p2" "${loop}p2" "${loop}2"; do
				if [ -b "$c" ]; then
					part=$c
					break
				fi
			done
			if [ -z "$part" ]; then
				# fall back to offset on whole loop
				losetup -d "$loop" 2>/dev/null || true
				echo "mount failed for $img p2" >&2
				rmdir "$mnt" 2>/dev/null || true
				exit 1
			fi
			mount "$part" "$mnt"
			copy_tree_to "$mnt"
			sync
			umount "$mnt"
			losetup -d "$loop"
		else
			echo "need mount -o loop,offset= or losetup" >&2
			rmdir "$mnt" 2>/dev/null || true
			exit 1
		fi
	else
		copy_tree_to "$mnt"
		sync
		umount "$mnt"
	fi
	rmdir "$mnt" 2>/dev/null || true
	echo "steam-host-prep: PASS (img $img p2; media only)"
	honesty_footer "option-3 img"
	exit 0
	;;
*)
	echo "internal: bad mode $mode" >&2
	exit 1
	;;
esac
