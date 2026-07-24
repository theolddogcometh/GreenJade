# GreenJade — Implementation Spec (coding baseline)

| Field | Value |
|-------|--------|
| **Project** | GreenJade |
| **Document** | Implementation Specification v0.4 |
| **Companion** | [Architecture](GREENJADE_KERNEL_SPEC.md) · [Security core](SECURITY_CORE_DESIGN.md) · [TODO](TODO.md) · [Style](../STYLE.md) |
| **Product** | General-purpose **desktop** |
| **First target** | `x86_64` + **UEFI** (Multiboot2 QEMU only as interim M0) |
| **Platform profile** | [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) |
| **Hardware bar** | **≥ 1 TiB RAM**, **SMP**, **SAS/SCSI** |
| **Language** | Pure C11 + minimal asm |
| **License** | **MIT OR Apache-2.0** |
| **Priority order** | **1. Security → 2. Performance → 3. Portability → 4. Readability** |

This document is what you implement against. Architecture policy lives in `GREENJADE_KERNEL_SPEC.md`; **this file is concrete**.

**Tradeoffs:** If a patch is faster but skips a capability check, reject it. If portable code is slower on one arch, keep a secure portable path and add an arch-optimized path only when measured. Hungarian/readability never outrank isolation.

**Product DoD (not M0):** boots via UEFI on real hardware; uses **> 1 TiB** RAM correctly; runs on **multiple CPUs**; block I/O via **SCSI/SAS** stack (plus NVMe/virtio); desktop userspace path exists (FB/compositor phased).

---

## 1. Repository layout

```
knano/
├── LICENSE                 # MIT
├── README.md
├── STYLE.md
├── Makefile                # top-level
├── docs/
│   ├── GREENJADE_KERNEL_SPEC.md
│   ├── IMPLEMENTATION.md   # this file
│   └── TODO.md
├── third_party/            # MIT/Apache only; empty until provenanced
│   └── PROVENANCE.md
├── tools/                  # host tools (C preferred)
│   └── (later: mkimage, etc.)
├── kernel/
│   ├── include/gj/         # public kernel headers
│   │   ├── types.h
│   │   ├── config.h
│   │   ├── error.h
│   │   ├── cap.h
│   │   ├── ipc.h
│   │   ├── space.h
│   │   ├── thread.h
│   │   ├── klog.h
│   │   └── syscall.h
│   ├── arch/
│   │   └── x86_64/
│   │       ├── boot.S      # Multiboot2 entry
│   │       ├── multiboot2.h
│   │       ├── linker.ld
│   │       ├── cpu.h / cpu.c
│   │       ├── gdt.c
│   │       ├── idt.c
│   │       ├── isr.S
│   │       ├── paging.c
│   │       ├── serial.c    # early console 16550
│   │       └── trap.c
│   ├── mm/
│   │   ├── pmm.c           # physical page allocator
│   │   ├── vmm.c           # kernel + user maps
│   │   └── kheap.c         # tiny kernel heap (bump or freelist)
│   ├── cap/
│   │   └── cap.c
│   ├── ipc/
│   │   └── ipc.c
│   ├── sched/
│   │   └── sched.c
│   ├── proc/
│   │   └── thread.c
│   ├── lib/
│   │   ├── string.c
│   │   ├── stdio_k.c       # kprintf
│   │   └── assert.c
│   └── main.c              # kmain
├── user/                   # later: init, servers (not M0)
│   └── README.md
└── scripts/
    ├── run-qemu.sh
    └── check-license.sh
```

**M0 builds only `kernel/`** into `build/greenjade.elf` (and optionally `.iso`).

---

## 2. Toolchain

| Item | Choice |
|------|--------|
| Compiler | `clang` or `x86_64-elf-gcc` freestanding |
| Flags | `-ffreestanding -fno-builtin -fno-stack-protector` (add stack protector later) `-fno-pic -mno-red-zone` (x86_64 kernel) `-Wall -Wextra -Werror` |
| Linker | `ld.lld` or GNU `ld` with `kernel/arch/x86_64/linker.ld` |
| Emulator | QEMU `qemu-system-x86_64` |
| Boot (M0) | Multiboot2 via GRUB or QEMU `-kernel` if ELF Multiboot2-compliant |

**Host deps (dev):** `qemu-system-x86`, `clang`/`lld` or cross gcc, `grub-mkrescue` (optional ISO), `xorriso`.

No Rust, no C++ in the Makefile.

---

## 3. Boot path

### 3.0 Product path — UEFI (x86_64)

```
UEFI firmware
    → GreenJade.efi (EFI stub / PE)
    → ExitBootServices
    → handoff: memory map, ACPI RSDP, optional GOP FB, initrd
    → long mode kernel entry
    → serial and/or GOP console
    → pmm (full map) → HHDM → AP bring-up → …
```

