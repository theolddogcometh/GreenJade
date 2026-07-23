# GreenJade — Coding TODO

Track implementation. Check boxes as you go.  
Policy: pure C, **MIT OR Apache-2.0**, no GPL, no OOP.  
**Priorities: 1. Security → 2. Performance → 3. Portability → 4. Readability** (**do not reorder for games**)  
**Product: desktop · UEFI · ≥1 TiB RAM · SMP · SAS/SCSI · adoption = Deck Top 50 (Proton, clean-room Linux ABI)**  
**Critical path:** [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) §12 · arch §0.5.3 · [PROTON](PROTON_PERSONALITY.md)  
**Design:** complete freeze — implement, do not re-litigate architecture  
Specs: [Architecture](GREENJADE_KERNEL_SPEC.md) · [Design complete](DESIGN_SPEC_COMPLETE.md) · [Security](SECURITY_CORE_DESIGN.md) · [Proton](PROTON_PERSONALITY.md) · [Hybrid ABI](LINUX_ABI_HYBRID.md)

---

## Legend

| Tag | Meaning |
|-----|---------|
| **M0** | Boot + print (Multiboot2 interim) |
| **M1** | Memory + threads + syscalls stub |
| **M2** | Capabilities + IPC + SMP design |
| **M3** | Userspace + virtio + UEFI path |
| **M4** | Large RAM PMM + HHDM + multi-CPU online |
| **M5** | scsi_mid + virtio-scsi / SAS |
| **M6** | Desktop input/GPU/compositor path |
| **P** | Process / policy |

---

## P0 — Project hygiene (do first)

- [x] Root `LICENSE` is **MIT OR Apache-2.0** dual
- [x] `STYLE.md` followed for all new C
- [x] Every source file: `SPDX-License-Identifier: MIT OR Apache-2.0`
- [x] `third_party/PROVENANCE.md` exists (empty OK)
- [x] `scripts/check-license.sh` rejects GPL strings in tree
- [x] README points at docs + product bar

---

## M0 — Bootable kernel (start here)

**Goal:** QEMU boots GreenJade; serial prints banner; stable idle.

### M0.1 Scaffold
- [x] Top-level `Makefile` (`all`, `run`, `clean`)
- [x] `kernel/arch/x86_64/linker.ld`
- [x] `kernel/arch/x86_64/boot.S` Multiboot2 header + jump to `kmain`
- [x] `kernel/include/gj/types.h`, `error.h`, `config.h`
- [x] `kernel/main.c` → `kmain`

### M0.2 Early I/O
- [x] `kernel/arch/x86_64/serial.c` — 16550 COM1 init + putchar
- [x] `kernel/lib/string.c` — `memset`, `memcpy`, `strlen`, `memcmp`
- [x] `kernel/lib/stdio_k.c` — `kprintf` (`%s` `%c` `%d` `%u` `%x` `%p` minimum)
- [x] Banner: `GreenJade M0` + `M0 OK` (in source; verify on QEMU)

### M0.3 Boot info
- [x] Parse Multiboot2 info pointer in `kmain`
- [x] Walk tags; find memory map
- [x] Log usable RAM regions (kprintf)

### M0.4 Physical memory
- [x] `kernel/mm/pmm.c` — freelist PMM + `pmm_release_high` after HHDM
- [x] Mark kernel image frames used
- [x] Test: alloc N pages, free, realloc; log free count
- [x] `pmm_alloc_pages` contiguous helper
- [x] HHDM at `GJ_HHDM_BASE` (2 MiB pages)

### M0.5 Traps (minimal) — **design freeze G-IDT-*** 
- [x] GDT user/kernel segs + reload (`gdt_init`) — boot GDT interim only
- [x] IDT + 32 exception stubs (`idt.c`, `isr_stubs.S`, `trap.c`)
- [x] On fault: dump vector/RIP/CR2; kill user thread / halt kernel
- [x] Supervisor-only kernel maps sweep + **SMEP** (`smep_maps.c`)
- [x] SMAP + STAC/CLAC in copy_from/to_user

### M0.6 Run target
- [x] `scripts/run-qemu.sh` — serial to stdio, appropriate memory
- [x] `make run` documented in README
- [x] **DoD:** boot verified on QEMU (`M0 OK` on serial) — Multiboot + OVMF smoke (`make smoke` / `scripts/smoke-all.sh`)

---

## M1 — Kernel services core

**Goal:** pages, heap, threads, timer tick, debug syscall.  
Design all structures for **SMP** and **>1 TiB** even if M1 tests are smaller.

### M1.1 Kernel heap
- [x] `kheap_alloc` / `kheap_free` (freelist over PMM pages)
- [x] Smoke alloc in `kmain`

### M1.2 Paging helpers — **design freeze G-AS / G-MO**
- [x] Map/unmap/protect 4K (`vmm.c`)
- [x] Per-process CR3 clone + COW on map (G-AS-*) — `vmm_as_create`, `process_as_*`
- [x] Memory objects own frames; vmm installs PTEs — `memobj.c` + mmap path
- [x] W^X + CapJit at Linux ABI layer
- [ ] Optional: higher-half kernel move / empty user half without identity share

### M1.3 Interrupts + timer
- [x] PIC remap + PIT 100 Hz IRQ0 (`timer.c`)
- [x] Periodic tick; `jiffies` / `timer_mono_nsec`
- [x] EOI on timer path
- [x] Local xAPIC enable + periodic timer vector 48 (PIT remains mono clock)
- [ ] x2APIC + full calibration / replace PIT for mono
- [x] QEMU Multiboot2 via GRUB ISO (`scripts/run-qemu.sh`) — verified on qemu-kvm

### M1.4 Threads + RR sched
- [x] `gj_thread` struct (context, stack, state)
- [x] Context switch asm (`switch_context`)
- [x] `thread_create`, `thread_yield`, block/wake, idle
- [x] Personality server thread + doors (proof of multi-thread)
- [x] Idle HLT when nothing runnable
- [x] Timer preemption / true RR quantum (`timer: preemption quantum PASS`)

### M1.5 Syscall entry
- [x] Hybrid **Option C** — [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) v1.4
- [x] **Pointer validation** `copy_from/to_user` + user VA window
- [x] **CapJit / W^X** on process (`gj_process_set_jit`) + `GJ_RIGHT_JIT` on spawn mint
- [x] **Linux-true futex** shared = physical address of word
- [x] **Blocking cold_ipc** + libprotonrt service
- [x] **Ring-3** trampoline + SYSCALL stack switch + `sysret` (`user_task_*`)
- [x] **Per-CPU** + **swapgs** + kernel stacks (`cpu_init_bsp`, `syscall_entry.S`)
- [x] **Scheduler** threads block/wake/yield (`thread.c`, `switch.S`)
- [x] **Sleeping futex** + **doors cold path** (kernel personality thread = bring-up)
- [x] `process.u64Cr3` field + switch hook
- [x] Per-process page-table clone (true multi-AS) — **G-AS-***
- [x] Userspace personality **ring-3 door server** + per-thr kstack + TSS — **G-PERS-*** (PERSONALITY_SERVE still kernel policy interim)
- [x] Doors as ENDPOINT install in CNode — **G-DOOR-*** (CDT/quota later)
- [x] CapJit cache + PROCESS `GJ_RIGHT_JIT` on spawn — **G-JIT-*** (migrate checks fully)
- [x] Idle HLT + PIT timer + futex timeouts — **G-SCHED / G-FUT** (APIC later)
- [x] AP online + per-CPU schedule + IPI resched / TLB / x2APIC — **G-LOCK-*** product lock audit remains open
- [x] IDT gate — **G-IDT-***
- [x] Native `GJ_SYS_*` stubs + `GJ_SYS_PROCESS_SPAWN` + cold dequeue/reply

### M1.6 First user process (stretch inside M1 or early M2)
- [x] Load embedded or Multiboot module user ELF (`init.elf` embed + live server embeds)
- [x] User page tables + ring 3 entry (trampoline)
- [x] User `init` prints via `SYS_DEBUG_LOG` and `SYS_EXIT` (freestanding `greenjade-init` / serial banner)

---

## M2 — Capabilities + IPC + quotas + time

**Goal:** implement [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) §§1–5.  
Caps, hierarchical quotas, sync IPC with timeouts, mono clock.

**Progress note (2026-07-23 — substantial incomplete soft product, not closed):**  
Exclusive deepen shipped greppable soft product surfaces for the open list — **none close product** / bar3:

