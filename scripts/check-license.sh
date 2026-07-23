#!/bin/sh
# SPDX-License-Identifier: MIT
# Fail if suspicious copyleft license files appear in the tree.
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

# Scan for license filenames / markers we reject as project policy.
# This is a coarse guard, not legal advice.
bad=0
# shellcheck disable=SC2044
for f in $(find . -type f \
    ! -path './.git/*' \
    ! -path './build/*' \
    \( -iname '*copying*' -o -iname '*gpl*' -o -name 'LICENSE*' -o -name 'LICENCE*' \) \
    2>/dev/null); do
    case "$f" in
        ./LICENSE|./LICENSE.md) continue ;;
        ./docs/*) continue ;;
    esac
    if grep -Eqi 'GNU GENERAL PUBLIC|Lesser General Public|Affero General Public|GPL-2|GPL-3|LGPL' "$f" 2>/dev/null; then
        echo "REJECT (copyleft marker): $f"
        bad=1
    fi
done

# Quick scan of all text-ish sources for SPDX GPL
if grep -Rni --include='*.c' --include='*.h' --include='*.S' --include='*.ld' \
    'SPDX-License-Identifier:.*GPL' . 2>/dev/null | grep -v 'Binary'; then
    echo "REJECT: GPL SPDX in source"
    bad=1
fi

if [ "$bad" -ne 0 ]; then
    echo "check-license: FAILED"
    exit 1
fi
echo "check-license: OK"
