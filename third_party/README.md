# third_party/

These trees are **not** product **MIT OR Apache-2.0**. They are **vendored
upstream** snapshots for later GreenJade ABI ports. Soft≠product.

| Path | Role | Product name later |
|------|------|--------------------|
| `dash/` | POSIX `/bin/sh` | dash as `/bin/sh` |
| `zsh/` | default interactive / login shell | zsh |
| `tcsh/` | C-shell option | tcsh |
| `openssh/` | OpenSSH-portable 10.5p1 | product `sshd` on Linux ABI |
| `openssl/` | OpenSSL 3.5.7 LTS libcrypto | libcrypto for that sshd |

**No bash.** There is no bash in this directory and none is planned.

Each subtree keeps its **upstream** `COPYING` / `LICENCE` / `Copyright` and a
GreenJade `NOTICE` (URL, version, fetch date, tarball hash, SPDX, omitted
files). See `PROVENANCE.md` for the table.

The path `thirdparty` is a symlink alias to this directory (`third_party/`).

## What this is not

- Not product kernel / `user/` code.
- Not a clean-room rewrite. The untracked `user/zsh/` (`zsh.c`) host-smoke
  dead-end was deleted; canonical shells are these vendor trees.
- **dash** is the product `/bin/sh` (embed + rootfs + `execve`). zsh/tcsh
  are still not linked. Port of those = GJ ABI later.
- **openssh** + **openssl** are the product SSH stack. `user/sshd/sshd_gj.c`
  is a frozen NATIVE probe. OpenSSL 3.x is Apache-2.0. Do not use
  `--without-openssl` as product (upstream experimental; security first).
- Not a reason to weaken `scripts/check-license.sh`. That gate still
  **skips** `third_party/` and still **fails** GPL in the product tree.

## License policy

Allowed here: MIT, Apache-2.0, BSD-2/3, ISC, CC0, zsh’s own MIT-like
licence. **Rejected:** GPL, LGPL, AGPL, CDDL. Individual GPL files found
in the official tarballs were **not imported** (listed in each `NOTICE`).