| Track | Soft greps (shipped) | Still open (product) |
|-------|----------------------|----------------------|
| Fault → pager + views | `fault: pager call soft` / `fault: cookie view map soft` / kill-timeout | Real door Call + FRAME map under CR3 |
| Ephemeral REPLY / transfer | `door: reply single-use soft PASS` / badge transfer counters | CNode `GJ_CAP_REPLY` MIG product |
| CDT R2 try-lock revoke | `cap: revoke try-lock` / `cap:cdt R2 soft` | Real CNode mutex/turnstile |
| CDT edges everywhere | `cap: cdt mint\|copy\|move` coverage | No empty-edge product audit |
| Bootstrap seal §13 | `process: bootstrap seal soft` / death tallies | Full retype / seal checklist product |
| Higher-half move | `vmm: higher-half soft OPEN` | Kernel image relocate |
| x2APIC mono vs PIT | `timer: apic mono preferred PASS` | Full x2APIC ICR/timer replace |
| IOMMU no open bus-master | `iommu: no open bus-master soft PASS` | Production default always-on HW |
| IOMMU window caps | `iommu: window cap soft` | Cap-typed window object |
| UDX Notification | `udx: notify soft` | Kernel notify from host sim |
| SPSC driver host | `udx: spsc soft` | Multi-process driver-host |
| Multi-server confine | `confine: expose soft` / ledger / death soft | Product seal + vfsd expose |

### M2.1 Capability tables
- [x] CNode allocate/slots (soft slot-quota ledger; hierarchical **task ledger** still soft) — `gj_cap_alloc_install` + `gj_cap_quota_*`
- [ ] cap types: CNODE, THREAD, SPACE, **PROCESS (task)**, ENDPOINT, NOTIFICATION, FRAME, REPLY, … (enum + PROCESS/ENDPOINT/NOTIFICATION/ROOT_META/MEMORY_OBJECT live; REPLY/FRAME/SPACE user path still open)
- [x] mint (weaker only), move, copy, delete — `gj_cap_mint/copy/move/delete` + native `GJ_SYS_CAP_*`; greppable `cap: mint/copy/move+cdt PASS`
- [x] **§1.1 policy + stubs**: DEAD/gen first, mandatory deferred slot invalidate — `cap.h` / `revoke.c`
- [x] **x86_64 addressing Scheme A**: `u64` slot + `u32` gen; slot 0 root meta — `cnode.c` + [CAP_ADDRESSING.md](CAP_ADDRESSING.md)
- [x] **Apple channel Accepted**: regions/objects, task ports, QoS, futex, session — [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md)
- [ ] Wire CDT + CNode try-lock slot walk into `gj_revoke_process_deferred` (soft CDT walk PASS + R2 soft try-lock/retry; product mutex still open)
- [x] Process + shared CNode + root meta + default pager PCB — `process.h` / `process.c` (+ smoke in `kmain`)
- [x] Root meta = **kernel ops only** (no mint PROCESS/CNODE from slot 0) — CAP_ADDRESSING v1.6
- [x] `GJ_CAP_PROCESS` mint to parent on spawn; rights matrix (KILL/WAIT/VM/…) — `spawn.c`
- [x] Wire process into threads via `thread_create(pProc, …)`; boot process + spawn
- [x] Map cookie + multi-page cluster + per-space fault lock policy — `fault.h` / `fault.c`
- [x] VM **regions + memory objects** (anon); object pager + PCB default pager fallback later
- [ ] `set_pager` kernel ref on ep; clear PCB on ep revoke (`gj_process_set_pager` + death clear exist; full ep-revoke path open)
- [ ] Fault: region lookup → Call pager + cookie + map **views** (object owns pages)
- [ ] Kernel ephemeral **REPLY** on Call; single-use
- [ ] Cluster coalesce adjacent not-present pages (cap GJ_FAULT_CLUSTER_MAX)
- [ ] Freeze `syscall.h` per Apple §5 (incl. futex, vm_*, process_*)
- [ ] Syscalls using `gj_cap_resolve` on all cap ops (native CAP_* ops resolve; not universal yet)
- [x] Drive `gj_revoke_process_deferred` from timer tick (R7) — `timer.c` + A2 partial already; idle/syscall-exit hooks optional

### M2.2 Quotas
- [ ] quota_account hierarchy (struct soft `pParent` shape; charge roll-up not product-complete)
- [x] charge on create; refund on destroy — soft slot charge/refund on install/delete/move (`gj_cap_quota_slot_*`)
- [x] `GJ_ERR_QUOTA` on exhaustion — greppable `cap: quota exhaust PASS`

### M2.3 Address space caps
- [ ] User `space_map` / `unmap` via FRAME caps
- [x] Activate AS on schedule — `process_as_activate` in `thread.c` schedule path
- [ ] death unmaps grants

### M2.4 Endpoints (sync IPC)
- [x] Endpoint + wait queues (doors single-flight + server/client/slot tags) — product path is doors, not generic seL4-style IPC yet
- [x] `door_call` / `door_recv` / `door_reply` + **mono timeout** — `door_call_timeout`; greppable `door: timeout/peer path PASS`
- [ ] cap transfer (small K); single-use reply (server-set badges: `door: badge transfer PASS`; full transfer/REPLY open)
- [x] peer death ⇒ client error — greppable `door: mid-call peer death PASS` (−EIO / G-DOOR-4)
- [x] no implicit foreign memory maps (doors pass message regs only)

### M2.5 Notifications + time
- [x] Notification (badge bitmask) — `notify.c` + MSI-X IRQ bind (`notify: MSI-X IRQ PASS`)
- [ ] mono clock; timer objects **quota-backed** (mono clock exists; quota-backed timer objects open)
- [ ] infinite wait only with privilege cap
- [x] `futex_wait` / `futex_wake` (mono timeout) — Apple §9; `futex.c` + PE32/Linux paths
- [x] Cross-process futex on shared memory objects — Proton **A0** (`winesrv` PA keys; M6 A0 checked)
- [x] Named shareable memory objects (wine-server + GPU export) — Proton **A0** (`memobj_create_named` / map)

### M2.7 QoS + session (design Accepted; code later)
- [ ] Thread QoS classes + capped PI — Apple §8 (soft QoS rank in sched; capped PI open)
- [x] W^X + JIT cap — Apple §10 (CapJit / `GJ_RIGHT_JIT` / mprotect gates; M1 checked)
- [x] Exception port on PCB (≠ pager) — Apple §12 (`except_port_*`; `except: port smoke PASS`)
- [ ] Bootstrap seal checklist — Apple §13

### M2.6 Tests
- [ ] IPC ping-pong 100k
- [x] kill server mid-call → client error — `door: mid-call peer death PASS`
- [x] quota exhaustion error — `cap: quota exhaust PASS`
- [x] timeout on blocked call — `door: timeout/peer path PASS`

---

## M3 — Userspace system skeleton + virtio

**Goal:** `init` + one real device path without GPL.

### M3.1 Userland build
- [x] `user/libgj` syscall stubs (`gj_syscall6` + Linux/native wrappers)
- [x] `user/init` — first process scaffold (`make userland` → `build/user/init.elf`)
- [x] ELF64 ET_EXEC loader + embed `init.elf` → process AS + user thr (serial banner)
- [ ] Multiboot module / initrd pack (optional; embed works for bring-up)

### M3.2 Supervisor
- [ ] `init` owns root CNode; hands caps to children
- [ ] Spawn second process (e.g. `echo` server)

### M3.3 Virtio MMIO (QEMU virt)
- [x] Discover PCI virtio on q35 (`virtio_pci_scan`) — net/gpu/input present in QEMU
- [x] virtio-gpu modern PCI + control virtqueue + 2D present (create/attach/scanout/flush)
- [ ] Prefer **virtio-mmio** on `virt` machine (optional) in addition to PCI
- [x] Clean-room PCI config scan from OASIS IDs (original C) — bind/rings next

### M3.4 virtio-blk + SPSC rings
- [x] Kernel virtio-blk modern PCI + request q0 (sector R/W) — QEMU 16 MiB disk
- [x] Read/write sector 0 smoke + magic verify
- [x] Interim `vfs_ram` mount `/dev/vda` over virtio-blk
- [ ] Userspace driver host process
- [ ] **SPSC ring** ownership protocol (security core §6)
- [x] **No** Linux virtio source (kernel path clean-room OASIS)
- [ ] hard IRQ → Notification only; batch in userspace (security core §7)

### M3.5 Simple FS (optional same milestone)
- [x] Interim ramfs + `/dev/vda` blk mount in-kernel
- [ ] Read-only tarfs or flat ramfs in userspace over blk

---

## M4 — Security shape + multi-server

**Goal:** OpenBSD-inspired confinement on top of caps.

- [ ] `confine(promises)` implemented in userspace runtime + kernel checks where needed
- [ ] `expose` path policy in `vfsd` (when VFS exists)
- [ ] Split `driver-host` process per device class
- [ ] `logd` / `auditd` stubs
- [ ] Drop ambient authority from app processes

