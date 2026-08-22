# GreenJade — agent rules

Freestanding dual **MIT OR Apache-2.0** microkernel + Linux-shaped userspace.
**No GPL in the product core.** Style: [STYLE.md](STYLE.md) (pure C11, Hungarian, SPDX).
Process: [docs/ASSURANCE_LITE.md](docs/ASSURANCE_LITE.md).

Grok harness for this repo lives under [`.grok/`](.grok/). Read this file before
spawning children. Subagents inherit a compacted copy — keep the law here, not
in chat.

## Product law (do not re-litigate)

| Rule | Meaning |
|------|---------|
| **G-AC-1** | No Linux `.ko` runs **in the kernel** as product. |
| **Product drivers** | Linux-shaped **userspace** over hot+cold ABI / doors / **DDI·UDX**. |
| **Soft≠product** | Soft lamps, catalogs, ksym, L1 `assurance-check` **PASS** ≠ product close. |
| **Freestanding class** | `GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`. Tree in `./abandoned` is not linked. |
| **T0 net** | **virtio-net** on QEMU. Laptop wire is **`rtl8168_udx`**. |
| **Dual DoD A** | USB via `xhci_udx`. **OPEN** until host USB path. Scratchpad ≠ close. Never `USBCMD.RS=1` unless the operator named that experiment. |
| **Dual DoD B** | NIC + stack + sshd. **OPEN** until host **interactive SSH login**. Banner / ARP / ping / PK_OK / SUCCESS are proven-class, not close. |
| **0.2.0** | Reserved. Do not ship. |
| **Fly stamp** | `GJ_IMAGE_VERSION` in `kernel/include/gj/config.h`. Do **not invent the next N**. Bump only on a real flashable cut the operator asked for. |

**Never claim:** Dual DoD A/B close, bar3 / Deck Top 50, Steam client, live dash, interactive SSH login — unless the operator has host evidence on **that** stamp.

## Product path (do not invert)

Goal: a **pure C11 microkernel TCB** that hosts **Linux-shaped apps and drivers in userspace** (hot+cold ABI / doors / DDI·UDX), then Steam/Proton. Not a Linux kernel. Not in-kernel class drivers. Not GPL. Not `.ko` in the TCB. Design order: [docs/DESIGN_SPEC_COMPLETE.md](docs/DESIGN_SPEC_COMPLETE.md) §12–13.

| Rung | Close | Not close |
|------|-------|-----------|
| **0 TCB** | Caps, AS, threads, doors, syscall entry. No class driver, no `.ko` | L1 PASS, SKIP lamps |
| **1 Linux ABI** | Userspace personality owns cold NRs; **real** Linux apps (BSD OpenSSH, dash, later Steam) can `socket`/`poll`/`fork`/`exec`/`pty` | NR-table counts, dispatch-soft catalogs, re-linking `abandoned/user/sshd` |
| **2 UDX datapath** | Laptop NIC/USB via `rtl8168_udx` / `xhci_udx` | inject, bind lamps, TE\|RE as Dual DoD close |
| **3 Dual DoD B** | Host **interactive SSH login** on **10.200.125.50** via a **Linux-ABI sshd** (OpenSSH-portable BSD, or Dropbear MIT) | banner / PK_OK / SUCCESS / `true`; RFC 4254 in abandoned `sshd_gj` |
| **4 Dual DoD A** | Host USB path (`xhci_udx`). RS-off until named | scratchpad |
| **5 Live dash** | Interactive `/bin/sh` (own plan; isolate today) | isolate SKIP |
| **6 GPU/input** | Userspace present + input; ICD **not** in kernel `C_SRCS` | vk kernel smoke |
| **7 Steam client** | Client process on GJ | media READY |
| **8 Proton / Top 50** | Matrix from **real** launches | continuum / wave deepen |

