# GreenJade UDX — Linux Driver Porter Contract

| Field | Value |
|-------|--------|
| **Document** | UDX Linux porter guide v1.2 |
| **Status** | **Accepted** surface for clean-room ports |
| **License** | Dual **MIT OR Apache-2.0** only — **no Linux source** · **no GPL** |
| **Law** | **G-AC-1** · **Soft ≠ product** · freestanding class drivers **SKIP** · product = **hot+cold ABI + UDX/DDI userspace** |
| **Code** | `user/udx/` |
| **Build** | `make udx` → `build/libudx.a` · `make udx-example` |
| **Companion** | [DDI_SOFT.md](DDI_SOFT.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) (games ICD uses GPU host; drivers stay UDX) |
| **Operator pivot (2026-08)** | **Stop freestanding rtl rabbit hole.** Freestanding class **SKIP** live. Dual DoD **A/B** = **UDX USB / UDX NIC** — both **OPEN**. Fly bar honesty: **v0.1.184** packed, not host-probed (do not invent next N). Dual DoD **A** park RS-off (never `USBCMD.RS=1`). Dual DoD **B** until host **interactive SSH login**. **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical. **0.2.0** reserved. No test-panel photo IDs. |
| **Honesty** | Host soft path ≠ full GJ grants / multi-server / bar3; Soft ≠ product complete; product lamps **0** for UDX datapath until evidence |

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

**Product path (normative):** hot + cold **Linux ABI** + **Linux-shaped UDX/DDI userspace** drivers.  
**Not product:** freestanding in-kernel class thrash (`rtl8168.c` / `xhci_msc.c`) · in-kernel Linux `.ko` wire (**G-AC-1**).

---

## 2. Best combo (locked)

| Layer | Choice | Why |
|-------|--------|-----|
| **API names** | Linux-shaped (`probe`/`remove`, `ioremap`, `request_irq`, `dma_*`, `schedule_work`) | Porters already know the flow |
| **Teardown** | DDI-style **`quiesce`** + `remove` | Stop DMA/IRQ before process death |
| **IRQ** | **Threaded only** (kernel Notification → UDX handler) | No hard-IRQ driver code |
| **Authority** | **Caps hidden** — never expose mint/revoke to driver `.c` | Security first |
| **Develop** | **`udx/host.h` inject** on Linux host | Write drivers before GJ kernel is ready |
| **License** | Clean-room dual **MIT OR Apache-2.0** C only | No GPL import |

This is the **best combo for Linux porters**: familiar surface, Solaris lifecycle teeth, GreenJade security underneath.

---

## 3. Non-negotiables for porters

| Rule | Detail |
|------|--------|
| **No Linux source** | Specs + manuals only; clean-room |
| **No kernel modules as product** | One **host process** per device (or function). **G-AC-1:** no Linux `.ko` **runs in kernel** as product AC |
| **No raw caps in driver .c** | Use `udx_*` only |
| **Hard IRQ is not in the driver** | Kernel notifies; UDX runs **threaded** handler |
| **Quiesce on remove/crash** | Like DDI `quiesce` + Linux `remove` |
| **Soft ≠ product** | Soft probe / inject / soft ddi lamps **do not** close Dual DoD A/B or TX/RX/BOT |
| **Freestanding SKIP** | In-kernel freestanding class drivers are **not** the porter product path (default `GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`) |

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
| **GJ soft bind** | `udx_host_bind_by_id` → SCAN / GET / OPEN / MAP_BAR (`GJ_SYS_DDI`) when kernel present |

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

G752 class skeletons (clean-room, dual-license): `user/drivers/rtl8168_udx` (`10ec:8168`) · `user/drivers/xhci_udx` (`8086:a12f`). Soft probe ≠ product wire / BOT.

---

## 9. What drivers must not call

- Cap mint/revoke/retype  
- `set_pager` / root meta  
- Raw IPC to kernel endpoints  
- Manual IOMMU page table programming  
- Linux `.ko` load / in-kernel module product paths (**G-AC-1**)

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
| PCI match/probe | inject + id table · soft SCAN/GET/OPEN | devmgr grants |
| MMIO | host window table · soft MAP_BAR | MMIO FRAME map |
| IRQ | `udx_host_fire_irq` | Notification cap |
| DMA | malloc cookie · soft DMA_NOTE | IOMMU window |
| Work | queue + `udx_run` flush | host work thread |
| Caps | hidden (none) | hidden (real) |

---

## 13. Soft status / honesty bounds (2026-08 operator pivot)

**Porter contract 1–12 stay Accepted.** This section is the honesty ledger under the 2026-08 operator pivot: host/sim UDX soft path vs full GreenJade grants, Dual DoD A/B, freestanding SKIP, and Steam **bar3**. Soft ≠ product. It does **not** re-litigate the Linux-shaped API map or invent product driver completion.

### Product model (normative)