### M4.1 VFS / net (minimal)
- [x] Interim `vfs_ram` open/read/write for cold path (product: userspace `vfsd`)
- [x] `vfsd` — freestanding live spawn + door mount/open/read/write (`vfsd: live spawn PASS`; full multi-server confine still open)
- [x] virtio-net modern TX (+ RX buffer post) — full netstackd later
- [x] `netstackd` — live spawn + virtio TX/user ring (`netstackd: live spawn PASS`; product stack depth still open)
- [x] `sshd` — freestanding live embed default on :22 (`sshd: live spawn PASS`; host smoke `make sshd`)

### M4.2 UDX runtime v0
- [x] Linux-shaped probe/remove, irq, dma, mmio, work — `user/udx/`
- [x] DDI-style quiesce + host inject (`udx/host.h`)
- [x] Porter contract [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md); `make udx` / `make udx-example`
- [ ] Wire UDX to real Notification + IOMMU window caps (needs M2/M3)
- [x] `scsi_mid` live embed + `GJ_SYS_SCSI` (`scsi_mid: live spawn PASS`; `udx_scsi_*` / SAS HBA host still open)

---

## M5 — Hardening + appliance profile

- [ ] Stack protector / fortify-style flags as feasible freestanding
- [ ] KASLR (basic)
- [ ] IOMMU story documented; enable when HW/QEMU allows
- [ ] Signed driver bundles (design + stub)
- [ ] Reproducible `make dist` effort
- [ ] Fuzz IPC decoder

---

## M4 — Large RAM + SMP (product bar)

**Goal:** machine with **>1 TiB RAM** and **multiple CPUs** is a supported class.

### M4.1 PMM rewrite
- [x] Freelist PMM (not flat bitmap); dual low/high freelists (prefer low)
- [x] Hierarchical order freelists (order-N blocks + split; multi-TiB path)
- [x] HHDM TiB cap fix (`<< 40`); high MMIO BAR map; virtio/kheap via HHDM
- [x] QEMU `GJ_MEM=4G` + HHDM + high RAM free path → `M0 OK`
- [x] PMM soak (alloc/free + contiguous) multi-TiB freelist (`pmm: soak PASS`)
- [x] Hierarchical free / large-RAM soak — **768 GiB soak PASS on host**
- [ ] **True 1 TiB soak** when host allows (QEMU `-m 1280G` / ≥1 TiB host — still optional)

### M4.2 SMP
- [x] Parse MADT from Multiboot2 ACPI RSDP + BIOS RSDP scan (local APIC + x2APIC entries)
- [x] AP trampoline (real→prot→long) + INIT/SIPI; per-CPU park stacks (static pool)
- [x] QEMU `GJ_SMP=2 make run` → `online=2` + `M0 OK` (APs HLT-park)
- [x] AP local APIC timer + GDT/IDT load; BSP-only mono (AP timer_ticks≥1)
- [x] Shrink static percpu pool (`GJ_CPU_STATIC_MAX=16`; design ceiling 256)
- [x] IPI resched vector + `smp_ap_run()` one-shot AP work (multi-CPU proof)
- [x] External thread affinity + `thread_create_on_cpu` (AP filter ready)
- [x] x2APIC CPUID detect (`x2apic_probe`)
- [x] Per-CPU idle + full `schedule` on AP (`scheduler_run_ap` / `AP schedule ready`)
- [x] TLB shootdown IPI (vec 50) on map/unmap/protect (`tlb_shootdown`)
- [x] x2APIC mode enable (per-CPU after SMP; MSR ICR/EOI/timer)
- [ ] PMM-backed percpu growth

### M4.3 UEFI
- [x] `make uefi-stub` + `gj_boot_info` handoff shape
- [x] Product PE32+ `build/GreenJade.efi` (PIC shared → pei-x86-64; ms_abi)
- [x] ExitBootServices + SimpleFileSystem `EFI/GREENJADE/KERNEL.ELF` load
- [x] QEMU OVMF boot: GPT ESP + `BOOTX64.EFI` (`make ovmf` / `scripts/run-ovmf.sh`)
- [x] Unified `gj_boot_info`; Multiboot fills; UEFI → `kmain_uefi` + identity 4 GiB
- [x] `make smoke` — Multiboot + OVMF `M0 OK (UEFI)` + license
- [x] Optional GOP early FB handoff in UEFI stub (`u64FbBase` / pitch / bpp)

---

## M5 — SCSI / SAS (product bar)

- [x] Interim kernel `scsi_mid` CDB builders + submit
- [x] virtio-scsi modern PCI (ctrl/event/req) + INQUIRY / READ_CAPACITY / READ10 (QEMU)
- [x] Userspace `scsi_mid` CDB library skeleton (`user/scsi_mid/`)
- [x] Kernel `scsi_door` submit path (userspace mid → virtio-scsi)
- [x] **Live embed** `scsi_mid.elf` boot spawn (`scsi_mid: live spawn PASS`)
- [ ] Full door ENDPOINT + HBA host death revoke
- [ ] SAS/SCSI HBA driver host (clean-room; pick documented family first)
- [ ] Block path: scsi → storaged → FS
- [ ] Driver host death revokes DMA/MMIO/IRQ

---

## M6 — Desktop userspace