| Item | Requirement |
|------|-------------|
| Firmware | UEFI 2.x |
| Protocols | Loaded image, file system (optional), GOP (desktop early FB) |
| Tables | ACPI RSDP for MADT (SMP), MCFI/DMAR later (IOMMU) |
| Secure Boot | Optional later |

### 3.1 Interim path — Multiboot2 (M0 only)

```
QEMU -kernel Multiboot2 ELF
    → boot.S → kmain(mb2_info)
    → serial → pmm (currently 4 GiB-capped — DEBT)
```

- Header magic `0xE85250D6`; parse mmap.
- **Not** product DoD. Replace with UEFI before claiming desktop boot.

### 3.2 Memory map (kernel virtual)

**M0 (debt):** identity map first few GiB; phys-as-virt for low memory.

**Product target:**

| Virtual | Use |
|---------|-----|
| `0xFFFF_8000_0000_0000` | HHDM of **all** physical RAM (1 TiB+) |
| `0xFFFF_FFFF_8000_0000` | Kernel image (higher-half) |
| User half | `0x0000_…` per space |

**Paging:** 4-level default; enable **LA57 / 5-level** when available and needed for large VA/PA configs.

### 3.3 Physical memory allocator (product)

| M0 (current) | Product requirement |
|--------------|---------------------|
| Flat bitmap, **4 GiB max frames** | **Illegal** for product |
| Linear scan alloc | Buddy / freelist per zone; per-CPU caches |
| Single lock optional | SMP-safe; prefer per-zone locks |

Must allocate/free pages across **sparse** memory maps (holes, >1 TiB).

---

## 4. Coding contracts

### 4.0a Capability addressing + revoke (x86_64)

See [CAP_ADDRESSING.md](CAP_ADDRESSING.md) and [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) §1.1.

| Rule | Implementation |
|------|----------------|
| User handle | **Scheme A:** `u64` slot + `u32` gen; slot 0 = root meta |
| CNode scope | **One per process**; all threads share it |
| Resolve | `gj_cap_resolve(pProcCnode, u64Slot, u32Gen, &out)` |
| Install | `gj_cap_slot_install` / `gj_cap_alloc_install` → `gj_cap_ref` |
| DEAD + object gen **first** | `gj_obj_revoke_begin` |
| Use fails closed | slot gen + `gj_obj_check_live(pObj, slot.u32ObjGen)` |
| Slot invalidate | S7 via `gj_cap_slot_invalidate` |
| Code | `kernel/cap/cnode.c`, `kernel/cap/revoke.c`, `kernel/proc/process.c` |
| Root meta | slot 0: process + CNode for **kernel ops only** (`gj_process_bootstrap_root_meta`) |
| Pager | PCB `refPager`; `gj_process_set_pager`; fault w/o pager → `GJ_ERR_FAULT` |

### 4.0 Hungarian notation (required)

All **variables** use Systems Hungarian — see [`STYLE.md`](../STYLE.md).

| Prefix | Use | Example |
|--------|-----|---------|
| `p` | pointer | `pTag`, `pRegions` |
| `c` | element count | `cRegions` |
| `cb` | byte count | `cbLength` |
| `pa` / `va` | phys / virt address | `paKernelStart` |
| `sz` | C string | `szFmt` |
| `f` | boolean flag | `fReady` |
| `i` | index | `iFrame` |
| `g_` | global + Hungarian | `g_cFramesFree` |
| `cap` | capability | `capEndpoint` |
| `st` | status | `stResult` |

Functions/types/macros stay non-Hungarian (`pmm_alloc`, `gj_thread_t`).

### 4.1 Types (`gj/types.h`)

```c
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uintptr_t gj_vaddr_t;
typedef uint64_t  gj_paddr_t;
typedef int       gj_status_t;   /* 0 = OK, negative = error */
/* caps: struct gj_cap_ref { u64 u64Slot; u32 u32SlotGen; } — Scheme A */
```

Use freestanding headers only (`stdint.h`, `stddef.h`, `stdbool.h`, `stdarg.h`). No libc.

### 4.2 Errors (`gj/error.h`)

```c
enum {
    GJ_OK            =  0,
    GJ_ERR_INVAL     = -1,
    GJ_ERR_NOMEM     = -2,
    GJ_ERR_NOENT     = -3,
    GJ_ERR_PERM      = -4,
    GJ_ERR_AGAIN     = -5,
    GJ_ERR_NOSUPPORT = -6,
    GJ_ERR_BUSY      = -7,
    GJ_ERR_FAULT     = -8,
};
```

### 4.3 Logging

- `kprintf(fmt, ...)` → serial COM1 (0x3F8) early; VGA optional later.
- Levels: `KLOG_ERR`, `KLOG_WARN`, `KLOG_INFO`, `KLOG_DEBUG` (compile-time gate).

---

## 5. Physical memory (PMM)

**API:**

