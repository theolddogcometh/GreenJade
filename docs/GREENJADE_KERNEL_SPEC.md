# Project GreenJade — Kernel Architecture Spec Sheet

| Field | Value |
|-------|--------|
| **Codename** | **GreenJade** (Project GreenJade) |
| **Document** | Architecture Specification v0.13 |
| **Design complete** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) **v1.0 Accepted** — remaining freezes |
| **Security core** | [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) · [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) |
| **Priorities** | **1. Security → 2. Performance → 3. Portability → 4. Readability** (**unchanged** by games) |
| **Doc precedence** | Law → Security → Caps → **Proton** → Apple → Solaris → sketches (Proton-over-Apple ≠ reorder priorities) |
| **x86_64 platform** | [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) (UEFI, VT-d, NUMA, …) |
| **Status** | Draft design baseline — **implementation started (M0)** |
| **Coding docs** | [IMPLEMENTATION.md](IMPLEMENTATION.md) · [TODO.md](TODO.md) |
| **Priority order** | **1. Security → 2. Performance → 3. Portability → 4. Readability** |
| **Product scope** | **General-purpose desktop**; adoption games = **Deck Top 50** (0.5.2) |
| **Honesty (Wave 87)** | Soft ship / continuum **toward 22900** ≠ product DoD / bar3; product lamps **0**; **no product claim** (23) |

| **Firmware** | **UEFI** (primary); legacy BIOS only as best-effort if ever |
| **Hardware bar** | **>1 TiB RAM**, **multi-CPU/SMP**, **SAS/SCSI** storage path |
| **Heritage** | Microkernel structure inspired by Pink/Taligent *Opus* (*structure only*, not OO culture) |
| **Security posture** | OpenBSD-grade defaults |
| **Language** | **Pure C only** (C11/C17) |
| **License** | **MIT OR Apache-2.0** (dual, at recipient’s option); no GPL family |
| **IP strategy** | BSD-style clean-room / rewrite-out of restricted trees |
| **Driver strategy** | Userspace drivers; Linux-*shaped* APIs written from scratch; no Linux source |
| **Date** | 2026-07-19 |

---

## 0. Non-Negotiable Project Law

These rules override every other section. If a design conflicts with this page, **this page wins**.

### 0.0 Priority order (tradeoff law)

When goals conflict, decide in this order only:

| Rank | Priority | Meaning |
|------|----------|---------|
| **1** | **Security** | Isolation, least privilege, fail-closed, small TCB, no ambient authority, auditability of trust boundaries. **Never sacrifice for speed or convenience.** |
| **2** | **Performance** | Fast IPC, tight hot paths, efficient drivers — *after* security holds. Measure before micro-optimizing. |
| **3** | **Portability** | Arch abstraction, clean interfaces, multi-arch design — without weakening security or inventing unsafe “fast ports.” |
| **4** | **Readability** | Clear C, Hungarian variables, consistent style — important for review, but last when it fights 1–3. |

**How to apply:**

| Conflict | Winner |
|----------|--------|
| Faster path vs capability check | **Security** — keep the check; optimize *around* it |
| Arch-specific hack vs clean portable layer | **Performance** may use `#ifdef` / arch files if measured; **Security** still bounds the hack |
| Clever dense code vs obvious code | Prefer **readability** unless **performance** is proven hot *and* **security** is unchanged |
| Portable slow path vs arch fast path | Keep portable fallback (**portability**); optional fast path if **security**-equivalent |
| “Just this once” skip bounds check | **Never** — violates rank 1 |

Security first does **not** mean ignore speed: it means the secure design is fixed first, then make *that* design fast, then portable, then easy to read.

**These four ranks do not change** for gaming, desktop popularity, or Proton. See 0.5.1 (product adoption vs priority law).

### 0.1 Language: pure C only

| Allowed | Forbidden |
|---------|-----------|
| ISO C11 / C17 | C++ |
| Assembler **only** where the arch requires it (entry, traps, atomics helpers) | Rust, Go, Zig, Nim, Swift, … |
| Plain structs, functions, function pointers | Classes, templates, exceptions, RAII as language features |
| Simple code generators that **emit C** (optional) | Any non-C source in the shipped tree |

- Kernel, servers, drivers, libc, tools that ship **in-tree**: **C**.
- Build system may use shell/`make`/`cmake`; project logic remains C.
- No “just this one crate” exceptions. No C++ “driver shims.” No Rust servers.

### 0.2 License: MIT / Apache-2.0 only

| Allowed origin of code | Forbidden origin of code |
|------------------------|---------------------------|
| Written from scratch by this project | **GPL, LGPL, AGPL** (any version) |
| Third-party code under **MIT** | CDDL, SSCL, proprietary blobs *as source* (firmware binary policy separate) |
| Third-party code under **Apache-2.0** | Copying from Linux, GNU userland, BusyBox, GPL bootloaders, etc. |
| Dual-licensed MIT/Apache when both grant use | “GPL with exception” Linux kernel files |

**Project license for original GreenJade code:** **MIT OR Apache-2.0** dual license  
(`SPDX-License-Identifier: MIT OR Apache-2.0`). Recipients may use either.

**Copying rule:** if it was not written here, it may enter the tree **only** if its license is MIT or Apache-2.0 (or public domain / CC0 treated as equivalent for this policy). Re-license is **not** magic: do not re-license GPL as MIT.

File headers:

```c
/* SPDX-License-Identifier: MIT OR Apache-2.0 */
```

### 0.3 BSD-style “copy out” of restricted ecosystems