- [x] Input: virtio-input event queue probe + poll
- [x] Display: virtio-gpu GET_DISPLAY_INFO + 2D present (GOP/UEFI later)
- [ ] USB HID / PS/2 fallbacks
- [x] Compositor interim (kernel) + sessiond skeleton
- [x] Session input hub (virtio-input fan-in ring)
- [x] HDA PCI probe stub + software stream / CORB / BDL / multi-stream mixer soft (`hda: stream/CORB/multi-stream … PASS`)
- [ ] POSIX personality subset for apps
- [x] **Proton + Deck Top 50 design re-applied** — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) **v1.6**; arch §0.5.2–0.5.3
- [x] **A0:** wine-server two-process demo (named maps + shared futex + socketpair + CapJit)
- [x] **A0:** eventfd2 cold path
- [x] **A0–A1:** exception port register/deliver (user trap → port; SEH-shaped stub)
- [x] **A1 T0:** Vulkan-shaped ICD present + session compositor + GPU_PRESENT syscall
- [x] **A1:** swapchain create/acquire/present (2D ICD)
- [x] **A1:** Khronos-shaped ICD (`vkCreateInstance`…`vkQueuePresentKHR` + loader negotiate)
- [x] **A1:** cmd clear + renderpass/pipeline stubs + image/memory (`vk: render path PASS`)
- [x] **A1:** host ICD `.so` + JSON + multi-frame present + descriptor stubs
- [x] **A1+ partial:** fsync, madvise no-op, fcntl lock stubs, thread QoS + affinity
- [x] **A1:** SPIR-V validate/parse + graphics stages + VBO triangle software raster (`vk: spirv path PASS`)
- [x] **A1:** HDA software PCM stream ring (`hda: stream path PASS`)
- [x] **A1:** `GJ_SYS_HDA_STREAM` native syscall (open/write/start/tick/stats)
- [x] **A1:** userspace sessiond host (scanout + input fanout smoke)
- [x] **A1:** netstackd host UDP echo + storaged host sector R/W smokes
- [x] **A1 partial:** multi-attr VBO (x,y + color stride-3)
- [x] **A1 partial:** textured FS center sample + sampler (`vk: texture path PASS`)
- [x] **A1:** UV-mapped textured triangles + integer bilinear (`vk: uv path PASS`)
- [x] **A1:** session door façade + `GJ_SYS_SESSION` (`session_door: PASS`)
- [x] **A1:** `GJ_SESS_OP_PRESENT_FB` userspace framebuffer present
- [x] **A1:** net door + `GJ_SYS_NET` (`net_door: PASS`)
- [x] **A1:** store door + `GJ_SYS_STORE` (`store_door: PASS`)
- [x] **A1:** HDA CORB/RIRB software rings + BDL (`hda: CORB/BDL path PASS`)
- [x] **A1+:** real POSIX advisory file locks (fcntl F_SETLK/GETLK/SETLKW)
- [x] **A1:** store door sector R/W + CAP (`store_door: rw lba1 … PASS`)
- [x] **A1:** net door socket/bind/listen/send/recv (`net_door: socket path PASS`)
- [x] **A1:** HDA BAR0 map attempt + MMIO-shaped CORB regs (`hda: MMIO CORB path PASS`)
- [x] **A1:** PMM soak (alloc/free + contiguous) multi-TiB freelist (`pmm: soak PASS`)
- [x] **A1:** timer preemption quantum + yield request (`timer: preemption quantum PASS`)
- [x] **A1:** session CLAIM/RELEASE/MAP_SCANOUT ownership (`session_door: ownership PASS`)
- [x] **A1:** freestanding `sessiond.elf` + libgj SESSION/NET/STORE/HDA wrappers
- [x] **A1:** **live spawn sessiond.elf** (native) claim→present→release (`sessiond: live spawn PASS`)
- [x] **A1:** **live spawn netstackd.elf** + storaged.elf (`netstackd/storaged: live spawn PASS`)
- [x] **A1:** net/store door CLAIM/RELEASE ownership gates
- [x] **A1:** HDA software CORB DMA engine (`hda: CORB DMA engine PASS`)
- [x] **A1+:** Linux poll + ioctl (TIOCGWINSZ/FIONREAD) cold paths
- [x] **A1:** net door virtio TX/RX/QUEUE_INFO (`net_door: virtio queue PASS`)
- [x] **A1:** store door multi-sector + FLUSH + QUEUE_INFO (`store_door: virtio queue PASS`)
- [x] **A1:** live netstackd uses virtio TX via door
- [x] **A1:** virtio ring EXPORT/MAP/KICK into user VA (`net_door: ring map PASS`)
- [x] **A1:** HDA BDL kick path (`hda: BDL kick PASS`)
- [x] **A1:** `GJ_SYS_PLATFORM_INFO` (IOMMU + MSI-X inventory)
- [x] **A1:** UDX ring programming without kernel TX helper (`net_door: user ring PASS`, `netstackd-gj: user ring PASS`)
- [x] **A1:** HDA hardware CORB DMA on real BAR (`hda: HW CORB path PASS`; QEMU intel-hda)
- [x] **A2 partial:** WoW64 NR map + personality hooks (`wow64: path PASS`)
- [x] **A2 partial:** `clock_getres` hot path (`linux: clock_getres PASS`)
- [x] **A2 partial:** PE32 header parse for WoW64 (`pe32: parse PASS`)
- [x] **A2 partial:** PE32 section map/stage (`pe32: section map PASS`)
- [x] **A2 partial:** PE32 map into user AS (`pe32: user map PASS`)
- [x] **A2 partial:** PE32+ process load + user thread spawn (`pe32: spawn PASS`)
- [x] **A2 partial:** pread64/pwrite64 hot + getdents64 listing
- [x] **A2 partial:** VT-d soft-only path when no DRHD (`iommu: vtd soft-only … PASS`)
- [x] **A2 partial:** PE32 i386 stage + 64-bit WoW64 trampoline (`pe32: wow64 trampoline PASS`; true compat CS deferred)
- [x] **A2 partial:** Linux dup/dup2, readlink, wait4 (ECHILD), kill self cold paths
- [x] **A2 partial:** uname/getuid/access/fstat/prlimit64 + GDT CS32 selector (`pe32: wow64 cs32 selector PASS`)
- [x] **A2 partial:** wait4 reaper for PE/user exit (`pe32: wait4 reap PASS`)
- [x] **A2 partial:** VT-d identity window grant integration (`iommu: vtd identity grant PASS`)
- [x] **A2 partial:** PE32 compat iret frame layout (`pe32: compat frame PASS`; hardware far-jump deferred)
- [x] **A2 partial:** getgroups, set_tid_address, chdir, writev, access/fstat/prlimit
- [x] **A2 partial:** aarch64 M0 scaffold (`make aarch64` when cross-gcc present)
- [x] **A2 partial:** clock_nanosleep + readv (vfs) + PE32 i386 soft-exec
- [x] **A2 partial:** VT-d TE soft-arm (`iommu: vtd TE soft-arm/path PASS`)
- [x] **A2 partial:** exception port register/deliver/take smoke
- [x] **SMP MADT/MB2 walk hardened** (size caps; no hang on corrupt tags)
- [x] **A2 partial:** G-PROC-5 as_destroy for wait-children only; sysinfo/times/getrusage/getpriority
- [x] **A2 partial:** chdir tracks getcwd; rename/ftruncate; net_lo accept
- [x] **A2 partial:** mremap; getsockopt/setsockopt; getsockname; shutdown; sendmsg/recvmsg
- [x] **A2 partial:** rt_sigaction / rt_sigprocmask / getitimer / setitimer stubs
- [x] **A2 partial:** F_DUPFD; symlink; utimensat; select; CS32 compat GDT validate (`pe32: compat gdt PASS`)
- [x] **A2 partial:** pipe2; hard link; alarm/pause; fchmod; rmdir; flock; real unlink
- [x] **A2 partial:** fallocate; sendfile; accept4; prctl; robust_list; waitid; eventfd2 PASS
- [x] **A2 partial:** real epoll (create1/ctl/wait + eventfd ready); statx; fork/vfork/clone/execve
- [x] **A2 partial:** timerfd_create/settime/read; signalfd4 + inject/read
- [x] **A2 partial:** PE32 compat enter-prep (iretq frame on stack page; HW jump deferred)
- [x] **A2 partial:** process death CNode slot clear for wait-children (`cnode_clear PASS`)
- [x] **A2 partial:** setuid/setgid/setreuid/setresuid/getresuid; setpgid/setsid; mlock family
- [x] **A2 partial:** process_vm_readv/writev (same-pid); pidfd_open/send_signal
- [x] **A2 partial:** membarrier; rseq; close_range
- [x] **A2 partial:** sched_getaffinity / sched_setaffinity
- [x] **A2 partial:** capget/capset; getcpu
- [x] **A2 partial:** sync / syncfs; PE32 soft-exec opcode expansion (jmp/ecx/edx/sysenter)
- [x] **A2 partial:** inotify_init1/add_watch/rm_watch; copy_file_range; renameat2; splice/tee
- [x] **A2 partial:** PE32 LAR hardware-ready probe (`pe32: hw ready PASS`; iretq still deferred)
- [x] **A2 partial:** gettimeofday / time / settimeofday; timer-driven `gj_revoke_process_deferred` (R7)
- [x] **A2 partial:** readlinkat; fchmodat; xattr family (ENODATA/empty list)
- [x] **A2 partial:** dup3; sendmmsg/recvmmsg (first-msg)
- [x] **A2 partial:** PE32 soft-iretq pipeline (LAR + frame + soft-exec)
- [x] **A2 partial:** statfs/fstatfs; openat2; faccessat2
- [x] **A2 partial:** poll/ppoll real readiness (eventfd/pipe/inotify/ram via vfs_ram_poll_mask)
- [x] **A2 partial:** getrlimit / setrlimit (prlimit-shaped limits)
- [x] **A2:** PE32 i386 true hardware iretq into 32-bit CS (`pe32: hw enter PASS` via int3)
- [x] **A2:** PE32 i386 `int 0x80` sys_exit from CS32 (`pe32: int80 exit PASS`)
- [x] **A2:** PE32 int80 getpid+exit round-trip (`pe32: int80 getpid+exit PASS`)
- [x] **A2:** PE32 int80 multi-NR (getuid/brk/write/set_thread_area/exit) (`pe32: int80 multi PASS`)
- [x] **A2:** PE32 int80 mmap2 real user page map + store (`pe32: int80 mmap2 PASS`)
- [x] **A2:** PE32 int80 pipe + socketcall (`pe32: int80 pipe_sock PASS`)
- [x] **A2:** PE32 int80 open/fstat64 (`pe32: int80 fstat64 PASS`)
- [x] **A2:** PE32 int80 getrandom (`pe32: int80 getrandom PASS`)
- [x] **A2:** PE32 int80 MAP_FIXED + mprotect (`pe32: int80 mmap_fixed PASS`)
- [x] **A2:** PE32 int80 clock_gettime (`pe32: int80 clock_gettime PASS`)
- [x] **A2:** PE32 int80 uname (`pe32: int80 uname PASS`)
- [x] **A2:** PE32 int80 clone (`pe32: int80 clone PASS`)
- [x] **A2:** PE32 int80 futex (`pe32: int80 futex PASS`)
- [x] **A2:** PE32 int80 TLS set_thread_area+modify_ldt (`pe32: int80 tls PASS`)
- [x] **A2:** PE32 vfs load + CS32 enter (`pe32: vfs load PASS` — stage PE on ramdisk, load_process, int80 exit)
- [x] **A2:** PE32 int80 nanosleep (`pe32: int80 nanosleep PASS`)
- [x] **A2:** PE32 int80 gettid (`pe32: int80 gettid PASS`)
- [x] **A2:** PE32 int80 time (`pe32: int80 time PASS`)
- [x] **A2:** PE32 int80 vfs open/write/lseek/read (`pe32: int80 vfs_io PASS`)
- [x] **A2:** PE32 int80 gettimeofday (`pe32: int80 gettimeofday PASS`)
- [x] **A2:** PE32 int80 getcwd+sysinfo (`pe32: int80 getcwd_sysinfo PASS`)
- [x] **A2 partial:** POSIX timer_create/settime/delete + inotify_init (`linux: posix_timer PASS`)
- [x] **A2:** PE32 int80 surface: read/open/close/time/lseek/brk/mmap2/uname/futex/set_thread_area/exit_group/clock_gettime/getuid*/pipe/socketcall/fstat64/fcntl64/getrandom/modify_ldt/clone/tls/nanosleep/gettid/vfs_io
- [x] **Install path partial:** `make stage-esp` → `build/esp` BOOTX64+KERNEL.ELF+INSTALL.txt (`stage-esp: PASS`)
- [x] **A2 partial:** preadv/pwritev; tkill/tgkill; sigaltstack; sched_get/set*; unshare/chroot/readahead
- [x] **A2 partial:** execveat; mlock2; pkey_alloc/free/mprotect stubs
- [x] **A2 partial:** futex_wait/wake (new NR); landlock/memfd_secret/cachestat stubs
- [x] **A2 partial:** io_setup/destroy/submit/getevents/cancel; epoll_create (legacy)
- [x] **A2 partial:** mount/umount2/pivot_root stubs; reboot EPERM
- [x] **A2 partial:** getsid; personality; syslog/ustat/sysfs stubs
- [x] **A2 partial:** renameat/linkat/symlinkat; clock_settime/clock_adjtime; seccomp stub
- [x] **A2 partial:** userfaultfd/bpf/keyctl/name_to_handle_at ENOSYS; io_pgetevents
- [x] **Bugfix:** `vmm_as_destroy` no longer frees PTs still shared with kernel (COW)
- [x] **Bugfix:** syscall dispatch binds `g_pLinuxProc` to calling process (init mmap maps own AS)
- [x] **IDT:** #BP/#OF DPL3 trap gates; int80 vector 128 DPL3
- [ ] **Deck Top 50 matrix / bar3:** dated snapshot + HCL; track majority (≥25) → met (≥40); **real-hw Steam client + title runs still open**
  - **When installable on real hardware:** install Steam → try Top 50 titles; fill [matrix](../matrix/deck-top50-2026-07-19.md) (not a SteamOS port)
