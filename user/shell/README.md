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
5. **Soft status one-liners** — product readiness / continuum soft tags (see below)
6. **Readline** — idle timeout in QEMU smoke → `readline idle PASS`; live serial → install mode (≤8 lines)
7. **Aggregate PASS** lines (incl. soft-status group lines) + `vfs PASS` + `interactive PASS` + exit(0)

Boot embed (parent): `kernel/proc/shell_embed.S` (`.incbin` of
`build/user/shell.elf`). Kernel spawn markers include `shell: live spawn PASS`.

## Soft status notes (scripted one-liners)

Soft readiness / continuum tags run in the freestanding script **before**
`version` / `install` / `exit`. They print simple facts (`0` / `1` or two
short lines for `ubar3soft`) and **never** hard-fail the interactive path.

**Honesty bounds (do not invent bar3):**

- Soft tags are **not** Deck Top 50 evidence and **not** bar3 close.
- `ubar3soft` / `ubar3open` / `usteamrun` stay soft inventory only — matrix
  title rows remain **NOT-TRIED** until a real client launch (see
  `docs/STEAM_BAR3_STATUS.md`).
- `1` means “path/soft surface wired or shipped as a soft fact”; `0` means
  “still open / not live yet” — not a FAIL of the shell smoke.

### Soft one-liners in the smoke script (product v1.65–v1.68)

| Cmd | Prints | Soft meaning |
|-----|--------|--------------|
| `ubar3soft` | `media path wired=1` / `steam client run=0` | Bar3 soft inventory (media READY path ≠ client run) |
| `umediapath` | `1` | Media path wired soft |
| `usteamrun` | `0` | Steam client run soft (still open) |
| `uhdamulti` | `0` | HDA multi soft ready tag until real |
| `uscsimid` | `0` | scsi mid live soft ready tag until real |
| `usshd` | `0` | sshd soft ready tag until real |
| `unetstack` | `0` | netstack soft ready tag until real |
| `upmmhier` | `0` | PMM hierarchical soft ready tag until real |
| `ucontinuum14500` | `1` | Continuum decade 14500 wired soft |
| `uhda_client` | `0` | hda_client embed soft tag until real |
| `uscsi_live` | `0` | scsi live soft tag until real |
| `u768gsoak` | `1` | Large-RAM soak path shipped soft (not bar3) |
| `umakefile14600` | `1` | Makefile decade soft fact |
| `ubar3open` | `0` | Bar3 still open soft |
| `ucontinuum14600` | `1` | Continuum decade 14600 wired soft |
| `uioringmin` | `1` | io_uring min rings soft surface shipped (not full SQE for games) |
| `u1tibopen` | `0` | Full 1 TiB host soak still open soft |

Earlier soft tags (`ubar3checklist`, `uprotonsoft`, `umesasoft`, …) exist as
named cmds but are **not** in the default Multiboot script table above.

### Aggregate soft-status PASS lines

After the continuum utilities pass lines, freestanding emits greppable group
lines (prefix-stable; do not rename):

```text
greenjade-shell: ubar3soft umediapath usteamrun PASS
greenjade-shell: uhdamulti uscsimid usshd PASS
greenjade-shell: unetstack upmmhier PASS
greenjade-shell: ucontinuum14500 uhda_client uscsi_live PASS
greenjade-shell: u768gsoak umakefile14600 ubar3open PASS
greenjade-shell: ucontinuum14600 uioringmin u1tibopen PASS
```

These mean the scripted soft-status cmds returned success (printed their
facts), **not** that bar3/Top-50 product bars closed.

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

Soft-status group lines above are bring-up inventory (not required by
`smoke-all` alone). Store/net soft cmds: `storecap` / `storestats` /
`storeflush` / `netstats` → matching `… PASS` lines.

### Typical freestanding sequence

```text
greenjade-shell: ready
greenjade-shell: abi PASS
greenjade-shell: console poll PASS
greenjade-shell: interactive script start
greenjade-shell$ echo …
…
greenjade-shell$ ubar3soft
media path wired=1
steam client run=0
greenjade-shell$ umediapath
1
…
greenjade-shell$ ucontinuum14600
1
greenjade-shell$ uioringmin
1
greenjade-shell$ u1tibopen
0
…
greenjade-shell: readline test
greenjade-shell: readline idle PASS
greenjade-shell: install ready PASS
greenjade-shell: stat touch cp PASS
…
greenjade-shell: ubar3soft umediapath usteamrun PASS
greenjade-shell: uhdamulti uscsimid usshd PASS
greenjade-shell: unetstack upmmhier PASS
greenjade-shell: ucontinuum14500 uhda_client uscsi_live PASS
greenjade-shell: u768gsoak umakefile14600 ubar3open PASS
greenjade-shell: ucontinuum14600 uioringmin u1tibopen PASS
greenjade-shell: storeflush PASS
greenjade-shell: vfs PASS
greenjade-shell: interactive PASS
```

| Marker | Meaning |
|--------|---------|
| `ready` / `abi PASS` | `_start` entered; ABI banner |
| `console poll PASS` | Non-blocking console poll returned ≥ 0 |
| `interactive script start` | Scripted command loop begins |
| soft-status group `… PASS` | Soft one-liners in that group returned 0 (facts only) |
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
