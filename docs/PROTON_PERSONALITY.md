# GreenJade — Proton Personality (game runtime tier)

| Field | Value |
|-------|--------|
| **Document** | Proton personality **v1.6** |
| **Status** | **Accepted** — primary **desktop adoption** surface for games |
| **Product role** | **Major game support** = **Steam Deck Top 50** ([GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) 0.5.2) |
| **License** | **No GPL/copyleft source**; **clean-room Linux-compatible ABI is in scope and expected to grow** |
| **Doc precedence** | **Proton overrides Apple-channel** on conflict (0.1) — still **below** project law & security core |
| **Priorities** | Still **Security → Performance → Portability → Readability** — games do not reorder ranks |
| **Not** | SteamOS distro port · importing GPL · loading Linux `.ko` · ambient root for anti-cheat |
| **Honesty** | **2026-07-24 Wave 81** — soft PE / Proton path ≠ bar3 (10–11 historical · 12); continuum toward **22300** soft only; lamps **0**; **no title PASS**; **no bar3 claim** |
| **Companion** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) · [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) · [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

**Acceptance trail:** v1.4 Deck Top 50 · v1.5 clean-room Linux ABI OK · **v1.6** re-applies Proton + Deck Top 50 as the focused product path under that license rule.

**Soft stamp (Wave 10, historical):** greppable PE32 / WoW64 / `libprotonrt` / `protonrt-user` / A0–A1 bring-up smokes are **soft PE/Proton path only**. They do **not** close Steam **bar3**, launch the Steam client, or move any Deck Top 50 matrix row off **NOT-TRIED**. See 10.

**Soft stamp (Wave 13, historical):** same soft PE / Proton bound restated — greppable PE32 / WoW64 / `libprotonrt` / `protonrt-user` / A0–A1 smokes remain **soft PE/Proton path only**. Soft PE / Proton path **≠ bar3**. **No title PASS.** **No bar3 claim.** See 11.

**Soft stamp (Wave 81):** soft PE / Proton path **≠ bar3** restated. Soft continuum high-water **advancing toward 22300** (honest scan may still be **makefile_max=22200** until parent wires) is **not** a title try and **not** product complete. Product lamps remain **0**. Matrix **NOT-TRIED × 50**. **No title PASS.** **No bar3 claim.** See 12.

---

## 0. Product facts (normative)

| Fact | Detail |
|------|--------|
| **P1** | Without **major game support**, there is no **popular** desktop take-up. |
| **P2** | **Major game support** = **Steam Deck Top 50** (dated snapshot, pass matrix) — arch 0.5.2. |
| **P3** | The stack that serves P2 is the **Proton personality** + out-of-tree Proton-class runtime + session/GPU path. |
| **P4** | Linux-**compatible** behavior is **grown clean-room** as Top 50 titles require — **not** banned as “no Linux ABI.” |
| **P5** | **Copying** GPL/LGPL source remains **forbidden**; clean-room reimplementation is the method. |
| **P6** | **0.0 priorities unchanged.** If a title needs ambient root, `.ko`, or TCB sabotage → **BLOCKED-SECURITY**, not a priority rewrite. |

---

## 0.1 Document precedence (normative)

| Rank | Source |
|------|--------|
| **1** | GreenJade project law (priorities 0.0, pure C in-tree, dual MIT/Apache, **no GPL source**, no ambient root) |
| **2** | [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) |
| **3** | [CAP_ADDRESSING.md](CAP_ADDRESSING.md) |
| **4** | **This document** — Deck Top 50 / Proton product surface |
| **5** | [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) — where it does not block Proton |
| **6** | [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) |
| **7** | UDX / platform / implementation sketches |

**P-PRECEDENCE:** Apple (or Solaris conveniences) lose when they make Deck Top 50 / Proton **impractical**.  
**P-PRIORITY:** Never elevate games above **Security**. Redesign or mark **BLOCKED-SECURITY**.  
**P-ABI:** Prefer **clean-room Linux-compatible** surfaces for game tasks when that shortens the path to Top 50 PASS; native GJ remains under the personality.

### Apple kept vs overridden (Deck Top 50 lens)

| Keep (helps games) | Override for Proton / Top 50 |
|--------------------|------------------------------|
| Memory objects / views | **Named shareable** objects (wine-server, GPU export) P0 |
| Task ports, QoS, session TCB | Cross-process **futex** P0; **socketpair/AF_UNIX-shaped** P0 |
| JIT entitlement | Exception/SEH-shaped delivery P0–P1 for game tasks |
| Map cookie security | Linux-compatible `mmap`/`mprotect`/`futex` **names** in libprotonrt |
| Doors IPC | Grow ABI depth from **Top 50 title needs**, not “minimal desktop” |

---

## 1. Goal architecture

```text
  Windows game (Deck Top 50 candidate)
       │
       ▼
  Proton-class runtime (out-of-tree; any license of its own)
       │  targets clean-room Linux-compatible + GJ contracts
       ▼
  ┌────────────────────────────────────────────────────┐
  │  libprotonrt (in-tree, pure C, MIT OR Apache-2.0)  │
  │  clean-room Linux/POSIX-compatible personality     │
  │  depth driven by Deck Top 50 matrix                │
  ├────────────────────────────────────────────────────┤
  │  GreenJade native: caps, VM objects, futex, doors, │
  │  session, GPU host (UDX), vfs, net                 │
  └────────────────────────────────────────────────────┘
```

| Layer | Rule |
|-------|------|
| In-tree | Pure C, dual MIT/Apache only |
| Out-of-tree Proton/Wine/DXVK | Separate product/repo; **not** mixed into GJ dual-license claim |
| No vendoring | Do not paste Wine/Proton/Linux GPL trees into GreenJade |

---

## 2. Deck Top 50 — operational focus

Normative definition: [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) **0.5.2**.

| Engineering rule | Detail |
|------------------|--------|
| **Matrix first** | Maintain a **dated** Top 50 snapshot + per-title row (PASS / PASS-OFFLINE / BLOCKED-SECURITY / FAIL / NOT-TRIED) |
| **ABI growth** | When a FAIL is “missing Linux behavior,” implement **clean-room** in libprotonrt (or native GJ + shim) — do not import GPL |
| **HCL** | Certify on stated HCL (T0 virtio minimum; T1+ GPU as available) |
| **Offline honesty** | Kernel AC-only multiplayer → PASS-OFFLINE or BLOCKED-SECURITY, not fake PASS |
| **Claim levels** | Targeting · majority (≥25) · met (≥40) · full (50 where law allows) |

### Adoption ladder (critical path)

| Stage | Proof | Implies kernel/userspace |
|-------|--------|---------------------------|
| **A0** | Personality smoke + wine-server two-process demo | futex (cross-proc), named objects, socketpair-shaped, JIT, PROCESS |
| **A1** | ≥1 real Top 50 title PASS or PASS-OFFLINE | Vulkan present path, session input/audio, vfs |
| **A2** | **Majority** ≥25/50 | ABI breadth, WoW64 path for 32-bit titles as needed |
| **A3** | **Met** ≥40/50 | Net where needed; file locks; stability |
| **A4** | **Full** 50/50 where law allows | Remaining BLOCKED-SECURITY only if no clean-room path |

**Program rule:** From M2 onward, work that unblocks **A0→A1** is on the **desktop critical path**, not “M6 hobby.” SAS/1 TiB still product requirements; they do not postpone A0 forever.

---

## 3. Clean-room Linux-compatible surface (Deck Top 50 driven)

### 3.1 Strategy

| Rule | Detail |
|------|--------|
| **S1** | Default game task view: **Linux-compatible** libc + syscall/personality surface (clean-room). |
| **S2** | Underneath: native GJ caps, objects, doors — games do not mint untyped or bind IRQs. |
| **S3** | Depth is **title-driven**: add interfaces when the Deck Top 50 matrix needs them. |
| **S4** | Document each major surface in `proton_rt_query` feature bits. |
| **S5** | Forbidden: paste from Linux/glibc/Wine GPL trees. |

### 3.2 Tier map (re-applied)

#### P0 — wine-class core (A0)

| Area | Surface (Linux-compatible where useful) | Backing |
|------|----------------------------------------|---------|
| Threads | `pthread_*` subset | GJ threads |
| Sync | futex wait/wake; **cross-process** on shared objects | Kernel futex; key `(object_id, offset)` |
| Time | `clock_gettime` mono/raw, sleeps | Kernel mono |
| Memory | `mmap`/`munmap`/`mprotect`/`madvise` | Regions + **named** memory objects |
| JIT | RW→RX; RWX with CapJit | Apple JIT right |
| TLS | TP / `arch_prctl`-shape | Arch + runtime |
| IPC | **socketpair / AF_UNIX-shaped** | Doors + shared mem + futex |
| FD table | int FDs → caps underneath | Never expose raw CNode to game |
| Process | exit, pid-like id | PROCESS / self |

#### P1 — files, async, present (A1+)

| Area | Choice |
|------|--------|
| Files | open/read/write/lseek/close/stat + **fsync** via vfs |
| Locks | fcntl/flock subset — Wine needs |
| Dirs | getdents-class for installs |
| epoll/poll | userspace demux first; **kernel demux** if Top 50 needs |
| eventfd/pipe | runtime + futex; kernel if needed |
| memfd-shape | named anon object, shareable |
| Exceptions | PCB exception port + SEH/signal-**shaped** delivery in libprotonrt |
| Graphics | **Vulkan** ICD + compositor present; clean-room DRM/ioctl shims **only if** a title/ICD needs them |
| Input/audio | session-filtered; Linux-shaped shims OK clean-room |
| Priority/affinity | map Win32/pthread → QoS; affinity P1 |

#### P2 — multiplayer & breadth (A2–A3)

| Area | Choice |
|------|--------|
| Sockets | BSD-shape → netstackd (clean-room) |
| WoW64 | 32-bit PE path in userspace (kernel stays 64-bit) |
| netlink/SCM zoo | clean-room **if** a Top 50 title requires |
| Casefold mounts | game library dirs |

---

## 4. Process, caps, memory (unchanged direction, Top 50 wording)

### 4.1 Process model

One **game task** + many threads; wine-server (if used) = **second task**; helpers via **spawn** + explicit caps (no ambient fork of whole CNode).

### 4.2 Typical game task bag

Restricted self · CapJit · vfs file caps · compositor surface · filtered input · audio client · wine-server endpoint · **named memory object** create/map/share · **not** IOMMU / IRQ bind-any / other apps’ surfaces.

### 4.3 Memory

Object owns pages; maps are views; **named share/export** first-class for wine-server and GPU; xstate required; WoW64 in-scope for Top 50 32-bit titles.

### 4.4 Cross-process futex

| Rule | Detail |
|------|--------|
| Key | `(memory_object_id, byte_offset)` |
| Ops P0 | WAIT, WAKE |
| Unmap | wake waiters safely |
| Timeout | mono deadline |

---

## 5. Graphics, input, audio (Deck Top 50)

| Piece | Focus |
|-------|--------|
| **Primary** | Vulkan → clean-room ICD or virtio-gpu → GPU host (UDX) → **session present** |
| **Export/sync** | Memory objects + gen-checked export; timeline/fence path P1 (blocks A1 if missing) |
| **DRM** | Not required to clone Linux DRM; **clean-room ioctl compatibility** allowed when a Top 50 stack needs it |
| **Input** | Session game capture; optional evdev-shaped shim |
| **Audio** | Stream client; soft-RT QoS |

---

## 6. Anti-cheat (Deck Top 50 honesty) — **Accepted**

Normative: [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) 7.

| Approach | Status |
|----------|--------|
| Load vendor Linux **`.ko`** | **No** |
| **Userspace AC agent** (INSPECT on game task only) | **Yes** |
| Server-side only | **Yes** |
| PASS-OFFLINE / BLOCKED-SECURITY | Per arch 0.5.2 |

Matrix files: `matrix/deck-top50-*.md`. Never weaken 0.0 for multiplayer.

---

## 7. Feature discovery

```c
#define PROTON_RT_VERSION_MAJOR 1
#define PROTON_RT_VERSION_MINOR 6

#define PROTON_FEAT_JIT           (1u << 0)
#define PROTON_FEAT_CROSS_FUTEX   (1u << 1)
#define PROTON_FEAT_VULKAN_ICD    (1u << 2)
#define PROTON_FEAT_AUDIO         (1u << 3)
#define PROTON_FEAT_NET           (1u << 4)
#define PROTON_FEAT_CASEFOLD_FS   (1u << 5)
#define PROTON_FEAT_WINE_IPC      (1u << 6)
#define PROTON_FEAT_UNIX_SOCKET   (1u << 7)
#define PROTON_FEAT_WOW64         (1u << 8)
#define PROTON_FEAT_FILE_LOCK     (1u << 9)
#define PROTON_FEAT_LINUX_COMPAT  (1u << 10) /* clean-room Linux-compatible personality present */

struct proton_rt_info {
    u32 u32Version;
    u32 u32Features;
};
int proton_rt_query(struct proton_rt_info *pOut);
```

In-tree path when landed: `user/libprotonrt/`.

---

## 8. Implementation order (aligned to A0–A3)

1. **A0 / hybrid:** Option C **v1.2** — ring-3 SYSCALL, user pointer checks, PA futex share, CapJit/W^X, blocking cold_ipc ([LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md))  
2. **A0 / M2 critical:** multi-process CR3, named memory objects, sleeping futex, socketpair-shaped IPC, mono TSC, xstate  
3. **A0:** `libprotonrt` FD table + vfs read/write + wine-server two-process demo  
3. **A0–A1:** exception port + SEH/signal-shaped delivery  
4. **A1:** Vulkan ICD + export/present/sync + session input/audio  
5. **A1+:** file locks, fsync, priority→QoS, affinity  
6. **A2:** WoW64 path; broaden clean-room Linux ABI from matrix FAILs  
7. **A2–A3:** netstack; Top 50 matrix majority → met  
8. Out-of-tree Proton against `proton_rt_query`  

---

## 9. One-page summary

| Topic | Decision |
|-------|----------|
| Adoption | **Deck Top 50** (arch 0.5.2) |
| Method | Proton-class runtime + **clean-room** Linux-compatible `libprotonrt` |
| License | **No GPL copy**; dual MIT/Apache in-tree |
| Priorities | Unchanged (0.0) |
| vs Apple | Proton wins on conflict among secure designs |
| vs SteamOS | Distro port out; ABI compatibility in |
| Anti-cheat | Userspace/GJ agent + PASS-OFFLINE; no `.ko` |
| Critical path | A0→A1 from M2, not deferred forever |
| Soft PE / Proton path | **≠ bar3** — greppable smokes only; matrix stays **NOT-TRIED** (10–11 historical · 12 Wave 81) |

---

## 10. Honesty refresh — soft PE / Proton path ≠ bar3 (Wave 10 · 2026-07-24) — historical ledger

**Additive only (Wave 10).** Product facts (0), precedence (0.1), architecture (1–8), and the one-page summary (9) stay **Accepted**. This section is the Wave 10 honesty ledger (kept as historical trail). Wave 13 restates the same bound in 11 without reopening architecture.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Normative Proton / Deck Top 50 product surface — ship toward these rules |
| **Soft PE / Proton path** | Partial / greppable PE32, WoW64, `libprotonrt`, personality door, A0–A1 kernel/userspace smokes — **bring-up honesty only** |
| **Open (product)** | Real Steam **client** on DUT; out-of-tree Proton-class runtime against titles; matrix leave **NOT-TRIED** |
| **bar3** | Steam **client** launch on real DUT + Deck Top 50 rows can leave `NOT-TRIED` — see [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

### 10.1 Soft PE / Proton path does not close bar3

| Soft surface (tree / host) | Closes A0 product demo path? | Closes A1 title path? | Closes bar3? | Moves matrix off NOT-TRIED? |
|----------------------------|------------------------------|-----------------------|--------------|-------------------------------|
| `pe32: * PASS` (parse / map / spawn / int80 / soft-exec / …) | **No** (PE bring-up only) | **No** | **No** | **No** |
| `wow64: path PASS` / CS32 / compat frame / trampoline smokes | **No** | **No** | **No** | **No** |
| `libprotonrt` cold Linux stubs / feature bits / `proton_rt_query` skeleton | Soft surface only | **No** | **No** | **No** |
| `protonrt-user: door server up` / soft serve ready / soft miss markers | Soft door loop only | **No** | **No** | **No** |
| A0 wine-server two-process demo (futex + socketpair-shaped + CapJit) | Soft A0 smoke | **No** | **No** | **No** |
| A1 T0 Vulkan ICD / present / session input / HDA stubs | Soft present path | **No** (≠ real title) | **No** | **No** |
| Host Steam media inventory **READY** (`steam-bar3-check`) | **No** | **No** | **No** | **No** |
| Continuum **makefile_max** CREATE-ONLY graph decades | **No** | **No** | **No** | **No** |
| Kernel smokes (io_uring min, 768G soak, aarch64 M0, hybrid ABI) | **No** | **No** | **No** | **No** |
| This document **Accepted** (v1.6) | Decisions only | Decisions only | **No** | **No** |

**Hard rule:** never promote greppable `pe32:*` / `wow64:*` / `protonrt-user:*` / A0–A1 soft smokes, media `STATUS=READY`, continuum soft gates, or design **Accepted** alone to “bar3 closed,” “Steam client runs,” “Proton titles work,” or any Deck Top 50 row off **NOT-TRIED**. Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) and [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md).

### 10.2 Soft PE / Proton ledger (bring-up bound)

| Soft PE / Proton surface | Honest bound (what greppable PASS means) | Remaining product (do not claim done) |
|--------------------------|------------------------------------------|----------------------------------------|
| **PE32 load / map / spawn** | Kernel PE header/section/user-map/spawn smokes | Out-of-tree Windows game PE via Proton-class runtime on GreenJade |
| **WoW64 / CS32 / int80** | 32-bit PE bring-up path; multi-NR int80 smokes | Product 32-bit Top 50 titles through full WoW64 + Proton stack |
| **libprotonrt** | Clean-room personality stubs; feature-bit query skeleton | Depth driven by real matrix FAILs; wine-class FD/vfs/IPC product surface |
| **protonrt-server (G-PERS)** | Ring-3 door loop soft markers; interim cold serve | Full userspace cold Linux service for game tasks |
| **A0 ladder proof** | Two-process wine-server-shaped demo smokes | Durable A0 product demo as operator-facing critical path |
| **A1 present / input / audio** | Vulkan ICD + session/HDA **stubs / soft present** | First real Top 50 title **PASS** or **PASS-OFFLINE** on DUT |
| **Out-of-tree Proton** | Documented product intent only | Separate product/repo targeting GJ contracts; **not** in-tree |

### 10.3 Explicit non-claims (Wave 10)

| Claim | Allowed? |
|-------|----------|
| “Proton personality v1.6 **Accepted**” | **Yes** — this document 0–9 |
| “Soft PE / WoW64 / libprotonrt / protonrt-user smokes greppable” | **Yes** — with soft bound (10.1–10.2) |
| “Soft PE / Proton path = bar3 closed” | **No** — **soft PE/Proton path ≠ bar3** |
| “Steam client launched on GreenJade” | **No** — bar3 client path remains **OPEN** |
| “Deck Top 50 titles tried / PASS from PE or A0–A1 soft smokes” | **No** — matrix stays **NOT-TRIED × 50** until real client + title runs |
| “Media READY / continuum / hybrid ABI closes Proton product or bar3” | **No** |
| “Out-of-tree Proton shipped in-tree” | **No** — remains separate product/repo |

**Bar3 remains OPEN** (client + matrix). Soft PE / Proton path is real bring-up work and may unblock later A0→A1 engineering — it is **not** a bar3 or Top 50 claim.

### 10.4 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ client run ≠ Top-50  
- [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) — hybrid soft surface ≠ bar3  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) · [HCL.md](HCL.md) — media / HCL soft stamps  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title rows (**NOT-TRIED**)  
- [TODO.md](TODO.md) · [IMPLEMENTATION.md](IMPLEMENTATION.md) — PE / Proton coding boxes  