- [x] **Steam option 2+3 scripts** (fetch/stage/host-prep media path; **not** real Steam client run) — `scripts/fetch-steam-bootstrap.sh`, `stage-steam-tree.sh`, `steam-host-prep.sh`
- [x] **Install path partial:** `make stage-esp` + `make install-img` + `make install-usb DEV=` (GPT ESP img for dd/USB)
- [x] **A2:** PE32 int80 brk with real page maps (`pe32: int80 brk PASS`)
- [x] **A2:** PE32 int80 access/mkdir/rename/unlink via vfs (`pe32: int80 access PASS`)
- [ ] **Real-hardware install path / bar3** (UEFI boot, storage, display, input, net sufficient for Steam client — ESP image ready; full userspace/Steam still open)
- [x] **io_uring min rings + SQE I/O** — setup/enter/register + package mmap + SQE READ/WRITE/FSYNC/CLOSE/READV/WRITEV (`linux: io_uring min rings PASS`, `linux: io_uring mmap PASS`, `linux: io_uring SQE I/O PASS`, `linux: io_uring register depth PASS`, `linux: io_uring more opcodes PASS`); game/title I/O still open
- [x] **A2 partial:** ELF PT_INTERP probe + ET_DYN load bias (`elf: PT_INTERP probe PASS`)
- [x] **A2 partial:** execve INTERP load path (`linux: execve INTERP PASS`, `linux: dynlink path PASS`)
- [x] **A2 partial:** `ld-gj` freestanding dynlinker scaffold → `lib/ld-gj.so.1` on rootfs + ESP
- [x] **A2 partial:** libgj string/mem helpers (`gj_strcmp`/`memmove`/`puts`/`itoa`)
- [x] **A2 partial:** fork creates private AS shell (`process: linux_fork as cr3=…`)
- [x] **A2 partial:** PT_DYNAMIC / DT_NEEDED parse + vfs resolve (`elf: DT_NEEDED probe PASS`, `linux: execve DT_NEEDED resolve PASS`)
- [x] **A2 partial:** R_X86_64_RELATIVE apply on load (`linux: execve RELA PASS`)
- [x] **A2 partial:** auxv-shaped handoff on PCB (`linux: execve auxv PASS`)
- [x] **A2 partial:** fork clones parent user pages in high band (`vmm_as_clone_user_pages`, `linux: fork clone path PASS`)
- [x] **A2 partial:** ld-gj handoff page at `0x6ff00000` + `/proc/self/{auxv,execfn}` (`linux: execve handoff PASS`, `ld-gj: handoff PASS`)
- [x] **A2 partial:** ld-gj userspace product path (handoff/auxv/EXECFN/libc/main/RELA/AT_ENTRY ready)
- [x] **Bugfix:** vfs_ram_write / PE parse under kernel CR3 (mcmodel=kernel high VA access)
- [x] **A2 partial:** GLOB_DAT/JUMP_SLOT + SYMTAB resolve + export table (`linux: execve SYM PASS`, `elf: GLOB_DAT/JUMP_SLOT PASS`)
- [x] **A2 partial:** INTERP-first stack-built auxv page at `0x6ff01000` (`elf: stack auxv va=… PASS`)
- [x] **A2 partial:** INTERP-first start entry/SP on PCB + `thread_exec_replace` (`linux: execve INTERP-first`, `INTERP-first ready`)
- [x] **A2 partial:** DT_NEEDED SO map into AS with per-SO bias (`linux: execve SO map PASS`)
- [x] **A2 partial:** SysV DT_HASH + GNU hash + linear scan across SO registry for GLOB_DAT/JUMP_SLOT (`elf: hash resolve … PASS`)
- [x] **A2 partial:** Prefer embedded packaged `ld-gj.so.1` as INTERP when vfs stub is tiny/non-ELF (`linux: execve INTERP embed … PASS`)
- [x] **A2 partial:** VFS max file data 32 KiB (real ld-gj ~30 KiB); SO registry `g_aSo[4]×32 KiB`
- [x] **A2 partial:** fork clone VA band expanded to INTERP/SO (0x2000_0000–0x8000_0000), limit 512 pages
- [x] **A2 partial:** Live INTERP-first ring-3 thr at ld-gj entry + handoff SP (`linux: INTERP-first live ring3 thr=… PASS`)
- [x] **A2 partial:** Multi-SO map (SysV + GNU hash) + GLOB_DAT/JUMP_SLOT across SOs (`elf: gnu-hash resolve`, SO map n=2)
- [x] **A2 partial:** True fork COW (share RO+PTE_COW, `vmm_cow_break_page` on write #PF) (`linux: fork COW break PASS`)
- [x] **A2 partial:** ld-gj handoff SO list + userspace multi-SO load/resolve (`ld-gj: multi-SO resolve PASS`)
- [x] **A2 partial:** COW frame refcounts on share/break/`as_destroy` (`vmm: as_destroy leaf=… cow_drop=…`, free_old)
- [x] **A2 partial:** ld-gj SysV+GNU hash + mapped-bias SO load (`ld-gj: hash/sym gj_so_export PASS`)
- [x] **A2 partial:** Private non-COW leaf reclaim on `as_destroy` (product VA band; skip identity/kernel) (`priv=` in as_destroy log)
- [x] **A2 partial:** GNU hash bloom filter (64-bit) before chain walk in kernel + ld-gj
- [x] **A2 partial:** Low-band private reclaim floor 0x400000 (classic PE ImageBase) + smoke map
- [x] **A2 partial:** ld-gj multi-SO registry 4 slots (match handoff); SO image 416B
- [x] **A2 partial:** fanotify_init/mark soft real fd path (`linux: fanotify PASS`)
- [x] **Packaging:** rootfs/ESP libgj-so + libgj-gnu + libc stubs; ld-linux-x86-64.so.2 alias
- [x] **#3:** product `libgj-so.so.1` / `libgj-gnu.so.1` real ELF (SysV + GNU hash); staged rootfs/ESP
- [x] **#4 partial:** freestanding `vfsd` (store block mount + named page-cache); live spawn; memfd unique paths; memobj USER maps
- [x] **#4 deepen:** vfsd mini FS (super/freemap/inode/data LBAs) create+read+overwrite; `file path PASS`
- [x] **libcgj:** `memfd_create` / `shm_open` / `shm_unlink`; init probes product SOs + named memobj
- [x] **Bugfix:** product VA band floor raised to **8 MiB** (kernel BSS past 0x400000 collided with low-band COW)
- [x] **vfs_door:** multi-client mini-FS (`GJ_SYS_VFS`); FORMAT/MOUNT/CREATE/READ/WRITE/LIST/STAT; soft RAM if no virtio
- [x] **HCL:** `docs/HCL.md` T0–T3 + install checklist
- [x] **vfs_door OPEN/CLOSE/READFD/WRITEFD** + `/mnt/` path normalize; 16 door fds
- [x] **shell:** echo/cat/ls via VFS door; live spawn `greenjade-shell: vfs PASS`
- [x] **VFS v3:** 8 inodes, multi-sector files ≤1 KiB; shell script (help/format/rm/bigfile)
- [x] **console:** `GJ_SYS_CONSOLE` poll/getc/read (COM1); shell console poll PASS
- [x] **T1 HCL:** `nvme_probe_scan` PCI class 01:08:02

---

## M7 — Ports & polish

- [x] **aarch64 product scaffold** — exceptions/GIC/timer/cpu/PMM/MMU/SVC/virtio-mmio + shared `string`/`stdio_k`/`pmm_freelist`/`sched_coop` + PSCI (`aarch64: shared C kernel PASS`, `psci PASS`); UEFI PE / full shared product kernel still open
- [x] **VFS capacity** raised (64 files / 96 fds / 16 pipes / 8 KiB data / 128 path) for install-shaped open storms
- [x] **IOMMU probe** (DMAR/IVRS inventory; no enforce yet) (`iommu: probe PASS`)
- [x] **MSI/MSI-X capability scan + enable bit** (`pci: MSI-X probe/enable PASS`)
- [x] **MSI/MSI-X table entry 0 program + unmask** (`pci: MSI-X table PASS`)
- [x] **IOMMU enforce windows** (grant/deny bus-master policy) (`iommu: enforce PASS`)
- [x] **MSI-X IRQ → Notification** (IDT bind + soft inject + `GJ_SYS_NOTIFY_WAIT`) (`notify: MSI-X IRQ PASS`)
- [x] **VT-d page tables** (root/context/SLPT identity 1 GiB; optional DRHD MMIO) (`iommu: vtd tables PASS`)
- [x] **VT-d identity window grant integration** (`iommu: vtd identity grant PASS`)
- [x] **Store MAP_RING** for virtio-blk UDX (`store_door: ring map PASS`, `storaged-gj: ring map PASS`)
- [x] **VT-d TE soft-arm** when no DRHD; HW path when DRHD present
- [x] **VT-d TE live-ready** tables+TE+identity (`iommu: vtd TE live-ready PASS`; HW when `GJ_INTEL_IOMMU=1`/DRHD)
- [ ] Clean-room NICs as needed
- [ ] `riscv64` experimental

---

## Explicit non-work (do not schedule)

- [ ] ~~Import Linux / GPL **source** (drivers, libc, …)~~
- [ ] ~~Rust or C++ components~~
- [ ] ~~OOP framework layer~~
- [ ] ~~ZFS unless dual-license-clean path~~
- [ ] ~~SteamOS / Deck **distro port** (shipping their rootfs as ours)~~
- [ ] ~~Legacy BIOS as product requirement~~
- [ ] ~~Load Linux `.ko` binaries~~
# Note: clean-room **Linux-compatible ABI** is **allowed** (not non-work) — see arch §11, PROTON v1.5

---

## Immediate next actions

1. [x] M0.1–M0.4 scaffold builds
2. [x] QEMU `make run` / `make smoke` → `M0 OK` (Multiboot + OVMF)
3. [x] M0.5 IDT + exceptions
4. [x] PMM freelist multi-TiB path + soak (not 4 GiB bitmap); hierarchical order freelists
5. [x] **UEFI** OVMF + **SMP** schedule shipped (PMM-backed percpu growth still open)
6. [x] Migrate SPDX to `MIT OR Apache-2.0` on all sources
7. [ ] **Bar3:** real-hw install + Steam client + Deck Top 50 title matrix
8. [ ] **True 1 TiB soak** when host allows (`-m 1280G`; 768 GiB soak already PASS)
9. [ ] Full multi-server security / `confine` + drop ambient authority
10. [x] `io_uring` min rings + SQE I/O + register depth/more opcodes soft PASS (game/title I/O still open)
11. [x] `aarch64` product-shaped scaffold (shared C string/stdio/pmm/sched + PSCI; UEFI PE still open)

---

## Progress log

| Date | Note |
|------|------|
| 2026-07-23 | **Wave 15 soft continuum (honesty only):** makefile target high-water **advancing toward 15700** (parent wires / CREATE-ONLY graph); honest scan may still report **makefile_max=15600** until parent wires — **do not hardcode false max**. **Soft ≠ product complete.** Product lamps remain **0**. Top 50 **NOT-TRIED × 50**. **bar3 still open**. |
| 2026-07-23 | **Wave 14 soft continuum (honesty only):** makefile target high-water **advancing toward 15600** (parent wires / CREATE-ONLY graph); soft deepen on non-w13 surfaces. **Soft ≠ product complete.** Product lamps remain **0**. Top 50 **NOT-TRIED × 50**. **bar3 still open**. |
| 2026-07-23 | **Parallel waves 5–6 soft shipped** (docs/scripts/headers only — not title claims): wave5 design docs + matrix/TODO **bar3 honesty**; header docs (`types`/`error`/`console`/`config`/`spinlock`); aarch64 smoke script markers. wave6 steam/install/EFI/smoke scripts soft deepen; kernel header docs (linux_abi NR catalog, timer/futex/door/cap/io_uring/process/virtio/mm/syscall/net/cpu/smp); continuum snippet honesty; README + STEAM_* bar3 honesty; linker dual SPDX. Product smoke noted M0 OK / UD=0 on those waves. **Not** bar3 / true 1 TiB soak / Deck Top 50 title runs (matrix still NOT-TRIED×50). **bar3 still open**. |
| 2026-07-23 | **Parallel waves 1–4 soft shipped** (kernel/media/graph only): wave2 `hda_client: live spawn PASS`; wave3 hierarchical **768 GiB** `soak_tib` **PASS on host**; continuum **makefile_max=14900** (`graph_batch14900.c` + soft decades); io_uring soft surface greppable (`linux: io_uring min rings/mmap/SQE I/O/register depth/more opcodes PASS`); aarch64 scaffold greppable (`aarch64: shared C kernel PASS`, `psci PASS`). **Not** bar3 / true 1 TiB soak / Deck Top 50 title runs (matrix still NOT-TRIED×50). **bar3 still open**. |
| 2026-07-23 | **TODO reconcile (parallel deepen):** flip stale M2 boxes with greppable PASSes — `cap: mint/copy/move+cdt PASS`, `cap: quota exhaust PASS`, `door: timeout/peer path PASS`, `door: mid-call peer death PASS`; plus AS-on-schedule, notify, futex A0, named memobj A0, CapJit/except port, timer R7 revoke. Note io_uring SQE I/O + aarch64 shared C/PSCI already in tree. Keep open: bar3, 1 TiB soak, confine multi-server, CDT try-lock product, REPLY/FRAME caps, IPC 100k. **bar3 still open**. |
| 2026-07-23 | **mmap + shared mm/sched + PSCI:** io_uring SQ/CQ package mmap (`linux: io_uring mmap PASS` + SQE fill without inject); shared `pmm_freelist`+`sched_coop` on aarch64; PSCI HVC/SMC with fault recovery (`psci PASS conduit=hvc`). **bar3 still open**. |
| 2026-07-23 | **SQE I/O + shared aarch64 C:** io_uring full SQE path (READ/WRITE/FSYNC/CLOSE/READV/WRITEV via vfs_ram; inject+CQE); greppable `linux: io_uring SQE I/O PASS`. aarch64 links shared `string.c`+`stdio_k.c` via `console_*` (`aarch64: shared C kernel PASS`). **bar3 still open**. |
| 2026-07-23 | **parallel wave:** ~40 exclusive-path agents; continuum **makefile_max=14900**; product polish across net/session/doors/caps/virtio/HDA/io_uring SQE soft/SMP/PMM/UDX/scripts. Parent fixed smoke regressions (door service-first + SMAP winesrv/futex). **M0 OK UD=0**. **bar3 still open**. |
| 2026-07-23 | **io_uring + aarch64:** min io_uring rings PASS (`setup`/`enter`/`register`, `linux: io_uring min rings PASS`); aarch64 product scaffold (cpu/pmm/mmu/svc/virtio-mmio + prior GIC/timer/exceptions) builds as `greenjade-aarch64.elf`. **bar3 still open**. |
| 2026-07-23 | **wave3:** hierarchical free / large-RAM soak — **768 GiB soak PASS on host**; true 1 TiB still optional when host allows. **bar3 still open** (real-hw Steam client + Deck Top 50 title runs / matrix). |
| 2026-07-23 | **wave2:** `hda_client` live embed; continuum decade pending **14600** (next after makefile_max=14500); parallel agents. **bar3 still open** (real-hw Steam client + Deck Top 50 title runs / matrix). |
| 2026-07-23 | **TODO reconcile:** flip stale done boxes (M0 OK/smoke, IDT/timer preempt/SMP schedule/UEFI OVMF, live daemons sessiond/netstackd/sshd/storaged/vfsd/shell/scsi_mid, HDA stream/CORB/multi-stream soft, dual+hierarchical freelist PMM + soak soft-skip, Steam option 2+3 scripts). Keep open: bar3 real-hw Steam+Deck Top 50, io_uring ENOSYS, multi-server confine, aarch64 product, 1 TiB soak when host allows. Parallel backlog already on tree (sshd live, scsi_mid embed, hierarchical PMM, Steam media scripts). |
| 2026-07-19 | M0 scaffold; `make` builds ELF |
| 2026-07-19 | v0.5: desktop, UEFI, dual MIT/Apache, 1 TiB+, SMP, SAS/SCSI |
| 2026-07-19 | UDX best-combo: host lib + inject/probe/irq/work; `make udx-example` |
| 2026-07-19 | **Accepted** Apple-channel decisions v1.0 (VM objects, task ports, QoS, session) |
| 2026-07-19 | Proton personality v1.0 (game runtime tier; not SteamOS) |
| 2026-07-19 | **Accepted** Proton personality v1.1 (normative P0/P1/P2 tiers) |
| 2026-07-19 | **Proton-over-Apple** precedence (PROTON v1.2, Apple v1.1, arch v0.8) |
| 2026-07-19 | Games = popular desktop adoption bar; **priorities unchanged** (arch §0.5.1, PROTON v1.3) |
| 2026-07-19 | **Major game support** = **Steam Deck Top 50** (arch §0.5.2, PROTON v1.4) |
| 2026-07-19 | Clarified: ban is **GPL copyleft source**, not Linux ABI; clean-room Linux compat OK (arch v0.11, PROTON v1.5) |
| 2026-07-19 | **Re-applied** Proton + Deck Top 50 focus under clean-room ABI (PROTON v1.6, arch v0.12 §0.5.3) |
| 2026-07-19 | **Option C hybrid** Linux syscall dispatch implemented (`kernel/syscall/`, LINUX_ABI_HYBRID.md) |
| 2026-07-19 | Open list 1–4: MSRs+GDT, vmm mmap, futex queues, cold_ipc→libprotonrt |
| 2026-07-19 | Spec refresh + ptr check, ring-3, PA futex, CapJit, blocking cold IPC |
| 2026-07-19 | per-CPU/swapgs, scheduler sleep, doors cold personality thread |
| 2026-07-19 | **DESIGN_SPEC_COMPLETE v1.0 Accepted** — all missing design freezes (clean-room) |
| 2026-07-19 | **Coding start:** IDT+traps, kernel map harden, SMEP (G-IDT / G-MAP) |
| 2026-07-19 | A1 SPIR-V path + VBO triangle; HDA stream ring; sessiond/netstackd/storaged host PASS |
| 2026-07-19 | Doors R/W+socket; PMM soak; timer quantum; HDA MMIO CORB shadow |
| 2026-07-19 | Session CLAIM ownership; CORB DMA engine; poll/ioctl; sessiond.elf |
| 2026-07-19 | Live sessiond.elf spawn PASS; net/store door ownership |
| 2026-07-19 | Live netstackd.elf + storaged.elf spawn PASS |
| 2026-07-19 | Virtio queue door ops; MSI-X/IOMMU probe |
| 2026-07-19 | UDX user ring (DESC_ALLOC+MAP_DMA+USER_AVAIL); MSI-X table; HDA HW CORB |
| 2026-07-19 | Notification + MSI-X IRQ bind; IOMMU enforce windows; WoW64 NR map |
| 2026-07-19 | clock_getres hot; MSI-X IDT stub + Notification pulse; self-IPI deferred |
| 2026-07-19 | VT-d root/context/SLPT tables; store MAP_RING; storaged-gj ring map |
| 2026-07-19 | PE32 header parse (i386) for WoW64/Proton path |
| 2026-07-19 | PE32 section stage; pread64/pwrite64; getdents64; GJ_INTEL_IOMMU |
| 2026-07-19 | PE32 user AS map; WoW64 image stage path |
| 2026-07-19 | PE32+ process load/spawn; pwrite64; VT-d soft-only |
| 2026-07-19 | Ring EXPORT/MAP/KICK; BDL kick; PLATFORM_INFO; MSI-X enable |
| 2026-07-19 | PE32 WoW64 trampoline; dup/readlink/wait4/kill; VT-d identity grant |
| 2026-07-19 | access/fstat/prlimit/uname; GDT CS32; wait4 reaper; expanded WoW64 NRs |
| 2026-07-19 | compat frame; getgroups; writev; chdir; aarch64 M0 scaffold |
| 2026-07-19 | clock_nanosleep; readv; i386 soft-exec; VT-d TE soft-arm; except smoke; SMP MADT guards |
| 2026-07-19 | mincore/msync; process_death G-PROC-5 partial |
| 2026-07-19 | AS destroy for wait-children; sysinfo/times/getrusage; chdir cwd; rename/ftruncate/accept |
| 2026-07-19 | mremap; sockopt/getsockname/shutdown; sendmsg/recvmsg cold |
| 2026-07-19 | rt_sigaction/procmask + itimer stubs |
| 2026-07-19 | F_DUPFD; symlink; utimensat; select; pe32 compat gdt L=0 D=1 validate |
| 2026-07-20 | pipe2; link; alarm/pause; fchmod; rmdir; flock; real unlink |
| 2026-07-20 | PE32 int80 multi-NR (getuid/brk/write/set_thread_area); soft-exec expanded |
| 2026-07-20 | renameat/linkat/symlinkat; clock_settime/adjtime; seccomp; VT-d TE live-ready |
| 2026-07-20 | stage-esp install tree; pidfd_getfd/io_uring ENOSYS; PE32 mmap2 real pages |
| 2026-07-20 | PE32 brk maps pages; wow64 NR map expanded (gettid/fork/execve/time) |
| 2026-07-20 | PE32 pipe/socketcall/mmap2 real pages; install-img GPT; sched_getattr/kcmp |
| 2026-07-20 | PE32 gettimeofday/wait4/kill/nanosleep; fanotify/kcmp cold stubs |
| 2026-07-20 | PE32 fstat64/open fd table; install-img in smoke; pipe/socketcall surface |
| 2026-07-20 | PE32 fork/clone/vfork/execve/sig/poll stubs; VFS 64 files / 96 fds |
| 2026-07-20 | mbind/get_mempolicy; PE32 modify_ldt/getrandom/set_tid_address |
| 2026-07-20 | PE32 int80 getrandom hardware PASS; ~185 smoke gates |
| 2026-07-20 | PE32 MAP_FIXED 0x58001000 + mprotect; install-img in smoke |
| 2026-07-20 | PE32 clock_gettime CS32; ~183 smoke gates |
| 2026-07-20 | PE32 uname CS32; PE32 int80 product surface for Wine/Proton bring-up |
| 2026-07-20 | PE32 clone CS32; full int80 Wine bring-up stack smoke green (~185 gates) |
| 2026-07-20 | PE32 futex FUTEX_WAKE CS32; Wine/Proton PE32 int80 stack: exit/getpid/mmap/pipe/sock/fstat/random/uname/clone/futex |
| 2026-07-20 | PE32 tls PASS; ~187 smoke gates; install-img GPT ready for real-hw dd |
| 2026-07-20 | PE32 vfs load PASS (PE on ramdisk → load_process → CS32 int80 exit) |
| 2026-07-20 | VFS 8 KiB file data + longer paths; smoke-all still green (~188 gates) |
| 2026-07-20 | PE32 nanosleep CS32; PE32 Wine/Proton int80 bring-up stack largely green |
| 2026-07-20 | init.elf uname+clock_gettime+mmap; libgj Linux wrappers expanded |
| 2026-07-20 | PE32 gettid CS32; ~190 smoke gates |
| 2026-07-20 | PE32 time CS32; PE32 int80 Wine bring-up suite complete for current batch |
| 2026-07-20 | PE32 vfs open/read/write/lseek; gettimeofday; getcwd/sysinfo; POSIX timers |
| 2026-07-20 | PE32 brk maps pages; install-usb.sh; init openat touch |
| 2026-07-20 | PE32 access/mkdir/rename/unlink vfs; ~197 smoke gates |
| 2026-07-20 | PE32 rename PASS; PE32 int80 FS surface growing for Wine |
| 2026-07-20 | PE32 getppid/getpgrp/setsid; install-usb; ~200 smoke gates |
| 2026-07-20 | PE32 getrlimit/setrlimit; waitpid ECHILD; PE32 FS+creds surface |
| 2026-07-20 | PE32 wait4 ECHILD CS32 PASS; ~201 smoke gates |
| 2026-07-20 | PE32 kill PASS; PE32 int80 process/FS/creds suite expanded (~202 gates) |
| 2026-07-20 | PE32 fsync PASS; PE32 open/fsync/close path |
| 2026-07-20 | PE32 poll PASS; PE32 int80 surface ~204 smoke gates |
| 2026-07-20 | PE32 select PASS; PE32 int80 Wine FS/process/select suite |
| 2026-07-20 | Fix Linux current=caller process (init mmap AS); smoke timeout 90s; init ABI depth |
| 2026-07-20 | PE32 real pipe2/socketpair/eventfd/getdents; pipe_rw+eventfd PASS |
| 2026-07-20 | PE32 getdents/link/clock_getres/symlink/pipe2; writev/readv/fcntl/poll real; openat |
| 2026-07-20 | PE32 pread64/pwrite64; init pipe2+eventfd+getdents+link; wow64 NR expand |
| 2026-07-20 | PE32 symlink/pipe2/pread_pwrite/timerfd; epoll/sendfile/fchmod/ftruncate/utime |
| 2026-07-20 | PE32 epoll+inotify PASS; INSTALL.txt install-usb path; ~220 smoke gates |
| 2026-07-20 | PE32 sendfile/signalfd/clock_nanosleep; fstat64 from vfs; smoke-all green |
| 2026-07-20 | PE32 memfd PASS; copy_file_range PE32 surface; Wine int80 IPC/FS/timer suite large |
| 2026-07-20 | PE32 fork/clone unique fake pids; alarm/itimer; init symlink+nanosleep; 218 smoke gates |
| 2026-07-20 | PE32 *at FS (renameat/linkat/symlinkat/unlinkat/mkdirat/faccessat); soft-exec NR fix |
| 2026-07-20 | PE32 readlinkat/fstatat64; smoke-all still green; Wine CS32 FS/*at surface deep |
| 2026-07-20 | PE32 fdatasync/sync/waitid; wow64 flock NR fix; 218 smoke gates green |
| 2026-07-20 | PE32 execve real PE load+frame rewrite; CLONE_VM real CS32 thread; Linux ELF execve |
| 2026-07-20 | PE32 wait4→process_wait4_ppid (parent-filtered); fork→process_linux_fork; init fork/wait |
| 2026-07-20 | Rootfs seed expand; shell.elf; stage-esp user/; threads 32; Linux CLONE_THREAD real |
| 2026-07-20 | PE32 vfork_wait PASS; ESP packages init+shell+servers |
| 2026-07-20 | fork deferred exit worker; wait4 yield-retry; PE32 fork_wait PASS |
| 2026-07-20 | stage-rootfs.sh (sbin/init, bin/sh, servers); smoke rootfs gates |
| 2026-07-21 | shell readline idle; USB/PS2/AHCI BAR soft; HCL T1 probes; smoke PASS |
| 2026-07-21 | AHCI probe T1; + NVMe/console/VFS v3/shell script; smoke PASS |
| 2026-07-21 | VFS v3 8 inodes+1KiB files; shell script cmds; console/serial; NVMe probe T1; smoke PASS |
| 2026-07-21 | vfs OPEN/READFD/CLOSE + /mnt/ paths; shell echo/cat/ls live spawn; smoke PASS |
| 2026-07-21 | vfs_door multi-client (GJ_SYS_VFS); vfsd door owner; docs/HCL.md; smoke PASS |
| 2026-07-21 | vfsd mini-FS file path; memfd/shm_open; low-band floor 8MiB (BSS collision fix); smoke PASS |
| 2026-07-21 | #3/#4: real libgj-so/gnu ELF SOs; vfsd block+named cache; memfd unique; live spawn; smoke PASS |
| 2026-07-21 | libcgj 1–4: buffered stdio; pthread/TLS/futex; GLIBC_2.* vers; ld-gj COPY/TLS/IRELATIVE/DT_INIT; smoke PASS |
| 2026-07-21 | libcgj surface growth: mmap/fork/stdio/ctype/strtol + dyn dlopen smoke; docs/GLIBC_COMPAT.md |
| 2026-07-21 | libcgj clean-room glibc-shaped libc → libc.so.6; cgj_smoke; docs/GLIBC_COMPAT.md |
| 2026-07-21 | libcgj splice/tee/vmsplice + sendmsg/recvmsg/sendmmsg/recvmmsg/accept4; kernel splice/tee arg fix |
| 2026-07-21 | libcgj mkfifo/mknod/pselect/dup3 + inotify/preadv/pwritev/fchdir/itimer/ttyname; ~422 T symbols |
| 2026-07-21 | libcgj POSIX timer_* + clock_nanosleep + mq_* ENOSYS + sem private futex; getline/freopen/fseeko |
| 2026-07-21 | libcgj fmemopen/open_memstream + pthread_rwlock_*; ~462 T symbols; Multiboot green |
| 2026-07-21 | open_memstream real grow; barrier/spin/rwlock_timed; mutex/cond timed; attr stacksize |
| 2026-07-21 | asprintf/vasprintf + setlinebuf/setbuffer; vsnprintf sizing fix; ~481 T; Multiboot green |
| 2026-07-21 | open_wmemstream+wchar; mutexattr type/protocol + recursive; condattr clock; sem_open shm |
| 2026-07-21 | mbrtowc/wcrtomb/mbsrtowcs UTF-8; fwprintf/swprintf; rwlockattr; EILSEQ |
| 2026-07-21 | btowc/wctob + isw*/tow*; fgetwc UTF-8; ~548 T symbols |
| 2026-07-21 | wmem*; wcscoll/wcsxfrm; fwscanf/swscanf; narrow scanf/sscanf/fscanf |
| 2026-07-20 | ELF PT_INTERP+ET_DYN; execve INTERP load; ld-gj scaffold; libgj string; fork AS shell; rootfs/ESP lib/ld-gj.so.1 |
| 2026-07-20 | process_death restores caller CR3 (vfork mid-syscall safe); fork stubs free AS via death; smoke-all PASS ~225+ gates |
| 2026-07-20 | DT_NEEDED+RELA+auxv; vmm_as_clone_user_pages fork clone; ld-gj self-ELF probe; libc.so.6 rootfs stub; smoke-all PASS |
| 2026-07-20 | ld-gj handoff page+verify; userspace ld-gj auxv/EXECFN/RELA/AT_ENTRY; kernel CR3 for vfs write/PE parse; smoke-all PASS |
| 2026-07-20 | GLOB_DAT/JUMP_SLOT + export table; stack auxv 0x6ff01000; ld-gj SYM path; smoke-all PASS |
| 2026-07-20 | INTERP-first entry/SP + thread_exec_replace; DT_NEEDED SO map (per-SO bias); smoke-all PASS |
| 2026-07-20 | **Hash/gnu-hash SO resolve:** SysV DT_HASH + GNU hash + scan across mapped SOs; GLOB_DAT→`gj_so_export`; SO layout fix (dyn after phdrs); smoke-all PASS |
| 2026-07-20 | **Real ld-gj INTERP embed:** `ld_gj_embed.S` + prefer packaged ELF over tiny vfs stub; INTERP-first uses full freestanding ld-gj (~30 KiB) |
| 2026-07-20 | fork clone VA band includes INTERP/SO (0x20000000+); clone limit 512; VFS_MAX_DATA 32 KiB |
| 2026-07-20 | **Live INTERP-first ring-3:** `thread_create_user` when no thr to replace; yield until EXITED; ld-gj runs userspace product path → AT_ENTRY exit stub |
| 2026-07-20 | **Multi-SO hash chains:** libgj-so (DT_HASH) + libgj-gnu (DT_GNU_HASH); GLOB_DAT+JUMP_SLOT n=2; smoke-all gates for live thr + gnu-hash |
| 2026-07-20 | **timer_sleep_until:** STI+yield+guard (no infinite HLT) so init nanosleep cannot hang after user #PF paths |
| 2026-07-20 | **True COW fork:** writable leaves shared RO+PTE_COW; write #PF / explicit break; `linux: fork COW break PASS` |
| 2026-07-20 | **ld-gj multi-SO:** handoff `cSo`+`aSo[]`; userspace open `/lib/*` + resolve; `ld-gj: multi-SO resolve PASS` |
| 2026-07-20 | **COW refcounts:** share/register, break/drop, `as_destroy` cow_drop; strip ld-gj; INTERP buf 64 KiB |
| 2026-07-20 | **ld-gj hash/sym:** SysV+GNU hash + linear scan; load SO image from mapped bias; `hash/sym gj_so_export PASS` |
| 2026-07-20 | **Private leaf reclaim:** as_destroy frees non-COW user pages in 0x2000_0000–0x8000_0000; skip identity/kernel-shared |
| 2026-07-20 | **GNU hash bloom:** 64-bit bloom filter in kernel `elf_gnu_hash_lookup` + ld-gj `hash_gnu_lookup` |
| 2026-07-20 | Low-band reclaim 0x400000; ld-gj SO_MAX=4; fanotify soft fd; rootfs/ESP SO packaging |