```c
struct gj_mem_region {
    gj_paddr_t paBase;
    u64        cbLength;
};

void       pmm_init(const struct gj_mem_region *pRegions, size_t cRegions,
                    gj_paddr_t paKernelStart, gj_paddr_t paKernelEnd);
gj_paddr_t pmm_alloc(void);           /* 1 page, 0 on fail */
void       pmm_free(gj_paddr_t paPage);
size_t     pmm_free_count(void);
```

**Algorithm (M0 only):** bitmap over usable RAM **capped** — **must be replaced** before large-RAM DoD.  
**Product:** hierarchical/zone freelists; 4K base; optional 2 MiB; cover **>1 TiB**.  
**Rules:** never free kernel image pages; reserve page 0 / firmware holes.

---

## 6. Virtual memory (VMM) — phased

### M0
- Bootloader/paging as required to run 64-bit code.
- Identity or loader map is enough for bring-up only.
- No user spaces yet.
- **Does not satisfy 1 TiB product bar.**

### M1
```c
typedef struct gj_space gj_space_t;

gj_space_t *space_create(void);
void        space_destroy(gj_space_t *s);
gj_status_t space_map(gj_space_t *s, gj_vaddr_t va, gj_paddr_t pa, u64 flags);
gj_status_t space_unmap(gj_space_t *s, gj_vaddr_t va, size_t n_pages);
void        space_activate(gj_space_t *s);
```

**Flags:** `GJ_MAP_READ`, `GJ_MAP_WRITE`, `GJ_MAP_EXEC`, `GJ_MAP_USER`, `GJ_MAP_GLOBAL`.  
Default new maps: **not** `WRITE|EXEC` together for user (W^X policy).

---

## 7. Threads & scheduler — phased

### M0
- Single BSP, idle loop (`hlt`).

### M1
```c
typedef struct gj_thread gj_thread_t;

gj_thread_t *thread_create(gj_space_t *space, gj_vaddr_t entry, gj_vaddr_t stack_top);
void         thread_yield(void);
void         sched_start(void);
```

**M1 scheduler:** round-robin, fixed quantum, no SMP.

### M2+
- Priorities, blocking on IPC, IRQ wakeups.

### SMP (required for product; start design M1–M2)

| Piece | Plan |
|-------|------|
| Discovery | ACPI MADT → local APICs / x2APIC |
| AP entry | Trampoline + per-CPU stack/GDT/TSS |
| Scheduler | Per-CPU queues; IPI reschedule |
| TLB | Shootdown IPI |
| PMM / caps / IPC | Explicit locks or lock-free where proven |
| `GJ_MAX_CPUS` | Compile-time ceiling high enough for workstations (e.g. 256/512), not `1` |

M0 may run UP; **no permanent UP-only data structures in core APIs**.

---

## 8. Capabilities — phased

### Design (implement M2)

Capability is a **slot** in a CNode (table):

```c
enum gj_cap_type {
    GJ_CAP_INVALID = 0,
    GJ_CAP_CNODE,
    GJ_CAP_THREAD,
    GJ_CAP_SPACE,
    GJ_CAP_ENDPOINT,
    GJ_CAP_NOTIFICATION,
    GJ_CAP_REPLY,
    GJ_CAP_IRQ,
    GJ_CAP_FRAME,      /* physical page */
    GJ_CAP_PAGE_TABLE, /* optional internal */
};

struct gj_cap {
    u16 type;
    u16 rights;
    u32 gen;           /* generation for ABA */
    union {
        void *ptr;
        u64   raw;
    } obj;
};
```

**Rights bits:** `GJ_RIGHT_READ`, `WRITE`, `GRANT`, `MAP`, `KILL`, …

**Syscalls (M2 sketch):**

| Call | Purpose |
|------|---------|
| `sys_cap_identify` | type/rights of a cap |
| `sys_cap_mint` | derive reduced rights |
| `sys_cap_revoke` | nuke subtree |
| `sys_cap_move` / `copy` | transfer between CNodes |

**M0–M1:** no caps yet; kernel-only objects with raw pointers.

---

## 9. IPC — phased (M2)

### Primitives

**Endpoint** — sync rendezvous:

```c
/* client */
gj_status_t ipc_call(u64 u64EpSlot, u32 u32EpGen, gj_msg_t *pMsg);
/* server */
gj_status_t ipc_recv(u64 u64EpSlot, u32 u32EpGen, gj_msg_t *pMsg);
gj_status_t ipc_reply(u64 u64ReplySlot, u32 u32ReplyGen, gj_msg_t *pMsg);
```

**Notification** — bitmask or counter wakeup for IRQs.

### Message (fixed small + optional grant)

```c
#define GJ_MSG_REGS 4
struct gj_msg {
    u64  tag;                 /* label + flags + length */
    u64  r[GJ_MSG_REGS];      /* plain data words */
    /* M3: caps-in-message, shared page grants */
};
```

Keep **small and boring** until something needs more.

---

## 10. Syscall surface (userspace later)

Kernel entry: `syscall` instruction (x86_64).  
Number in `rax`, args `rdi rsi rdx r10 r8 r9` (Linux-ish **shape**, original numbers).