Historical parallel: the original BSD project **rewrote** its way out from under AT&T UNIX licensing. **GreenJade** does the same relative to **GPL-dominated** stacks.

| Practice | Meaning |
|----------|---------|
| **Clean-room** | Read hardware manuals, RFCs, public specs; implement in original C |
| **Idea, not code** | Architecture ideas from Linux/BSD/OpenBSD are fine; **source text is not** |
| **No GPL dependency** | Build must not require linking or vendoring GPL components |
| **Prefer rewrite over port** | A worse first driver we own beats a better GPL driver we cannot ship |
| **Provenance log** | Every imported file records license + upstream URL + hash |

**OpenBSD influence is about security process and simplicity**, not about pasting OpenBSD ISC/BSD sources unless those files are also available under MIT/Apache (most are **not** automatically MIT). Default: **reimplement**, do not import.

### 0.4 OOP is not a goal

| Not a goal | What we do instead |
|------------|--------------------|
| Object-oriented kernel or frameworks | Procedural C, clear modules, plain structs |
| C++ vtables / “everything is an object” | Function tables only when a driver ops vector is required |
| Taligent-style framework web | Small, boring servers with explicit C APIs |
| Inheritance hierarchies | Composition via handles and capabilities |

**Heritage kept** (from classic Opus ideas): tiny privileged core, services outside the kernel.  
**Heritage dropped**: OO religion, C++ wrappers, framework maximalism.

### 0.5 Product scope (required)

| Requirement | Spec |
|-------------|------|
| **Product** | **General-purpose desktop / workstation OS** (interactive GUI, local apps, media, development, **game runtime path**) |
| **Firmware** | **UEFI** boot as **primary** path (x86_64 first; aarch64 UEFI later). Legacy BIOS is not a target. |
| **RAM** | Correct operation with **≥ 1 TiB** physical RAM (design for multi-TiB; test goal 1 TiB+) |
| **CPUs** | **SMP required** — multi-socket / multi-core production target, not an afterthought |
| **Storage** | **SAS and SCSI** (plus NVMe/SATA/virtio) via clean-room userspace stacks |
| **License** | **MIT OR Apache-2.0** dual |
| **Games (adoption)** | **Major game support** = **Steam Deck Top 50** (see 0.5.1–0.5.2) via Proton personality |

Desktop means userspace (compositor, input, GPU policy, audio, **game runtime**) is **in product scope**; it still lives **outside** the microkernel. Kernel stays minimal.

#### 0.5.1 Popular desktop take-up vs priority law (normative)

| Statement | Status |
|-----------|--------|
| Without **credible major game support**, a general-purpose desktop will not see **popular** take-up | **Accepted product fact** |
| **Major game support** means the **Steam Deck Top 50** catalog bar — 0.5.2 | **Accepted definition** |
| Therefore GreenJade **must plan and ship** a Proton-class path ([PROTON_PERSONALITY.md](PROTON_PERSONALITY.md)) aimed at that bar, as part of the **desktop product**, not as a hobby side quest | **Accepted** |
| **Priority order 0.0 is unchanged:** Security → Performance → Portability → Readability | **Immutable** |
| Games do **not** become priority #1, do **not** outrank security, do **not** justify **importing GPL/copyleft code** or ambient root | **Immutable** |
| Doc conflict **Proton over Apple** is about *which product surface to implement among secure designs* — **not** about reordering 0.0 | **Clarification** |

**How to decide:**

```text
Need a game feature for Deck Top 50?
  → Does it violate Security (caps, fail-closed, no ambient, TCB)?  → redesign or refuse (title may remain unsupported)
  → Else implement under Performance / Portability / Readability as usual
  → Prefer Proton-shaped APIs over Apple conveniences when both are secure
```

**Not promised:** full Steam catalog, SteamOS distro port, **GPL code in tree**, or titles that **require** loading Linux `.ko` anti-cheat with no clean-room path.  
**Allowed:** **clean-room** Linux-compatible ABI / Proton surface (no copyleft source) — 11.  
**Promised as product intent:** drive the stack so **Deck Top 50** is the adoption target under GreenJade law (0.5.2).

#### 0.5.2 Deck Top 50 — major game support definition (normative)

| Field | Value |
|-------|--------|
| **Name** | **Deck Top 50** adoption bar |
| **Meaning** | The fifty titles on Valve’s **Steam Deck** “most played” / Top charts used as the **reference popularity set** for GreenJade desktop games |
| **Source** | Public Steam Deck top-played list (or Valve’s published Deck top chart); **snapshot dated** in release notes / test matrix when certifying a GreenJade version |
| **Pass rule (product)** | A GreenJade release claims “major game support” only if it meets the **pass criteria** below for that snapshot |
| **Runtime** | Out-of-tree Proton-class runtime + in-tree [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) — **not** SteamOS |

**Pass criteria (per title in the snapshot Top 50):**

| Result | When |
|--------|------|
| **PASS** | Boots to gameplay on GreenJade HCL (at least T0 virtio or documented T1+ GPU path); input + audio + present work; save/load works; **playable** (not a menu-only smoke) at a documented quality tier |
| **PASS-OFFLINE** | Same as PASS but multiplayer/online disabled or broken **only** due to anti-cheat / network policy documented for that title |
| **BLOCKED-SECURITY** | Requires in-kernel Linux module, ambient root, or other violation of 0.0 / project law — **not** a GreenJade defect to “fix” by weakening security; listed as unsupported |
| **FAIL** | In scope of personality + ICD + session but broken; counts against the bar until fixed |
| **NOT-TRIED** | Not yet on the matrix; counts against claiming the bar |

