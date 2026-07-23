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
# Soft deepen / honesty:
#   STATUS=READY  → bootstrap files present under steam/ (media path only)
#   STATUS=SKELETON → placeholders; run fetch on lab host
#   stage PASS    → tree copied / skeleton written; NOT bar3 client run
#   Deck Top 50 stays NOT-TRIED until real title launch on GreenJade
#   Pair: ./scripts/steam-bar3-check.sh
#
# Usage:
#   ./scripts/stage-steam-tree.sh [dest]
# Default dest: build/steam-stage
#
# Env:
#   GJ_STEAM_TREE   host tree (default build/steam-tree)
#   GJ_STEAM_FORCE_SKELETON=1  never copy client; skeleton only
#   GJ_STEAM_AS_ROOTFS=1       force rootfs-shaped /opt/steam + /usr/bin/steam
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

dest="${1:-build/steam-stage}"
src_tree="${GJ_STEAM_TREE:-build/steam-tree}"
src_steam="$src_tree/steam"

mkdir -p "$dest/steam"

write_readme_common() {
	# $1 = STATUS token for human README
	_st=$1
	cat >"$dest/steam/README.txt" <<EOF
GreenJade hardware-test — staged Steam tree (option 2)
======================================================

STATUS: ${_st}

Policy: prebuilt Steam payload on media. No dpkg/apt on GreenJade.
Steam proprietary binaries are NOT in the GreenJade git tree; the lab
host fetches them (\`./scripts/fetch-steam-bootstrap.sh\` / \`make steam-fetch\`)
and this directory is packed onto GJ-PERSIST (and optionally rootfs /opt/steam).

Paths on media
  GJ-PERSIST/steam/     durable client tree (primary for USB hwtest)
  /opt/steam/           rootfs copy when staged into build/rootfs

Honesty (do not conflate)
  READY on media  = bootstrap tree present (launcher scripts + support)
  stage PASS      = host copy/skeleton only
  Neither is a Steam client launch or Deck Top 50 title result.
  Bar3 remains OPEN: real-DUT UEFI → client run → matrix NOT-TRIED→PASS/FAIL.

Lab host prep
  ./scripts/fetch-steam-bootstrap.sh
  ./scripts/stage-steam-tree.sh
  ./scripts/steam-bar3-check.sh   # soft exit 0
  make hwtest-img                 # pack onto p2 GJ-PERSIST/steam

DUT / operator
  Mount GJ-PERSIST after boot (or from lab host) and inspect steam/STATUS.
  Full client launch still requires Linux ABI + GPU/audio/net (bar3 open).
EOF
}

has_client=0
if [ "${GJ_STEAM_FORCE_SKELETON:-0}" != "1" ] && [ -d "$src_steam" ]; then
	if [ -e "$src_steam/usr/bin/steam" ] || [ -L "$src_steam/usr/bin/steam" ] ||
		[ -e "$src_steam/bin/steam" ] ||
		[ -d "$src_steam/usr/lib/steam" ] || [ -d "$src_steam/usr" ]; then
		has_client=1
	fi
fi

soft_count() {
	_d=$1
	if [ ! -d "$_d" ]; then
		echo 0
		return 0
	fi
	find "$_d" -type f 2>/dev/null | head -n 5000 | wc -l | tr -d ' ' || echo 0
}

soft_launcher_kind() {
	_base=$1
	if [ -L "$_base/usr/bin/steam" ]; then
		echo "symlink→$(readlink "$_base/usr/bin/steam" 2>/dev/null || echo ?)"
	elif [ -f "$_base/usr/bin/steam" ]; then
		echo "file"
	elif [ -f "$_base/bin/steam" ]; then
		echo "bin/steam"
	elif [ -f "$_base/bin/steam.placeholder" ]; then
		echo "placeholder"
	else
		echo "missing"
	fi
}

soft_bootstrap_blob() {
	_base=$1
	for _b in "$_base/usr/lib/steam"/bootstraplinux_*.tar.xz \
		"$_base/usr/lib/steam"/bootstraplinux_*.tar.gz; do
		if [ -f "$_b" ]; then
			_sz=$(wc -c <"$_b" | tr -d ' ')
			echo "$(basename "$_b") (${_sz}B)"
			return 0
		fi
	done
	echo "none"
}

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
	write_readme_common "READY (bootstrap tree present)"
	echo READY >"$dest/steam/STATUS"
	if [ -f "$src_tree/MANIFEST.txt" ]; then
		cp -f "$src_tree/MANIFEST.txt" "$dest/steam/MANIFEST.txt"
	fi
	# Soft honesty stamp on staged tree (agent-readable; not a PASS claim)
	{
		echo "stage_date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)"
		echo "status=READY"
		echo "src=$src_steam"
		echo "dest=$dest/steam"
		echo "launcher=$(soft_launcher_kind "$dest/steam")"
		echo "bootstrap_blob=$(soft_bootstrap_blob "$dest/steam")"
		echo "files≈$(soft_count "$dest/steam") (capped sample; soft)"
		echo "honesty=media-bootstrap-only; bar3-client-run=OPEN; deck-top50=NOT-TRIED"
	} >"$dest/steam/STAGE_META.txt"
	echo "stage-steam-tree: STATUS=READY"
else
	mkdir -p "$dest/steam/bin" "$dest/steam/usr/bin"
	cat >"$dest/steam/bin/steam.placeholder" <<'EOF'
#!/bin/sh
echo "GreenJade: Steam bootstrap not staged. On lab host: ./scripts/fetch-steam-bootstrap.sh && ./scripts/stage-steam-tree.sh (or make steam-fetch && make steam-stage)" >&2
exit 127
EOF
	chmod +x "$dest/steam/bin/steam.placeholder"
	write_readme_common "SKELETON (placeholders only — run fetch on lab host)"
	echo SKELETON >"$dest/steam/STATUS"
	{
		echo "stage_date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)"
		echo "status=SKELETON"
		echo "src=${src_steam} (missing or forced skeleton)"
		echo "dest=$dest/steam"
		echo "launcher=placeholder"
		echo "honesty=media-skeleton-only; bar3-client-run=OPEN; deck-top50=NOT-TRIED"
		echo "next=./scripts/fetch-steam-bootstrap.sh && ./scripts/stage-steam-tree.sh"
	} >"$dest/steam/STAGE_META.txt"
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
# Honesty: presence of this wrapper ≠ Steam client run (bar3 still open).
for p in /opt/steam/usr/bin/steam /opt/steam/bin/steam \
	/mnt/gj-persist/steam/usr/bin/steam /mnt/gj-persist/steam/bin/steam; do
	if [ -x "$p" ]; then
		exec "$p" "$@"
	fi
done
echo "steam: bootstrap not found (stage with ./scripts/fetch-steam-bootstrap.sh && ./scripts/stage-steam-tree.sh)" >&2
exit 127
EOF
	chmod +x "$dest/usr/bin/steam"
	cat >"$dest/etc/greenjade/steam.env" <<'EOF'
# GreenJade Steam staging (option 2 — prebuilt tree on media)
GJ_STEAM_ROOT=/opt/steam
GJ_STEAM_PERSIST=/mnt/gj-persist/steam
GJ_STEAM_POLICY=prebuilt-tree-no-dpkg
# bar3 client run remains open until real DUT launch
GJ_STEAM_BAR3=OPEN
EOF
fi

st=$(cat "$dest/steam/STATUS" 2>/dev/null || echo ?)
n_dest=$(soft_count "$dest/steam")
lk=$(soft_launcher_kind "$dest/steam")
bb=$(soft_bootstrap_blob "$dest/steam")

echo "stage-steam-tree: PASS dest=$dest STATUS=$st (media stage only)"
echo "  files:    steam≈$n_dest (capped; soft)"
echo "  launcher: $lk"
echo "  blob:     $bb"
echo "  open:     bar3 client run + Deck Top 50 still NOT-TRIED"
echo "  note:     STATUS=$st ≠ client launch ≠ Top50 PASS"
echo "  check:    ./scripts/steam-bar3-check.sh"
