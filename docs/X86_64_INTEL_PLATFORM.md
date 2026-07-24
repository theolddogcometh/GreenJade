# GreenJade — x86_64 Intel Platform Profile (normative)

| Field | Value |
|-------|--------|
| **Document** | x86_64 Intel platform profile v1.0 |
| **Status** | **Accepted** for product x86_64 path |
| **Scope** | How GreenJade **uses Intel-class PC/server hardware** — not OS philosophy |
| **OS design** | Still [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md), [CAP_ADDRESSING.md](CAP_ADDRESSING.md), [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) |
| **Heritage** | Intel SDM / platform firmware / VT-d / Linux-oriented platform practice — **under GreenJade law** |
| **Honesty** | **2026-07-23 Wave 32** — soft vs product IOMMU/platform open (§14); continuum **toward 17400** soft only; product lamps **0**; **no product claim**; **no bar3 claim** |

This profile does **not** replace Solaris-first OS design or L4-style capabilities. It defines the **platform contract** for x86_64 machines (especially Intel, and compatible AMD where noted).

**Soft stamp (Wave 9, historical):** greppable IOMMU / VT-d / platform smokes on tree are **bring-up honesty only**. They do **not** close production DMA policy (P-DMA-*), full platform product, or Steam **bar3**.

**Soft stamp (Wave 32):** same soft-vs-product platform bound restated under Wave 32 exclusive ownership of this file’s honesty ledger. Continuum high-water **advancing toward 17400** soft only (CREATE-ONLY parent wire). Product lamps remain **0**. Soft platform ≠ product complete ≠ bar3. See §14.

---

## 1. Profile identity

```text
Profile name:  x86_64-intel-desktop / x86_64-intel-workstation
Applies to:    Product builds on UEFI x86_64 PCs and workstations
Priorities:    Security → Performance → Portability → Readability
```

| Layer | Owner doc |
|-------|-----------|
| Caps, IPC, process, pager | Security core + Cap addressing + Solaris remaining |
| Boot, IRQ, DMA, paging, time, NUMA | **This document** |

---

## 2. Boot and firmware (normative)

| Rule | Requirement |
|------|-------------|
| **P-BOOT-1** | **UEFI** is the primary product boot path (x86_64). |
| **P-BOOT-2** | Multiboot2 / QEMU `-kernel` is **dev-only**, not product DoD. |
| **P-BOOT-3** | Consume **UEFI memory map** after `ExitBootServices`; do not rely on legacy E820 alone for product. |
| **P-BOOT-4** | Locate **ACPI** (RSDP from UEFI configuration table); use for topology and DMA remapping discovery. |
| **P-BOOT-5** | Secure Boot / measured launch (Boot Guard / TXT ecosystem) is **optional production profile**, not required for T0 virtio CI. |

---

## 3. Interrupts (normative)

| Rule | Requirement |
|------|-------------|
| **P-IRQ-1** | Prefer **Local APIC + x2APIC** when available (large CPU counts, multi-socket). |
| **P-IRQ-2** | Prefer **MSI / MSI-X** for devices; IOAPIC legacy paths only as fallback. |
| **P-IRQ-3** | 8259 PIC is **bring-up only**, not product steady-state. |
| **P-IRQ-4** | Hard IRQ path stays minimal (GreenJade IRQ policy): notify userspace; no driver logic in hard IRQ. |
| **P-IRQ-5** | IRQ storm control remains in force (mask + `devmgr`); not architecture-specific but required on this profile. |

---

## 4. DMA and IOMMU (normative)

| Rule | Requirement |
|------|-------------|
| **P-DMA-1** | **VT-d (Intel IOMMU)** shall be used for production **bus-master** devices when present. |
| **P-DMA-2** | DMA is expressed as **window caps** (DDI-like); program remapping hardware only through that path. |
| **P-DMA-3** | On device/host **revoke**: **disable IOMMU window in hardware first**, then free frames (aligns with revoke §1.1 Phase A). |
| **P-DMA-4** | Production desktop/workstation: **no open bus-master** without a working IOMMU window for that device. |
| **P-DMA-5** | Dev/QEMU profiles may soft-allow missing IOMMU with explicit warning — **not** production default. |
| **P-DMA-6** | AMD-Vi is a **portability** target on the same abstract window API (priority after Intel VT-d bring-up). |

---

## 5. Paging and large memory (normative)