**Product SSH** is **OpenSSH-portable 10.5p1** + **OpenSSL 3.5.7 LTS libcrypto** (Apache-2.0) on the Linux ABI. `sshd_gj` is **abandoned** (`abandoned/user/sshd/`, not linked). Do not ship `--without-openssl` (upstream **EXPERIMENTAL**; security first). OpenSSH is **BSD**. OpenSSL **3.x** is Apache-2.0. OpenSSL 1.1.1 is not this tree.

**Next object-code (T0, after 2026-08-22 overnight):** keep QEMU51 keys
(`Server listening`, `271 ret=1`, accept, USER fork) on the current ELF, then
LCN inherit so parent `close(newsock)` is not last-ref, then
`execve /usr/libexec/sshd-session` (then `sshd-auth`) until `SSH-2.0`.
PTY / dash `/bin/sh` are later. Dual DoD **B** still needs host interactive
login. Playbook: [docs/OVERNIGHT.md](docs/OVERNIGHT.md). No stamp invent.
No GOP un-isolate. No `USBCMD.RS=1`. Do not re-link `abandoned/user/sshd`.

**Stop (off-path unless the operator names it this turn):** exclusive soft deepen · continuum `makefile_max` / `graph_batch*` as work · leftover flybar docs as primary · re-link `abandoned/` · in-kernel class drivers · treating `.ko` as product · agent harvest without a named Dual DoD or workflow · **rewriting SSH in-tree instead of the Linux ABI** · STOP-loop / path-lock farms · stacking virtio+ppoll+inherit in one ELF after an identical hang.

Do not climb rungs 6–8 while Dual DoD **B** is OPEN.

## Lab DUT

ASUS ROG **G752VT** · NIC `10ec:8168` · xHCI `8086:a12f` · lab **10.200.125.50** · `:22`.
Panel title after flash: `STATUS (static) v${GJ_IMAGE_VERSION}`.

## Build (what you fly)

```sh
make                      # build/greenjade.elf
make assurance-check      # L1 only — not Dual DoD close
make drivers-udx dash
make hwtest-img           # build/greenjade-hwtest.img
./scripts/gj-image-version.sh --report
```

**Pack proof is extracted media, not `build/esp`.** Mount or `mcopy` from
`build/greenjade-hwtest.img` ESP (GPT p1, byte offset 1 MiB) and SHA-256
`KERNEL.ELF` / `drivers/rtl8168_udx` / `drivers/xhci_udx` /
`rootfs-full/bin/sh` against `build/`. A stale `build/esp` is not evidence.

## Forbidden without an explicit operator sentence this turn

- `dd` to `/dev/sd*`, `/dev/nvme*`, `/dev/mmcblk*`
- `scripts/install-hwtest-usb.sh`, `scripts/install-usb.sh`, `make install-hwtest-usb`, `make install-usb`, `make install-linux-hwtest`
- `scripts/embed-linux-fw.sh` / `embed-linux-mod.sh` (rewrites abandoned kernel embeds)
- Inventing a fly stamp, closing Dual DoD from lamps, densifying kprintf/STATUS/GOP “residuals”
- Exclusive soft deepen, continuum `makefile_max` / `graph_batch*` as work, leftover flybar docs as primary
- Parallel writers on the same file (`kernel/main.c`, `rtl8168_udx.c`, …)
- Overnight STOP-loop / path-lock farms; more than one durable coordinator

Flash is a **human** step: `sudo ./scripts/install-hwtest-usb.sh /dev/sdX` in a
real shell (the flash-deny hook blocks it from Grok). Override only if the
**parent** process has `GJ_FLASH_ALLOW=1` in its environment.

## Agent controls

| Kind | Name | Role |
|------|------|------|
| Agent | `honesty-auditor` | Read-only verdicts. Soft≠product, G-AC-1, Dual DoD wording, stamp match. |
| Agent | `hwtest-packer` | May run `make`. Must not flash. Must not edit product sources unless the task named the file. |
| Agent | `pr-writer` | Patreon + X **only on major milestones**. Stills from `media/` (logo/mascot) or `image_edit` thereof. Never publish. |
| Persona | `fail-closed` | No claim without extracted-object or host-probe evidence. |
| Persona | `no-densify` | No extra lamps, bind dumps, or residual comments unless the task named them. |
| Persona | `overnight` | One hole, one writer file, QEMU51 baseline, identical-hang cap 2. |
| Persona | `m0-ok` | Lab-diary public voice: stamp, evidence, still OPEN. |

