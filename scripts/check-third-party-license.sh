#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Fail if omitted copyleft paths reappear under third_party/, or if a
# leftover GNU/CDDL *license file* lands in a vendored shell tree.
#
# Soft deepen (keep vendor gates hard):
#   soft skip  NOTICE / PROVENANCE.md (they document omissions)
#   soft skip  configure, aclocal.m4 (Autoconf unlimited-permission)
#   soft skip  official package licenses that *mention* omitted GPL
#              (dash/COPYING documents mksignames.c; zsh/LICENCE warns
#              that some functions may be GPL and may be omitted)
#   soft report  per-tree file counts and omitted-path inventory
#
# Hard gates (exit 1):
#   - any listed-omitted GPL path exists under third_party/{dash,zsh,tcsh}
#   - a license-named file under those trees *is* a GNU GPL/LGPL/AGPL
#     or CDDL license text (FSF/CDDL title/body), except NOTICE/PROVENANCE
#
# Product kernel/user is still dual MIT OR Apache-2.0 (check-license.sh).
# This script is the vendor gate. See third_party/PROVENANCE.md.
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

bad=0
omitted_checked=0
omitted_present=0
license_scanned=0
license_skip=0
license_hits=0

echo "check-third-party-license: scanning third_party/{bsd,apache-2.0,licenseref-zsh,public-domain}"
# scripts/openssl-gj-perl is GJ MIT OR Apache-2.0 glue (not CPAN; not this gate).

# Listed-omitted GPL (must remain absent). Paths relative to repo root.
omitted='
third_party/bsd/dash/compile
third_party/bsd/dash/depcomp
third_party/bsd/dash/missing
third_party/bsd/dash/src/mksignames.c
third_party/licenseref-zsh/zsh/config.guess
third_party/licenseref-zsh/zsh/config.sub
third_party/licenseref-zsh/zsh/Completion/Linux/Command/_qdbus
third_party/licenseref-zsh/zsh/Completion/Unix/Command/_darcs
third_party/licenseref-zsh/zsh/Completion/openSUSE/Command/_osc
third_party/licenseref-zsh/zsh/Completion/openSUSE/Command/_zypper
third_party/bsd/tcsh/acaux/config.guess
third_party/bsd/tcsh/acaux/config.sub
third_party/bsd/tcsh/nls/pl
third_party/bsd/openssh/config.guess
third_party/bsd/openssh/config.sub
third_party/apache-2.0/openssl/external/perl/Text-Template-1.56/LICENSE
'

# Portable file count (find + wc; strip leading blanks).
count_files() {
    dir=$1
    if [ ! -d "$dir" ]; then
        echo 0
        return
    fi
    find "$dir" -type f 2>/dev/null | wc -l | awk '{ print $1 }'
}

n_dash=$(count_files third_party/bsd/dash)
n_zsh=$(count_files third_party/licenseref-zsh/zsh)
n_tcsh=$(count_files third_party/bsd/tcsh)
n_openssh=$(count_files third_party/bsd/openssh)
n_openssl=$(count_files third_party/apache-2.0/openssl)

# --- omitted paths must stay gone ---
# Use a here-doc so POSIX sh does not rely on bashisms.
while IFS= read -r p; do
    [ -n "$p" ] || continue
    omitted_checked=$((omitted_checked + 1))
    if [ -e "$p" ]; then
        echo "REJECT (omitted GPL path present): $p"
        bad=1
        omitted_present=$((omitted_present + 1))
    fi
done <<EOF
$omitted
EOF

# Vendor NOTICE + upstream license file must exist (provenance table).
for n in \
    third_party/bsd/dash/NOTICE \
    third_party/bsd/dash/COPYING \
    third_party/licenseref-zsh/zsh/NOTICE \
    third_party/licenseref-zsh/zsh/LICENCE \
    third_party/bsd/tcsh/NOTICE \
    third_party/bsd/tcsh/Copyright \
    third_party/bsd/openssh/NOTICE \
    third_party/bsd/openssh/LICENCE \
    third_party/apache-2.0/openssl/NOTICE \
    third_party/apache-2.0/openssl/LICENSE.txt \
    third_party/public-domain/ed25519/NOTICE \
    third_party/public-domain/ed25519/ed25519.c
do
    if [ ! -f "$n" ]; then
        echo "REJECT (missing provenance file): $n"
        bad=1
    fi
done

# --- license-named files: fail only on leftover GNU/CDDL license blobs ---
# Filename heuristic matches check-license.sh plus copyright/copyleft.
# Do not hard-fail configure / aclocal.m4 (Autoconf unlimited-permission).
# Official package licenses that mention omitted GPL are not FSF blobs.
license_list="${TMPDIR:-/tmp}/gj-tp-license-list.$$.txt"
cleanup_tp() {
    rm -f "$license_list" 2>/dev/null || true
}
trap cleanup_tp EXIT INT TERM
: >"$license_list"

find ./third_party/bsd/dash ./third_party/licenseref-zsh/zsh ./third_party/bsd/tcsh \
     ./third_party/bsd/openssh ./third_party/apache-2.0/openssl \
     ./third_party/public-domain/ed25519 -type f \
    \( -iname '*copying*' -o -iname '*copyright*' -o -iname '*gpl*' \
       -o -iname 'LICENSE*' -o -iname 'LICENCE*' -o -iname '*copyleft*' \) \
    2>/dev/null >"$license_list" || true

# FSF/CDDL license-document markers (the file *is* that license).
# Mentions of "GNU General Public License" in a BSD/zsh NOTICE or in
# dash/COPYING's omitted-mksignames paragraph are not this pattern.
copyleft_blob='GNU GENERAL PUBLIC LICENSE[[:space:]]+Version|GNU LESSER GENERAL PUBLIC LICENSE[[:space:]]+Version|GNU LIBRARY GENERAL PUBLIC LICENSE[[:space:]]+Version|GNU AFFERO GENERAL PUBLIC LICENSE[[:space:]]+Version|TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION|Everyone is permitted to copy and distribute verbatim copies of this license document|COMMON DEVELOPMENT AND DISTRIBUTION LICENSE|SPDX-License-Identifier:[[:space:]]*(GPL|LGPL|AGPL|CDDL)'

while IFS= read -r f; do
    [ -n "$f" ] || continue
    license_scanned=$((license_scanned + 1))
    case "$f" in
        */NOTICE|*/NOTICE.md|*/PROVENANCE.md|./third_party/PROVENANCE.md)
            echo "check-third-party-license: soft skip omission-doc $f"
            license_skip=$((license_skip + 1))
            continue
            ;;
        */configure|*/aclocal.m4)
            echo "check-third-party-license: soft skip autoconf $f"
            license_skip=$((license_skip + 1))
            continue
            ;;
    esac
    if grep -Eqi "$copyleft_blob" "$f" 2>/dev/null; then
        echo "REJECT (copyleft license file): $f"
        bad=1
        license_hits=$((license_hits + 1))
    fi
done <"$license_list"

cleanup_tp
trap - EXIT INT TERM

echo "check-third-party-license: soft report dash_files=$n_dash zsh_files=$n_zsh tcsh_files=$n_tcsh openssh_files=$n_openssh openssl_files=$n_openssl omitted_checked=$omitted_checked omitted_present=$omitted_present license_name_scanned=$license_scanned license_skip=$license_skip copyleft_license_hits=$license_hits"

if [ "$bad" -ne 0 ]; then
    echo "check-third-party-license: FAILED"
    exit 1
fi
echo "check-third-party-license: OK"
