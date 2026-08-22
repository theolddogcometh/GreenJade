# Third-party provenance

Project code is dual-licensed **MIT OR Apache-2.0**.

Allowed third-party licenses in this tree:

- **MIT**, **Apache-2.0**, **BSD** (2-clause / 3-clause / ISC)
- CC0 / public domain
- **zsh’s own MIT-like licence** (`LicenseRef-zsh`)

Do **not** import GPL / LGPL / AGPL / CDDL.

| Path | Upstream | License | Version / git | Hash | Notes |
|------|----------|---------|---------------|------|-------|
| `third_party/dash/` | http://gondor.apana.org.au/~herbert/dash/files/dash-0.5.13.5.tar.gz | BSD-3-Clause | 0.5.13.5 (2026-08-14) | sha256 `40090101a2a491f13e901d3d48e90414f26634628b9bfff35ff540363c227a7d` (tarball) | Official Herbert Xu release. Git: git.kernel.org utils/dash. Omitted GPL: `compile`, `depcomp`, `missing`, `src/mksignames.c`. Role: `/bin/sh`. |
| `third_party/zsh/` | https://www.zsh.org/pub/zsh-5.9.2.tar.xz | LicenseRef-zsh (MIT-like; `LICENCE`) | 5.9.2 / tag `zsh-5.9.2` (2026-08-14) | sha256 `36fa734374b44783582cec09bcd67822e2f992c779ec1624ab5596df078d2f81` (tarball; matches zsh.org SHA256SUM) | Official zsh.org release. Omitted GPL: `config.guess`, `config.sub`, `_qdbus`, `_darcs`, `_osc`, `_zypper`. Role: default login. |
| `third_party/tcsh/` | https://astron.com/pub/tcsh/tcsh-6.24.16.tar.gz | BSD-3-Clause | 6.24.16 / tag `TCSH6_24_16` (2026-08-14) | sha256 `4208cf4630fb64d91d81987f854f9570a5a0e8a001a92827def37d0ed8f37364` (tarball) | Official astron.com / tcsh-org release. Omitted GPL: `acaux/config.guess`, `acaux/config.sub`, entire `nls/pl/` (“Licencja GPL”). Role: C-shell. |
| `third_party/openssh/` | https://cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-10.5p1.tar.gz | BSD/ISC (`LICENCE`; no GPL) | 10.5p1 (2026-08-21) | sha256 `d44d28a839ea9daf969cc69150fde59910b2b39361dad81a3bd6cbd19218db11` (tarball) | Official portable. Omitted GPL: `config.guess`, `config.sub`. Product SSH on Linux ABI. libcrypto = `third_party/openssl`. |
| `third_party/openssl/` | https://github.com/openssl/openssl/releases/download/openssl-3.5.7/openssl-3.5.7.tar.gz | Apache-2.0 | 3.5.7 LTS (2026-08-21) | sha256 `a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8` (tarball) | Official 3.5 LTS (EOL 2030). OpenSSH 10.5 needs ECC/P-521. Omitted GPL-dual: `external/perl/Text-Template-1.56/`. |

Product `sshd_gj` remains dual MIT OR Apache-2.0 **lab probe** (`user/sshd/`). Product daemon is OpenSSH-portable + OpenSSL 3.5 libcrypto. Ed25519 primitive in the probe is vendored SUPERCOP/OpenSSH **public domain** (`user/sshd/src/ed25519.c`).

| Path | Upstream | License | Version / git | Hash | Notes |
|------|----------|---------|---------------|------|-------|
| `user/sshd/src/ed25519.c` | https://raw.githubusercontent.com/openssh/openssh-portable/ecbf552d8286468e88e6c75d9c090c2f8c60ec7b/ed25519.c | Public Domain (Bernstein, Duif, Lange, Schwabe, Yang; SUPERCOP `crypto_sign/ed25519/ref` supercop-20221122) | OpenSSH portable `ed25519.c` `$OpenBSD: ed25519.c,v 1.8 2026/06/14` / git `ecbf552d8286468e88e6c75d9c090c2f8c60ec7b` | sha256 `ac65b7840b922f11d7618320714f5cf22bd6cece8efab146e9688c70ffbe9895` (upstream raw) | Includes adapted (`includes.h` / OpenSSL wrap removed). SHA-512 backend is product MIT OR Apache-2.0 (`ssh_crypto.c` / `crypto_hash_sha512`). Not the OpenSSH daemon. |

Canonical shells live **here**. The untracked `user/zsh/zsh.c` host-smoke dead-end was deleted — do not continue that clean-room as the product shell.

## VENDOR_STATUS

| Tree | Status | SPDX imported |
|------|--------|----------------|
| dash | **OK** | BSD-3-Clause |
| zsh | **OK** | LicenseRef-zsh |
| tcsh | **OK** | BSD-3-Clause |
| openssh | **OK** | BSD/ISC |
| openssl | **OK** | Apache-2.0 |

If a fetch had failed, the tree would be a stub plus `LICENSE-FAIL` (URL tried). None failed.

## Import checklist

1. Confirm license is MIT, Apache-2.0, BSD, ISC, CC0/public domain, or zsh’s MIT-like licence
2. Record URL + version + content hash
3. Do **not** import GPL/LGPL/AGPL/CDDL (omit that file; do not rewrite the shell)
4. Keep upstream license files; add `NOTICE` (URL, version, date)
5. Prefer rewrite under GreenJade MIT OR Apache-2.0 only if unsure — **not** for these shells