### Allocated numbers (stable once userspace exists)

**Normative sketch:** [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) §5 (includes futex, vm_*, process_*, QoS).  
Do **not** use older incomplete maps in this file as ABI truth.

| Num | Name | Phase |
|-----|------|-------|
| 0 | `SYS_DEBUG_LOG` | M1 |
| 1 | `SYS_YIELD` | M1 |
| 2 | `SYS_EXIT` | M1 |
| 10–12 | IPC call/recv/reply | M2 |
| 20–24 | cap mint/move/copy/revoke/identify | M2 |
| 30 | untyped_retype | M2 |
| 40–43 | vm_map / unmap / protect / map_object | M2 |
| 50–52 | process set_pager / spawn / kill·wait | M2 |
| 70–71 | futex wait/wake | M2/M3 |

Exact ABI frozen in `kernel/include/gj/syscall.h` when M1 userspace starts — **do not renumber after first userland**.

---

## 11. Interrupts (M1+)

| Piece | Plan |
|-------|------|
| IDT | All vectors; default panic/log |
| PIC or IOAPIC | M1: 8259 PIC simple; M3: IOAPIC |
| Timer | PIT or HPET or APIC timer — tick for RR sched |
| Serial IRQ | Optional; polled serial OK for M0–M1 |
| IRQ → notification | M2/M3 with `GJ_CAP_IRQ` |

---

## 12. Userspace (post-M0)

Not built in M0. Order:

1. **static user ELF** loaded by kernel (embedded or Multiboot module) — `user/init`
2. **init** prints via `SYS_DEBUG_LOG`, exits
3. **init** spawns servers (M3+)

`user/` code: freestanding C or tiny `libgj` syscall stubs — still **pure C**, MIT.

---

## 13. UDX drivers & storage (desktop)

See architecture spec §7. Order:

1. virtio transport + virtio-blk/net/gpu (QEMU desktop dev)
2. NVMe clean-room
3. **SCSI mid-layer** (`scsi_mid`) original C
4. **virtio-scsi** then **SAS HBA** clean-room (docs-based)
5. AHCI/SATA, xHCI+HID, audio, Ethernet
6. GPU beyond virtio/GOP as clean-room allows

All in **userspace driver host** processes, not kernel.

---

## 14. Testing strategy

| Phase | Test |
|-------|------|
| M0 | QEMU boots; serial shows banner; does not triple-fault |
| M1 | PMM alloc/free stress; map page; two threads yield |
| M2 | ping-pong IPC 100k calls; no leak; multi-CPU smoke |
| M3 | virtio-blk read; UEFI boot path |
| Large RAM | QEMU `-m 1280G` (or host max) stress alloc/free/map |
| SMP | QEMU `-smp 8` (and higher) schedule on all CPUs |
| SCSI/SAS | virtio-scsi + real HBA when available |

**CI (later):** `make test` runs QEMU with timeout, greps serial for `GJ_OK` markers.

---

## 15. Config knobs (`gj/config.h`)

```c
#define GJ_PAGE_SIZE        4096u
#define GJ_KERNEL_STACK     (16u * 1024u)
#define GJ_SERIAL_PORT      0x3F8u
#define GJ_MAX_CPUS         256u    /* product: multi-CPU; M0 may use 1 online */
#define GJ_DEBUG            1
#define GJ_KLOG_LEVEL       3       /* info */
#define GJ_PMM_MAX_PHYS_TIB 4u      /* design headroom multi-TiB */
```

---

## 16. Definition of Done

### M0 (bring-up only)

- [x] `make` produces `build/greenjade.elf` (interim Multiboot2)
- [ ] `make run` → serial `GreenJade` / `M0 OK`
- [ ] Freestanding pure C + asm only
- [ ] `SPDX-License-Identifier: MIT OR Apache-2.0`
- [ ] No third_party GPL

### Product (desktop) — later gates

- [ ] UEFI boot on QEMU OVMF and at least one real machine
- [ ] RAM **> 1 TiB** path validated (QEMU or hardware)
- [ ] SMP with **N>1** CPUs online and scheduling
- [ ] Block I/O via **scsi_mid** + (virtio-scsi and/or SAS HBA)
- [ ] Desktop input + framebuffer/compositor path (phased)
- [ ] **Deck Top 50** track: Proton **A0** (libprotonrt smoke) → **A1** (first real title) → majority/met — [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) v1.6 · arch §0.5.2–0.5.3
- [ ] Clean-room Linux-compatible personality depth as matrix requires (**no GPL source**)

---

## 17. What to implement first (order)

**Normative dependency graph:** [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) §12.