---

## 11. Honesty refresh — soft PE / Proton path ≠ bar3 (Wave 13 · 2026-07-24)

**Additive only (Wave 13 exclusive for this file).** Product facts (0), precedence (0.1), architecture (1–8), one-page summary (9), and the Wave 10 honesty ledger (10) stay **Accepted** / historical. This section is Wave 13 honesty only: reaffirm that greppable **soft PE / Proton** bring-up is **not** Steam **bar3** and does **not** move Deck Top 50 rows. It does **not** re-litigate architecture, license, 0.0 priorities, or invent title PASS.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Normative Proton / Deck Top 50 product surface — ship toward these rules (0–9) |
| **Soft PE / Proton path** | Partial / greppable PE32, WoW64, `libprotonrt`, personality door, A0–A1 kernel/userspace smokes — **bring-up honesty only** |
| **Open (product)** | Real Steam **client** on DUT; out-of-tree Proton-class runtime against titles; matrix leave **NOT-TRIED** |
| **bar3** | Steam **client** launch on real DUT + Deck Top 50 rows can leave `NOT-TRIED` — see [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

**Hard stamp (Wave 13):** **soft PE / Proton path ≠ bar3.** Greppable soft PE surfaces do **not** close bar3, launch the Steam client, or promote any matrix row off **NOT-TRIED**.

### 11.1 Soft PE / Proton path still does not close bar3

| Soft surface (tree / host) | Closes A0 product demo path? | Closes A1 title path? | Closes bar3? | Moves matrix off NOT-TRIED? |
|----------------------------|------------------------------|-----------------------|--------------|-------------------------------|
| `pe32: * PASS` (parse / map / spawn / int80 / soft-exec / …) | **No** (PE bring-up only) | **No** | **No** | **No** |
| `wow64: path PASS` / CS32 / compat frame / trampoline smokes | **No** | **No** | **No** | **No** |
| `libprotonrt` cold Linux stubs / feature bits / `proton_rt_query` skeleton | Soft surface only | **No** | **No** | **No** |
| `protonrt-user: door server up` / soft serve ready / soft miss markers | Soft door loop only | **No** | **No** | **No** |
| A0 wine-server two-process demo (futex + socketpair-shaped + CapJit) | Soft A0 smoke | **No** | **No** | **No** |
| A1 T0 Vulkan ICD / present / session input / HDA stubs | Soft present path | **No** (≠ real title) | **No** | **No** |
| Host Steam media inventory **READY** (`steam-bar3-check`) | **No** | **No** | **No** | **No** |
| Continuum **makefile_max** CREATE-ONLY graph decades | **No** | **No** | **No** | **No** |
| Kernel smokes (io_uring min, 768G soak, aarch64 M0, hybrid ABI) | **No** | **No** | **No** | **No** |
| This document **Accepted** (v1.6) + Wave 10/13 honesty | Decisions / honesty only | Decisions / honesty only | **No** | **No** |

**Hard rule (Wave 13):** never promote greppable `pe32:*` / `wow64:*` / `protonrt-user:*` / A0–A1 soft smokes, media `STATUS=READY`, continuum soft gates, or design **Accepted** alone to “bar3 closed,” “Steam client runs,” “Proton titles work,” or any Deck Top 50 row off **NOT-TRIED**. Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) and [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md).