If two agents can touch the same path, one of them is wrong. Shard by directory.

### Major milestones (`pr-writer` / `/pr-post` only)

Spawn `pr-writer` **only** when one of these is newly proven (glass + host on that stamp), or the operator names it as the milestone this turn:

| Milestone | Proven when |
|-----------|-------------|
| **M0 OK** | Already shipped (do not re-post unless asked) |
| **Laptop L3** | Host ARP **and** ping on lab **10.200.125.50** (already proven; do not re-post unless asked) |
| **Dual DoD B** | Host **interactive SSH login** (banner / `nc` Connected ≠ this) |
| **Dual DoD A** | Host USB path on `xhci_udx` (scratchpad / RS-off ≠ this) |
| **Live dash** | Interactive `/bin/sh` on the DUT (isolate SKIP ≠ this) |
| **0.2.0** | Operator reserved that stamp and named the close |

**Not a milestone:** any `0.1.N` fly, GOP/STATUS cleanup, hold7 flash, Own-all reclaim, spad clamp, isolate, pack-only, glass without a row above. Refuse those. The operator may override with the sentence **treat this as a major milestone**.

## Workflows (named pipelines — prefer these over ad-hoc swarms)

| Command | What it does |
|---------|----------------|
| `/workflow hwtest-pack` | Inventory → build → extract-hash prove → honesty. Completes a report. Does not flash unless `ask_flash` + human resume. |
| `/workflow check-work-gj {"target":"HEAD"}` | Disjoint read-only review + adversarial verify. |
| `/workflow glass-triage {"stamp":"0.1.184"}` | Score glass/holds/host probes vs a stamp. No writers. |
| `/workflow overnight-sshd {"hole":"…"}` | **One Dual DoD B cycle:** path-lock vs QEMU51 keys, optional one writer file, score, 12-line report. Caps identical hangs. Does not flash. |

Watch runs in `/workflows`. Overnight “work toward goals” is **not** a substitute
for a named workflow and a budget.

## Overnight (hard)

Playbook: [docs/OVERNIGHT.md](docs/OVERNIGHT.md). Persona: `overnight`.

| Rule | Meaning |
|------|---------|
| **Named workflow** | Use `/workflow overnight-sshd`. Do not farm STOP / path-lock `/loop`s. |
| **One hole** | One sentence. Newest log after `Server listening` names it — not PTY/vfork/dash/listen-not-working/`0x1000000`. |
| **One writer** | At most one product file per cycle. Two agents on one path ⇒ one is wrong. |
| **Baseline gate** | Keep QEMU51 keys (`271 ret=1`, accept, USER fork). Losing them is a **regress** — revert that ELF; do not stack another cut. |
| **Hang cap 2** | Two consecutive boots, same hang line, no `271 ret=` after listen → **stop writers**. |
| **One scheduler** | At most one durable coordinator. Prompt must name the **live** hole. STOP at the operator Chicago deadline. |
| **DUT** | Do not relink `build/openssh-dut` unless the operator named it. Entry **0x4000000**. |
| **Scoreboard** | End the cycle in 12 lines. Dual DoD B stays OPEN. |

## Plan vs pack

Use **`/plan`** for Dual DoD A RS-on, live dash, or **Linux ABI enough to run OpenSSH-portable / Dropbear** (do not re-link abandoned `sshd_gj`).
Do **not** plan a stamp bump. Use **`/goal`** for “interactive SSH login on
10.200.125.50” only after a **Linux-ABI OpenSSH** can session — banner / PK_OK /
SUCCESS are not completion evidence.
