# OpenSSH-portable (product SSH)

Vendored **OpenSSH 10.5p1** on the **Linux ABI**, with **OpenSSL 3.5.7 LTS
libcrypto** (Apache-2.0). Not `sshd_gj`. DUT trio exists (`make openssh-dut`).
Compile / embed ≠ Dual DoD **B** close.

| Field | Value |
|-------|--------|
| **Vendor** | [`third_party/bsd/openssh/`](../../third_party/bsd/openssh/) · [`third_party/apache-2.0/openssl/`](../../third_party/apache-2.0/openssl/) |
| **Glue** | this directory (`config.h`, crt0, libc/OpenSSL include overlays) |
| **License** | OpenSSH: BSD/ISC (no GPL). OpenSSL 3.x: **Apache-2.0**. Glue: MIT OR Apache-2.0 |
| **Build** | Glue: `make openssl-gj` then `make openssh-gj`. DUT: `make openssl-libcgj` then `make openssh-dut`. No `sshd_gj` embed. |
| **Host libcrypto** | `build/openssl/libcrypto.a` (host-libc; compile stepping stone) |
| **DUT libcrypto** | `make openssl-libcgj` → `build/openssl-libcgj/libcrypto.a` |
| **DUT trio** | `make openssh-dut` → `build/openssh-dut/{sshd,sshd-session,sshd-auth}` (LINUX @ `0x4000000`) |
| **Dual DoD B** | **OPEN** until host interactive login via this daemon |
| **sshd_gj** | **Abandoned** (`abandoned/user/sshd/`). Not linked. |
| **WITH_OPENSSL** | **1** in `include/config.h` (do not `#undef`) |
| **SANDBOX_NULL** | **1** — not a confine claim |

## Why libcrypto (security first)

OpenSSH 10.5 **requires** ECC in libcrypto, including **NIST P-521**.
Upstream documents `--without-openssl` as **EXPERIMENTAL**. Product SSH
does not take that reduced path. 3.5 is LTS through **2030**.

OpenSSL **1.1.1** is not this tree (old OpenSSL+SSLeay license, EOL).
OpenSSL **4.0** is not this cut (shorter support window).

Sandbox: glue uses `SANDBOX_NULL` until GJ has a real sandbox. That is
**not** a confine claim.

## Glue headers

Host POSIX compiles use `#include_next` into glibc (and gcc `limits.h`)
for overlays. Do not fake `openssl/configuration.h`: openssl-gj writes
it under `build/openssl/include`. `include/openssl/opensslv.h` is a thin
`#include_next` of that generated tree.

`nlist.h` is a BSD parse stub only (`HAVE_NLIST` unset). It does **not**
`#include_next` host elfutils `nlist.h` (GPL, different layout).

## Glue TUs (`make openssh-gj`)

`openssh-gj` depends on `openssl-gj` and compiles `OPENSSH_GLUE_OBJS`.
It does **not** link a daemon. Stale “ssherr + crt0 only” is wrong.

Freestanding (`OPENSSH_CFLAGS`; no host libc):

| Object | Source |
|--------|--------|
| `build/user/openssh/ssherr.o` | `third_party/bsd/openssh/ssherr.c` |
| `build/user/openssh/crt0.o` | `user/openssh/crt0.S` |

Host POSIX (`OPENSSH_POSIX_CFLAGS`; generated OpenSSL headers; not a
libcgj shadow):

```sh
cc -std=c11 -Wall \
  -Iuser/openssh/include -Ithird_party/bsd/openssh \
  -Ithird_party/bsd/openssh/openbsd-compat \
  -Ibuild/openssl/include -Ithird_party/apache-2.0/openssl/include \
  -DHAVE_CONFIG_H -c
```

| Object | Source |
|--------|--------|
| `atomicio.o` | `atomicio.c` |
| `bitmap.o` | `bitmap.c` |
| `match.o` | `match.c` |
| `xmalloc.o` | `xmalloc.c` |
| `log.o` | `log.c` |
| `fatal.o` | `fatal.c` |
| `sshbuf.o` | `sshbuf.c` (`sshbuf.h` uses generated `openssl/bn.h` / `ec.h` / `evp.h`) |
| `sshbuf-getput-basic.o` | `sshbuf-getput-basic.c` |
| `openbsd-compat/strlcpy.o` | `openbsd-compat/strlcpy.c` |
| `openbsd-compat/strlcat.o` | `openbsd-compat/strlcat.c` |
| `openbsd-compat/timingsafe_bcmp.o` | `openbsd-compat/timingsafe_bcmp.c` |
| `openbsd-compat/openssl-compat.o` | `openbsd-compat/openssl-compat.c` |

Not in `OPENSSH_GLUE_OBJS`: the DUT trio (`make openssh-dut`). `sshd_gj` is
abandoned (not linked). Glue compile ≠ Dual DoD **B** close.

Dual DoD **B** **OPEN**. No stamp bump. No flash.
