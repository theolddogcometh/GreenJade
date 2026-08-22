# Overnight OpenSSH — Dual DoD B

Law: [AGENTS.md](../AGENTS.md). Stamp `GJ_IMAGE_VERSION`. Dual DoD **B OPEN** until host
**interactive SSH login** on **10.200.125.50**. Banner / `nc` Connected / PK_OK /
SUCCESS / QEMU `SSH-2.0` ≠ close. No flash. No DUT relink unless named. No stamp invent.

Prefer **`/workflow overnight-sshd`** over ad-hoc STOP-loop swarms.

## Baseline (do not lose)

Keep a hashed **QEMU51-class** kernel ELF (listen + `271 ret=1` + accept + USER fork).
Log: `/tmp/gj-overnight-sshd51.log` (2026-08-22). DUT `build/openssh-dut/sshd`
entry **0x4000000**, size **7967144**, mtime **02:46** — do not relink.

After last `Server listening` a passing T0 boot must still show:

| Key | Meaning |
|-----|---------|
| `sshd: live OpenSSH` `entry=0x4000000` | Product SSH, not `sshd_gj`, not `0x1000000` |
| `Server listening on 0.0.0.0 port 22.` | Listen works — not the hole |
| `271 ret=1` (not only `ret_later`) | `ppoll` returned ready |
| `sys nr=43` `ret=` (newsock) | accept |
| `process: linux_fork user child` | USER child after accept |
| no `SSH-2.0` | expected until session exec; Dual DoD B still OPEN |

QEMU51 then last-ref `soft close :22 fd=99` (parent close of newsock). That is the
**inherit** wall — only after handshake+accept+fork still work.

## One cycle

1. **One named hole** (one sentence). Not PTY, vfork, dash, listen-not-working, `0x1000000`.
2. **One writer file.** Parallel writers on the same path are forbidden.
3. **One QEMU id** (`sshdN.log`). Hash `build/greenjade.elf` before the boot.
4. Score vs baseline keys. **Regress** = baseline had 271/accept/fork and this boot does not.
5. **Identical-hang cap = 2.** Two consecutive boots with the same last greppable line
   after `Server listening` (e.g. `rx demux first` / `tcp_poll after_first`) and no
   `271 ret=` → **stop writers**. Revert to the baseline ELF. Do not stack virtio +
   ppoll + inherit in one ELF.
6. Honesty (read-only) + at most one explore on the named hole. No STOP-loop farm.
7. End the cycle with a 12-line scoreboard. Dual DoD B OPEN.

## Coordinator

At most **one** durable schedule. Its prompt must name the **live** hole and the
baseline log. Update the prompt when the hole changes. STOP at the operator
deadline (`TZ=America/Chicago`). Do not spawn sibling STOP/path-lock loops.

## Off-path unless named this turn

PTY / `TIOCSCTTY` · vfork · dash `/bin/sh` · GOP/STATUS lamps · `rtl8168_udx` laptop
wire · Dual DoD A `USBCMD.RS=1` · densify kprintf · relink DUT · stamp bump.

Product SSH = OpenSSH-portable 10.5p1 LINUX `user.ld` **0x4000000**.
`abandoned/user/sshd` is not product.
