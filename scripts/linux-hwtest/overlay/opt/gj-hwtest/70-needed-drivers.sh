#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a DUT-specific NEEDED-DRIVERS list from live Linux (inventory image).
# Operators collect these modules for GreenJade install media.
# Soft≠product: listing only; does not copy .ko into GreenJade source tree.
#
set -eu
echo "--- needed drivers (PCI → kernel driver / modules) ---"

out="${GJ_NEEDED_OUT:-}"
if [ -z "$out" ]; then
	# When run under run-all, caller may redirect; still print to stdout.
	out=""
fi

emit() {
	echo "$*"
}

emit "# GreenJade linux-hwtest — NEEDED-DRIVERS (DUT live)"
emit "# Generated: $(date -u -Iseconds 2>/dev/null || date -u)"
emit "# Product path: collect modules → stage on GJ install media; ABI hosts them."
emit "# GPL .ko binaries may be operator-collected; GreenJade tree stays dual-license source."
emit ""

if ! command -v lspci >/dev/null 2>&1; then
	emit "FAIL: lspci missing"
	exit 0
fi

# Prefer -nnk for [vend:dev] + Kernel driver in use / Kernel modules
lspci -nnk 2>/dev/null | awk '
BEGIN {
  bdf=""; id=""; drv=""; mods="";
}
/^[0-9a-fA-F][0-9a-fA-F]:[0-9a-fA-F]/ {
  if (bdf != "" && id != "") {
    printf "PCI %s id=%s driver=%s modules=%s\n", bdf, id, (drv==""?"-":drv), (mods==""?"-":mods)
  }
  bdf=$1
  id=""
  drv=""
  mods=""
  # extract [vvvv:dddd]
  if (match($0, /\[[0-9a-fA-F]+:[0-9a-fA-F]+\]/)) {
    id=substr($0, RSTART+1, RLENGTH-2)
  }
  next
}
/Kernel driver in use:/ {
  sub(/^.*Kernel driver in use:[[:space:]]*/, "")
  drv=$0
  next
}
/Kernel modules:/ {
  sub(/^.*Kernel modules:[[:space:]]*/, "")
  mods=$0
  next
}
END {
  if (bdf != "" && id != "") {
    printf "PCI %s id=%s driver=%s modules=%s\n", bdf, id, (drv==""?"-":drv), (mods==""?"-":mods)
  }
}
'

echo
echo "--- priority IDs (G752VT class) ---"
for id in "8086:a12f" "10ec:8168" "13fe:6400" "8086:2822"; do
	if lspci -nn 2>/dev/null | grep -qi "$id"; then
		echo "PRESENT $id"
		lspci -nnk 2>/dev/null | awk -v id="$id" '
			index($0, id) { hit=1; print; next }
			hit && /^[0-9a-fA-F][0-9a-fA-F]:/ { hit=0 }
			hit { print }
		' || true
	else
		echo "ABSENT  $id"
	fi
done

echo
echo "--- lsmod (top 40) ---"
if command -v lsmod >/dev/null 2>&1; then
	lsmod | head -40
else
	echo "(lsmod missing)"
fi

echo
echo "--- collect hint ---"
echo "On lab host after report lands on GJ-LNX-ESP:"
echo "  ./scripts/collect-linux-drivers.sh"
echo "  make hwtest-img   # packs build/linux-drivers onto GJ-PERSIST"
echo "greppable: needed-drivers: list PASS"
