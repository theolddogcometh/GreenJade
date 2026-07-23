# GreenJade — Proton Personality (game runtime tier)

| Field | Value |
|-------|--------|
| **Document** | Proton personality **v1.6** |
| **Status** | **Accepted** — primary **desktop adoption** surface for games |
| **Product role** | **Major game support** = **Steam Deck Top 50** ([GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) §0.5.2) |
| **License** | **No GPL/copyleft source**; **clean-room Linux-compatible ABI is in scope and expected to grow** |
| **Doc precedence** | **Proton overrides Apple-channel** on conflict (§0.1) — still **below** project law & security core |
| **Priorities** | Still **Security → Performance → Portability → Readability** — games do not reorder ranks |
| **Not** | SteamOS distro port · importing GPL · loading Linux `.ko` · ambient root for anti-cheat |
| **Companion** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) · [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) · [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) |

**Acceptance trail:** v1.4 Deck Top 50 · v1.5 clean-room Linux ABI OK · **v1.6** re-applies Proton + Deck Top 50 as the focused product path under that license rule.

---

## 0. Product facts (normative)

| Fact | Detail |
|------|--------|
| **P1** | Without **major game support**, there is no **popular** desktop take-up. |
| **P2** | **Major game support** = **Steam Deck Top 50** (dated snapshot, pass matrix) — arch §0.5.2. |
| **P3** | The stack that serves P2 is the **Proton personality** + out-of-tree Proton-class runtime + session/GPU path. |
| **P4** | Linux-**compatible** behavior is **grown clean-room** as Top 50 titles require — **not** banned as “no Linux ABI.” |
| **P5** | **Copying** GPL/LGPL source remains **forbidden**; clean-room reimplementation is the method. |
| **P6** | **§0.0 priorities unchanged.** If a title needs ambient root, `.ko`, or TCB sabotage → **BLOCKED-SECURITY**, not a priority rewrite. |

---

## 0.1 Document precedence (normative)

| Rank | Source |
|------|--------|
| **1** | GreenJade project law (priorities §0.0, pure C in-tree, dual MIT/Apache, **no GPL source**, no ambient root) |
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

Normative definition: [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) **§0.5.2**.

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

Normative: [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) §7.

| Approach | Status |
|----------|--------|
| Load vendor Linux **`.ko`** | **No** |
| **Userspace AC agent** (INSPECT on game task only) | **Yes** |
| Server-side only | **Yes** |
| PASS-OFFLINE / BLOCKED-SECURITY | Per arch §0.5.2 |

Matrix files: `matrix/deck-top50-*.md`. Never weaken §0.0 for multiplayer.

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
| Adoption | **Deck Top 50** (arch §0.5.2) |
| Method | Proton-class runtime + **clean-room** Linux-compatible `libprotonrt` |
| License | **No GPL copy**; dual MIT/Apache in-tree |
| Priorities | Unchanged (§0.0) |
| vs Apple | Proton wins on conflict among secure designs |
| vs SteamOS | Distro port out; ABI compatibility in |
| Anti-cheat | Userspace/GJ agent + PASS-OFFLINE; no `.ko` |
| Critical path | A0→A1 from M2, not deferred forever |

---

*Proton personality v1.6 **Accepted** — Deck Top 50 focus; clean-room Linux ABI grows with the matrix; no GPL; priorities unchanged.*
