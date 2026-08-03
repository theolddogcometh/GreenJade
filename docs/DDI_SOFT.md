# GreenJade — Soft DDI / devmgr (ABI-first stub)

| Field | Value |
|-------|--------|
| **Status** | **Soft stub** — types + lamps only; not product match/grant graph |
| **Law** | Dual MIT OR Apache-2.0 only; **no GPL source in tree**; **no Linux `.ko` product AC** |
| **Code** | `kernel/include/gj/devmgr.h` · `kernel/drv/devmgr_soft.c` |
| **Companions** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) · [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [HCL.md](HCL.md) |

## One-sentence goal

**Publish a greppable soft devmgr surface (PCI enumerate note + DMA window grant note) so the ABI-first pivot can host userspace driver hosts later — without freestanding class-driver thrash or shipping GPL.**

## Why soft, not product

| Soft stub claims | Does **not** claim |
|------------------|--------------------|
| `devmgr: soft init PASS` | Full `devmgr` userspace server |
| PCI scan **deferred** (no duplicate bus walk) | Match graph / personality attach |
| DMA window **note** (software table) | Live VT-d program + mintable window caps |
| Types for BDF / BAR / grant result | MMIO_FRAME / IRQ / DMA cap install to hosts |

Product order remains [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md): Linux ABI → virtio T0 apps → **DDI caps** → real-HW class drivers (clean-room or out-of-tree dual-license).

---

## 1. Userspace driver host model

```text
┌─────────────────────────────────────────────────────────────┐
│  out-of-tree dual-license driver .c  (probe/remove/quiesce) │
├─────────────────────────────────────────────────────────────┤
│  UDX runtime (`user/udx/`) — Linux-shaped API, caps hidden  │
├─────────────────────────────────────────────────────────────┤
│  driver host process — one CNode; only granted rights       │
├─────────────────────────────────────────────────────────────┤
│  devmgr (policy) + kernel (mechanism)                       │
│    device untyped → MMIO / IRQ Notification / DMA windows   │
└─────────────────────────────────────────────────────────────┘
```

| Piece | Rule |
|-------|------|
| **Where drivers run** | **Userspace host process** per device (or function) — not kernel modules |
| **API surface** | UDX Linux-shaped names (`probe` / `remove` / `pfnQuiesce`, `ioremap`, threaded IRQ, `dma_*`) — [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) |
| **Authority** | Caps only; **devmgr** mints MMIO / IRQ / DMA; driver `.c` never sees mint/revoke |
| **Lifecycle** | Solaris DDI spirit: **attach / detach / quiesce** — stop DMA and mask IRQ before process death |
| **Crash** | Process death ⇒ kernel Phase-A revoke of windows / maps / IRQ binds ([SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) 1.1, 0) |
| **Hard IRQ** | Kernel only notifies; **no** driver logic in hard IRQ (threaded UDX handler) |

### Soft vs inject

| Mode | Use |
|------|-----|
| **UDX host inject** (Linux bring-up) | Write drivers before full GJ grants exist |
| **This kernel soft stub** | Greppable init/scan/window-note lamps; type ABI seed |
| **Product** | `devmgr` match → grant → confined host with live IOMMU + IRQ Notification |

---

## 2. No GPL / no Linux `.ko`

| Allowed | Forbidden |
|---------|-----------|
| Clean-room register programs from public specs / manuals | GPL or Linux kernel **source** in the GreenJade tree |
| Out-of-tree **dual MIT/Apache or BSD** drivers against published GJ DDI / UDX | Shipping GPL drivers as project product |
| Userspace stacks over **cap-gated** PCI/MMIO/IRQ/DMA | Linux **`.ko` product AC** (G-AC-1) |
| Operator **linux-hwtest** inventory media | Claiming inventory PASS = freestanding T1 product |

Law source: [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md), project dual license (MIT OR Apache-2.0).

In-kernel freestanding class drivers (`xhci_msc`, `rtl8168`, …) remain **soft / lab** scaffolds — T0 product I/O stays **virtio** until DDI hosts ship real HW via userspace.

---

## 3. Caps for MMIO / IRQ / DMA

Authority model: [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) §0–1, §7–8, §9.5–9.6; DMA windows: [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) §12.

| Cap class | Who mints | Host may |
|-----------|-----------|----------|
| **MMIO_FRAME** (device map) | `devmgr` from device untyped | Map BAR pages into host AS only for granted range |
| **IRQ → Notification** | `devmgr` / kernel IRQ bind | Receive badge; threaded handler; no hard-IRQ work |
| **DMA window** | `devmgr` + IOMMU authority | Bus-master only inside window; revoke disables HW first |

```text
create_window(device, iova, frames[], rw) → window_cap
  only with IOMMU authority from devmgr
destroy/revoke window → disable HW first (Phase A), then free
```

| Soft API today (`devmgr.h`) | Product later |
|-----------------------------|---------------|
| `struct gj_devmgr_bdf` / `gj_devmgr_bar` | Device nub + BAR objects under match graph |
| `devmgr_soft_pci_scan` → deferred | Single inventory + match; drivers do **not** walk full PCI |
| `devmgr_soft_dma_window_note` | Mint window cap; optional `iommu_window_grant` / VT-d cover |
| Soft `u32WinId` | Real cap slot + gen ([CAP_ADDRESSING.md](CAP_ADDRESSING.md)) |

**Fail closed:** no IOMMU on a production device class ⇒ **no bus-master** (QEMU/dev profile may warn-and-allow). Storm control: mask IRQ + notify `devmgr` ([SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) §7).

---

## 4. Soft kernel API (this drop)

Header: `kernel/include/gj/devmgr.h`  
Implementation: `kernel/drv/devmgr_soft.c`

| Symbol | Behavior |
|--------|----------|
| `devmgr_soft_init` | Idempotent; logs **`devmgr: soft init PASS`** |
| `devmgr_soft_ready` | Non-zero after init |
| `devmgr_soft_pci_scan` | Soft stub: logs **`devmgr: soft pci scan deferred`**; returns 0 devices (does not re-walk CF8/CFC) |
| `devmgr_soft_dma_window_note` | Records BDF + PA range in soft table; logs **`devmgr: soft dma window note`**; **no** cap mint |
| `gj_devmgr_window_grant` | Soft result (`u8Ok`, `u32WinId`, range) |

Greppable markers (stable for smoke):

```text
devmgr: soft init PASS
devmgr: soft pci scan deferred
devmgr: soft dma window note
```

### Makefile link line (coordinator)

Do **not** ship this edit in the soft-only drop; coordinator wires:

```make
kernel/drv/devmgr_soft.c \
```

Object expected:

```text
build/kernel/drv/devmgr_soft.o
```

Call site (optional bring-up): after `iommu_probe` / before class soft smoke, `devmgr_soft_init()` then optional `devmgr_soft_pci_scan(NULL, 0)`.

---

## 5. How out-of-tree dual-license drivers attach later

```text
1. Publish stable DDI / UDX headers (this repo: user/udx + future grant syscalls)
2. Porter writes clean-room driver .c (MIT OR Apache-2.0 or BSD) out of tree
3. Build as userspace host binary (not .ko); link libudx
4. Register id table → udx_pci_register_driver (match performed by host/devmgr)
5. On match: devmgr grants MMIO + IRQ Notification + DMA window into host CNode
6. probe() programs device; IRQs are threaded; DMA stays inside window
7. remove / crash → quiesce best-effort + kernel revoke windows
```

| Step | In-tree today | Later |
|------|---------------|-------|
| Id table / probe shape | UDX + `skeleton_pci_driver.c` | Same; real grants |
| Match | Host inject / soft | **devmgr** personality match |
| MMIO | Host inject map | MMIO_FRAME cap map |
| IRQ | `udx_host_fire_irq` / soft MSI-X | Notification badge from kernel |
| DMA | Host calloc cookie | Window cap + IOMMU |
| License gate | CI / review: dual only | Same; **never** GPL product ship |

Out-of-tree trees stay **separate**; GreenJade only consumes dual-license binaries/headers against the published contract. No Linux driver source import.

---

## 6. Honesty

| Claim | Status |
|-------|--------|
| Soft DDI types + init/scan/window-note lamps | **This drop** (in tree: `devmgr_soft.c` / `gj/devmgr.h`) |
| Product multi-server `devmgr` | **Not claimed** |
| Live cap mint of MMIO/IRQ/DMA to hosts | **Not claimed** |
| Freestanding xHCI/rtl as T1 product | **No** — lab soft; T0 = virtio |
| bar3 / Steam | **OPEN** — unrelated to this stub |
| GPL / `.ko` product | **Forbidden** (G-AC-1) |
| Wave inventory cross-ref | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) · [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) |