1. **IDT** + supervisor-only kernel maps + SMEP/SMAP (gate) — **done**  
2. Finish M0 QEMU verify — **done** (`qemu-kvm` + GRUB Multiboot2 ISO)  
3. **AS clone** (per-process CR3 + COW) — **done**  
4. Caps + ENDPOINT doors + spawn/PROCESS — **done** (CDT/quota later)  
5. **Userspace** personality door server — **done** (PERSONALITY_SERVE policy interim)  
6. Memory objects on mmap; futex timeouts + idle HLT + PIT timer — **done**  
7. virtio-gpu 2D present + virtio-net TX/RX + input — **done** (Vulkan ICD next)  
8. HHDM + freelist PMM + xAPIC timer — **done** (1 TiB free path / x2APIC next)  
8b. virtio-blk R/W + vfs `/dev/vda` + APIC-calibrated mono — **done**  
8c. UEFI stub compile (`make uefi-stub`) + `gj_boot_info` — scaffold **done** (OVMF PE next)  
8d. MADT + AP trampoline (`GJ_SMP=2` online=2) + `user/libgj`/`init.elf` — **done**  
8e. AP local timer + BSP-only mono; `GJ_CPU_STATIC_MAX` — **done**  
8f. virtio-scsi + scsi_mid INQUIRY/READ_CAPACITY/READ10 + `/dev/sda` — **done**  
8g. UEFI ExitBootServices stub + minimal `GreenJade.efi` (mkpe_efi) — **done**  
8h. `smp_ap_run` + IPI wake (AP executes work on cpu=1) — **done**  
8i. net_eth ICMP echo — **done**  
8j. OVMF GPT ESP boot of `BOOTX64.EFI` (`make ovmf` → serial `GJ-EFI`/`GJ-OVMF`) — **done**  
8k. x2APIC CPUID detect + external thread affinity — **done**  
8l. Unified `gj_boot_info` + ESP stages `KERNEL.ELF`; `make smoke` — **done**  
8m. Dual AP handoff (`ap_run` + `ap_run2` timer progress) — **done**  
8n. UEFI SimpleFileSystem load KERNEL.ELF → `kmain_uefi` + identity map — **done**  
8o. Per-CPU AP schedule + TLB shootdown IPI — **done**  
8p. x2APIC mode enable (per-CPU after SMP) — **done**  
8q. Userspace scsi_mid CDB skeleton — **done** (server door next)  
8r. socketpair/pipe2 ring buffers (Proton A0 IPC shape) — **done**  
8s. Named memobj registry + map (`memobj_create_named`) — **done**  
8t. Proton A0 wine-server demo (cross-proc futex + socketpair + CapJit) — **done**  
8u. Exception port + eventfd2 + scsi_door + UEFI GOP handoff — **done**  
8v. A1 T0: compositor + gj_vk_icd present + GJ_SYS_GPU_* — **done**  
8w. net_lo send/recv peer ring + fcntl/madvise cold — **done**  
8x. A1 swapchain acquire/present + session_input hub + HDA probe stub — **done**  
8y. Khronos-shaped Vulkan ICD (`vulkan_icd.c` + loader negotiate) — **done**  
8z. Cmdbuf clear/submit + renderpass/pipeline stubs + image/memory — **done**  
8aa. Host ICD `.so`/`GreenJade_icd.json` + `icd_host_smoke` multi-frame — **done**  
8ab. Multi-frame kernel present + descriptor stubs — **done**  
9. Deck Top 50 matrix title runs (deferred; need Proton out-of-tree)  
10. SAS HBA + full scsi_mid door ENDPOINT  
11. SPIR-V software path + HDA stream ring + sessiond/netstackd/storaged host — **done** (door hand-off next)
12. Textured FS / multi-attr VS; HDA CORB DMA; userspace door ownership  

See [TODO.md](TODO.md), [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md), [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md).

---

## Progress note — 2026-07-23

Parallel waves **soft shipped** (kernel / media / continuum graph only). **Open bars stay open.**

**Soft stamp:** continuum **makefile_max=14900** (`graph_batch14900.c` wired; CREATE-ONLY). Host `steam-bar3-check` media inventory **READY** — **not** client. **bar3: OPEN.** Top 50 **NOT-TRIED × 50**.

Residual **#UD** closed; **scsi_mid** live; **HDA multi-stream** kernel PASS; continuum **makefile_max=14900** (soft only); **bar3** open.

Hierarchical **free_range** + **768G soak_tib** **PASS**; product **≥ 1 TiB** path still **open** when host allows; **bar3** open.

### Status rows (additive — 2026-07-23 ship honesty)

