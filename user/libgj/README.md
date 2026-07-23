# libgj (freestanding userspace)

Tiny **pure C** runtime for GreenJade freestanding ELFs — string/memory helpers
and syscall wrappers. Not a libc: no `malloc`, no `stdio`, no TLS. Product path
for `init`, `shell`, `ld-gj`, and door servers (`vfsd`, `sessiond`, `netstackd`,
`sshd`, `storaged`, `scsi_mid`, `hda_client`, …).

## License

**MIT OR Apache-2.0** (dual). Every source file carries:

```text
SPDX-License-Identifier: MIT OR Apache-2.0
Copyright (c) 2026 Project GreenJade contributors
```

Does **not** implement glibc symbols and does **not** touch `user/libcgj/`.

## Status

| Artifact | Build | Role |
|----------|-------|------|
| `build/user/libgj.a` | `make userland` / parent archive rule | Static freestanding library |

Linked into freestanding user ELFs with `-Iuser/libgj/include` and
`user/init/user.ld` (typical load VA `0x1000000`).

## Layout

| Path | Role |
|------|------|
| `include/gj/gj.h` | Umbrella — string + syscalls |
| `include/gj/string.h` | Freestanding string/memory/format API |
| `include/gj/syscalls.h` | `GJ_SYS_*` / `LINUX_NR_*` + convenience inlines |
| `src/string_u.c` | String/memory/format implementations |
| `src/syscall.S` | `gj_syscall6` (x86_64 `SYSCALL`) |
| `README.md` | This file |

Source of truth under this tree: the files above only. Shared objects for
dynlink smokes live in sibling trees:

| Tree | Product SO |
|------|------------|
| `user/libgj-so/` | `libgj-so.so.1` — SysV `DT_HASH` |
| `user/libgj-gnu/` | `libgj-gnu.so.1` — `DT_GNU_HASH` |

## Headers

```c
#include <gj/gj.h>          /* or: */
#include <gj/string.h>
#include <gj/syscalls.h>
```

### String / memory (`gj/string.h`)

| Symbol | Notes |
|--------|-------|
| `gj_strlen` / `gj_strnlen` | Length; NULL-safe → 0 |
| `gj_strcmp` / `gj_strncmp` | Lexicographic compare |
| `gj_strcpy` / `gj_strncpy` | Copy |
| `gj_strcat` / `gj_strncat` | Concat |
| `gj_strlcpy` / `gj_strlcat` | BSD-style bounded; always NUL-terminates when `cbDst > 0` |
| `gj_strchr` / `gj_strrchr` / `gj_strstr` | Search |
| `gj_memchr` / `gj_memcpy` / `gj_memmove` / `gj_memset` / `gj_memcmp` | Memory |
| `gj_puts` | Linux `write(1, …)` of a C string |
| `gj_write` | Linux `write(fd, …)` |
| `gj_dlog` | Native `GJ_SYS_DEBUG_LOG` of a C string |
| `gj_itoa` / `gj_utoa` / `gj_xtoa` | Decimal / unsigned decimal / lowercase hex |

### Syscalls (`gj/syscalls.h`)

- **Raw:** `gj_syscall0`…`gj_syscall6` (args → SysV → Linux/GJ `SYSCALL` regs).
- **Native doors:** `gj_exit`, `gj_yield`, `gj_debug_log`, session/net/store/vfs,
  HDA, SCSI, notify, memobj, GPU present, platform info, console.
- **Linux personality:** `linux_read` / `linux_write` / `linux_openat` /
  `linux_mmap` / `linux_fork` / `linux_execve` / `linux_nanosleep` / … (subset).

Native numbers must stay aligned with `kernel/include/gj/syscall.h`.

## Build

Parent Makefile archives objects already listed there:

```sh
make userland
# → build/user/libgj.a
#   (from src/string_u.c + src/syscall.S)
```

Manual freestanding flags (match other user ELFs):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/libgj/src/string_u.o user/libgj/src/string_u.c
cc … -c -o build/user/libgj/src/syscall.o user/libgj/src/syscall.S
ar rcs build/user/libgj.a \
   build/user/libgj/src/string_u.o build/user/libgj/src/syscall.o
```

## Smoke markers

Freestanding consumers exercise libgj on boot; examples:

```text
greenjade-init: libgj string PASS
ld-gj: hash/sym gj_so_export PASS
ld-gj: hash/sym gj_gnu_export PASS
```

(`gj_so_export` / `gj_gnu_export` come from the sibling product SOs, not this
archive.)

## Related (outside this tree)

| Component | Relation |
|-----------|----------|
| `user/init/` | First process; links `libgj.a` |
| `user/ld-gj/` | Dynlinker; resolves product SOs |
| `user/libgj-so/` | SysV-hash product SO |
| `user/libgj-gnu/` | GNU-hash product SO |
| `user/libcgj/` | Full libc surface — separate ownership; do not merge |
