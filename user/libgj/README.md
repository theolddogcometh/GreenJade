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
| `include/gj/string.h` | Freestanding string/memory/format/parse API |
| `include/gj/syscalls.h` | `GJ_SYS_*` / door ops / `LINUX_NR_*` + convenience inlines |
| `src/string_u.c` | String/memory/format/parse implementations |
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
| `gj_strcasecmp` / `gj_strncasecmp` | ASCII case-insensitive |
| `gj_strcpy` / `gj_strncpy` | Copy |
| `gj_strcat` / `gj_strncat` | Concat |
| `gj_strlcpy` / `gj_strlcat` | BSD-style bounded; always NUL-terminates when `cbDst > 0` |
| `gj_strchr` / `gj_strrchr` / `gj_strstr` | Search |
| `gj_strspn` / `gj_strcspn` | Character-set spans |
| `gj_tolower` / `gj_toupper` | ASCII case |
| `gj_isdigit` / `gj_isalpha` / `gj_isalnum` / `gj_isspace` / `gj_isxdigit` / `gj_isprint` | ASCII class (0/1) |
| `gj_memchr` / `gj_memcpy` / `gj_memmove` / `gj_memset` / `gj_memcmp` / `gj_memccpy` | Memory |
| `gj_atol` / `gj_strtol` / `gj_strtoul` | Parse (no locale / no errno) |
| `gj_puts` | Linux `write(1, …)` of a C string |
| `gj_write` / `gj_read` | Linux `write` / `read` |
| `gj_dlog` | Native `GJ_SYS_DEBUG_LOG` of a C string |
| `gj_itoa` / `gj_utoa` / `gj_xtoa` | Decimal / unsigned decimal / lowercase hex |
| `gj_snprintf` | Soft bounded format (`%s %c %d %u %x %p %%` only; args as `long`/`unsigned long`) |

### Syscalls (`gj/syscalls.h`)

Thin header-only wrappers around `gj_syscall6` (no policy). Soft deepen keeps
numbers aligned with `kernel/include/gj/syscall.h` and door headers.

- **Raw:** `gj_syscall0`…`gj_syscall6` (args → SysV → Linux/GJ `SYSCALL` regs).
- **Native table:** diagnostics, IPC, cap, VM/process, futex/thread, cold IPC,
  GPU, memobj, HDA, session/net/store/vfs, platform, notify, console, SCSI.
- **Door ops:** session / net (1–11, 25–26) / store (1–12) / vfs (1–16 + seek) /
  HDA / SCSI / platform / console — match kernel door headers.
- **Native convenience:** `gj_exit`, `gj_yield`, `gj_debug_log`, IPC/cap/process,
  session/net/store/vfs claim helpers, HDA/SCSI open-path helpers, notify poll,
  memobj, GPU present, platform info.
- **Net ring soft wrappers:** `gj_net_virtio_tx` … `gj_net_bounce_fill` use fixed
  op numbers 12–24 (macros for those ops are opt-in via `GJ_LIBGJ_NET_RING_OPS`
  so freestanding daemons with local fallbacks stay `-Werror` clean).
- **Linux personality:** expanded subset — I/O, path, process, poll/epoll,
  socket, futex, clock, random, memfd, clone/exec, … plus soft flag shapes
  (`GJ_AT_FDCWD`, `GJ_O_*`, `GJ_PROT_*`, `GJ_MAP_*`, `GJ_CLOCK_*`, `GJ_FUTEX_*`).

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

No new `.c` units (parent `LIBGJ_OBJS` is fixed; deepen only existing sources +
headers).

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