| Item | Status | Notes |
|------|--------|-------|
| Continuum **makefile_max=14900** | **soft shipped** | CREATE-ONLY graph batches through `graph_batch14900.c`; parallel-wave soft only; **not** Steam client / Top-50 |
| **io_uring** min rings + mmap/SQE soft | **PASS** | `kernel/syscall/io_uring_min.c`; smokes `linux: io_uring min rings PASS`, `mmap PASS`, `SQE I/O PASS` — vfs_ram/kernel soft surface; **not** game I/O |
| **768GiB hierarchical soak** | **PASS** | `GJ_MEM=768G` / `pmm_soak_tib`; product bar still **≥ 1 TiB** when host allows |
| **aarch64 M0 scaffold** | **smoke PASS** | `make aarch64` / `aarch64-smoke`; exceptions→cpu→GIC→timer→pmm→mmu→svc→virtio-mmio; shared C + PSCI smokes; shared product kernel **not** ported to Deck/x86 path |
| **HDA multi-stream** | **kernel PASS** | mixer/SD0 path; **not** Steam/game audio |
| Steam media `STATUS=READY` | **media only** | host soft check READY (stage/rootfs); bootstrap tree staged/packed — **not** client launch |
| Deck Top 50 title runs | **NOT-TRIED** × 50 | claim **targeting only**; **no titles tried**; bar3 client open |
| Bar3 (Steam client on DUT) | **open** | parallel soft waves / kernel/media gates ≠ title matrix |
| Product RAM **≥ 1 TiB** full path | **open** | 768G soak ≠ TiB bar; separate from bar3 |

---

## Progress note — GreenJade Wave 10 honesty (2026-07-23)

**Additive only.** Prior progress rows and soft stamps above stay historical ledger. This section is Wave 10 honesty: continuum high-water and bar3 bound. It does **not** re-litigate M0–product DoD, close any product bar, or promote Deck Top 50 rows.

### Soft stamp (Wave 10)

| Field | Value |
|-------|--------|
| **Wave** | GreenJade Wave 10 (honesty refresh) |
| **Continuum** | **makefile_max=15200** — **soft only** (CREATE-ONLY graph parent wire / `graph_batch15200.c` high-water when present on tree) |
| **bar3** | **OPEN** |
| **Matrix** | **NOT-TRIED × 50** (no titles tried) |

**Soft stamp (Wave 10):** continuum **makefile_max=15200** soft only (`graph_batch15200.c` parent wire / CREATE-ONLY decades when present on tree). Soft continuum ≠ runtime ABI product ≠ Steam **client** launch. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

### Soft continuum ≠ bar3 (Wave 10 honesty)

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **makefile_max=15200** | CREATE-ONLY soft graph high-water only (libcgj parent wire) | Runtime ABI product; Steam client; Deck Top 50; bar3 closed |
| Host media `STATUS=READY` / `steam-bar3-check` | Bootstrap inventory on media | Client launch; title try |
| Kernel / parallel soft smokes | Surface ship honesty | bar3 client run; matrix fill |

**Hard rule:** never promote continuum **makefile_max=15200**, media READY, or soft ship gates to “bar3 closed,” Deck Top 50 `PASS`, or product DoD complete. Soft continuum ≠ bar3.

### Explicit non-claims (Wave 10)

| Claim | Allowed? |
|-------|----------|
| “Continuum **makefile_max=15200** soft graph high-water (CREATE-ONLY)” | **Yes** — soft only |
| “Soft continuum 15200 = Steam client / Top-50 / product score” | **No** |
| “bar3 closed by continuum 15200 / media READY / kernel soft gates” | **No** |
| “Deck Top 50 titles tried / PASS from this stamp” | **No** — matrix stays **NOT-TRIED** |
| Any **bar3** closed claim from IMPLEMENTATION soft stamp alone | **No** |

### Open (unchanged by Wave 10)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN**
- Deck Top 50 title runs — all **NOT-TRIED** (claim **targeting only**)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Soft continuum growth past prior decades — **soft only**; does **not** close bar3

**Bar3 remains OPEN.** Wave 10 honesty: continuum **makefile_max=15200 soft only**; soft continuum ≠ bar3; **no false PASS**.

### Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — media path / continuum soft scope  
- [HCL.md](HCL.md) — soft probes ≠ full HCL product  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title matrix honesty  

---

*Wave 10 honesty (2026-07-23): continuum **makefile_max=15200 soft only**; **bar3 remains OPEN**.*

---

## Progress note — GreenJade Wave 12 honesty (2026-07-23)

**Additive only (Wave 12 exclusive for this file).** Prior progress rows and Wave 10 soft stamps above stay historical ledger. This section is Wave 12 honesty: continuum high-water and bar3 bound. It does **not** re-litigate M0–product DoD, close any product bar, or promote Deck Top 50 rows.

### Soft stamp (Wave 12)

| Field | Value |
|-------|--------|
| **Wave** | GreenJade Wave 12 (honesty refresh) |
| **Continuum** | **makefile_max=15400** — **soft only** (CREATE-ONLY graph parent wire / `graph_batch15400.c` high-water when present on tree) |
| **bar3** | **OPEN** |
| **Matrix** | **NOT-TRIED × 50** (no titles tried) |

**Soft stamp (Wave 12):** continuum **makefile_max=15400** soft only (`graph_batch15400.c` parent wire / CREATE-ONLY decades when present on tree). Soft continuum ≠ runtime ABI product ≠ Steam **client** launch. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