| Rule | Meaning |
|------|---------|
| **G-AC-1** | No Linux `.ko` binary **runs in the kernel** as product |
| **Product** | Hot + cold **Linux ABI** + **Linux-shaped UDX/DDI userspace** drivers |
| **Freestanding class drivers** | **Not product.** Default **SKIP** (`GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`). **Stop freestanding rtl rabbit hole.** |
| **Soft residual** | Soft bind / inject / soft ddi lamps / module eng path ≠ product AC |

### Dual DoD A/B (retargeted — freestanding net/USB out)

| # | DoD | Status | Close when |
|---|-----|--------|------------|
| **A** | **Linux-shaped USB** (laptop) via **UDX/DDI** (`xhci_udx` …) | **OPEN** | Host USB path. RS-off / scratchpad ≠ close. Never `USBCMD.RS=1` unless the operator named that experiment. **Not** freestanding MSC. **Not** `usb_storage.ko` init in kernel. Soft lamps alone ≠ close. |
| **B** | **Linux-shaped NIC** (laptop) via **UDX/DDI** (`rtl8168_udx` …) | **OPEN** | Host **interactive SSH login**. Banner / :22 / PK_OK / SUCCESS ≠ login. Wire hop is `rtl8168_udx`. **Not** freestanding rtl R-climb. **Not** in-kernel `r8169.ko` wire. Soft lamps alone ≠ close. |

Flash bar honesty for operator media: panel **`STATUS (static) v0.1.184`** packed, not host-probed from `GJ_IMAGE_VERSION` (`kernel/include/gj/config.h`). **Confirm stamp after flash** in docs. Confirm cut after flash. **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical. Public evidence = lamps / serial only — **no test-panel photo IDs**.

| Item | State |
|------|--------|
| **UDX host soft path** | Present — `make udx` → `libudx.a`, host inject / fire_irq, skeleton + class soft hosts |
| **Full GJ grants (PCI/IRQ/DMA)** | **Open** — see 12 Host vs Full GJ |
| **Product multi-server confine** | **Open** — UDX soft path does **not** close multi-server confine product |
| **Dual DoD A (UDX USB)** | **OPEN** |
| **Dual DoD B (UDX NIC)** | **OPEN** until host **interactive SSH login** — laptop ARP/ping **proven** (2026-08-14) |
| **Freestanding rtl / freestanding USB** | **SKIP default** — residual opt-in only; **not** Dual DoD close |
| **Product lamps (TX/RX · BOT · live IRQ)** | Remain **0** / **OPEN** by design until evidence |
| **bar3 / Steam client** | **Out of scope** for this doc — no claim; **bar3 remains OPEN** |

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Host inject / `udx_host_fire_irq` / skeleton | Clean-room Linux-shaped porter develop path | Full GJ MMIO FRAME / IOMMU window / Notification product |
| Soft DDI bind (`udx_host_bind_by_id`) | Soft SCAN/GET/OPEN/MAP_BAR path | Dual DoD A/B close; product TX/RX or BOT |
| `make udx` / `libudx.a` | Host archive for porters | Multi-server confined driver host product |
| Soft class probes (`rtl8168_udx` / `xhci_udx`) | Soft skeleton path | Product wire ownership; freestanding substitute |

**Hard stamp (2026-08):** UDX host soft path **≠** full GJ grants **≠** product multi-server **≠** Dual DoD A/B closed **≠** bar3. Freestanding class **SKIP**. Product = **ABI + UDX**. **Soft ≠ product complete.**

### Explicit non-claims (2026-08)

| Claim | Allowed? |
|-------|----------|
| “UDX Linux-shaped porter surface **Accepted**” | **Yes** — this document |
| “Host soft path greppable (`make udx`, skeleton)” | **Yes** — host/sim only |
| “Full GJ PCI/IRQ/DMA grants product-complete” | **No** |
| “UDX soft path = Dual DoD A/B closed” | **No** |
| “Freestanding rtl closes Dual DoD B / product NIC” | **No** — freestanding **SKIP**; product NIC = UDX |
| “In-kernel `r8169.ko` / freestanding MSC is product” | **No** — **G-AC-1** / freestanding SKIP |
| Any **bar3** closed claim from UDX alone | **No** |
| Flash bar **v0.1.184** without matching cut / invent next N | **No** — honesty: fly **v0.1.184** packed, not host-probed only · **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical · **0.2.0** reserved |

---

*UDX = Linux-shaped surface, DDI quiesce, GreenJade security underneath.*  
*Dual MIT OR Apache-2.0 · **G-AC-1** · **Soft ≠ product** · freestanding **SKIP** · product = **ABI + UDX**.*  
*Dual DoD **A** OPEN until host USB path · **B** OPEN until interactive SSH login. Stop freestanding rtl rabbit hole.*  
*Flash bar honesty: **STATUS (static) v0.1.184** packed, not host-probed — Dual DoD **A** park RS-off (never `USBCMD.RS=1`) · Dual DoD B **OPEN** until interactive SSH login · **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical · do not invent next N · **0.2.0** reserved. No test-panel photo IDs.*  
*13 honesty (2026-08): host soft path ≠ full GJ grants / multi-server / Dual DoD close / bar3.*
