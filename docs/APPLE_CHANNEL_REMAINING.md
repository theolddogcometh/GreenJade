# GreenJade — Remaining Design Choices (Apple-channel)

| Field | Value |
|-------|--------|
| **Document** | Apple-channel decisions v1.1 |
| **Status** | **Accepted** — product VM / task / session judgment under GreenJade law |
| **Persona** | “What would an Apple systems kernel engineer ship for a desktop?” |
| **Conflict rule** | **Proton personality wins** over this document when they conflict — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) §0.1 — **does not** change Security→Performance→Portability→Readability |
| **Completion** | Isolation/doors/JIT production freezes in [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) |
| **Companion** | [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) · [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) |

GreenJade law still wins: caps, security-first, pure C, dual MIT/Apache, no GPL, no ambient root.

**Heritage split (normative):**

| Domain | Channel |
|--------|---------|
| Locks, doors IPC shape, DDI quiesce, turnstiles, HCL, resource pools | **Solaris** — [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) |
| Cap teeth, untyped, CDT, revoke DEAD/gen | **L4/seL4** mechanics |
| VM objects/views, task ports, QoS, session/compositor, match→grant drivers, JIT entitlement, futex early | **Apple** — this document |
| **Game runtime / Deck Top 50** | **Proton v1.6** — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) (**overrides Apple on conflict**; clean-room Linux ABI OK) |

---

## 0. Apple engineering instincts used here

1. **Task + threads** — process is the unit apps understand; hide untyped from normal code.  
2. **Memory objects + map views** — shared libs and file maps are first-class, not one door for all VA.  
3. **Special ports with rights** — task / exception / bootstrap; not “meta is everything.”  
4. **Request/response IPC** — ephemeral reply rights (MIG/XPC spirit).  
5. **Match → grant → confined driver** — DriverKit/IOKit personality spirit via `devmgr` + UDX.  
6. **QoS for desktop feel** — interactive and audio paths are product requirements.  
7. **Entitlements, not holes** — W^X default; JIT only with a cap.  
8. **Session TCB** — compositor and seat are trusted; apps don’t get raw HID/GPU.  
9. **Ship an HCL** — general-purpose desktop means supported tiers, not infinite hardware.

---

## 1. Virtual memory — regions + memory objects (P0)

### Decision: **Mach-shaped map; not a single process-wide pager only**

```text
process
  └── space (one primary AS for v1)
        └── regions [base, len, prot, flags]
              └── memory_object (anon | file | device | phys)
                    ├── pager endpoint (optional)
                    └── resident pages (object-owned)
```

| Rule | Detail |
|------|--------|
| **V1** | Each region references a **memory object**; fault resolves **region → object → pager** |
| **Default pager (PCB)** | Fallback only for regions that did not set an object pager — **not** the sole mechanism |
| **Anon** | Anon memory object + default/anon pager (zero-fill); ordinary `malloc` never opens a door |
| **File maps** | `map_file` / vfs installs region + **file memory object** (pager = `vfsd` or helper) |
| **Untrusted apps** | Cannot register arbitrary third-party pagers for arbitrary VA in v1 |
| **No pager / unknown region** | **Kill** faulting thread (fail closed) |
| **Cookie / cluster / fault lock** | Unchanged security: kernel-only cookie, multi-page cluster, one fault lock per space — [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) §7, [CAP_ADDRESSING.md](CAP_ADDRESSING.md) |

### Fault path (updated)

```
fault at VA in space S
  → acquire S.fault_lock
  → lookup region containing VA; if none → kill; unlock
  → object = region.memory_object; pager = object.pager || process.default_pager
  → if no pager → kill; unlock
  → cluster within region (+ coalesce cap); kernel cookie bound to {S, object, cluster, …}
  → Call pager (mono timeout) with fault_msg + cookie
  → validate cookie; install PTEs as **views** of object pages; charge object/ledger
  → invalidate cookie; resume or kill; unlock
```

*Apple voice:* WindowServer and dyld-shaped sharing need **objects**, not one process door.

---

## 2. FRAME / page ownership after map (P0)