**Claiming the bar (release DoD):**

| Claim level | Requirement |
|-------------|-------------|
| **Targeting Deck Top 50** | Public matrix exists; engineering aimed at the snapshot |
| **Deck Top 50 — majority** | **≥ 25 / 50** PASS or PASS-OFFLINE on the dated snapshot |
| **Deck Top 50 — met** | **≥ 40 / 50** PASS or PASS-OFFLINE; all remaining are BLOCKED-SECURITY or documented FAIL with owners |
| **Deck Top 50 — full** | **50 / 50** PASS or PASS-OFFLINE (BLOCKED-SECURITY titles force either userspace anti-cheat path or honest “49/50” claim) |

**Snapshot rules:**

1. Freeze a **dated** Top 50 list per certification (lists churn).  
2. Re-cert on major GreenJade releases; do not silently reuse an old snapshot.  
3. HCL for certification must be stated (e.g. QEMU virtio-gpu vs named GPU).  
4. Priorities 0.0 still win: a title that needs illegal design stays **BLOCKED-SECURITY**, not a reason to import GPL or break caps.  
5. Missing Linux-compatible **behavior** is fixed by **clean-room** growth of [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) — not by pasting GPL.

**Relationship to milestones:** A0→A4 ladder in Proton doc. **From M2 on**, work that unblocks **A0/A1** (futex, named objects, PROCESS, present path) is on the **desktop critical path** alongside other product bars — not deferred to “after SAS.”

#### 0.5.3 Desktop critical path (Deck Top 50 + law)

| Track | Content | License rule |
|-------|---------|--------------|
| **Game / Proton** | libprotonrt, wine-server shape, Vulkan present, session input/audio, Top 50 matrix | Clean-room Linux-compatible OK; no GPL source |
| **Platform** | UEFI, 1 TiB PMM, SMP, VT-d | Unchanged |
| **Storage** | virtio → NVMe → scsi_mid → SAS | Clean-room drivers |
| **Never** | Import GPL · load `.ko` · reorder 0.0 for catalog size | |

---

## 1. Mission

Build a **modern microkernel desktop OS** — **Project GreenJade** — that keeps a tiny privileged core and isolated services, under:

1. **Security first** — OpenBSD-class defaults; small TCB; fail closed; capabilities; drivers outside the kernel.
2. **Performance second** — fast IPC, SMP scaling, large-memory efficiency, storage/GPU paths.
3. **Portability third** — x86_64 + UEFI first; aarch64 UEFI; riscv64 later; same security model.
4. **Readability fourth** — pure C, Hungarian variables, reviewable modules.

Non-negotiable: **MIT OR Apache-2.0**, **no GPL**, **no OOP**, **clean-room** hardware support, **1 TiB+ RAM**, **SMP**, **SAS/SCSI**, **general-purpose desktop**, **priorities 0.0**.  
Product intent: **Deck Top 50** major game support for popular desktop take-up, via **clean-room Linux-compatible Proton personality** — **without** changing those priorities (0.5.1–0.5.3).

**One-line thesis:**  
*A secure pure-C capability microkernel that boots UEFI, scales past a tebibyte of RAM and many CPUs, talks SAS/SCSI, and aims **Deck Top 50** games through clean-room Linux-compatible userspace — no GPL copy, security still first.*

---

## 2. Design Goals & Non-Goals

### 2.1 Goals

| ID | Goal | Measure of success |
|----|------|--------------------|
| G1 | Minimal privileged kernel | Audited **C** core; no class drivers in kernel |
| G2 | Capability-based isolation | All resources named by unforgeable capabilities |
| G3 | Secure by default | No ambient authority; least privilege at process birth |
| G4 | Desktop product | Compositor, input, audio, local apps under 0.0 |
| G4b | **Deck Top 50** | Dated matrix; majority/met per 0.5.2; Proton critical path 0.5.3 |
| G5 | UEFI boot | Boot on modern UEFI firmware (and QEMU UEFI) |
| G6 | Large memory | **≥ 1 TiB** RAM: boot, map, allocate, free without 4 GiB-era limits |
| G7 | SMP | Multi-CPU scheduling, IPIs, per-CPU structures; scale beyond 1 core |
| G8 | Storage breadth | virtio-blk, NVMe, AHCI/SATA, **SCSI/SAS** path to block devices |
| G9 | Clean-room Linux shapes | ABIs **reimplemented** MIT/Apache — depth for Deck Top 50 as needed |
| G10 | Multi-arch | `x86_64` first; `aarch64`; later `riscv64` |
| G11 | License purity | Dual MIT/Apache; CI scan; zero GPL **source** in tree |

### 2.2 Non-Goals

| ID | Non-goal | Rationale |
|----|----------|-----------|
| N1 | **Copying** Linux / GPL kernel or userspace **source** | Copyleft; clean-room reimplementation is OK (0.2–0.3) |
| N2 | Drop-in load of Linux `.ko` binaries | Wrong kernel binary interface; modules are almost always GPL-bound |
| N3 | C++, Rust, or mixed-language TCB | Project law 0.1 |
| N4 | OOP / Taligent frameworks | Project law 0.4 |
| N5 | Full POSIX *in kernel* | POSIX is a userspace personality |
| N6 | Formal seL4-level proof in v1 | Optional later |
| N7 | Legacy BIOS as first-class | **UEFI primary**; BIOS not required |
| N8 | ZFS-in-tree if CDDL/GPL conflicts | Prefer dual-license-clean FS we write or can take |
| N9 | Shipping GPL firmware blobs as if they were our code | Blobs may be loaded under separate notices; C tree stays dual MIT/Apache |

