# shell (product)

Freestanding product shell for GreenJade — pure C11, libgj only. VFS door
client (NATIVE personality) that runs a scripted command suite for smoke,
then exits without hanging. Optional install-mode accepts a bounded number
of serial lines after the script.

## License

Product code: **MIT OR Apache-2.0** (dual license; no GPL source).

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/shell.elf` | `make shell-gj` / `make userland` | Freestanding product shell (`shell.c`) |

Linked freestanding with `libgj` and `user/init/user.ld` (load VA `0x1000000`).

This directory owns only `user/shell/**`. Parent Makefile / kernel embed /
boot spawn live outside this tree.

### Freestanding path (`shell.c`)

1. **Banner** — `ready` + `abi PASS`
2. **Console poll** — non-blocking `gj_console_poll` (must not hang smoke)
3. **Scripted suite** — echo/cat/ls/… continuum utilities over VFS door
4. **Store / net soft** — storecap, storestats, storeflush, netstats
5. **Readline** — idle timeout in QEMU smoke → `readline idle PASS`; live serial → install mode (≤8 lines)
6. **Aggregate PASS** lines + `vfs PASS` + `interactive PASS` + exit(0)

Boot embed (parent): `kernel/proc/shell_embed.S` (`.incbin` of
`build/user/shell.elf`). Kernel spawn markers include `shell: live spawn PASS`.

## Smoke markers

`scripts/smoke-all.sh` greps these substrings (prefix-stable; do not rename):

```text
greenjade-shell: vfs PASS
greenjade-shell: interactive PASS
greenjade-shell: console poll PASS
greenjade-shell: readline idle PASS
greenjade-shell: install ready PASS
greenjade-shell: install cmd PASS
greenjade-shell: stat touch cp PASS
greenjade-shell: storecap PASS
greenjade-shell: storestats PASS
greenjade-shell: storeflush PASS
greenjade-shell: netstats PASS
greenjade-shell: whoami date env PASS
GreenJade shell
```

### Typical freestanding sequence

```text
greenjade-shell: ready
greenjade-shell: abi PASS
greenjade-shell: console poll PASS
greenjade-shell: interactive script start
greenjade-shell$ echo …
…
greenjade-shell: readline test
greenjade-shell: readline idle PASS
greenjade-shell: install ready PASS
greenjade-shell: stat touch cp PASS
…
greenjade-shell: storeflush PASS
greenjade-shell: vfs PASS
greenjade-shell: interactive PASS
```

| Marker | Meaning |
|--------|---------|
| `ready` / `abi PASS` | `_start` entered; ABI banner |
| `console poll PASS` | Non-blocking console poll returned ≥ 0 |
| `interactive script start` | Scripted command loop begins |
| `readline idle PASS` | No serial input in smoke (timeout) |
| `install ready PASS` | Install-mode path finished or idle path OK |
| `vfs PASS` | VFS door command path green (required) |
| `interactive PASS` | Full script + readline path green (required) |

Failure lines: `greenjade-shell: cmd FAIL` + the failing line; exit codes
`7` console, `8` readline, `10+i` script step `i`.

Kernel companion (not from this directory):

```text
shell: live spawn PASS
```

## Packaging (parent scripts)

- rootfs: `bin/sh`, `bin/greenjade-shell`, `usr/bin/sh` ← `build/user/shell.elf`
- ESP: `EFI/GREENJADE/user/shell.elf`

## Build

```sh
make shell-gj
# → build/user/shell.elf

# or:
make build/user/shell.elf
make userland
```

Manual (same `USER_CFLAGS` / link as other freestanding ELFs):

```sh
cc -std=c11 -ffreestanding -fno-builtin -fno-stack-protector \
   -fno-pic -mno-red-zone -msse2 -Wall -Wextra -Werror \
   -Iuser/libgj/include -g -O2 \
   -c -o build/user/shell/shell.o user/shell/shell.c
ld -nostdlib -static -T user/init/user.ld -z max-page-size=0x1000 \
   -o build/user/shell.elf build/user/shell/shell.o build/user/libgj.a
```

## Layout

| Path | Role |
|------|------|
| `shell.c` | `_start` freestanding product shell + scripted smoke |
| `README.md` | This file |

Source of truth under this tree: `shell.c` only.

## Live embed (parent — docs only)

Parent tree owns:

- Makefile target `shell-gj` → `build/user/shell.elf`
- `kernel/proc/shell_embed.S` (`.incbin` + blob symbols)
- Boot spawn of the ELF after VFS is ready

Do not treat this directory as owning Makefile or kernel edits.