| Rule | Requirement |
|------|-------------|
| **P-MEM-1** | Default **4-level** paging on x86_64. |
| **P-MEM-2** | Support **5-level paging (LA57)** when CPU + firmware enable it and capacity requires it (multi-TiB / large VA). |
| **P-MEM-3** | Product bar remains **≥ 1 TiB** RAM: no permanent 4 GiB-only structures (M0 bitmap is **debt**). |
| **P-MEM-4** | Prefer **2 MiB** large pages where beneficial (HHDM, big mappings); **1 GiB** where `PDPE1GB` and useful. |
| **P-MEM-5** | Higher-half kernel + **HHDM** (or equivalent) covering **all** RAM for product — not identity-map-only. |
| **P-MEM-6** | Enforce **NX**, **SMEP**, **SMAP** (and related) on supported CPUs; user maps respect **W^X** (no WRITE\|EXEC). |
| **P-MEM-7** | **CET** (shadow stacks / IBT): **tier-2** hardening after core M2 paths; plan hooks, not day-one requirement. |

---

## 6. NUMA and multi-CPU (normative)

| Rule | Requirement |
|------|-------------|
| **P-SMP-1** | **SMP is required** for product; do not freeze UP-only data structures. |
| **P-SMP-2** | Discover CPUs via **ACPI MADT**; bring up APs with per-CPU stacks/state. |
| **P-SMP-3** | Prefer **x2APIC** for large logical CPU counts. |
| **P-SMP-4** | Physical memory allocator shall be **NUMA-aware** when **SRAT** (and optional SLIT) is present: per-proximity **zones**. |
| **P-SMP-5** | Scheduler: per-CPU runqueues + IPI reschedule/TLB shootdown (OS design); platform must deliver reliable IPIs. |
| **P-SMP-6** | `GJ_MAX_CPUS` remains a compile-time ceiling suitable for workstations (e.g. 256+), not `1`. |

---

## 7. Time (normative)

| Rule | Requirement |
|------|-------------|
| **P-TIME-1** | Security timeouts and sched budgets use **monotonic** time only (never wall clock). |
| **P-TIME-2** | Prefer **invariant TSC** when CPUID advertises it; use as mono backbone. |
| **P-TIME-3** | If invariant TSC unavailable: calibrate TSC against **HPET** and/or APIC; document VM caveats. |
| **P-TIME-4** | Prefer **APIC timer** (deadline mode when available) for per-CPU ticks/timeouts. |
| **P-TIME-5** | Wall clock remains **userspace** (`timed` + RTC); not used for IPC/fault security timeouts. |

---

## 8. Syscall and userspace ABI (normative on this arch)

| Rule | Requirement |
|------|-------------|
| **P-ABI-1** | Syscalls use **System V AMD64** register convention (`rdi, rsi, rdx, r10, r8, r9`). |
| **P-ABI-2** | Capability names use **Scheme A**: `u64` slot + `u32` gen as separate args when a cap is named ([CAP_ADDRESSING.md](CAP_ADDRESSING.md)). |
| **P-ABI-3** | Kernel never accepts user-supplied **kernel virtual addresses** as capabilities (SMAP/SMEP world). |

---

## 9. Drivers and I/O culture (normative alignment)

| Rule | Requirement |
|------|-------------|
| **P-DRV-1** | Class drivers stay **userspace** (UDX); aligns with high-performance **userspace I/O** practice (DPDK/SPDK *spirit*), not GPL Linux in-tree drivers. |
| **P-DRV-2** | Clean-room / dual MIT-Apache only — **Intel’s Linux GPL ecosystem is not imported**. |
| **P-DRV-3** | HCL tiers still apply ([SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) §13): T0 virtio → T1 NVMe/AHCI/HID → T2 SAS family → T3 GPU. |
| **P-DRV-4** | Prefer MSI-X capable devices in T1+ documentation. |

---

## 10. Security features checklist (x86_64 product)

| Feature | Product expectation |
|---------|---------------------|
| NX / XD | On |
| SMEP | On when available |
| SMAP | On when available |
| W^X user maps | Enforced |
| KASLR | Planned product; entropy from CPU RNG + firmware when possible |
| CET | Tier-2 after core paths |
| VT-d for bus-master | Production default when hardware present |
| Invariant TSC | Prefer for mono |

---

## 11. Profile summary (copy-paste)