| Soft status (2026-08-02) | |
|--------------------------|--|
| Class | **Soft stub** — lamps + types only |
| Product bar | **Open** — grant graph / host CNode mint later |
| Next under ABI-first | After Linux ABI + virtio apps: real MMIO/IRQ/DMA caps |
| Laptop soft host | [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) — G752VT UDX path; **G-AC-1** no `.ko` |

---

## 7. Door / sys-op notes (soft → product)

Soft DDI is not yet a full door server. Operators and porters should expect this shape:

| Surface | Soft today | Product later |
|---------|------------|---------------|
| **devmgr soft API** | In-kernel `devmgr_soft_*` lamps + tables (`devmgr.h`) | Same policy owner; mint into host CNode |
| **`ddi_door` (soft)** | Greppable door/sys-op **note** path for DDI-shaped ops (observe only) | Live door: match / grant / revoke / quiesce coordination |
| **Sys-op class** | Observation / inventory ops only; fail closed on grant | MMIO_FRAME map · IRQ→Notification bind · DMA window create/destroy |
| **UDX host bind** | Host inject + soft ddi markers (`udx: soft ddi-*`); no cap rights | Spawn host with granted slots only; driver `.c` never mints |
| **Lifecycle sys-ops** | Soft catalog: probe / remove / **quiesce** (DDI spirit) | Server enforces quiesce-before-revoke; crash → Phase-A revoke |

```text
soft:   app/host  →  ddi_door note / soft sys-op  →  lamps only
product: host     →  door_call(DDI_OP_*)          →  devmgr mints caps
                  →  UDX maps MMIO / waits IRQ Notification / DMA in window
```

| Operator grep (soft host path) | Product claim? |
|--------------------------------|----------------|
| `devmgr: soft pci scan…` | **No** |
| `ddi_door` soft serve/call | **No** — door note ≠ grant graph |
| `udx` bind / `udx: soft ddi-*` | **No** |
| Live window cap + VT-d cover | **Only** when product mint lands |

**Fail closed:** unknown DDI op → error; no ambient bus-master; no Linux `.ko` load path (**G-AC-1**).

G752VT class soft hosts (`rtl8168_udx` / `xhci_udx`) sit **above** this door — they do not replace it. See [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md).

*Soft confine ≠ product multi-server; soft window note ≠ VT-d enforce; soft ddi_door ≠ live grant.*
