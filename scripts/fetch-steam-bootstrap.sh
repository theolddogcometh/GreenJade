#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Fetch Valve's Linux Steam *bootstrap* (launcher package) onto the lab host
# into build/steam-tree/. Does NOT commit binaries — tree is gitignored.
#
# Option 2 (hardware test): prebuilt Steam tree staged onto media via
#   ./scripts/stage-steam-tree.sh
#   make hwtest-img   # picks up build/steam-tree when present
#
# This is NOT a .deb installer on GreenJade — we only use the archive as a
# convenient host-side payload and extract a filesystem tree.
#
# Usage:
#   ./scripts/fetch-steam-bootstrap.sh
#   GJ_STEAM_URL=... ./scripts/fetch-steam-bootstrap.sh
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

out="${GJ_STEAM_TREE:-build/steam-tree}"
dl="$out/download"
extract="$out/extract"
# Official Valve bootstrap .deb (Ubuntu packaging); we extract only, never run dpkg.
url="${GJ_STEAM_URL:-https://cdn.cloudflare.steamstatic.com/client/installer/steam.deb}"

need() {
	command -v "$1" >/dev/null 2>&1 || {
		echo "missing tool: $1" >&2
		exit 1
	}
}
need curl
need ar
need tar

mkdir -p "$dl" "$extract" "$out/steam"
# Absolute paths so ar/tar still work after cd into extract/
out_abs="$(CDPATH= cd -- "$out" && pwd)"
dl_abs="$(CDPATH= cd -- "$dl" && pwd)"
extract_abs="$(CDPATH= cd -- "$extract" && pwd)"
deb="$dl_abs/steam.deb"

echo "fetch-steam-bootstrap: GET $url"
if [ ! -f "$deb" ] || [ "${GJ_STEAM_FORCE:-0}" = "1" ]; then
	curl -fL --retry 3 -o "$deb.partial" "$url"
	mv -f "$deb.partial" "$deb"
fi
sz=$(wc -c <"$deb" | tr -d ' ')
echo "fetch-steam-bootstrap: deb size=${sz}B"

rm -rf "$extract_abs"
mkdir -p "$extract_abs"
(
	cd "$extract_abs"
	ar x "$deb"
	# data.tar.* holds the payload
	data=
	for f in data.tar.xz data.tar.gz data.tar.zst data.tar.bz2 data.tar; do
		if [ -f "$f" ]; then
			data=$f
			break
		fi
	done
	if [ -z "$data" ]; then
		echo "no data.tar in steam.deb" >&2
		ls -la >&2
		exit 1
	fi
	case "$data" in
	*.xz) tar -xJf "$data" ;;
	*.gz) tar -xzf "$data" ;;
	*.bz2) tar -xjf "$data" ;;
	*.zst)
		if command -v zstd >/dev/null 2>&1; then
			zstd -d -c "$data" | tar -xf -
		else
			echo "need zstd to unpack $data" >&2
			exit 1
		fi
		;;
	*) tar -xf "$data" ;;
	esac
)

# Normalize into build/steam-tree/steam/ (client root for media stage)
rm -rf "$out_abs/steam"
mkdir -p "$out_abs/steam"
# Deb layout typically: usr/bin/steam, usr/lib/steam/, usr/share/doc/steam/, …
if [ -d "$extract_abs/usr" ]; then
	cp -a "$extract_abs/usr" "$out_abs/steam/usr"
fi
if [ -d "$extract_abs/etc" ]; then
	cp -a "$extract_abs/etc" "$out_abs/steam/etc"
fi
# Keep a flat bin pointer for operators
mkdir -p "$out_abs/steam/bin"
if [ -f "$out_abs/steam/usr/bin/steam" ]; then
	cp -f "$out_abs/steam/usr/bin/steam" "$out_abs/steam/bin/steam" 2>/dev/null || true
	chmod +x "$out_abs/steam/bin/steam" 2>/dev/null || true
fi

# Manifest (no Valve source — provenance only)
{
	echo "GreenJade staged Steam bootstrap (host fetch)"
	echo "date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)"
	echo "url=$url"
	echo "deb_bytes=$sz"
	if command -v sha256sum >/dev/null 2>&1; then
		echo "deb_sha256=$(sha256sum "$deb" | awk '{print $1}')"
	fi
	echo "layout=steam/usr steam/bin (extracted from .deb; not dpkg-installed)"
	echo "policy=option-2-prebuilt-tree-on-hwtest-media"
	echo "license_note=Valve Steam proprietary; not committed to GreenJade VCS"
} >"$out_abs/MANIFEST.txt"

cat >"$out_abs/README.txt" <<'EOF'
GreenJade — host Steam tree (option 2)
======================================

This directory is built on the *lab host* and staged onto hardware-test media.
It is gitignored; do not commit Valve binaries into the GreenJade tree.

Contents
  steam/         Extracted bootstrap (launcher scripts + support files)
  download/      Cached steam.deb
  MANIFEST.txt   URL + hash of the fetched package

Next steps
  make steam-stage          # → build/rootfs/opt/steam + stage helper tree
  make hwtest-img           # packs GJ-PERSIST/steam when this tree exists
  # or:
  ./scripts/stage-steam-tree.sh /path/to/dest

Full Steam client still downloads runtimes/apps on first run when network
and ABI allow. This bootstrap only places the launcher tree on the stick.
EOF

# Marker for packaging scripts
date -u +%Y-%m-%dT%H:%MZ >"$out_abs/READY" 2>/dev/null || echo ready >"$out_abs/READY"

echo "fetch-steam-bootstrap: PASS tree=$out_abs"
echo "  next: make steam-stage && make hwtest-img"
ls -la "$out_abs/steam/bin" 2>/dev/null || ls -la "$out_abs/steam" | head