**Wave 13 formula:** **soft PE / Proton path ≠ bar3.**

### 11.2 Soft PE / Proton ledger (bring-up bound — unchanged product open)

| Soft PE / Proton surface | Honest bound (what greppable PASS means) | Remaining product (do not claim done) |
|--------------------------|------------------------------------------|----------------------------------------|
| **PE32 load / map / spawn** | Kernel PE header/section/user-map/spawn smokes | Out-of-tree Windows game PE via Proton-class runtime on GreenJade |
| **WoW64 / CS32 / int80** | 32-bit PE bring-up path; multi-NR int80 smokes | Product 32-bit Top 50 titles through full WoW64 + Proton stack |
| **libprotonrt** | Clean-room personality stubs; feature-bit query skeleton | Depth driven by real matrix FAILs; wine-class FD/vfs/IPC product surface |
| **protonrt-server (G-PERS)** | Ring-3 door loop soft markers; interim cold serve | Full userspace cold Linux service for game tasks |
| **A0 ladder proof** | Two-process wine-server-shaped demo smokes | Durable A0 product demo as operator-facing critical path |
| **A1 present / input / audio** | Vulkan ICD + session/HDA **stubs / soft present** | First real Top 50 title **PASS** or **PASS-OFFLINE** on DUT |
| **Out-of-tree Proton** | Documented product intent only | Separate product/repo targeting GJ contracts; **not** in-tree |