### Decision: **Memory object owns physical pages; maps are views**

Supersedes the strict reading of “client always owns FRAME as a CNode object after every map” for **shared** and **file** cases. Cookie security rules stay.

| Case | Physical page owner | Client gets |
|------|---------------------|-------------|
| Anon private | Memory object of that process (quota/ledger) | PTE view; optional FRAME cap only if policy asks |
| File / shared | File (or shared) memory object | PTE view; many processes may map same object |
| Device / export | Object under devmgr/driver policy | View rights only |

| Rule | Detail |
|------|--------|
| **O1** | Kernel **only** installs PTEs after cookie + FRAME validation (pager never ambient-maps client). |
| **O2** | Most **desktop** apps do not manage per-page FRAMEs — libc/libvm hide this. **Proton tasks** create/map/**share named memory objects** (wine-server, GPU export) — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) §4. |
| **O3** | Space death / unmap drops **views**; object refcount; free pages when object dies. |
| **O4** | Shared libraries: one file memory object, many maps; **COW** on write planned (v1 may private-copy if needed to ship). |
| **O5** | Pager death: object dead → further faults fail closed; resident pages policy v1 = unmap/teardown in order with §1.1. |

*Apple voice:* “Client owns every FRAME in its CNode” fights shared cache and compositor buffers.

---

## 3. `GJ_CAP_PROCESS` — task port (P0)

### Decision: **Kernel mints PROCESS (task) into parent on spawn; rights matrix**

Not derived from child’s root meta (root meta remains kernel-ops-only).

| Right (bit / name) | Meaning |
|--------------------|---------|
| `IDENTIFY` | Type / stable id |
| `KILL` | Terminate task |
| `WAIT` | Reap exit (or Notification) |
| `VM` | Default pager, region create (narrow) |
| `SPAWN` | Optional: further spawn under this ledger slice |
| `INSPECT` | Debug registers — **off by default** |
| `GRANT` | Transfer task port (rare) |

### `set_pager` / VM authority matrix

| Caller | Allowed? |
|--------|----------|
| PROCESS cap with **VM** on that process | Yes |
| Restricted **self** port with VM (own default pager only) | Yes |
| Peer with only an IPC endpoint | **No** |
| `init` / bootstrap holding platform task ports | Yes |

Child does **not** receive a full self-control task port by default; restricted self for common ops only.

---

## 4. Reply caps (P0)

### Decision: **Kernel ephemeral single-use REPLY** (MIG/XPC shape)

```
Call:
  kernel creates REPLY object
  delivers single-use reply right to server (message / reply slot)
  client blocks
Reply:
  one success → REPLY DEAD, client resumed
  second reply → STALE
  server death → client PEER_DEAD
  timeout → REPLY dead, client TIMEOUT
```

| Rule | Detail |
|------|--------|
| Userspace cannot mint REPLY | Kernel-only |
| No “pass reply to helper” in v1 | Security first |
| Nested Call depth | ≤ 8; each level own reply |
| Message | Copy-in: tag + regs[4] + optional ≤ 256 B; no durable shared kernel msg buffer |

---

## 5. Syscall ABI freeze (P0)

### Decision: **Single normative table in `kernel/include/gj/syscall.h` when created; docs must match**

Sketch (numbers stable once userland exists — **do not renumber**):

| Num | Name | Notes |
|-----|------|--------|
| 0 | `debug_log` | ptr, len (gated later) |
| 1 | `yield` | — |
| 2 | `exit` | code |
| 10 | `ipc_call` | ep_slot, ep_gen, msg*, mono deadline |
| 11 | `ipc_recv` | ep_slot, ep_gen, msg*, mono deadline |
| 12 | `ipc_reply` | reply_slot, reply_gen, msg* |
| 20 | `cap_mint` | … |
| 21 | `cap_move` | … |
| 22 | `cap_copy` | … |
| 23 | `cap_revoke` | … |
| 24 | `cap_identify` | … |
| 30 | `untyped_retype` | … |
| 40 | `vm_map` / region map | FRAME or object view |
| 41 | `vm_unmap` | … |
| 42 | `vm_protect` | W^X rules |
| 43 | `vm_map_memory_object` | file/shared object |
| 50 | `process_set_pager` | default pager (PCB); needs PROCESS VM or self |
| 51 | `process_spawn` | entry, stack, cap_bundle, quota slice |
| 52 | `process_kill` / `process_wait` | via PROCESS cap |
| 60 | `wait_timeout` | mono deadline |
| 70 | `futex_wait` | addr, expected, mono deadline |
| 71 | `futex_wake` | addr, count |
| 72 | `thread_set_qos` | or sched_context ops |

Supersedes incomplete / drifting tables in older IMPLEMENTATION sketches.

---

## 6. Device + IRQ bootstrap (P1)

### Decision: **Platform graph → match → grant → UDX host** (DriverKit / IOKit spirit)

```
UEFI/ACPI/PCI enumerate (kernel early and/or platform with cfg cap)
  → devmgr device graph (nubs)
  → match (UDX id tables / personalities)
  → spawn driver host with MMIO windows, IRQ→Notification binds, DMA slice
  → UDX probe / remove / quiesce
```

| Rule | Detail |
|------|--------|
| MSI-X / vector bind | **Kernel** on bind path: “bind vector → my Notification + badge bit” |
| Driver does not freely own all cfg | Only granted cfg/MMIO caps |
| Storm | Kernel mask + notify devmgr (security core) |
| No IOMMU production | No bus-master |

---

## 7. Quotas — task ledgers (P1)

### Decision: **Per-process ledger; spawn takes a slice**

| Resource | Ledger |
|----------|--------|
| CNode slots, threads, phys pages, endpoints, timers, IRQ binds, IPC budget | Yes |

| Rule | Detail |
|------|--------|
| No create without ledger room | `GJ_ERR_QUOTA` |
| Spawn | Carve slice from parent remaining; cannot exceed parent |
| v1 soft jetsam | Optional later; hard fail is OK |
| Desktop split | `init` partitions drivers / gui / app_pool |
| User-visible QUOTA cap | Optional later; kernel-internal ledger sufficient for v1 |

---

## 8. Scheduler — QoS classes (P1)

### Decision: **Thread QoS + capped PI** (desktop product)

| QoS | Use |
|-----|-----|
| `USER_INTERACTIVE` | Input, compositor hot path |
| `USER_INITIATED` | Foreground app work |
| `UTILITY` | Downloads, index |
| `BACKGROUND` | Maintenance |
| `DRIVER` / soft-RT | UDX hosts, audio — **quota-backed** |

| PI | Along Call chain; boost **capped** by server max QoS and budget — no unbounded inversion exploit |

Solaris turnstile mechanism remains; QoS is the **product policy** on top.

---

## 9. Futex (P0 with Proton — ship early)

### Decision: **`futex_wait` / `futex_wake` in M2/M3; cross-process with objects is P0**

| Rule | Detail |
|------|--------|
| Timeout | Mono deadline required (infinite only with privilege) |
| Same-process | Required for pthread |
| **Cross-process** | **P0** on shared memory objects — key `(object_id, offset)`; Proton **overrides** “same-process first only” |
| Purpose | pthread + **wine-server**; see [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) §8 |

---

## 10. W^X + JIT entitlement (P1)

### Decision: **Hard W^X default; JIT only with cap**

| Default | No simultaneous WRITE\|EXEC on user maps |
|---------|------------------------------------------|
| `vm_protect` | RW ↔ RX allowed; RWX only with **`CapJit` / MAP_JIT`-style** right |
| Who gets JIT | Browser / language runtime / **Proton-class game tasks** via launcher policy — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) |
| Codesign | Later; not required for first desktop bring-up |

---

## 11. Session / compositor (P2)

### Decision: **Single-seat session TCB; apps sandboxed**

```
session leader (trusted)
  → seat: input, display, clipboard broker
  → compositor (high trust)
  → apps: surface endpoints + filtered input — not raw HID / not raw GPU MMIO
```

v1: single-user, single-seat. Multi-user later.

---

## 12. Exception ports (P1 for Proton, P2 desktop default)

### Decision: **Separate from pager** (Mach exception port spirit)

| Event | Path |
|-------|------|
| Not-present / prot fault | VM region → object → pager |
| #GP, breakpoint, etc. | Exception endpoint on PCB if set; else kill thread |
| Double fault / no stack | Kill process; no infinite recurse |

| Audience | Policy |
|----------|--------|
| **Proton game tasks** | Launcher **always** registers exception sink; libprotonrt maps to SEH/signal-shape — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) §14 (**P0–P1**, not deferred P2) |
| Other desktop apps | Optional; kill if unset (Apple default) |

Debug requires PROCESS **INSPECT** + exception port — not ambient ptrace.

---

## 13. Bootstrap seal (P1)

### Decision: **One-way seal checklist**

1. After critical servers up: **seal** privileged retype (IOMMU authority, IRQ bind-any, root untyped abuse).  
2. Only `devmgr` holds IOMMU / broad IRQ bind.  
3. Each task gets **sticky bootstrap** specials (ns / restricted self) — **not** root meta as factory.  
4. Seal is a **one-way** kernel flag.

---

## 14. Name service restart (P2)

### Decision: **Sticky bootstrap + supervised restart**

- Spawn installs sticky bootstrap endpoint/Notification from parent/`init`.  
- `PEER_DEAD` → retry resolve with mono backoff.  
- Critical servers supervised; new caps pushed — prefer session alive over purity cosplay.

---

## 15. Firmware blobs (P2)

### Decision: **Out of dual-license C claim**

- Blobs in `firmware/` or out-of-tree with **separate license files**.  
- Load via policy service + **hash allowlist**.  
- Kernel does not parse blob formats beyond load-to-window.

---

## 16. UDX (confirmed)

### Decision: **Keep best combo; Apple adds matching discipline**

| Layer | Choice |
|-------|--------|
| Names | Linux-shaped for porters |
| Lifecycle | DDI `attach` / `detach` / **`quiesce`** |
| Authority | Caps hidden |
| Discovery | **devmgr match**, not every driver walks full PCI |
| Bring-up | Host inject until kernel wired |

See [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md).

---

## 17. Explicit rejects (Apple channel)

| Reject | Why |
|--------|-----|
| Full `fork` + inherit all caps | Ambient authority |
| Full POSIX signals in kernel | Notification + exception ports |
| Pager with permanent map-any on client | Confused deputy |
| In-kernel GPU/USB class stacks for “speed” | TCB |
| Apps managing CNode slots per page | Ergonomics fail |
| Delay futex/QoS until “all servers done” | Dead desktop feel |
| Infinite HCL day one | Support hell |

---

## 18. Implementation order (product feel early)

1. PROCESS task port + thread + space + CNode + spawn/kill/wait  
2. Syscall freeze + IPC Call/Reply + futex  
3. VM regions + anon object + default pager + cookie map path  
4. File memory objects / shared maps  
5. devmgr match + IRQ→Notification + virtio UDX host  
6. QoS + PI when multi-server latency shows  
7. Session + compositor on GOP/virtio-gpu  
8. UEFI / 1 TiB PMM / SMP in parallel once UP paths work  

**Interactive proof:** keyboard → compositor → pixel (not max syscall count).

---

## 19. Supersession map

| Earlier text | Now |
|--------------|-----|
| Single PCB pager as only fault path | **Default pager** + **per-region object pager** |
| “Client owns FRAME” as sole model | **Object owns pages; maps are views** (cookie rules kept) |
| IMPLEMENTATION incomplete syscall list | **This §5** + future `syscall.h` |
| PROCESS only prose | **§3** typed task port + rights |
| Futex “later” | **§9** early |

Solaris remaining doc stays authoritative for untyped ladder, CDT, CNode lock order, HCL tiers, nested CNode v1 ban, DDI DMA windows.

---

*Accepted Apple-channel decisions v1.1 — task ports, VM objects/views, QoS, session, match→grant. **Proton personality overrides this doc on conflict.***