---

## 3. Heritage Mapping (classic microkernel ideas → GreenJade)

| Classic idea (e.g. Opus-era) | GreenJade translation |
|-------------------|---------------------------|
| Tiny microkernel | Capability microkernel (simple IPC; avoid Mach bloat) |
| C++ kernel wrappers | **Deleted** — pure C APIs only |
| System frameworks outside kernel | Isolated **servers** with procedural C interfaces |
| Object-oriented services | **Deleted as goal** — structs + ops tables only as needed |
| Multi-personality ambition | Optional thin POSIX personality in C |
| Never shipped | Ship iteratively: QEMU → UEFI hardware → **desktop** |

---

## 4. Security Model (OpenBSD DNA)

Security process and defaults, **reimplemented in original C** under MIT/Apache. Do not import OpenBSD source unless it is MIT/Apache-cleared (assume **rewrite**).

### 4.1 Principles

1. **Least privilege by construction** — near-zero rights at process start; capabilities granted explicitly.
2. **Fail closed** — missing permission ⇒ hard error.
3. **W^X everywhere** — no writable+executable maps by default.
4. **No ambient root** — admin is a bundle of capabilities, not uid 0 magic.
5. **Small TCB** — kernel + critical bootstrap only.
6. **Default deny** net/FS — spirit of `pledge` / `unveil`, our own APIs.
7. **Privilege separation** — multi-process daemons; tiny helpers for privileged ops.
8. **Readable security** — declarative policy files + clear C, not ioctl soup.

### 4.2 Kernel Security Features (required)

| Feature | Spec |
|---------|------|
| Address space isolation | Per-task ASID/PCID; strict user/kernel split |
| SMEP/SMAP / PAN / PXN | Enforced on supported arches |
| KASLR | Randomized kernel map; no casual kernel pointer leaks |
| Stack protection | Canaries baseline; arch shadow stacks when available |
| CFI | Tier-2 goal where arch supports it |
| Crypto in kernel | **No** TLS stacks in kernel; minimal or none |
| Secure boot hooks | Optional measured/verified boot for appliance profile |
| Audit stream | Append-oriented events from kernel + critical servers |

### 4.3 Process Confinement (inspired by OpenBSD, original code)

| Mechanism | Behavior |
|-----------|----------|
| `confine(promises)` | Irreversibly drop classes of ops |
| `expose(path, rights)` | Path reveal; unmarked paths invisible |
| Capability mode | Only held caps/FDs work after enter |
| Profiles | Simple text/TOML policies for system servers |

Kernel substrate: capability rights masks and revocation — not string parsing of paths inside the microkernel.

### 4.4 Threat Model (v1)

| In scope | Out of scope (initially) |
|----------|---------------------------|
| Malicious apps | Hostile DMA without IOMMU |
| Compromised non-critical servers | Full formal proof of all servers |
| Driver bugs contained to driver host | Physical attack / evil maid |
| Supply chain of third-party **permissive** code | Shipping GPL “for convenience” |

**IOMMU required** on production profiles. No IOMMU ⇒ no bus-master by default.

---

## 5. Kernel Architecture

### 5.1 Ring 0 contents

**Only:**

| Subsystem | Responsibility |
|-----------|----------------|
| Boot / arch bring-up | CPU modes, early console, APs |
| Capability tables | Create, derive, revoke, transfer |
| Address spaces | Page tables, mapping rights, grants |
| Threads & scheduling | Priorities; optional RT budgets |
| IPC | Sync call/reply + async notify |
| IRQ routing | IRQs as messages to handler caps |
| Timers | Clock, timeouts |
| SMP | IPI, simple kernel synchronization |
| Debug / tracing | Capability-gated |

**Not in kernel:** filesystems, network stacks, GPU, USB class stacks, POSIX, auth, TLS, device drivers (optional tiny polled UART under `config` for bring-up only).

### 5.2 IPC Model (procedural C)

**Normative detail:** [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) 1–3, 6.

| Primitive | Description |
|-----------|-------------|
| Endpoint | Synchronous call/reply **with mono timeout** |
| Notification | Async event (IRQ, completion); badge bitmask |
| Shared memory grant | Capability-controlled maps for **SPSC rings** only (bulk) |
| Reply capability | Single-use reply right |
| Badge | Server-set; clients cannot forge |

No CORBA, no OO ORB. **No implicit mapping of caller memory into callee.**  
Bulk I/O uses **zero-copy SPSC rings** (6 security core), not large kernel copies.

**Order-of-magnitude targets (aspirational; security checks stay):**

| Path | Target |
|------|--------|
| Fast-path IPC RTT (same core) | ≤ 500–800 cycles *after* cap checks |
| IRQ → userspace wakeup | ≤ 2–5 µs typical (batched under load) |
| Bulk I/O | SPSC rings, zero-copy |

### 5.3 Capability kinds (core set)

```
cnode, thread, address_space,
endpoint, notification,
irq_handler, mmio_page, ioport (x86),
reply, sched_context, untyped (or memory factory),
quota_account
```

Rights: bitmasks (`READ`, `WRITE`, `GRANT`, `MINT`, `MAP`, `DESTROY`, …).  
**Bootstrap seal** after `init` setup — see security core 1.  
**All creates charge hierarchical quotas** — security core 2.

### 5.4 Scheduling (SMP-first)

| Class | Use |
|-------|-----|
| Timeshare | Default desktop apps |
| Soft realtime | Audio / media pipelines |
| Budgeted | Optional RT / latency-sensitive |
| Idle | Power |

**SMP is required for product**, not optional polish:

