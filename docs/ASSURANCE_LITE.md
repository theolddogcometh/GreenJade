# GreenJade — Assurance Lite

| Field | Value |
|-------|--------|
| **Status** | Living process notes (not a certification claim) |
| **As of** | 2026-08-21 · fly **v0.1.184** packed, not host-probed (exec TX drain after 183 Sending command **PASS** / exec 124; not login; GOP isolate; Dual DoD A/B OPEN; **0.2.0** reserved) |
| **Law** | Dual **MIT OR Apache-2.0**; **no GPL** source in tree |
| **Lawful use** | **You** must not use this project where illegal — [LEGAL_DISCLAIMER.md](LEGAL_DISCLAIMER.md) (not legal advice; not DO-178C) |
| **Inspiration** | **DO-178C** habits (objectives, evidence, independence, config identity) + **Richard Hipp / SQLite** reliability lessons (test what you fly, do not trust compilers alone, design for testability) — **not** DO-178C compliance or “we are SQLite” |
| **Talk pointer** | R. Hipp, *Reliability Lessons From SQLite*, SSW 2026 — [youtu.be/V_qzqY1bb7I](https://youtu.be/V_qzqY1bb7I) (DO-178B-inspired MC/DC story; “we do not trust compilers”) |
| **Does not claim** | Airborne certification · DO-178C/ED-12C conformity · ISO 26262 · bar3 · Steam Top 50 · product net complete · 100% MC/DC for GreenJade · lawful use in every jurisdiction |
| **Companions** | [TODO.md](TODO.md) · [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) · [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [LEGAL_DISCLAIMER.md](LEGAL_DISCLAIMER.md) |

**One sentence:** Use criticality-based evidence so lab DoD, soft residual, and product claims cannot be confused — verify the **binary you flash**, not only the source you wrote.

**Greppable:** `assurance: lite` · Soft≠product · G-AC-1 · Dual DoD · `GJ_IMAGE_VERSION` · `test what you fly` · `object code` · `gj-assurance-check`

**Automated L1 (tree process only):**

```bash
make assurance-check          # scripts/gj-assurance-check.sh — H1/H3 + SKIP + UDX + Soft≠product
make image-version            # stamp report; not Dual DoD close
```

Soft≠product: `assurance-check` **PASS** ≠ Dual DoD **A/B** close (still needs stamped flash + host USB path for **A** + host **interactive SSH login** for **B**). Banner / `nc` Connected / arping / ping / a one-shot `ssh … true` **exit 0** are proven-class, not B close. Fly **0.1.184** packed, not host-probed. **0.1.183** host: `Sending command: true` **PASS**; exec 124. **L1 alone never closes Dual DoD.**

### L1 check surfaces (`gj-assurance-check.sh`)

| Surface | Hard FAIL when | Soft≠product note |
|---------|----------------|-------------------|
| **H1** | `net_eth_poll();` call in timer/apic/x2apic/irq_msix | Permanent hazard; not DoD close |
| **H3** | Missing `thread_exit_process` process+thread wiring | Permanent hazard; not DoD close |
| **Freestanding SKIP defaults** | `GJ_RTL8168_PROBE` or `GJ_XHCI_MSC_PROBE` default ≠ **0** in `config.h` | Freestanding class drivers **not product** |
| **Freestanding SKIP deepen** | Missing `#ifndef` guards; live `kernel/drv/rtl8168.c` / `xhci_msc.c` without SKIP, **or** those files missing from both `kernel/` and `abandoned/` | SKIP = process law, not Dual DoD close. Abandoned + not linked is PASS. |
| **freestanding_no_exec** | `GJ_SOFT_MODULE_RUN_INIT` default ≠ **0**. Live `kernel/mm/linux_module.c` without G-AC-1 honesty (abandoned copy is leftover, not linked) | Soft SKIP `.ko` init in kernel; eng residual |
| **UDX product path** | Missing `user/udx` / class hosts / product-direction honesty (G-AC-1 · Soft≠product · freestanding SKIP · virtio T0) | Product drivers = **userspace UDX+ABI** |
| **UDX deepen** | Missing surface headers (ddi/pci/host/…) · runtime src · `xhci_udx` README parity · thin host `.c` Soft!=product / freestanding SKIP honesty | Headers/src/hosts ≠ product AC; Soft residual |
| **Soft≠product / L1 honesty** | Docs/script claim Dual DoD close from L1, or drop L3 interactive-login close rule | **PASS ≠ Dual DoD close** |
| **Stamp / dual license** | Missing `GJ_IMAGE_VERSION` or thin dual MIT/Apache greps | Identity only; not product PASS |
| **Flash bar** | Stamp shape not `0.N.N` (current fly **v0.1.184**) or historical `YYYY.MM.DD.N` | **Test what you fly**; L3 needs this stamp on media |
| **H2 heuristic** | (warn only) high `kprintf` count in virtio_net | Stamp-storm class watch |

Greppable L1 tokens: `gj-assurance-check` · `assurance: lite` · `Soft!=product` · `freestanding_no_exec` · `freestanding SKIP` · `GJ_RTL8168_PROBE` · `GJ_XHCI_MSC_PROBE` · `UDX` · `G-AC-1` · `product=UDX+ABI` · `test what you fly`

---

## 1. Scope and non-goals

### In scope
- How to **classify** work (soft / lab DoD / product / security).
- What **evidence** closes a claim.
- How **image identity** binds flash to verification.
- **Three verification layers:** test suite · source · **deliverable object code**.
- **Toolchain humility** (compilers and agents as untrusted components).
- **Design for testability** and layered testing (structure + host/DUT + fuzz when useful).
- **Hazards** that stay permanent design rules.
- Rules for **agents**, harvest, and independent check.

### Out of scope
- Full DO-178C PSAC/SDP/SVP packages or DAL assignment for certification.
- Project-wide structural coverage (MC/DC) or mutation testing as a bar.
- Replacing [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) or [TODO.md](TODO.md) dual DoD tables.
- Declaring GreenJade “DO-178C certified,” “safety certified,” or “as reliable as SQLite.”

### Normative product law (do not re-litigate)
See [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md).

| Rule | Meaning |
|------|---------|
| **G-AC-1** | No Linux `.ko` binary runs **in the kernel as product**. |
| **Product drivers** | Linux-shaped **userspace** over hot+cold ABI / DDI·UDX / caps. |
| **Soft residual** | `RUN_INIT=0` / `freestanding_no_exec` = eng only. Soft≠product. |
| **Freestanding class** | Default **SKIP**: `GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`. Not product. |
| **T0 product net** | **virtio-net** on QEMU. Laptop wire is **rtl8168_udx**. |
| **Product drivers (hosts)** | Userspace **UDX/DDI** (`rtl8168_udx` / `xhci_udx` …) over hot+cold ABI. |
| **Lab dual DoD B** | UDX NIC + stack + sshd. L3 arping/ping/banner are **proven-class**. Close is host **interactive SSH login**. |
| **Bar3** | [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) **only**. |
| **L1 vs Dual DoD** | `make assurance-check` is **L1 process** only — **never** Dual DoD A/B close. |

---

## 2. Claim classes (criticality)

Map work to a class **before** coding or spinning agents. Higher class ⇒ stronger evidence and more independence.

| Class | Name | Examples | May claim PASS when | Forbidden as PASS |
|-------|------|----------|---------------------|-------------------|
| **C0** | Soft / eng residual | Soft ksym, soft inventory, wave deepen, INIT=0 lamps | Greppable soft lamp only | Dual DoD A/B · bar3 · G-AC-1 product · “net works” |
| **C1** | Lab dual DoD | UDX NIC wire, interactive SSH login, UDX USB | Dual DoD **B** close = host **interactive SSH login** on a **named image stamp** | Soft inventory alone · STATUS “:22” · host `nc`/banner/PK_OK/SUCCESS as Dual DoD **B** close |
| **C2** | Product path | virtio T0, UDX/DDI hosts, doors, Linux ABI hot/cold | Spec-linked test or DoD; Soft≠product honesty | Soft scaffold as product AC |
| **C3** | Security / isolation | Caps, revoke, user_copy, trap policy, SMEP maps, process AS death order | Review + regression of hazard | Soft lamp without behavior check |

**Default for agent residual:** **C0** unless the prompt names C1–C3 and a DoD/hazard.

---

## 3. Plans lite (one page each)

DO-178C-style plan *roles*, not certification binders.

### 3.1 What we claim (PSAC-lite)

| Claim surface | Canonical home | Close criterion |
|---------------|----------------|-----------------|
| Dual DoD **A** (USB path) | [TODO.md](TODO.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) | DUT proof per TODO (not soft-only) |
| Dual DoD **B** (interactive SSH login) | [TODO.md](TODO.md) | Host arping + ping **10.200.125.50** **proven** 2026-08-14; banner/`nc`/PK_OK proven. Close is host **interactive SSH login** on stamped image |
| Hybrid SOFT gate0 | [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) | Eng honesty only; Soft≠product |
| Bar3 / Top 50 | [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) | That file only |
| Product net | ABI pivot · virtio T0 · later UDX | Not freestanding rtl forever |

### 3.2 How we build (SDP-lite)

| Item | Rule |
|------|------|
| Language | Pure freestanding **C11**; dual **MIT OR Apache-2.0** |
| Forbidden | GPL source in tree; OOP as project policy |
| Product priorities | **Security → Performance → Portability → Readability** (do not reorder for games) |
| Architecture | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) freeze — implement, do not re-litigate |
| Toolchain | Known freestanding flags; `-Werror`; kernel avoids casual UB-prone patterns; opt flags documented in Makefile |
| Image stamp | `GJ_IMAGE_VERSION` in `kernel/include/gj/config.h`; panel `STATUS (static) v…` |
| Report tool | `./scripts/gj-image-version.sh --report` |
| Deliverable | What is **flashed** (`greenjade.elf` in img) is the V&V subject for C1 — not “sources on disk after residual harvest” |

### 3.3 How we verify (SVP-lite)

| Layer | Method | Independence |
|-------|--------|--------------|
| Compile | `make build/greenjade.elf` (and unit objects) | Syntax / Werror only — **not** correctness |
| Soft residual | Greppable serial lamps | **Not** DoD close |
| Lab flash | `make hwtest-img` · `install-hwtest-usb` | Config baseline |
| Lab panel | STATUS title + NET/R counters | Supporting only — **not** host reachability |
| Lab DoD **B** | Host **interactive SSH login** against **stamped guest**. arping/ping/`nc` are proven-class | **Required independent** to close |
| FAULT | No `STATUS FAULT PINNED`; run loop continues | DUT photo / serial |
| Security (C3) | Hazard regression + code review | Prefer second pair of eyes |
| Optional depth | Second compiler/opt when cheap; fuzz later | Strengthens object-code trust |

**Hipp-aligned rule:** *If it has not been tested (on the artifact that matters), it does not work* for that claim class.

### 3.4 Configuration identity (SCMP-lite)

| Artifact | Role |
|----------|------|
| `GJ_IMAGE_VERSION` | Compile-time stamp baked into ELF |
| STATUS row 0 | Operator-visible identity after boot |
| `build/greenjade.elf` | Deliverable object code under test after pack |
| `build/greenjade-hwtest.img` | Flash media; may lag tree residual |
| Host `gj-image-version.sh` | Pre-flash identity check |

**Rules:**
1. Dual DoD evidence **must name the stamp** (e.g. current fly flash bar `v0.1.184`; **v0.1.178** historical SUCCESS proven, not live bar).
2. Tree residual after pack is **not** on DUT until re-image.
3. Do not close DoD against “latest workspace” without a flash bar.
4. **Test what you fly:** host probes exercise the **flashed** image, not an unstamped ELF left in `build/`.
5. **Flash bar honesty:** `GJ_IMAGE_VERSION` / STATUS row 0 identity only — Soft≠product; **not** Dual DoD A/B close; **not** UDX product AC.

### 3.5 Process health (SQA-lite)

| Check | Rule |
|-------|------|
| Soft≠product | Soft PASS never rewrites product AC or bar3 |
| Stamp storms | No multi-kprintf ret\*angle floods (FAULT class) |
| Agent harvest | Exclusive files only; stamp-free; compile after merge |
| Independent DoD | Host **interactive SSH login** closes **B**, not STATUS / `nc` / banner |
| Toolchain humility | Compile PASS ≠ DoD PASS; see §4 |

---

## 4. Three layers of verification · toolchain · object code

Adapted from DO-178B/C spirit as used by SQLite (Hipp, SSW 2026): do not stop at “source looks right.”

### 4.1 The three layers

| # | Layer | Question | GreenJade examples |
|---|-------|----------|--------------------|
| **L1** | **Tests / procedure** | Do our checks measure what we claim? | Host scripts; STATUS photo discipline; V1–V8 table; greppable lamps that match real counters |
| **L2** | **Source** | Does the C express the intended behavior? | Spec freeze; code review; `-Werror`; Soft≠product comments; no UB-by-accident freestanding patterns |
| **L3** | **Deliverable object code** | Does the **binary we ship/flash** behave correctly? | Stamped `greenjade.elf` on DUT; host arping/ping/`nc` (proven-class); **interactive SSH login** to close B |

**Close Dual DoD B only with L3 evidence of host interactive SSH login** (plus L1 that the probes are the right ones). Banner / PK_OK / SUCCESS ≠ close. L2 alone is never enough.

### 4.2 “We do not trust compilers” (practical, not conspiracy)

**Meaning for GreenJade (Hipp column):** honest toolchains can still miscompile, over-optimize, or differ by version. Source-correct ≠ binary-correct.

| Practice | Detail |
|----------|--------|
| **Prove the artifact** | C1 Dual DoD B close uses **flashed** stamp + host **interactive SSH login** |
| **Known flags** | Freestanding kernel flags fixed in Makefile; do not silently change opt for “DoD cuts” without re-verify |
| **Optional diversity** | When feasible on C1/C3 cuts: second GCC version, or `-O0` smoke vs `-O2` for a suspected codegen bug |
| **Workarounds** | If a compiler bug is hit: greppable comment + residual note; do not paper over with Soft PASS |
| **Not primary focus** | Full Ken Thompson *Reflections on Trusting Trust* (malicious self-reproducing compiler) is supply-chain / bootstrap depth — out of scope for dual DoD unless product policy expands |

**Meaning we already use elsewhere:** STATUS “:22” and soft inventory are **not** compilers, but they are also **not L3**. Same humility.

### 4.3 Trust stack (who may lie)

| Component | Can lie by… | Mitigate by… |
|-----------|-------------|--------------|
| Author / agent | Wrong code, Soft≠product claim creep | Class C0–C3; harvest rules; review |
| Compiler / linker | Codegen bug, flag surprise | L3 host/DUT; optional multi-toolchain |
| Soft residual lamps | Counting eng paths as product | Soft≠product · G-AC-1 |
| STATUS panel | Stale hold; :22 claimed without RX | Host probes; R climb under flood |
| Host tools | Wrong iface, permanent ARP neigh | Lab checklist; flush neigh if needed |
| Flashed image | Tree moved on after pack | Stamp identity V1 |

### 4.4 MC/DC and deep structural coverage (optional, narrow)

SQLite pursued **100% MC/DC** (machine-code branch both ways; bitmasks that matter) after DO-178B. That investment is **not** GreenJade’s default bar.

| Policy | Rule |
|--------|------|
| **Default** | No project-wide MC/DC requirement |
| **Optional C3** | Tiny security/correctness cores may later adopt branch/MC/DC or sanitizer campaigns (user_copy, demux, trap policy) — document per-module |
| **Not a substitute** | Coverage % never closes Dual DoD B without host interactive SSH login |
| **Evolution** | Hipp: after MC/DC, **fuzzers and semantic/AI tests** still found bugs — keep room for adversarial testing on net/parse paths later |

### 4.5 Design for testability (architecture habit)

Prefer seams that make rare failures injectable without a room full of power-cycle robots (SQLite VFS / fault-sim spirit):

| Area | Prefer | Avoid |
|------|--------|-------|
| Net | Poll-owned path; residual RX repair lamps; hybrid gate honesty | IRQ-only eth that cannot be reasoned about offline |
| Process | Explicit death order; refuse user enter on dead AS | Silent use-after-free thr |
| Soft vs product | `RUN_INIT=0` lamps; G-AC-1 | Soft path that rewrites freestanding BAR without residual |
| Faults | Sticky FAULT pin + wild-RIP class lamps | Silent halt with no STATUS |

**Rule:** product/lab code may include **test/diagnostic hooks** that are no-ops or rate-limited in ship builds (greppable, Soft≠product where eng-only). Hooks must not become the only place “correctness” lives.

### 4.6 Comments, asserts, and invariants

Hipp: comments and asserts are part of reliability culture (executable comments when tests pass).

| Practice | GreenJade |
|----------|-----------|
| Comments | Prefer intent + Soft≠product / hazard notes over stamp catalogs |
| Runtime asserts | Kernel: prefer explicit fail paths + kprintf lamps over silent UB; freestanding has no libc assert culture by default — **invariants via policy + FAULT/halt honesty** |
| Soft inventory | Capped; never substitutes for L3 |

---

## 5. Traceability (requirements → proof)

Keep rows short. Expand only for C1–C3.

### 5.1 Dual DoD **B** (lab UDX NIC + sshd) — template

| ID | Layer | Statement |
|----|-------|-----------|
| B-HLR-1 | High-level | Lab host can reach UDX guest at **10.200.125.50** |
| B-HLR-2 | High-level | Lab host can complete **interactive SSH login** on that guest |
| B-LLR-1 | Low-level | `rtl8168_udx` RX delivers frames to kernel `net_tcp` |
| B-LLR-2 | Low-level | ARP request for guest IP gets UDX reply |
| B-LLR-3 | Low-level | ICMP echo request gets UDX reply |
| B-LLR-4 | Low-level | TCP demux accepts dest lab IP; `sshd.elf` listen :22 |
| B-CODE | Code (examples) | `user/drivers/rtl8168_udx/` · `kernel/net/net_tcp.c` · `user/sshd/src/sshd_gj.c` |
| B-V&V-1 | Verify (L3) | STATUS: no FAULT PINNED; hold7 lab IP; Dual DoD **B OPEN** on **stamped** image |
| B-V&V-2 | Verify (L3) | Host: `arping -I <if> 10.200.125.50` receives replies |
| B-V&V-3 | Verify (L3) | Host: `ping -c 3 10.200.125.50` 0% loss (or documented partial) |
| B-V&V-4 | Verify (L3) | Host: `nc -v -w 3 10.200.125.50 22` connects / banner (**proven-class**, not B close) |
| B-V&V-5 | Verify (L3) | Host: **interactive SSH login** on stamped media |

**Close rule:** B-HLR-2 only after B-V&V-5 **L3** on a **named** `GJ_IMAGE_VERSION`. Banner/`nc`/ARP/ping are proven-class. Soft lamps (L2 eng) alone **OPEN**.

### 5.2 Dual DoD **A** (USB path) — pointer

Canonical criteria live in [TODO.md](TODO.md) and [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md). Class **C1** (lab) / **C2** (userspace UDX product). Soft `usb_storage` seed is **C0** unless DUT proof is claimed.

### 5.3 Generic row pattern

```text
HLR  → user/lab/product outcome
LLR  → mechanism (who owns wire, who polls, what must not run on IRQ)
CODE → path(s)  [L2 source]
V&V  → independent evidence + stamp on deliverable binary  [L3]
```

---

## 6. Verification objectives (lab flash cut)

Use on every flash intended for Dual DoD work. Fill stamp after pack.  
These are **L3 (object code / DUT)** objectives unless noted.

| # | Objective | Pass criterion | Evidence |
|---|-----------|----------------|----------|
| V1 | Image identity | Panel `STATUS (static) vSTAMP` matches pack report | Photo · serial `main: image version=` · `gj-image-version.sh` |
| V2 | No kernel halt FAULT | No sticky `STATUS FAULT PINNED` / KERNEL FAULT HALTED from expected smokes | STATUS photo · serial |
| V3 | Run loop alive | Soft/run-loop activity after boot (e.g. sched run loop / soft PASS) | Serial · STATUS |
| V4 | Net identity | Panel hold7 shows lab **10.200.125.50** when UDX NIC owns wire | STATUS IP line |
| V5 | RX live under host traffic | UDX inj/tx/lnk and host arping/ping succeed | STATUS during host flood |
| V6 | L2/L3 reachability | Host arping and/or ping succeed | Host command output |
| V7 | :22 reachability | Host `nc`/`ssh` to :22 sees banner / KEX (**proven-class**) | Host command output |
| V10 | Interactive SSH login | Host **interactive SSH login** on stamped media | Host session (not banner, not `nc` Connected) |
| V8 | Soft honesty | Soft r8169 INIT=0 / hybrid WIRE=FS does not claim product `.ko` wire | STATUS · serial Soft≠product |
| V9 | Deliverable match | Host probes hit the **same stamp** as V1 (not a newer tree ELF) | Stamp string · media build time |

**Current dual DoD B:** V1–V7 and V9 are **proven-class** (reachability). **Close** needs V10 **interactive SSH login**. V8 always on.  
**Until closed:** leave TODO Dual DoD **B OPEN**.

---

## 7. Hazard register (living)

Hazards become **permanent design rules**. Add rows when a lab FAULT or DoD regression teaches a new class.

| ID | Hazard | Class | Forbidden pattern | Permanent rule | Greppable / residual |
|----|--------|-------|-------------------|----------------|----------------------|
| H1 | IRQ stack smash / #PF I=1 | C3/C1 | `net_eth_poll` from timer/APIC/IRQ | Eth poll only from `scheduler_run` thr stack | `net_eth_poll=run_loop_only` · `net_eth_irq=0` |
| H2 | String-as-code / stamp storm | C3 | Hundreds of sequential soft-deepen `kprintf`s | No ret\*angle stamp storms; inventory capped | `trap: string-as-code` · soft rate caps |
| H3 | Clone sibling into dead AS | C3 | `USER32_ENTRY` thr after `process_death` AS destroy | `thread_exit_process` before as_destroy; refuse enter if !alive/cr3==0 | `process: death thr_exit` · `sched: thr skip user` |
| H4 | Soft BAR thrash → R0 | C1 | Soft CF8/iomap reprograms freestanding rings | Hybrid SOFT gate0; freestanding owns wire until UDX | [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) · `rtl8168: soft rx` |
| H5 | Soft claim = product | C0→wrong | Soft PASS closes DoD/bar3 | Soft≠product · G-AC-1 | Soft lamps with honesty tags |
| H6 | STATUS :22 without host | C1 | Close DoD B from panel / `nc` / banner | Host **interactive SSH login** (L3) | Dual DoD B table |
| H7 | Tree ≠ media | C1 | Verify DoD on unflashed residual | Stamp + re-image for DoD cuts; **test what you fly** | `GJ_IMAGE_VERSION` · V9 |
| H8 | Source-only trust | C1/C3 | “Compiled clean / looks right” as DoD close | L3 host **interactive SSH login** on stamped binary | §4 three layers |
| H9 | Toolchain surprise | C1/C3 | Silent flag/opt change on a DoD cut without re-flash verify | Document flags; re-run V1–V7 after toolchain change | Makefile · pack notes |

---

## 8. Independence matrix

| Work product | Implementer | Independent check |
|--------------|-------------|-------------------|
| C0 soft residual | Agent / author exclusive file | Parent harvest · compile · Soft≠product skim |
| C1 Dual DoD B code | Author / agents on rtl·l2·eth·tcp | Host **interactive SSH login** on **stamped flash** (L3). arping/ping/`nc` are proven-class |
| C1 FAULT fix | Author | Next DUT STATUS without FAULT PINNED |
| C2 product path | Author | Spec link + non-soft test |
| C3 security | Author | Second review preferred · hazard regression |
| Image pack | Build scripts | `gj-image-version.sh --report` before flash |
| Compiler / linker | Host toolchain | L3 behavior; optional second toolchain on suspicion |

**Agents are untrusted development tools**, not independent verifiers (DO-330 spirit without formal tool qualification).  
**Compilers are untrusted code generators** for claim close (Hipp / DO-178B spirit without project-wide MC/DC).

---

## 9. Agent and harvest rules

| Rule | Detail |
|------|--------|
| Exclusive ownership | One agent · one file (or tight exclusive set); no overlapping edits |
| Stamp-free residual | Do **not** bump `GJ_IMAGE_VERSION` in residual agents |
| No stamp storms | No multi-line kprintf floods; lean greppable lamps only |
| Class in prompt | State C0/C1/C2/C3 and Soft≠product |
| Harvest | Copy exclusive paths only; compile; fix nested-comment / unused-symbol breaks |
| Product claims | Residual agents must not rewrite G-AC-1 or bar3 |
| Agents ≠ L3 | Agent “PASS” or soft inventory never closes Dual DoD; host interactive SSH login does |
| After harvest | New residual is **tree-only** until re-pack + re-flash for C1 claims |

---

## 10. Problem report (C1/C3 only)

Use for FAULT / Dual DoD regression / security / suspected codegen. Skip for routine C0 residual.

| Field | Content |
|-------|---------|
| **Stamp** | Image or tree id under test |
| **Symptom** | Observable (panel / serial / host) |
| **Class** | C1 / C3 |
| **Root cause** | Mechanism (not “soft wave failed”) |
| **Fix** | Paths + invariant |
| **Verify** | Independent **L3** evidence where applicable |
| **Hazard** | New or existing H# |

**Lab exemplars (historical):**
- Stamp storm → #PF I=1 string-as-code → strip storms + rate caps (H2).
- pe32 `clone_vm` child RIP `0x58240013` after parent death → thr_exit before as_destroy (H3).
- Freestanding **R0** with UP/TX → RX orphan / soft thrash residual (H4).
- STATUS :22 claimed while host `nc` times out → do not close B (H6). Banner/`nc` Connected still does not close B.

---

## 11. Evidence checklist (operator)

Before claiming Dual DoD **B** progress on a cut:

- [ ] `GJ_IMAGE_VERSION` / pack report recorded  
- [ ] Flashed media matches that stamp (STATUS row 0) — **test what you fly**  
- [ ] V1–V3 (identity, no FAULT, run loop)  
- [ ] V4–V5 (lab IP, R climbs under host traffic)  
- [ ] V6–V7 (host arping/ping/`nc` banner) recorded against **that** media — proven-class, not B close  
- [ ] V10 (host **interactive SSH login**) before claiming Dual DoD **B** close  
- [ ] V9 (probes not aimed at a newer unstamped tree build)  
- [ ] Soft≠product / G-AC-1 not violated in claim language  
- [ ] TODO Dual DoD **B** updated only if close criteria met  

---

## 12. Relationship to DO-178C and SQLite reliability lessons

### 12.1 DO-178C ideas we use

| DO-178C idea | GreenJade use |
|--------------|---------------|
| DAL / rigor by criticality | Claim classes C0–C3 |
| Objectives + evidence | §6 verification table · Dual DoD close rules |
| Traceability | §5 HLR/LLR/CODE/V&V |
| Independence | §8 host probes · harvest compile |
| Configuration baseline | `GJ_IMAGE_VERSION` · flash bar |
| Tool caution | Agents ≠ independent V&V; compilers ≠ free pass |
| Full certification package | **Not adopted** |
| Project-wide MC/DC | **Not adopted** (optional narrow C3 only — §4.4) |

### 12.2 Hipp / SQLite lessons we use

| SQLite / Hipp lesson | GreenJade use |
|----------------------|---------------|
| If it has not been tested, it does not work | C1 Dual DoD B close needs host interactive SSH login, not soft lamps |
| Do not trust compilers alone | Prove **flashed** binary; optional multi-toolchain |
| Test what you fly | V1 + V9 · media stamp |
| Design for testability | §4.5 seams · residual inject · hybrid honesty |
| MC/DC is powerful but not the whole story | No project-wide bar; later fuzz/semantic optional |
| Test investment enables bold change | Stronger DoD/FAULT regression → safer net residual |
| Small team, few deps, solve more problems than you create | Aligns with freestanding dual-license posture |

### 12.3 Honesty

This document does **not** make GreenJade airborne software, DO-178C-compliant, or SQLite-equivalent in reliability investment.

---

## 13. Maintenance

| When | Action |
|------|--------|
| New Dual DoD close | Update TODO; keep this file’s **process** stable |
| New FAULT class | Add hazard row H# |
| Toolchain / opt change on DoD cut | Re-pack, re-flash, re-run V1–V7/V9 |
| Product path change | Point at ABI pivot; do not restate bar3 here |
| Flash cut for DoD | Fill §6 stamp; run operator checklist §11 |
| Before claiming process health | `make assurance-check` (L1 only — not Dual DoD close) |
| Freestanding SKIP / UDX path change | Re-run L1; keep Soft≠product; do not rewrite G-AC-1 |
| Soft residual harvest | Confirm freestanding_no_exec + SKIP defaults still greppable |

**Canonical live DoD status:** [TODO.md](TODO.md) Current track.  
**Canonical product driver law:** [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md).  
**Canonical bar3:** [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md).  
**Tree L1 check:** `make assurance-check` · `scripts/gj-assurance-check.sh` (H1/H3 · freestanding SKIP deepen · freestanding_no_exec · UDX deepen · Soft≠product · flash bar stamp shape).  
**Image identity:** `make image-version` · `scripts/gj-image-version.sh --report` (stamp ≠ product PASS; current fly flash bar e.g. `v0.1.184`; **v0.1.178** historical SUCCESS, not live bar).

---

*Dual MIT OR Apache-2.0. Soft ≠ product. G-AC-1. Freestanding class SKIP. Product = userspace UDX+ABI. Assurance lite — process honesty, not certification. L1 ≠ Dual DoD close. Flash bar honesty. Test what you fly.*
