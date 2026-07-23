#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Fail if suspicious copyleft license files / GPL SPDX markers appear in the
# *product* tree. This is a coarse guard, not legal advice.
#
# Soft deepen (exclusions + inventory — keep product gates hard):
#   soft skip  .git/, build/, tools/  (tools/ is gitignored host toolchain;
#              aarch64-linux-musl-cross ships Linux uapi headers with
#              GPL-2.0 WITH Linux-syscall-note SPDX — not product code)
#   soft skip  third_party/ (provenance only; not product kernel/user)
#   soft skip  docs/ license filename hits (design notes may quote GPL text)
#   soft skip  ./LICENSE, ./LICENSE.md (project dual MIT OR Apache-2.0 root)
#   soft report  scanned license-filename hits and SPDX scan scope
#
# Hard gates (exit 1):
#   - copyleft markers in product license-named files
#   - SPDX-License-Identifier:.*GPL in product kernel/, user/, scripts/
#     sources (*.c *.h *.S *.ld) — never tools/, never build/
#
# See also: third_party/PROVENANCE.md, make license, scripts/smoke-all.sh
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

bad=0
hits=0
scanned=0
soft_skip=0
spdx_hits=0

echo "check-license: scanning product tree (soft-exclude .git/build/tools/third_party)"

# Scan for license filenames / markers we reject as project policy.
# shellcheck disable=SC2044
for f in $(find . -type f \
    ! -path './.git/*' \
    ! -path './build/*' \
    ! -path './tools/*' \
    ! -path './third_party/*' \
    \( -iname '*copying*' -o -iname '*gpl*' -o -name 'LICENSE*' -o -name 'LICENCE*' \) \
    2>/dev/null); do
    scanned=$((scanned + 1))
    case "$f" in
        ./LICENSE|./LICENSE.md)
            echo "check-license: soft ok root license $f"
            continue
            ;;
        ./docs/*)
            # Design docs may quote foreign licenses; soft skip filename hit.
            echo "check-license: soft skip docs license-name $f"
            soft_skip=$((soft_skip + 1))
            continue
            ;;
        ./media/*|./matrix/*)
            echo "check-license: soft skip non-source $f"
            soft_skip=$((soft_skip + 1))
            continue
            ;;
    esac
    if grep -Eqi 'GNU GENERAL PUBLIC|Lesser General Public|Affero General Public|GPL-2|GPL-3|LGPL' "$f" 2>/dev/null; then
        echo "REJECT (copyleft marker): $f"
        bad=1
        hits=$((hits + 1))
    fi
done

# SPDX GPL scan of product freestanding sources only (never tools/ uapi).
# Scope: kernel/, user/, scripts/ with *.[cSh] / *.ld.
spdx_list="${TMPDIR:-/tmp}/gj-license-spdx-list.$$.txt"
cleanup_spdx() {
    rm -f "$spdx_list" 2>/dev/null || true
}
trap cleanup_spdx EXIT INT TERM
: >"$spdx_list"

# Portable path walk — avoid grep -R --exclude-dir variance across hosts.
find ./kernel ./user ./scripts \
    \( -name '*.c' -o -name '*.h' -o -name '*.S' -o -name '*.ld' \) \
    -type f 2>/dev/null |
while IFS= read -r f; do
    if grep -Eqi 'SPDX-License-Identifier:.*GPL' "$f" 2>/dev/null; then
        printf '%s\n' "$f"
    fi
done >"$spdx_list" || true

if [ -s "$spdx_list" ]; then
    while IFS= read -r f; do
        [ -n "$f" ] || continue
        echo "REJECT: GPL SPDX in source: $f"
        bad=1
        spdx_hits=$((spdx_hits + 1))
    done <"$spdx_list"
fi
cleanup_spdx
trap - EXIT INT TERM

# Soft inventory of host tools/ presence (never fails — gitignored toolchain).
if [ -d tools ]; then
    echo "check-license: soft info tools/ present (excluded from gate; host toolchain)"
else
    echo "check-license: soft info tools/ absent"
fi

echo "check-license: soft report license-name scanned=$scanned soft_skip=$soft_skip copyleft_hits=$hits spdx_hits=$spdx_hits"

if [ "$bad" -ne 0 ]; then
    echo "check-license: FAILED"
    exit 1
fi
echo "check-license: OK"