| Piece | Requirement |
|-------|-------------|
| Bring-up | BSP + AP start (x86: MADT/APIC; UEFI + ACPI tables) |
| Scheduler | Per-CPU runqueues or equivalent; load balance |
| IPI | Reschedule, TLB shootdown, panic stop |
| Sync | Documented locking model for caps, PMM, IPC (no “add SMP later rewrite”) |
| Affinity | Optional later; correctness first |

M0 may be UP for bring-up; **architecture and data structures must not assume `GJ_MAX_CPUS == 1` permanently**.

### 5.5 Large memory (≥ 1 TiB RAM)

| Constraint | Design rule |
|------------|-------------|
| Physical size | Support **> 1 TiB** usable RAM (multi-TiB capable structures) |
| No flat 4 GiB bitmap | M0 bitmap is **temporary**; replace with scalable PMM |
| PMM | Zone/region freelists or hierarchical bitmap; O(1)/amortized alloc; 4K + optional large pages |
| Virtual | x86_64 4-level minimum; **5-level paging** when CPU/firmware expose LA57 and needed |
| Direct map | HHDM or equivalent covering **all** RAM (not first 4 GiB only) |
| Frame caps | Must not require dense arrays sized by max PFN without sparse structure |
| Early boot | Identity map only until HHDM; then abandon phys==virt assumptions |

**M0 code that caps at 4 GiB is explicitly debt** and fails product DoD.

### 5.6 Boot (UEFI primary)

| Path | Role |
|------|------|
| **UEFI** | **Product path**: PE/COFF or EFI stub, ExitBootServices, memory map, GOP optional for early FB |
| Multiboot2 / QEMU `-kernel` | **Dev-only** until UEFI path works |
| Secure Boot | Optional appliance/desktop profile; design hooks, not required for first desktop bring-up |

Boot services hand kernel: memory map, ACPI RSDP, framebuffer (optional), initrd/modules.

---

## 6. System Layout (userspace, all C)

```
┌──────────────────────────────────────────────────────────┐
│  Apps / POSIX personality (C libc)                       │
├──────────────┬───────────────┬───────────────────────────┤
│  netstackd   │  vfsd / fsd   │  optional compositor      │
├──────────────┴───────────────┴───────────────────────────┤
│  devmgr      │  driver hosts (sandboxed C processes)     │
│              │    native drivers + UDX runtime           │
├──────────────────────────────────────────────────────────┤
│  init / supervisor (capability distributor)              │
├──────────────────────────────────────────────────────────┤
│  GreenJade microkernel (pure C)                         │
└──────────────────────────────────────────────────────────┘
```

| Server | Trust | Notes |
|--------|-------|-------|
| `init` | TCB-adjacent | Spawns services; hands out caps |
| `devmgr` | High | Enumerates devices; spawns hosts |
| `vfsd` | High | Namespace / mounts |
| `netstackd` | High | TCP/IP in process |
| `storaged` | High | Block aggregation |
| `driver-host-*` | Medium | One class or device per host |
| `cryptod` | High | Separated, heavily confined |
| `logd` / `auditd` | High | Logging / audit |

All of the above: **pure C**, MIT/Apache.

---

## 7. Drivers & “Linux portability” without GPL

### 7.1 What we will **not** do

| Forbidden approach | Why |
|--------------------|-----|
| Vendor Linux driver `.c` files | **GPL** |
| Link against Linux kernel headers as a product dependency | Pulls GPL culture and often GPL constraints |
| Load Linux `.ko` | Wrong kernel, wrong license |
| “LDX runs Linux driver sources” | That design is **rejected** under 0.2 |
| Copy FreeBSD/OpenBSD drivers unless MIT/Apache | Typical BSD licenses are fine *in spirit*, but **project law is MIT/Apache only** — rewrite unless upstream is MIT/Apache |

### 7.2 What “Linux driver portability” means here

**Portability of *skill and structure*, not of Linux source.**

Engineers who know Linux drivers should feel at home because **UDX** (Userspace Driver eXecution) provides a **small, original, MIT/Apache C API** that *resembles* common patterns (probe/remove, ops tables, DMA helpers, workqueues) **written from scratch**.

| Concept | GreenJade approach |
|---------|----------------|
| Familiar ops vector | `struct block_ops { int (*read)(...); ... };` — our headers |
| Register maps | From vendor manuals / open docs — clean-room C |
| PCI IDs | Public IDs; our match tables |
| DMA API | Our IOMMU-backed helpers |
| IRQ | Cap-delivered notifications |
| Firmware | Load via policy service; binary blobs not mixed into license of C source |

**Mental model:** “I can reimplement this NIC driver in a week because the *pattern* is familiar,” **not** “I pasted `r8169.c`.”

### 7.3 UDX (Userspace Driver eXecution) — original C runtime

**Porter contract:** [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) · code `user/udx/` · `make udx`

```
┌─────────────────────────────────────────────┐
│  Driver Host (confined C process)           │
│  ┌───────────────────────────────────────┐  │
│  │  UDX runtime (MIT/Apache, original)   │  │
│  │  Linux-shaped: probe/remove, irq,     │  │
│  │  dma_*, ioremap, work — caps hidden   │  │
│  └───────────────────────────────────────┘  │
│  ┌───────────────────────────────────────┐  │
│  │  Device driver (clean-room C)         │  │
│  └───────────────────────────────────────┘  │
└───────────────────┬─────────────────────────┘
                    │ capabilities only
                    ▼
              GreenJade kernel
```

