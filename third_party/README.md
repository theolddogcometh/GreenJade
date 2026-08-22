# third_party/

These trees are **not** product **MIT OR Apache-2.0**. They are **vendored
upstream** snapshots, grouped **by license**. Do not put this source under
`kernel/`, `user/`, `scripts/`, or `abandoned/` next to GJ dual-licensed
code. Canonical table: [PROVENANCE.md](PROVENANCE.md). Soft≠product.

| Folder (license) | Package | Linked into product? |
|------------------|---------|----------------------|
| `bsd/dash/` | POSIX `/bin/sh` (BSD-3-Clause) | **yes** (`make dash` + rootfs) |
| `bsd/openssh/` | OpenSSH-portable 10.5p1 (BSD/ISC) | **yes** (`make openssh-dut` + embeds) |
| `bsd/tcsh/` | C-shell (BSD-3-Clause) | **no** |
| `apache-2.0/openssl/` | OpenSSL 3.5.7 LTS libcrypto | **yes** (DUT libcrypto) |
| `licenseref-zsh/zsh/` | zsh 5.9.2 | **no** |
| `public-domain/ed25519/` | SUPERCOP/OpenSSH `ed25519.c` | **no** (abandoned `sshd_gj` only) |

**No bash.** There is no bash in this directory and none is planned.

Each subtree keeps its **upstream** `COPYING` / `LICENCE` / `LICENSE.txt` /
`Copyright` and a GreenJade `NOTICE` (URL, version, fetch date, tarball
hash, SPDX, omitted files).

The path `thirdparty` is a symlink alias to this directory (`third_party/`).

## What this is not

- Not product kernel TCB. GJ glue is `user/openssh/` and `user/dash/`
  (MIT OR Apache-2.0).
- Not a clean-room rewrite of the shells.
- **dash** is the product `/bin/sh`. zsh/tcsh are not linked.
- **openssh** + **openssl** are the product SSH stack.
  `abandoned/user/sshd/` (`sshd_gj`) is **not** linked. OpenSSL 3.x is
  Apache-2.0. Do not ship `--without-openssl` (upstream experimental).
- Not a reason to weaken `scripts/check-license.sh`. That gate **skips**
  `third_party/` and **fails** GPL SPDX in the product tree.

## License policy

Allowed here: MIT, Apache-2.0, BSD-2/3, ISC, CC0, zsh’s own MIT-like
licence. **Rejected:** GPL, LGPL, AGPL, CDDL. Individual GPL files found
in the official tarballs were **not imported** (listed in each `NOTICE`
and `PROVENANCE.md`). Packed images copy vendor texts to
`/usr/share/licenses/` (`scripts/stage-rootfs.sh`).