### 11.3 Explicit non-claims (Wave 13)

| Claim | Allowed? |
|-------|----------|
| “Proton personality v1.6 **Accepted**” | **Yes** — this document 0–9 |
| “Soft PE / WoW64 / libprotonrt / protonrt-user smokes greppable” | **Yes** — with soft bound (10 / 11.1–11.2) |
| “Soft PE / Proton path = bar3 closed” | **No** — **soft PE/Proton path ≠ bar3** |
| “Steam client launched on GreenJade” | **No** — bar3 client path remains **OPEN** |
| “Deck Top 50 titles tried / PASS from PE or A0–A1 soft smokes” | **No** — matrix stays **NOT-TRIED × 50** until real client + title runs |
| “Media READY / continuum / hybrid ABI closes Proton product or bar3” | **No** |
| “Out-of-tree Proton shipped in-tree” | **No** — remains separate product/repo |
| “Wave 13 honesty closes bar3 or any matrix row” | **No** — honesty refresh only |

**Bar3 remains OPEN** (client + matrix). Soft PE / Proton path is real bring-up work and may unblock later A0→A1 engineering — it is **not** a bar3 or Top 50 claim. Wave 13 does **not** invent title PASS.

### 11.4 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ client run ≠ Top-50  
- [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) — hybrid soft surface ≠ bar3  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) · [HCL.md](HCL.md) — media / HCL soft stamps  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title rows (**NOT-TRIED**)  
- [TODO.md](TODO.md) · [IMPLEMENTATION.md](IMPLEMENTATION.md) — PE / Proton coding boxes  
- 10 above — Wave 10 historical soft PE / Proton ≠ bar3 ledger  



