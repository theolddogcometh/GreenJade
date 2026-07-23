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
# Soft deepen / honesty:
#   - PASS here means host extract + tree layout only
#   - Does NOT download Steam Runtime apps, launch client, or try Deck titles
#   - Bar3 (DUT client run + Top 50) remains open until real launch
#   - Pair: ./scripts/steam-bar3-check.sh  (media READY | SKELETON | MISSING)
#
# Usage:
#   ./scripts/fetch-steam-bootstrap.sh
#   GJ_STEAM_URL=... ./scripts/fetch-steam-bootstrap.sh
#   GJ_STEAM_FORCE=1 ./scripts/fetch-steam-bootstrap.sh   # re-download
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
else
	echo "fetch-steam-bootstrap: reuse cached deb (GJ_STEAM_FORCE=1 to re-fetch)"
fi
sz=$(wc -c <"$deb" | tr -d ' ')
echo "fetch-steam-bootstrap: deb size=${sz}B"
if [ "$sz" -lt 1000 ]; then
	echo "fetch-steam-bootstrap: deb too small (${sz}B) — refuse" >&2
	exit 1
fi

# Soft integrity: ar table of contents (never run dpkg)
if ! ar t "$deb" >/dev/null 2>&1; then
	echo "fetch-steam-bootstrap: deb is not a readable ar archive" >&2
	exit 1
fi
ar_members=$(ar t "$deb" 2>/dev/null | tr '\n' ' ' | head -c 200 || true)
echo "fetch-steam-bootstrap: ar members: ${ar_members:-?}"

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
	echo "fetch-steam-bootstrap: unpack $data"
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
if [ -f "$out_abs/steam/usr/bin/steam" ] || [ -L "$out_abs/steam/usr/bin/steam" ]; then
	# Prefer real file behind symlink for bin/steam convenience copy
	if [ -L "$out_abs/steam/usr/bin/steam" ]; then
		# Resolve one level for a portable host copy (soft)
		_link=$(readlink "$out_abs/steam/usr/bin/steam" 2>/dev/null || true)
		if [ -n "$_link" ] && [ -f "$out_abs/steam/usr/bin/$_link" ]; then
			cp -f "$out_abs/steam/usr/bin/$_link" "$out_abs/steam/bin/steam" 2>/dev/null || true
		elif [ -f "$out_abs/steam/usr/lib/steam/bin_steam.sh" ]; then
			cp -f "$out_abs/steam/usr/lib/steam/bin_steam.sh" "$out_abs/steam/bin/steam" 2>/dev/null || true
		else
			cp -fL "$out_abs/steam/usr/bin/steam" "$out_abs/steam/bin/steam" 2>/dev/null || true
		fi
	else
		cp -f "$out_abs/steam/usr/bin/steam" "$out_abs/steam/bin/steam" 2>/dev/null || true
	fi
	chmod +x "$out_abs/steam/bin/steam" 2>/dev/null || true
fi

# Soft inventory (agent honesty; never claims client run)
soft_count() {
	_d=$1
	if [ ! -d "$_d" ]; then
		echo 0
		return 0
	fi
	find "$_d" -type f 2>/dev/null | head -n 5000 | wc -l | tr -d ' ' || echo 0
}
n_steam=$(soft_count "$out_abs/steam")
n_extract=$(soft_count "$extract_abs")

launcher_kind="missing"
if [ -L "$out_abs/steam/usr/bin/steam" ]; then
	launcher_kind="symlink→$(readlink "$out_abs/steam/usr/bin/steam" 2>/dev/null || echo ?)"
elif [ -f "$out_abs/steam/usr/bin/steam" ]; then
	launcher_kind="file"
elif [ -f "$out_abs/steam/bin/steam" ]; then
	launcher_kind="bin/steam"
fi

bootstrap_blob="none"
for _b in "$out_abs/steam/usr/lib/steam"/bootstraplinux_*.tar.xz \
	"$out_abs/steam/usr/lib/steam"/bootstraplinux_*.tar.gz; do
	if [ -f "$_b" ]; then
		bootstrap_blob=$(basename "$_b")
		bootstrap_sz=$(wc -c <"$_b" | tr -d ' ')
		bootstrap_blob="$bootstrap_blob (${bootstrap_sz}B)"
		break
	fi
done

deb_sha=""
if command -v sha256sum >/dev/null 2>&1; then
	deb_sha=$(sha256sum "$deb" | awk '{print $1}')
fi

# Manifest (no Valve source — provenance only)
{
	echo "GreenJade staged Steam bootstrap (host fetch)"
	echo "date=$(date -u +%Y-%m-%dT%H:%MZ 2>/dev/null || echo unknown)"
	echo "url=$url"
	echo "deb_bytes=$sz"
	if [ -n "$deb_sha" ]; then
		echo "deb_sha256=$deb_sha"
	fi
	echo "layout=steam/usr steam/bin (extracted from .deb; not dpkg-installed)"
	echo "policy=option-2-prebuilt-tree-on-hwtest-media"
	echo "license_note=Valve Steam proprietary; not committed to GreenJade VCS"
	echo "files_steam≈$n_steam files_extract≈$n_extract (capped sample; soft)"
	echo "launcher=$launcher_kind"
	echo "bootstrap_blob=$bootstrap_blob"
	echo "honesty=host-extract-only; bar3-client-run=OPEN; deck-top50=NOT-TRIED"
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
  READY          Host-fetch stamp (not a client-run claim)

Honesty
  fetch PASS  =  host extract + tree layout only
  STATUS READY (after stage)  =  bootstrap files on media
  Neither means Steam client launched or Deck Top 50 tried.
  Bar3 remains OPEN until real-DUT client run + title matrix fills.

Next steps
  ./scripts/stage-steam-tree.sh   # → build/steam-stage + optional rootfs
  ./scripts/steam-bar3-check.sh   # soft: READY | SKELETON | MISSING (exit 0)
  make hwtest-img                 # packs GJ-PERSIST/steam when this tree exists
  # or:
  ./scripts/steam-host-prep.sh --to-label | --to-img | --to-mount DIR

Full Steam client still downloads runtimes/apps on first run when network
and ABI allow. This bootstrap only places the launcher tree on the stick.
EOF

# Marker for packaging scripts
date -u +%Y-%m-%dT%H:%MZ >"$out_abs/READY" 2>/dev/null || echo ready >"$out_abs/READY"

echo "fetch-steam-bootstrap: PASS tree=$out_abs (host extract only)"
echo "  files:    steam≈$n_steam extract≈$n_extract (capped; soft)"
echo "  launcher: $launcher_kind"
echo "  blob:     $bootstrap_blob"
if [ -n "$deb_sha" ]; then
	echo "  sha256:   $deb_sha"
fi
echo "  open:     bar3 client run + Deck Top 50 still NOT-TRIED"
echo "  next:     ./scripts/stage-steam-tree.sh && ./scripts/steam-bar3-check.sh"
echo "  note:     media READY ≠ smoke-all PASS ≠ client/title PASS"
ls -la "$out_abs/steam/bin" 2>/dev/null || ls -la "$out_abs/steam" | head
