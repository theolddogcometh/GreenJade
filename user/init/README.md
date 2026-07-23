# init (first process)

Freestanding first userspace process for GreenJade — pure C11, libgj only,
Linux personality bring-up and named-memobj smoke.

## License

Product code: **MIT OR Apache-2.0** (dual license; no GPL source).

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/init.elf` | `make userland` / `make build/user/init.elf` | First process scaffold (`init.c`) |

Linked freestanding with `libgj` and `user.ld` (load VA `0x1000000`).

This directory owns only `user/init/**`. Parent Makefile / kernel embed /
boot spawn live outside this tree.

### Freestanding path (`init.c`)

1. **Banner** — `write(1, …)` hello from userspace
2. **libgj** — `gj_puts` string path
3. **Linux ABI (hard)** — uname, `CLOCK_MONOTONIC`, gettimeofday, mmap/mprotect/munmap
4. **FS probe (soft)** — openat/write/fsync, getcwd, getrandom, brk
5. **Rootfs / SO probe (soft)** — `/etc/os-release`, hostname, `ld-gj`, `libc.so.6`, product SOs
6. **Named memobj (soft)** — create + map `init-shm` (wine/GPU export shape)
7. **IPC-ish (soft)** — pipe2, eventfd2, link/symlink/readlink, nanosleep, fork/wait4
8. **Exit** — sched_yield + `abi PASS` + exit(0)

Personality: **LINUX** (Option C hybrid). Kernel embeds the ELF via
`kernel/proc/init_embed.S` (outside this directory).

Hard-fail exit codes: `1` uname, `2` clock_gettime, `3` mmap.

## Smoke markers

`scripts/smoke-all.sh` requires the substring `greenjade-init` in the boot log.
Typical freestanding lines (prefix-stable):

```text
greenjade-init: hello from userspace
greenjade-init: libgj string PASS
greenjade-init: named memobj PASS
greenjade-init: abi PASS
```

| Marker | Meaning |
|--------|---------|
| `hello from userspace` | `_start` banner via `write(1)` |
| `libgj string PASS` | `gj_puts` / string path live |
| `named memobj PASS` | create+map `init-shm` (optional if soft-miss) |
| `abi PASS` | Hard Linux ABI path green; exit(0) |

Packaging (parent scripts):

- rootfs: `sbin/init` ← `build/user/init.elf` (`stage-rootfs`)
- ESP: `EFI/GREENJADE/user/init.elf` (`stage-esp`)

## Build

```sh
make build/user/init.elf   # or: make userland
# → build/user/init.elf
```

Manual (same `USER_CFLAGS` / link as other freestanding ELFs):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/init/init.o user/init/init.c
ld -nostdlib -static -T user/init/user.ld -z max-page-size=0x1000 \
   -o build/user/init.elf build/user/init/init.o build/user/libgj.a
```

## Layout

| Path | Role |
|------|------|
| `init.c` | `_start` freestanding bring-up |
| `user.ld` | Shared freestanding linker script (also used by other user ELFs) |
| `README.md` | This file |

Source of truth under this tree: `init.c` + `user.ld`.

## Live embed (parent — docs only)

Parent tree owns:

- Makefile target → `build/user/init.elf`
- `kernel/proc/init_embed.S` (`.incbin` + blob symbols)
- Boot spawn of the first process

Do not treat this directory as owning Makefile or kernel edits.