## 12. Honesty refresh — soft PE / Proton path ≠ bar3 (Wave 81 · 2026-07-24)

**Additive only (Wave 81 exclusive for this file).** Product facts (0), precedence (0.1), architecture (1–8), one-page summary (9), and the Wave 10/13 honesty ledgers (10–11) stay **Accepted** / historical. This section is Wave 81 honesty only: reaffirm that greppable **soft PE / Proton** bring-up is **not** Steam **bar3**, that soft continuum toward **22300** does **not** move Deck Top 50 rows, and that product lamps remain **0**. It does **not** re-litigate architecture, license, 0.0 priorities, or invent title PASS.

| Term | Meaning in this document |
|------|--------------------------|
| **Soft PE / Proton path** | Partial / greppable PE32, WoW64, `libprotonrt`, personality door, A0–A1 kernel/userspace smokes — **bring-up honesty only** |
| **Soft continuum** | CREATE-ONLY libcgj graph parent wire **advancing toward 22300**; honest scan may still report **makefile_max=22200** until parent wires |
| **Product lamps** | Soft bar3-ready / product-score stubs remain **0** by design — **soft ≠ product complete** |
| **Open (product)** | Real Steam **client** on DUT; out-of-tree Proton-class runtime against titles; matrix leave **NOT-TRIED** |
| **bar3** | Steam **client** launch on real DUT + Deck Top 50 rows can leave `NOT-TRIED` — see [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

**Hard stamp (Wave 81):** **soft PE / Proton path ≠ bar3.** Soft continuum toward **22300** ≠ bar3 and ≠ product complete. Greppable soft PE surfaces do **not** close bar3, launch the Steam client, or promote any matrix row off **NOT-TRIED**. Product lamps remain **0**.

### 12.1 Soft PE / Proton path still does not close bar3

| Soft surface (tree / host) | Closes A0 product demo path? | Closes A1 title path? | Closes bar3? | Moves matrix off NOT-TRIED? |
|----------------------------|------------------------------|-----------------------|--------------|-------------------------------|
| `pe32: * PASS` / `wow64: *` / `libprotonrt` / `protonrt-user` / A0–A1 soft smokes | Soft only | **No** | **No** | **No** |
| Host Steam media inventory **READY** (`steam-bar3-check`) | **No** | **No** | **No** | **No** |
| Continuum high-water **toward 22300** (scan may still be **22200**) | **No** | **No** | **No** | **No** |
| Product lamps **0** | **No** | **No** | **No** | **No** |
| Kernel smokes (io_uring min, 768G soak, aarch64 M0, hybrid ABI) | **No** | **No** | **No** | **No** |
| This document **Accepted** (v1.6) + Wave 10/13/15 honesty | Decisions / honesty only | Decisions / honesty only | **No** | **No** |

**Hard rule (Wave 81):** never promote greppable soft PE smokes, media `STATUS=READY`, continuum soft gates (toward **22300** or prior tip **22200**), product lamps, or design **Accepted** alone to “bar3 closed,” “Steam client runs,” “Proton titles work,” product complete, or any Deck Top 50 row off **NOT-TRIED**.

**Wave 81 formula:** **soft PE / Proton path ≠ bar3.** Soft continuum ≠ product complete. Product lamps **0**. Matrix **NOT-TRIED × 50**.

### 12.2 Explicit non-claims (Wave 81)

| Claim | Allowed? |
|-------|----------|
| “Proton personality v1.6 **Accepted**” | **Yes** — this document 0–9 |
| “Soft PE / WoW64 / libprotonrt / protonrt-user smokes greppable” | **Yes** — with soft bound |
| “Continuum high-water advancing toward **22300** soft only (scan may still be 22200)” | **Yes** — soft only |
| “Soft PE / Proton path = bar3 closed” | **No** — **soft PE/Proton path ≠ bar3** |
| “Product lamps lit / product complete from continuum or PE soft smokes” | **No** — lamps remain **0** |
| “Steam client launched on GreenJade” | **No** — bar3 client path remains **OPEN** |
| “Deck Top 50 titles tried / PASS from PE or A0–A1 soft smokes” | **No** — matrix stays **NOT-TRIED × 50** until real client + title runs |
| “Media READY / continuum / hybrid ABI closes Proton product or bar3” | **No** |
| “Wave 81 honesty closes bar3 or any matrix row” | **No** — honesty refresh only |

**Bar3 remains OPEN** (client + matrix). Soft PE / Proton path is real bring-up work and may unblock later A0→A1 engineering — it is **not** a bar3 or Top 50 claim. Wave 81 does **not** invent title PASS.

### 12.3 Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ client run ≠ Top-50  
- [IMPLEMENTATION.md](IMPLEMENTATION.md) — Wave 81 continuum toward 22300 soft stamp  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title rows (**NOT-TRIED**)  
- 10–11 above — Wave 10/13 historical soft PE / Proton ≠ bar3 ledgers  

---

*Proton personality v1.6 **Accepted** — Deck Top 50 focus; clean-room Linux ABI grows with the matrix; no GPL; priorities unchanged.*  
*Wave 10 honesty (10, historical): **soft PE / Proton path ≠ bar3**; matrix **NOT-TRIED × 50**; **bar3 remains OPEN**.*  
*Wave 13 honesty (11, historical): **soft PE / Proton path ≠ bar3**; matrix **NOT-TRIED × 50**; **bar3 remains OPEN**; **no title PASS**.*  
*Wave 81 honesty (12): **soft PE / Proton path ≠ bar3**; continuum toward **22300** soft only (scan may still be **22200**); product lamps **0**; **soft ≠ product complete**; matrix **NOT-TRIED × 50**; **bar3 remains OPEN**; **no title PASS**.*