| Rule | Detail |
|------|--------|
| Language | Pure C |
| License | MIT and/or Apache-2.0 only |
| Sleep | Host threads; no Linux scheduler code |
| MMIO | Only pages granted by `devmgr` |
| DMA | IOMMU windows only |
| Panic | Abort **host process**; system continues |

### 7.4 Isolation levels

| Level | Mechanism | When |
|-------|-----------|------|
| L0 | Threads in one host | Trusted first-party drivers |
| L1 | Separate process + `confine` | **Default** |
| L2 | Separate process + tighter sandbox / optional lightweight isolate | Complex devices, blobs |
| L3 | Hardware virtualization with assigned device | Highest risk (optional) |

### 7.5 Driver priority (write ourselves) — desktop + workstation

| Class | Priority | Notes |
|-------|----------|-------|
| virtio-* (blk/net/**gpu**/console/**input**) | **P0** | Dev + VMs + **Deck Top 50 T0 HCL** |
| UEFI GOP / simple FB | P0 | Early desktop path |
| 16550 / PL011 | P0 | Bring-up console |
| **libprotonrt / game present path** | **P0/P1** | **Adoption critical path** 0.5.3 — not “later hobby” |
| NVMe | P0 | Public spec; common desktop/workstation |
| **SCSI mid-layer (original)** | **P0/P1** | **Required product path** |
| **SAS HBA class** (clean-room when docs allow) | **P1** | Workstation; does not block A0 |
| AHCI / SATA | P1 | Laptops/desktops |
| xHCI + HID | P1 | Desktop + gamepads via session |
| PS/2 fallback | P1 | Still useful |
| GPU beyond virtio (clean-room) | P1 | After T0 game present works |
| Audio (HDA / virtio) | P1 | Game audio; virtio first OK |
| Ethernet (clean-room) | P1/P2 | Multiplayer Top 50 |
| Wi-Fi | P2 | Heavy confinement; firmware policy |
| Thunderbolt/USB4 | P3 | Later |

#### SCSI / SAS stack (product requirement)

```
App / FS
   → storaged (block)
      → scsi_mid (CDB, LUN, sense, queues)     [userspace server]
         → transport: sas | scsi_parallel | virtio-scsi | usb-storage
            → HBA driver host (MMIO/IRQ/DMA caps)
```

| Layer | Responsibility |
|-------|----------------|
| `scsi_mid` | Command model, timeouts, multipath-light later |
| SAS transport | Discovery, expanders as docs allow, SSP |
| HBA drivers | Clean-room from public manuals / programming guides only |
| Security | One host per HBA; DMA only via IOMMU windows; kill host ⇒ revoke windows |

**No Linux `scsi_*` / `mpt*` / `megaraid*` source.** Specs + manuals + clean-room only.

Desktop GUI stack (product, userspace): compositor, font, inputd, session manager — phased after block/GPU/input.

### 7.6 Optional external MIT/Apache code

May vendor **only** if license is MIT or Apache-2.0 and provenance is recorded. Examples of the *kind* of thing allowed (illustrative — verify at import time):

- Spec-derived protocol code under MIT/Apache
- Crypto from a MIT/Apache library (or write our own minimal set)
- Device tree / ACPI parsers if MIT/Apache

**Default bias:** write it ourselves if unclear.

### 7.7 Clean-room procedure (required for non-trivial drivers)

1. Collect **public** specs (PCI, NVMe, virtio, vendor programmer manuals).
2. Authors implementing must **not** paste from GPL trees; if they read GPL for ideas, use a **clean-room split** (reader vs implementer) for sensitive ports.
3. Implement in project C style under MIT/Apache.
4. Record `PROVENANCE` entry: specs used, no-GPL attestation, reviewers.
5. Conformance tests: bind, I/O, reset, unplug, IRQ flood.

---

## 8. Interfaces (C + simple IDL → C)

OOP is out. Interfaces are:

- **C headers** with opaque handles (`gj_cap_t`, `gj_ep_t`, …)
- Optional **IDL** that generates **only C** stubs/skeletons (structs + function prototypes)

Illustrative hand-written style:

```c
struct block_ops {
    int (*read)(void *dev, uint64_t lba, uint32_t count, void *buf);
    int (*write)(void *dev, uint64_t lba, uint32_t count, const void *buf);
    int (*flush)(void *dev);
};

struct block_device {
    const struct block_ops *ops;
    void *priv;
};
```

No inheritance. No RTTI. No method sugar.

---

## 9. Memory & I/O

| Topic | Policy |
|-------|--------|
| Kernel allocator | Scalable freelists / slabs; SMP-aware; no driver zoo in kernel |
| Physical | **≥ 1 TiB**; sparse/ hierarchical free tracking |
| Large pages | 2 MiB (and 1 GiB where useful) optional for HHDM / apps |
| User allocators | Per-process; hardened option |
| Shared memory | Explicit grants; non-executable default |
| DMA | IOMMU on production desktop/workstation; revoke + IOTLB flush on host death |
| Swap | Userspace pager (desktop may enable) |

---

## 10. Networking & Storage

### 10.1 Networking

| Piece | Policy |
|-------|--------|
| NIC drivers | Original C under UDX |
| Stack | Userspace `netstackd` written in C (from scratch or MIT/Apache only) |
| Firewall | Userspace first; pf-*ideas* OK, **not** OpenBSD source dump |
| Sockets | POSIX personality libc |

### 10.2 Storage

| Piece | Policy |
|-------|--------|
| Block layer | `storaged` + shared rings |
| Filesystems | Write MIT/Apache FS servers (simple FS first) |
| ZFS | **Out** unless a pure MIT/Apache path exists (do not take CDDL into tree) |
| Disk encryption | Separate service; keys out of driver hosts |

---

## 11. POSIX / Apps / Linux compatibility

**License rule (normative — what you actually required):**

| Allowed | Forbidden |
|---------|-----------|
| **Clean-room** reimplementation of Linux/POSIX **behavior** and ABIs under MIT OR Apache-2.0 | **Copying** GPL/LGPL/AGPL (or other copyleft) **source** into the tree |
| Reading public docs, man pages, ABI descriptions, hardware manuals | Pasting Linux/`glibc`/BusyBox/coreutils **code** |
| Growing a Linux-**compatible** userspace personality for apps/Proton | “Re-license” GPL as MIT (not legal magic) |
| MIT/Apache third-party only | Linking GPL libraries into the product TCB |

**Linux ABI is not banned.** Compatibility is a **product choice** (especially for Deck Top 50 / Proton). The ban is **copyleft code**, not “ideas that look like Linux.”

| Layer | Plan |
|-------|------|
| libc | Written here and/or **MIT/Apache-only** permissive pieces; clean-room Linux-compatible as needed |
| Kernel syscalls | Native GJ mechanism; optional **clean-room Linux syscall personality** in userspace or thin translation layer |
| Linux app ABI | **In scope via clean-room**; **depth driven by Deck Top 50** matrix FAILs |
| **Proton personality** | **Accepted** primary game path — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) v1.6; **not** SteamOS; **no GPL source** |
| Shell/utils | Rewrite or MIT/Apache only — **no GPL coreutils** |

---

## 12. Languages, Style, Tooling

| Layer | Requirement |
|-------|-------------|
| All runtime code | **Pure C11/C17** |
| Arch glue | Minimal `.S` / inline asm |
| IDL compiler | Prefer implemented in C; if a bootstrap tool is temporary non-C, it must not ship in the product TCB and must be replaced — **policy preference: C only even for tools** |
| Build | `make` + Clang or GCC; warnings hard |
| Formatter | `clang-format` with project style |
| Forbidden | C++, Rust, and any other language in `kernel/`, `servers/`, `drivers/`, `lib/` |

**Style:** close to OpenBSD/KNF *spirit* (readable, consistent), documented in `STYLE.md`, original wording.

---

## 13. License Engineering (CI)

| Control | Requirement |
|---------|-------------|
| `LICENSE` | Dual **MIT OR Apache-2.0** at root |
| Per-file SPDX | `SPDX-License-Identifier: MIT` or `Apache-2.0` |
| `third_party/` | Only MIT/Apache; each with `NOTICE` / provenance |
| CI scanner | Fail build on GPL/LGPL/AGPL keywords in licenses |
| Link check | No linking GPL libraries |
| Contributor DCO | Certify original work or MIT/Apache rights |

**Tagline for counsel:** this tree is a **permissive island**. GPL stays on the other side of the clean-room wall.

---

## 14. Build Profiles

| Profile | Description |
|---------|-------------|
| `virt` | QEMU + virtio only; CI |
| `appliance` | Fixed devices; signed driver bundles; verified boot |
| `desktop` | More drivers; stricter isolation for risky devices |
| `rt` | Budget scheduling; minimal drivers |
| `hosted` | Services under another OS for bring-up (still our C, our license) |

---

## 15. Performance Budget

| Metric | Target |
|--------|--------|
| Kernel text (x86_64 release) | < 512 KiB core |
| Idle RAM (virt) | < 32 MiB services baseline |
| NVMe sequential (native driver) | Competitive with mainstream OSes on same HW |
| Isolation | Driver host crash ≠ system panic |

No “must match Linux by importing Linux.”

---

## 16. Verification & Quality

| Practice | Requirement |
|----------|-------------|
| Dual review | All kernel changes |
| Fuzz | IPC, loaders, cap decode, UDX helpers |
| Sanitizers | Kernel + user C |
| Static analysis | clang-tidy / compiler analyzers |
| Reproducible builds | Appliance profile goal |
| License CI | Blocking |

---

## 17. Positioning

| Property | GreenJade |
|----------|--------|
| Language | Pure C |
| License | MIT / Apache-2.0 only |
| Kernel type | Microkernel |
| Drivers | Userspace, original code |
| Linux source | **Never** |
| OOP | **Not a goal** |
| Security defaults | OpenBSD-inspired, reimplemented |
| GPL shed | Explicit project purpose (parallel to BSD vs AT&T) |

---

## 18. Milestones

| Phase | Deliverable |
|-------|-------------|
| **M0** | Kernel boots on QEMU; IPC; caps — all C, MIT/Apache |
| **M1** | init + virtio-blk/net native drivers + simple FS |
| **M2** | `confine` / `expose`; auditd; W^X audit |
| **M3** | UDX + first clean-room PCI NIC driver |
| **M4** | C libc + shell + basic net utils (no GPL userland) |
| **M5** | Appliance profile + signed bundles + license CI green |
| **M6** | Broader driver set via clean-room program |

---

## 19. Open Questions

**Resolved** (see [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) 14):

| Topic | Decision |
|-------|----------|
| License | MIT OR Apache-2.0 dual |
| Firmware blobs | Separate license + hash policy |
| VM / task / session | Apple channel + this freeze |
| Linux ABI | Clean-room compatible; depth = Deck Top 50 |
| Libc | Clean-room + optional MIT/Apache pieces only |
| Netstack | Clean-room userspace first |
| Clean-room wall | Reader/implementer split + CI + provenance |
| Personality TCB | Kernel thread bring-up → userspace doors product |
| Isolation gates | IDT, supervisor-only kernel maps, AS clone |

**No product-blocking design questions remain.** Further choices are implementation detail or measured performance under 0.0.

---

## 20. One-Page Spec Sheet

| Item | Spec |
|------|------|
| **Name** | GreenJade (Project GreenJade) |
| **Type** | Capability microkernel → **general-purpose desktop** |
| **Priorities** | **Security → Performance → Portability → Readability** |
| **Language** | **Pure C only** |
| **License** | **MIT OR Apache-2.0** dual; no GPL |
| **Firmware** | **UEFI** primary |
| **RAM** | **≥ 1 TiB** design/test bar |
| **CPU** | **SMP / multi-CPU** required |
| **Storage** | NVMe/SATA/virtio + **SCSI/SAS** |
| **IP method** | Clean-room / rewrite-out |
| **OOP** | **Not a goal** |
| **Kernel contains** | Caps, VM, threads, IPC, IRQ, timers, SMP |
| **Drivers** | Userspace C; UDX; original implementations |
| **GPL source** | **Forbidden**; clean-room Linux-compatible ABI **allowed** |
| **Arches** | x86_64, aarch64 (+ riscv64 later) |
| **Design freeze** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) |
| **Success** | Secure desktop; SAS/SCSI; **Deck Top 50**; dual-license; **priorities unchanged** |

---

## 21. Tagline

> **GreenJade** — *Security first. UEFI. Teibibytes of RAM. Many CPUs. SAS/SCSI. A real desktop **with games**. Pure C. MIT OR Apache-2.0. No GPL.*

---

## 22. Open design risks (accepted with this scope)

| Risk | Mitigation direction |
|------|----------------------|
| Clean-room GPU/Wi-Fi for desktop is multi-year | virtio-gpu + FB first; progressive HBA/NIC/GPU list |
| SAS HBA diversity | scsi_mid + one well-documented HBA family first |
| 1 TiB+ + SMP + caps concurrency | Spec locking early; no UP-only core APIs |
| UEFI + ACPI surface | Parse only what we need; keep ACPI interpret out of kernel if possible |
| Proton/Wine + clean-room Linux compat | libprotonrt may grow Linux-compatible surface **without GPL source**; still no SteamOS |
| **Deck Top 50** bar | Dated snapshot + matrix; PASS-OFFLINE / BLOCKED-SECURITY for anti-cheat; majority→met claims (0.5.2) |
| 32-bit PE in Top 50 | WoW64-class path; kernel stays 64-bit |
| Multiplayer anti-cheat `.ko` | **BLOCKED-SECURITY** if no userspace path — does not weaken 0.0 |

---

## Status — 2026-07-24

- **Live daemons:** `sessiond` / `netstackd` / `sshd` / `storaged` / `vfsd` / shell / `scsi_mid`
- Residual **#UD** closed
- Parallel soft waves: continuum high-water **advancing toward 22900** (CREATE-ONLY soft graph / parent wire only — not client run; honest `makefile_max` is a Makefile scan and may still report prior tip (e.g. **22800**) until the next decade is wired)
- Product lamps remain **0** by design (soft continuum ≠ product complete)
- **bar3 still open** (Steam client / Deck Top 50 title runs; matrix NOT-TRIED × 50)

---

## 23. Honesty bounds — soft ship ≠ product DoD / bar3 (Wave 87 · 2026-07-24)

**Additive only (Wave 87 exclusive for this file).** Project law (0), product bars, architecture, and the rest of this sheet stay normative. This section is a Wave 87 honesty ledger only: greppable soft ship and continuum growth do **not** close product DoD or Steam **bar3**. It does **not** reorder 0.0, invent title PASS, or claim multi-server product complete.

| Term | Meaning on this document |
|------|--------------------------|
| **Soft ship** | Greppable kernel/media/continuum bring-up on tree — honesty only |
| **Soft continuum** | CREATE-ONLY libcgj graph parent wire; high-water **advancing toward 22900** soft only |
| **Product lamps** | Soft score / bar3-ready continuum stubs — remain **0** by design |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN** |

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 22900** | CREATE-ONLY soft graph high-water advance (parent wires) | Runtime ABI product; Steam client; matrix fill; product lamps lit |
| Live embeds / soft smokes | Bring-up / skeleton honesty | Full multi-server product DoD; bar3 closed |
| Design freeze / this sheet | Normative law + architecture | Product complete; titles tried |

**Hard stamp (Wave 87):** soft ship / continuum **toward 22900** ≠ product DoD ≠ bar3. Product lamps **0**. **Soft ≠ product complete.** Matrix stays **NOT-TRIED × 50**.

**Hard rule:** never promote continuum soft gates, media READY, or design text alone to “bar3 closed,” Deck Top 50 `PASS`, product lamps > 0, or product DoD complete. Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) / [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md).

### Explicit non-claims (Wave 87)

| Claim | Allowed? |
|-------|----------|
| “Architecture / project law as stated in this sheet” | **Yes** — design law |
| “Soft continuum high-water advancing toward **22900** (CREATE-ONLY)” | **Yes** — soft only |
| “Product lamps lit / product DoD closed from soft continuum” | **No** |
| “bar3 closed / Deck Top 50 leave NOT-TRIED from this sheet alone” | **No** |

---

*End of spec sheet v0.13 — design complete freeze; Deck Top 50; clean-room Linux ABI; no GPL source; priorities unchanged.*  
*23 Wave 87 honesty (2026-07-24): soft ship / continuum **toward 22900 soft only**; product lamps **0**; **soft ≠ product complete**; **bar3 remains OPEN**.*