### Soft continuum ≠ bar3 (Wave 12 honesty)

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **makefile_max=15400** | CREATE-ONLY soft graph high-water only (libcgj parent wire) | Runtime ABI product; Steam client; Deck Top 50; bar3 closed |
| Host media `STATUS=READY` / `steam-bar3-check` | Bootstrap inventory on media | Client launch; title try |
| Kernel / parallel soft smokes | Surface ship honesty | bar3 client run; matrix fill |

**Hard rule:** never promote continuum **makefile_max=15400**, media READY, or soft ship gates to “bar3 closed,” Deck Top 50 `PASS`, or product DoD complete. Soft continuum ≠ bar3.

### Explicit non-claims (Wave 12)

| Claim | Allowed? |
|-------|----------|
| “Continuum **makefile_max=15400** soft graph high-water (CREATE-ONLY)” | **Yes** — soft only |
| “Soft continuum 15400 = Steam client / Top-50 / product score” | **No** |
| “bar3 closed by continuum 15400 / media READY / kernel soft gates” | **No** |
| “Deck Top 50 titles tried / PASS from this stamp” | **No** — matrix stays **NOT-TRIED** |
| Any **bar3** closed claim from IMPLEMENTATION soft stamp alone | **No** |

### Open (unchanged by Wave 12)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN**
- Deck Top 50 title runs — all **NOT-TRIED** (claim **targeting only**)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Soft continuum growth past prior decades — **soft only**; does **not** close bar3

**Bar3 remains OPEN.** Wave 12 honesty: continuum **makefile_max=15400 soft only**; soft continuum ≠ bar3; **no false PASS**.

### Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — media path / continuum soft scope  
- [HCL.md](HCL.md) — soft probes ≠ full HCL product  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title matrix honesty  

---

*Wave 12 honesty (2026-07-23): continuum **makefile_max=15400 soft only**; **bar3 remains OPEN**.*

---

## Progress note — GreenJade Wave 14 soft continuum (2026-07-23)

**Additive only (Wave 14 exclusive for this file).** Prior progress rows and Wave 10/12 soft stamps above stay historical ledger. This section is Wave 14 honesty: soft continuum deepen toward the next parent-wire high-water. It does **not** re-litigate M0–product DoD, close any product bar, or promote Deck Top 50 rows.

### Soft stamp (Wave 14)

| Field | Value |
|-------|--------|
| **Wave** | GreenJade Wave 14 (soft continuum / non-w13 deepen) |
| **Continuum** | makefile target high-water **advancing toward 15600** — **soft only** (parent wires / CREATE-ONLY graph; tree may still show prior decade e.g. 15500) |
| **bar3** | **OPEN** |
| **Matrix** | **NOT-TRIED × 50** (no titles tried) |
| **Product lamps** | **0** (soft stubs stay zero by design) |

**Soft stamp (Wave 14):** continuum high-water **advancing toward 15600** soft only (parent wires). Soft deepen on non-w13 surfaces. **Soft ≠ product complete.** Soft continuum ≠ runtime ABI product ≠ Steam **client** launch. Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

### Soft continuum ≠ bar3 (Wave 14 honesty)

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 15600** | CREATE-ONLY soft graph high-water advance (libcgj parent wire) | Runtime ABI product; Steam client; Deck Top 50; bar3 closed |
| Soft deepen (non-w13 surfaces) | Honesty / bring-up surface deepen | Product complete; product lamps lit |
| Host media `STATUS=READY` / `steam-bar3-check` | Bootstrap inventory on media | Client launch; title try |
| Product lamps **0** | Soft score / bar3-ready stubs remain zero | Any product-score claim |

**Hard rule:** never promote continuum advancing toward **15600**, media READY, soft deepen, or soft ship gates to “bar3 closed,” Deck Top 50 `PASS`, product lamps > 0, or product DoD complete. Soft continuum ≠ bar3. **Soft ≠ product complete.**

### Explicit non-claims (Wave 14)

| Claim | Allowed? |
|-------|----------|
| “Continuum high-water advancing toward **15600** (parent wire soft)” | **Yes** — soft only |
| “Soft continuum toward 15600 = Steam client / Top-50 / product score” | **No** |
| “Product lamps lit / product complete from Wave 14 soft deepen” | **No** — lamps remain **0** |
| “bar3 closed by continuum toward 15600 / media READY / kernel soft gates” | **No** |
| “Deck Top 50 titles tried / PASS from this stamp” | **No** — matrix stays **NOT-TRIED** |
| Any **bar3** closed claim from IMPLEMENTATION soft stamp alone | **No** |

### Open (unchanged by Wave 14)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN**
- Deck Top 50 title runs — all **NOT-TRIED** (claim **targeting only**)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Soft continuum growth toward **15600** — **soft only**; does **not** close bar3; product lamps stay **0**

**Bar3 remains OPEN.** Wave 14 honesty: continuum **advancing toward 15600 soft only**; soft deepen ≠ product complete; product lamps **0**; soft continuum ≠ bar3; **no false PASS**.

### Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — media path / continuum soft scope  
- [HCL.md](HCL.md) — soft probes ≠ full HCL product  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title matrix honesty  

---

*Wave 14 soft continuum (2026-07-23): high-water **advancing toward 15600 soft only**; product lamps **0**; **soft ≠ product complete**; **bar3 remains OPEN**.*

---

## Progress note — GreenJade Wave 35 soft continuum (2026-07-23)

**Additive only (Wave 35 exclusive for this file).** Prior progress rows and Wave 10/12/14/16/17 soft stamps above stay historical ledger (Wave 17 high-water was **toward 15900** / scan tip **15900** when wired). This section is Wave 35 honesty: soft continuum deepen toward the next parent-wire high-water. It does **not** re-litigate M0–product DoD, close any product bar, or promote Deck Top 50 rows.

### Soft stamp (Wave 35)

| Field | Value |
|-------|--------|
| **Wave** | GreenJade Wave 35 (soft continuum honesty) |
| **Continuum** | makefile target high-water **advancing toward 17700** — **soft only** (parent wires / CREATE-ONLY graph; honest scan may still show prior tip **makefile_max=17600**; **do not hardcode false 17700**) |
| **Soft deepen surfaces** | **retfortress** / **retpalace** (CREATE-ONLY soft graph names only — not product / not bar3) |
| **bar3** | **OPEN** |
| **Matrix** | **NOT-TRIED × 50** (no titles tried) |
| **Product lamps** | **0** (soft stubs stay zero by design) |

**Soft stamp (Wave 35):** continuum high-water **advancing toward 17700** soft only (parent wires). Soft deepen surfaces **retfortress** / **retpalace** are CREATE-ONLY soft graph names only. **Do not hardcode false `makefile_max=17700`** if `./scripts/gj-continuum-makefile-snippet.sh --max` still reports **17600** until parent wires. **Soft ≠ product complete.** Soft continuum ≠ runtime ABI product ≠ Steam **client** launch. Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.** multi_server=0 confine=0 lamps 0.

### Soft continuum ≠ bar3 (Wave 35 honesty)

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 17700** | CREATE-ONLY soft graph high-water advance (libcgj parent wire) | Runtime ABI product; Steam client; Deck Top 50; bar3 closed |
| Honest scan **makefile_max=17600** (when still tip) | Makefile wire truth until parent wires **17700** | Claim that **17700** is already greppable |
| Soft deepen **retfortress** / **retpalace** | Wave 35 CREATE-ONLY soft graph surface names | Product complete; product lamps lit; bar3 |
| Soft deepen | Honesty / bring-up surface deepen | Product complete; product lamps lit |
| Host media `STATUS=READY` / `steam-bar3-check` | Bootstrap inventory on media | Client launch; title try |
| Product lamps **0** | Soft score / bar3-ready stubs remain zero | Any product-score claim |

**Hard rule:** never promote continuum advancing toward **17700**, media READY, soft deepen, or soft ship gates to “bar3 closed,” Deck Top 50 `PASS`, product lamps > 0, or product DoD complete. Soft continuum ≠ bar3. **Soft ≠ product complete.** Never hardcode a false `makefile_max` higher than the Makefile scan.

### Explicit non-claims (Wave 35)

| Claim | Allowed? |
|-------|----------|
| “Continuum high-water advancing toward **17700** (parent wire soft)” | **Yes** — soft only |
| “Honest scan still `makefile_max=17600` until parent wires 17700” | **Yes** — scan is source of truth |
| “Soft continuum toward 17700 = Steam client / Top-50 / product score” | **No** |
| “Product lamps lit / product complete from Wave 35 soft deepen” | **No** — lamps remain **0** |
| “bar3 closed by continuum toward 17700 / media READY / kernel soft gates” | **No** |
| “Deck Top 50 titles tried / PASS from this stamp” | **No** — matrix stays **NOT-TRIED** |
| Any **bar3** closed claim from IMPLEMENTATION soft stamp alone | **No** |

### Open (unchanged by Wave 35)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` — **OPEN**
- Deck Top 50 title runs — all **NOT-TRIED** (claim **targeting only**)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Soft continuum growth toward **17700** — **soft only**; does **not** close bar3; product lamps stay **0**

**Bar3 remains OPEN.** Wave 35 honesty: continuum **advancing toward 17700 soft only**; scan may still be **17600**; soft deepen ≠ product complete; product lamps **0**; soft continuum ≠ bar3; **no false PASS**.

### Related honesty surfaces

- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — media path / continuum soft scope  
- [HCL.md](HCL.md) — soft probes ≠ full HCL product  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — title matrix honesty  

---

*Wave 35 soft continuum (2026-07-23): high-water **advancing toward 17700 soft only**; soft deepen **retfortress**/**retpalace**; honest scan may still be **17600**; product lamps **0**; **soft ≠ product complete**; **bar3 remains OPEN**.*

