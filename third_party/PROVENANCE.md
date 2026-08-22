# Third-party provenance

Project **kernel / `user/` glue / `scripts/` / `abandoned/`** is dual-licensed
**MIT OR Apache-2.0** (root `LICENSE`). That dual license does **not**
cover vendored trees. Non-dual source lives **only** under `third_party/`,
in a **folder named for that license** (`apache-2.0/`, `bsd/`,
`licenseref-zsh/`, `public-domain/`). Do not mix vendor sources into
`kernel/`, `user/`, or `abandoned/`.

Allowed third-party licenses in this repo:

- **MIT**, **Apache-2.0**, **BSD** (2-clause / 3-clause / ISC)
- CC0 / public domain
- **zsh’s own MIT-like licence** (`LicenseRef-zsh`)

Do **not** import GPL / LGPL / AGPL / CDDL into the product core or
vendor trees. Gates: `make license` → `scripts/check-license.sh` (product)
and `scripts/check-third-party-license.sh` (vendor omissions).

This is not legal advice. Dual DoD A/B stay **OPEN**.

## What is actually used (linked / shipped)

| Artifact | Sources | License to follow | Linked? |
|----------|---------|-------------------|---------|
| Product kernel TCB (`build/greenjade.elf`) | `kernel/` | MIT OR Apache-2.0 | yes |
| DUT OpenSSH trio | `third_party/bsd/openssh` + `user/openssh` glue + `third_party/apache-2.0/openssl` libcrypto | OpenSSH **BSD/ISC**; OpenSSL **Apache-2.0**; glue **MIT OR Apache-2.0** | yes (`make openssh-dut`; embeds) |
| Product `/bin/sh` | `third_party/bsd/dash` + `user/dash` glue (`crt0`, headers, `signames.c`) | dash **BSD-3-Clause**; glue **MIT OR Apache-2.0** | yes (`make dash`) |
| zsh / tcsh | `third_party/licenseref-zsh/zsh`, `third_party/bsd/tcsh` | LicenseRef-zsh / BSD-3-Clause | **no** (vendor only; not in `C_SRCS`) |
| `sshd_gj` | `abandoned/user/sshd/` (GJ dual only) | MIT OR Apache-2.0 | **no** (abandoned, not linked) |
| Abandoned Ed25519 | `third_party/public-domain/ed25519/` | Public Domain | **no** (not next to GJ dual sources) |
| `scripts/openssl-gj-perl/` | original GJ `FindBin` / `IPC::Cmd` / `Time::Piece` stand-ins | **MIT OR Apache-2.0** | host Configure only; **not** Perl’s dual GPL/Artistic modules; not in libcrypto |
| Host-collected `.ko` | `scripts/collect-linux-drivers.sh` → ESP leftover | often **GPL binaries** | **not** kernel-linked (**G-AC-1**). No GPL **source** in this tree |

Packed rootfs must carry vendor texts under `/usr/share/licenses/`
(`scripts/stage-rootfs.sh`): `greenjade/LICENSE`, `dash/COPYING`,
`openssh/LICENCE`, `openssl/LICENSE.txt` (+ GJ `openssl/NOTICE`).

## Vendor trees