```text
Profile: x86_64-intel-desktop / workstation

  Boot:      UEFI + ACPI
  IRQs:      x2APIC + MSI-X (APIC/IOAPIC as needed)
  DMA:       VT-d window caps; no open bus-master in production
  Memory:    NUMA zones, ≥1 TiB capable PMM, 2MiB (1GiB optional), LA57 optional
  Time:      invariant TSC when available; APIC timer; mono for security
  Hardening: SMEP/SMAP/NX/W^X; CET later
  Drivers:   userspace UDX; HCL T0–T1 first; SAS on roadmap
  Caps/IPC:  unchanged GreenJade/Solaris-first OS design
```

---

## 12. Explicit non-goals of this profile

| Non-goal | Reason |
|----------|--------|
| Redefining capability model to “Intel style” | Intel has no seL4-equivalent OS mandate |
| Requiring SGX / TDX for desktop v1 | Optional future; not product baseline |
| Importing GPL Intel Linux drivers | GreenJade license law |
| Legacy BIOS as product | UEFI primary |
| PowerPC / SPARC platform rules | Separate profiles if ever needed |

---

## 13. Relation to other docs

| If conflict | Winner |
|-------------|--------|
| Caps, revoke, IPC, process, pager | Security core / Cap addressing / Solaris remaining |
| Boot, IRQ controller, IOMMU programming, TSC, NUMA, large pages | **This profile** on x86_64 product builds |
| Priority order | Still Security → Performance → Portability → Readability |

---

## 14. Honesty refresh — soft vs product (Wave 32 · 2026-07-23)

**Additive only (Wave 32 exclusive for this file).** Profile rules in §§1–13 stay **Accepted** (normative platform contract). This section is a Wave 32 honesty ledger: what is **soft** on the tree vs what remains **open** for product IOMMU / platform. It does **not** re-litigate architecture or weaken P-DMA production defaults. Wave 16 §14 ledger text is superseded here as the same honesty formula under Wave 32 exclusive ownership of this file.

| Term | Meaning in this document |
|------|--------------------------|
| **Accepted** | Normative platform rule frozen for x86_64 product path — ship toward these rules |
| **Soft** | Partial / greppable / bring-up path exists; **not** full product close of the rule |
| **Open** | Rule accepted; code / enforcement / product path still incomplete or not default |
| **Soft continuum** | CREATE-ONLY libcgj graph parent wire; high-water **advancing toward 17400** soft only |
| **Product lamps** | Soft score / bar3-ready continuum stubs — remain **0** by design |
| **bar3** | Steam **client** on DUT + Deck Top 50 leave `NOT-TRIED` — **out of scope to claim here** |

### 14.1 Soft does not close product platform (or bar3)

| Soft surface (tree / host) | Closes product IOMMU / P-DMA? | Closes product platform? | Closes bar3? |
|----------------------------|-------------------------------|--------------------------|--------------|
| `iommu: probe PASS` / DMAR soft inventory | **No** | **No** | **No** |
| `iommu: enforce PASS` / software window table | **No** (software enforce ≠ production hardware default) | **No** | **No** |
| `iommu: vtd * PASS` (tables, TE path, identity grant soft) | **No** | **No** | **No** |
| Dev/QEMU soft-allow missing IOMMU (P-DMA-5) | **No** — explicit **non**-production | **No** | **No** |
| Continuum high-water **toward 17400** CREATE-ONLY | **No** | **No** | **No** |
| Host Steam media inventory **READY** | **No** | **No** | **No** |
| Kernel smokes (virtio T0, HDA, io_uring min, 768G soak, aarch64 M0) | **No** | **No** (may soft-touch related surfaces) | **No** |
| Profile **Accepted** in this file | Decisions only | Decisions only | **No** |

