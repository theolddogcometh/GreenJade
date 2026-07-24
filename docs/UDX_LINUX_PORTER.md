# GreenJade UDX — Linux Driver Porter Contract

| Field | Value |
|-------|--------|
| **Document** | UDX Linux porter guide v1.1 |
| **Status** | **Accepted** surface for clean-room ports |
| **License** | MIT OR Apache-2.0 only — **no Linux source** |
| **Code** | `user/udx/` |
| **Build** | `make udx` → `build/libudx.a` · `make udx-example` |
| **Companion** | [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [APPLE_CHANNEL_REMAINING.md](APPLE_CHANNEL_REMAINING.md) 6, 16 · [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) (games ICD uses GPU host; drivers stay UDX) |
| **Honesty (Wave 62)** | Host soft path ≠ full GJ grants / multi-server / bar3; continuum **toward 20400** soft only; product lamps **0** (13) |

---

## 1. Goal

Let people who know **Linux device drivers** port hardware support by rewriting **register programs** against a **Linux-shaped API**, while GreenJade keeps:

- capabilities, IOMMU windows, IRQ notify, process isolation  
- no GPL code  
- drivers in a **userspace host process**

```text
┌──────────────────────────────────────────┐
│  driver .c  (probe, irq, dma, mmio)      │  ← porter lives here
├──────────────────────────────────────────┤
│  UDX runtime (this library)              │  ← hides caps / IPC
├──────────────────────────────────────────┤
│  devmgr + kernel (caps, VT-d, IRQ)       │  ← host inject for now
└──────────────────────────────────────────┘
```

---

## 2. Best combo (locked)

| Layer | Choice | Why |
|-------|--------|-----|
| **API names** | Linux-shaped (`probe`/`remove`, `ioremap`, `request_irq`, `dma_*`, `schedule_work`) | Porters already know the flow |
| **Teardown** | DDI-style **`quiesce`** + `remove` | Stop DMA/IRQ before process death |
| **IRQ** | **Threaded only** (kernel Notification → UDX handler) | No hard-IRQ driver code |
| **Authority** | **Caps hidden** — never expose mint/revoke to driver `.c` | Security first |
| **Develop** | **`udx/host.h` inject** on Linux host | Write drivers before GJ kernel is ready |
| **License** | Clean-room MIT/Apache C only | No GPL import |

This is the **best combo for Linux porters**: familiar surface, Solaris lifecycle teeth, GreenJade security underneath.

---

## 3. Non-negotiables for porters

| Rule | Detail |
|------|--------|
| **No Linux source** | Specs + manuals only; clean-room |
| **No kernel modules** | One **host process** per device (or function) |
| **No raw caps in driver .c** | Use `udx_*` only |
| **Hard IRQ is not in the driver** | Kernel notifies; UDX runs **threaded** handler |
| **Quiesce on remove/crash** | Like DDI `quiesce` + Linux `remove` |

---

## 4. Linux → UDX map

| Linux | UDX |
|-------|-----|
| `module_init` / `exit` | `udx_init` + register from `main` / `udx_exit` |
| `pci_register_driver` | `udx_pci_register_driver` |
| `probe` / `remove` | same names on `struct udx_pci_driver` |
| *(DDI)* `quiesce` | `pfnQuiesce` on `struct udx_pci_driver` |
| `pci_enable_device` | `udx_pci_enable` |
| `pci_request_regions` | `udx_pci_request_regions` |
| `ioremap` / `iounmap` | `udx_ioremap` / `udx_iounmap` |
| `readl` / `writel` | `udx_readl` / `udx_writel` |
| `request_irq` | `udx_request_irq` (threaded) |
| `free_irq` | `udx_free_irq` |
| `dma_alloc_coherent` | `udx_dma_alloc_coherent` |
| `dma_map_single` | `udx_dma_map_single` |
| `dma_unmap_single` | `udx_dma_unmap_single` |
| `schedule_work` | `udx_schedule_work` |
| `spin_lock_irqsave` | `udx_spin_lock` (host context; IRQs already threaded) |
| `printk` | `udx_printk` |
| `scsi_host_alloc` … | via `scsi_mid` later (`udx_scsi_*` placeholders) |

---

## 5. Host model

| Piece | Rule |
|-------|------|
| Process | One **driver host**; one process-shared CNode (kernel) |
| Caps | **devmgr** passes MMIO/IRQ/DMA via spawn (hidden in UDX dev) |
| Crash | Process death ⇒ kernel revokes windows; **quiesce** best-effort in `remove` |
| Threads | IRQ thread(s) + optional work; all share host state with locks |
| **Host sim** | `udx_host_inject_pci` / `udx_host_fire_irq` for bring-up on Linux |

---

## 6. IRQ model (Linux threaded-IRQ mental model)

```text
Hardware IRQ
  → kernel: mask/ack as needed, Notification to host
  → UDX irq path: calls driver handler(void *dev_id)
  → driver: read device, schedule_work, kick ring

Host sim:
  udx_host_fire_irq(n) → same dispatch
```

Do **not** write “hard IRQ only” Linux patterns that assume non-preemptible kernel BH.

---

## 7. DMA model

| Rule | Detail |
|------|--------|
| Window | Established by devmgr/IOMMU (VT-d); UDX uses it |
| Host sim | Coherent buffer ≈ `calloc`; DMA addr cookie = CPU pointer |
| API shape | Linux `dma_*` names |
| Direction | `UDX_DMA_TO_DEVICE`, `FROM_DEVICE`, `BIDIRECTIONAL` |
| Coherent | `udx_dma_alloc_coherent` |

---

## 8. PCI id tables

```c
static const struct udx_pci_device_id my_ids[] = {
    { UDX_PCI_DEVICE(0x8086, 0x100e) }, /* real IDs from docs */
    { 0, }
};
```

Match performed by UDX; `probe` gets `struct udx_pci_dev *`.

Host inject:

```c
u64 aBarLen[6] = { 0x1000 }; /* BAR0 size */
udx_host_inject_pci(0x8086, 0x100e, 0, 0, 11, aBarLen, NULL, &pPdev);
```

---

## 9. What drivers must not call

- Cap mint/revoke/retype  
- `set_pager` / root meta  
- Raw IPC to kernel endpoints  
- Manual IOMMU page table programming  

---

## 10. Clean-room reminder

Port **behavior** from public specs (PCI, NVMe, virtio, vendor manuals).  
Do **not** paste Linux `.c` files. When in doubt, rewrite.

---

## 11. Code layout

```text
user/udx/
  include/udx/
    udx.h          — umbrella + best-combo notes
    types.h
    device.h
    pci.h
    irq.h
    dma.h
    mmio.h
    work.h
    host.h         — inject / fire_irq (host bring-up)
  src/
    core.c
    host.c
    pci.c
    irq.c
    dma.c
    mmio.c
    work.c
  examples/
    skeleton_pci_driver.c
```

```sh
make udx           # build/libudx.a
make udx-example   # build/udx_skeleton
./build/udx_skeleton
```

---

## 12. Status vs full GreenJade

| Piece | Host (now) | Full GJ |
|-------|------------|---------|
| PCI match/probe | inject + id table | devmgr grants |
| MMIO | host window table | MMIO FRAME map |
| IRQ | `udx_host_fire_irq` | Notification cap |
| DMA | malloc cookie | IOMMU window |
| Work | queue + `udx_run` flush | host work thread |
| Caps | hidden (none) | hidden (real) |

---

## 13. Soft status / honesty bounds (Wave 62 · 2026-07-24)

**Additive only (Wave 62 exclusive for this file).** Porter contract 1–12 stay **Accepted**. This section is a Wave 62 honesty ledger: host/sim UDX soft path vs full GreenJade grants, multi-server product, and Steam **bar3**. Wave 62 soft-status text is superseded here under Wave 62 exclusive ownership of this file’s honesty ledger. It does **not** re-litigate the Linux-shaped API map or invent product driver completion.

| Item | State |
|------|--------|
| **UDX host soft path** | Present — `make udx` → `libudx.a`, host inject / fire_irq, skeleton example |
| **Full GJ grants (PCI/IRQ/DMA)** | **Open** — see 12 Host vs Full GJ |
| **Product multi-server confine** | **Open** — UDX soft path does **not** close multi-server confine product |
| **Soft continuum** | High-water **advancing toward 20400** CREATE-ONLY parent wire only — **not** UDX product; **not** bar3 |
| **Product lamps** | Remain **0** by design |
| **bar3 / Steam client** | **Out of scope** for this doc — no claim; **bar3 remains OPEN** |

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Host inject / `udx_host_fire_irq` / skeleton | Clean-room Linux-shaped porter develop path | Full GJ MMIO FRAME / IOMMU window / Notification product |
| `make udx` / `libudx.a` | Host archive for porters | Multi-server confined driver host product |
| Continuum **toward 20400** | Soft graph wire only | UDX product complete; Steam client; matrix fill |

**Hard stamp (Wave 62):** UDX host soft path **≠** full GJ grants **≠** product multi-server **≠** bar3. Soft continuum **toward 20400** does **not** close UDX product. Product lamps **0**. **Soft ≠ product complete.**

**Soft stamp (2026-07-24 / Wave 62):** UDX remains a **host/sim soft path** for clean-room Linux-shaped ports. Host inject ≠ product multi-server confine. Multi-server confine product path stays **open**. Continuum soft gates **≠** UDX product. **No bar3 claim.**

### Explicit non-claims (Wave 62)

| Claim | Allowed? |
|-------|----------|
| “UDX Linux-shaped porter surface **Accepted**” | **Yes** — this document |
| “Host soft path greppable (`make udx`, skeleton)” | **Yes** — host/sim only |
| “Full GJ PCI/IRQ/DMA grants product-complete” | **No** |
| “UDX soft path = multi-server confine closed” | **No** |
| “Continuum toward 20400 closes UDX product or bar3” | **No** |
| Any **bar3** closed claim from UDX alone | **No** |

---

*UDX = Linux-shaped surface, DDI quiesce, GreenJade security underneath.*  
*13 Wave 62 honesty (2026-07-24): host soft path ≠ full GJ grants / multi-server / bar3; continuum **toward 20400 soft only**; product lamps **0**; **soft ≠ product complete**.*