| Path | Upstream | License | Version / git | Hash | Notes |
|------|----------|---------|---------------|------|-------|
| `third_party/bsd/dash/` | http://gondor.apana.org.au/~herbert/dash/files/dash-0.5.13.5.tar.gz | BSD-3-Clause | 0.5.13.5 (2026-08-14) | sha256 `40090101a2a491f13e901d3d48e90414f26634628b9bfff35ff540363c227a7d` (tarball) | Official Herbert Xu release. Omitted GPL: `compile`, `depcomp`, `missing`, `src/mksignames.c`. Product `/bin/sh`. GJ `user/dash/signames.c` replaces mksignames (original table, not bash). |
| `third_party/licenseref-zsh/zsh/` | https://www.zsh.org/pub/zsh-5.9.2.tar.xz | LicenseRef-zsh (MIT-like; `LICENCE`) | 5.9.2 / tag `zsh-5.9.2` (2026-08-14) | sha256 `36fa734374b44783582cec09bcd67822e2f992c779ec1624ab5596df078d2f81` (tarball; matches zsh.org SHA256SUM) | Official zsh.org. Omitted GPL completions / `config.guess` / `config.sub`. **Not linked.** |
| `third_party/bsd/tcsh/` | https://astron.com/pub/tcsh/tcsh-6.24.16.tar.gz | BSD-3-Clause | 6.24.16 / tag `TCSH6_24_16` (2026-08-14) | sha256 `4208cf4630fb64d91d81987f854f9570a5a0e8a001a92827def37d0ed8f37364` (tarball) | Official astron.com. Omitted GPL: `acaux/config.guess`, `acaux/config.sub`, `nls/pl/`. **Not linked.** |
| `third_party/bsd/openssh/` | https://cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-10.5p1.tar.gz | BSD/ISC (`LICENCE`; “OpenSSH contains no GPL code.”) | 10.5p1 (2026-08-21) | sha256 `d44d28a839ea9daf969cc69150fde59910b2b39361dad81a3bd6cbd19218db11` (tarball) | Official portable. Omitted GPL Autoconf: `config.guess`, `config.sub`. Product SSH on Linux ABI. libcrypto = `third_party/apache-2.0/openssl`. |
| `third_party/apache-2.0/openssl/` | https://github.com/openssl/openssl/releases/download/openssl-3.5.7/openssl-3.5.7.tar.gz | Apache-2.0 (`LICENSE.txt`) | 3.5.7 LTS (2026-08-21) | sha256 `a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8` (tarball) | Official 3.5 LTS (EOL 2030). OpenSSH 10.5 needs ECC/P-521. Omitted GPL-dual: `external/perl/Text-Template-1.56/`. Empty upstream submodule dirs (`oqs-provider`, `wycheproof`, …) have **no source** imported. |
| `third_party/public-domain/ed25519/` | OpenSSH portable `ed25519.c` / SUPERCOP `crypto_sign/ed25519/ref` | Public Domain | OpenBSD `ed25519.c,v 1.8 2026/06/14` / git `ecbf552d8286468e88e6c75d9c090c2f8c60ec7b` | sha256 `ac65b7840b922f11d7618320714f5cf22bd6cece8efab146e9688c70ffbe9895` (upstream raw) | Bernstein, Duif, Lange, Schwabe, Yang. **Not** dual-licensed GJ. Moved out of `abandoned/user/sshd/src/`. SHA-512 backend stays GJ dual in `abandoned/user/sshd/src/ssh_crypto.c`. |

Each subtree keeps upstream `COPYING` / `LICENCE` / `LICENSE.txt` / `Copyright` plus a GreenJade `NOTICE` (URL, version, date, hash, omitted files).

The path `thirdparty` is a symlink alias to `third_party/`.

## Abandoned (not product, not linked)

| Path | Upstream | License | Notes |
|------|----------|---------|-------|
| `abandoned/user/sshd/` | GJ NATIVE probe | MIT OR Apache-2.0 | Was `user/sshd/`. `make sshd-gj` exits 1. Do not re-link. Public Domain `ed25519.c` is **not** here — see `third_party/public-domain/ed25519/`. |

## Product glue (GJ dual, not vendor)

| Path | Role | License |
|------|------|---------|
| `user/openssh/` | DUT crt0, `config.h`, include overlays | MIT OR Apache-2.0 |
| `user/dash/` | DUT crt0, headers, `signames.c` | MIT OR Apache-2.0 |
| `scripts/openssl-gj-perl/` | EL9 Configure stand-ins (`FindBin`, `IPC::Cmd`, `Time::Piece`) | MIT OR Apache-2.0 — **not** CPAN Perl (Artistic/GPL) |
| `scripts/openssl-gj-configure.sh` / `openssl-libcgj-configure.sh` | out-of-tree OpenSSL Configure | MIT OR Apache-2.0 |

## Leftover host `.ko` media (not product source)

`make collect-linux-drivers` / `make hwtest-img` may copy host Linux
modules (e.g. `r8169.ko`) onto ESP/GJ-PERSIST. Those blobs are often GPL.
They are **not** in `C_SRCS` / `S_SRCS`, **not** executed in the kernel
(**G-AC-1**), and **not** GreenJade dual-license source. Do not treat
them as product AC. Freestanding class drivers live under `abandoned/`
and are not linked.

## VENDOR_STATUS

| Tree | Status | SPDX imported |
|------|--------|----------------|
| dash | **OK** | BSD-3-Clause |
| zsh | **OK** | LicenseRef-zsh |
| tcsh | **OK** | BSD-3-Clause |
| openssh | **OK** | BSD/ISC |
| openssl | **OK** | Apache-2.0 |
| ed25519 | **OK** | Public Domain |

If a fetch had failed, the tree would be a stub plus `LICENSE-FAIL`. None failed.

## Import checklist

1. Confirm license is MIT, Apache-2.0, BSD, ISC, CC0/public domain, or zsh’s MIT-like licence
2. Record URL + version + content hash in this file and the subtree `NOTICE`
3. Do **not** import GPL/LGPL/AGPL/CDDL (omit that file; do not rewrite the shell)
4. Keep upstream license files; add `NOTICE` (URL, version, date)
5. Prefer rewrite under GreenJade MIT OR Apache-2.0 only if unsure — **not** for these vendor trees
6. If the artifact is **linked or packed**, stage the license text with the binary