**Hard rule:** never claim “product IOMMU closed,” “no open bus-master product default shipped,” “platform product complete,” product lamps > 0, bar3 closed, or Deck Top 50 title PASS from greppable `iommu:*` / soft continuum toward **17400** / media `STATUS=READY` / design **Accepted** alone. **Soft ≠ product complete.** Matrix honesty lives in [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — this profile does not promote those rows.

**Hard stamp (Wave 32):** soft IOMMU/platform **≠** product close. Soft continuum **toward 17400** ≠ product platform ≠ bar3. Product lamps **0**. **No bar3 claim.**

### 14.2 IOMMU / DMA ledger (normative → ship honesty)

| Rule | Normative (Accepted) | Soft on tree (honest bound) | Remaining product (do not claim done) |
|------|----------------------|-----------------------------|----------------------------------------|
| **P-DMA-1** | VT-d for production bus-master when present | DMAR probe + VT-d table/TE soft (`iommu: vtd * PASS`, CAP soft synthetic when no live DRHD) | Full VT-d programming as production default on real DUT with working remapping |
| **P-DMA-2** | DMA as **window caps**; program remapping only through that path | Software window grant / count + identity grant soft; syscall enforce toggle | `create_window` as true window **cap**; remapping only via cap path end-to-end |
| **P-DMA-3** | Revoke: **disable IOMMU window in hardware first**, then free frames | Soft window table / TE arm path; not full Phase A HW-first product | Destroy/revoke **HW-first** on real remapping hardware; align revoke §1.1 product |
| **P-DMA-4** | Production: **no open bus-master** without working IOMMU window | Soft `iommu_busmaster_ok` enforce smoke; bring-up may leave enforce off after smoke | Production desktop/workstation class: enforce on by default; no free-form bus-master |
| **P-DMA-5** | Dev/QEMU may soft-allow missing IOMMU **with warning** — **not** production default | Soft-allow / synthetic CAP paths exist for bring-up | Keep production default hard; never promote soft-allow to product DoD |
| **P-DMA-6** | AMD-Vi portability on same abstract window API | Not product-claimed | After Intel VT-d product path |

### 14.3 Other platform areas (soft bound vs product open)

| Area | Soft on tree (honest bound) | Remaining product (open) |
|------|-----------------------------|--------------------------|
| **Boot (P-BOOT)** | Multiboot2 + OVMF UEFI bring-up; UEFI path soft | UEFI primary as sole product DoD; Secure Boot / measured launch optional profile not required for T0 |
| **IRQ (P-IRQ)** | x2APIC / MSI-X soft paths; hard IRQ minimal policy | Product steady-state: no 8259; MSI-X preference documented for T1+ |
| **Memory (P-MEM)** | 4-level paging; hierarchical freelist; 768 GiB soak soft PASS on capable hosts | Product **≥ 1 TiB** capable PMM path; LA57 when needed; HHDM covering all RAM product |
| **SMP / NUMA (P-SMP)** | SMP AP bring-up soft; `GJ_MAX_CPUS` ceiling | NUMA-aware zones from SRAT/SLIT product; large logical CPU product path |
| **Time (P-TIME)** | Mono path for security timeouts (policy) | Invariant TSC preference + APIC deadline product backbone on class |
| **Hardening (§10)** | NX / SMEP / SMAP / W^X soft enforcement paths | KASLR product; CET tier-2 later (unchanged) |
| **Drivers (P-DRV)** | T0 virtio + UDX host soft; live embeds | T1+ HCL product matrix; no GPL Intel Linux import (unchanged law) |

### 14.4 Explicit non-claims (Wave 32)

| Claim | Allowed? |
|-------|----------|
| “x86_64 Intel platform profile **Accepted**” | **Yes** — this document §§1–13 |
| “Soft IOMMU probe / enforce / VT-d TE greppable” | **Yes** — with soft bound (§14.2) |
| “Soft continuum high-water advancing toward **17400** (CREATE-ONLY)” | **Yes** — soft only |
| “Product IOMMU / no open bus-master production default closed” | **No** |
| “P-DMA-5 soft-allow is production default” | **No** — explicitly non-production |
| “Continuum / media READY / kernel smokes close platform product” | **No** |
| “Product lamps lit / product complete from soft platform or continuum” | **No** — lamps remain **0** |
| “Deck Top 50 titles tried / PASS from this doc” | **No** — matrix stays **NOT-TRIED** until real client runs |
| “bar3 closed” | **No** — **bar3 remains OPEN** (client + matrix) |
| “Wave 32 honesty closes bar3 or platform product” | **No** — honesty refresh only |

### 14.5 Related honesty surfaces

- [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) §12 / §19 — IOMMU API design + remaining ledger  
- [HCL.md](HCL.md) — T0 VT-d soft row; continuum toward 17400; open bars  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 OPEN; READY ≠ NOT-TRIED  
- [TODO.md](TODO.md) · [IMPLEMENTATION.md](IMPLEMENTATION.md) — coding boxes / soft stamp  
- [CAP_ADDRESSING.md](CAP_ADDRESSING.md) §9.5 — continuum toward 17400 soft  

---

*Normative x86_64 Intel-class platform profile for Project GreenJade.*  
*Wave 32 honesty (§14): soft IOMMU/platform ≠ product close; continuum **toward 17400 soft only**; product lamps **0**; production no open bus-master still **open**; **soft ≠ product complete**; **bar3 remains OPEN**.*
