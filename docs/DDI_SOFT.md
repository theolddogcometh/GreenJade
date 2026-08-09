# GreenJade — Soft DDI / devmgr (ABI-first stub)

| Field | Value |
|-------|--------|
| **Status** | **Soft stub** — types + lamps only; not product match/grant graph |
| **Law** | Dual **MIT OR Apache-2.0** only; **no GPL source in tree**; **no Linux `.ko` product AC** (**G-AC-1**); **Soft ≠ product** |
| **Product** | Hot + cold **Linux ABI** + **Linux-shaped UDX/DDI userspace** drivers |
| **Freestanding** | Class drivers **SKIP** live (`GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`) — **not** product; **not** Dual DoD close; **stop freestanding rtl rabbit hole** |
| **Dual DoD A/B** | **A** UDX USB **OPEN** · **B** UDX NIC **OPEN** (soft DDI ≠ close; Dual DoD = UDX path) |
| **Code** | `kernel/include/gj/devmgr.h` · `kernel/drv/devmgr_soft.c` · `kernel/drv/ddi_door.c` |
| **Companions** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) · [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) · [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [HCL.md](HCL.md) |
| **Flash bar honesty** | Panel **`STATUS (static) v2026.08.04.72`** (`GJ_IMAGE_VERSION`) — DUT cut; freestanding **SKIP** live; Dual DoD A/B **OPEN**. Do **not** invent .73. No test-panel photo IDs. |

## One-sentence goal

**Publish a greppable soft devmgr surface (PCI enumerate note + DMA window grant note + DDI door ops) so the ABI-first pivot can host userspace UDX driver hosts later — without freestanding class-driver thrash, without freestanding rtl rabbit holes, and without shipping GPL.**

## Why soft, not product

| Soft stub claims | Does **not** claim |
|------------------|--------------------|
| `devmgr: soft init PASS` | Full `devmgr` userspace server |
| Soft PCI scan / found ID lamps | Match graph / personality attach / Dual DoD A/B close |
| DMA window **note** (software table) | Live VT-d program + mintable window caps |
| Types for BDF / BAR / grant result | MMIO_FRAME / IRQ / DMA cap install to hosts |
| `ddi_door` soft SCAN/GET/OPEN/MAP_BAR | Product CNode mint or product TX/RX/BOT |

Product order remains [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md): Linux ABI → virtio T0 apps → **DDI caps** → real-HW **userspace UDX** class drivers (clean-room or out-of-tree dual-license).

**Soft ≠ product.** Soft DDI bind lamps on laptop do **not** close Dual DoD **A** (UDX USB) or **B** (UDX NIC).

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

### Soft vs inject vs product

| Mode | Use |
|------|-----|
| **UDX host inject** (Linux bring-up) | Write drivers before full GJ grants exist |
| **This kernel soft stub** | Greppable init/scan/window-note + DDI door soft ops; type ABI seed |
| **Product** | `devmgr` match → grant → confined host with live IOMMU + IRQ Notification + **UDX datapath** |

---

## 2. No GPL / no Linux `.ko` / freestanding SKIP

| Allowed | Forbidden |
|---------|-----------|
| Clean-room register programs from public specs / manuals | GPL or Linux kernel **source** in the GreenJade tree |
| Out-of-tree **dual MIT/Apache or BSD** drivers against published GJ DDI / UDX | Shipping GPL drivers as project product |
| Userspace stacks over **cap-gated** PCI/MMIO/IRQ/DMA | Linux **`.ko` product AC** (**G-AC-1**) |
| Operator **linux-hwtest** inventory media | Claiming inventory PASS = freestanding T1 product |
| Soft DDI / UDX bind lamps | Claiming soft bind = Dual DoD A/B closed |

Law source: [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md), project dual license (MIT OR Apache-2.0).

### Freestanding disposition (2026-08 operator)

| Code | Default | Product bar? |
|------|---------|--------------|
| `kernel/drv/rtl8168.c` | **`GJ_RTL8168_PROBE=0` SKIP** | **No** — stop freestanding rtl rabbit hole; product NIC = **`rtl8168_udx`** userspace |
| `kernel/drv/xhci_msc.c` | **`GJ_XHCI_MSC_PROBE=0` SKIP** | **No** — product USB = **`xhci_udx`** userspace |
| Soft module path / `RUN_INIT=0` / `freestanding_no_exec` | Eng residual: soft SKIP **exec** of staged `.ko` init in kernel | **No** — not “never use Linux-shaped drivers”; product stays **userspace UDX** |

In-kernel freestanding class drivers remain **lab residual only** (opt-in). T0 product I/O stays **virtio** until UDX hosts ship real HW via userspace. **Freestanding is not Dual DoD close.**

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

| Soft API today (`devmgr.h` / `ddi_door`) | Product later |
|------------------------------------------|---------------|
| `struct gj_devmgr_bdf` / `gj_devmgr_bar` | Device nub + BAR objects under match graph |
| `devmgr_soft_pci_scan` → soft inventory / found lamps | Single inventory + match; drivers do **not** walk full PCI |
| `devmgr_soft_dma_window_note` | Mint window cap; optional `iommu_window_grant` / VT-d cover |
| Soft `u32WinId` / soft OPEN/MAP_BAR | Real cap slot + gen ([CAP_ADDRESSING.md](CAP_ADDRESSING.md)) |

**Fail closed:** no IOMMU on a production device class ⇒ **no bus-master** (QEMU/dev profile may warn-and-allow). Storm control: mask IRQ + notify `devmgr` ([SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md) §7).

---

## 4. Soft kernel API (this drop)

Header: `kernel/include/gj/devmgr.h`  
Implementation: `kernel/drv/devmgr_soft.c`  
Door: `kernel/drv/ddi_door.c` · `GJ_SYS_DDI=103`

| Symbol | Behavior |
|--------|----------|
| `devmgr_soft_init` | Idempotent; logs **`devmgr: soft init PASS`** |
| `devmgr_soft_ready` | Non-zero after init |
| `devmgr_soft_pci_scan` | Soft inventory (real CF8/CFC walk when wired); may log **`devmgr: soft pci scan PASS n=`** / **`devmgr: soft found …`** |
| `devmgr_soft_dma_window_note` | Records BDF + PA range in soft table; logs **`devmgr: soft dma window note`**; **no** cap mint |
| `gj_devmgr_window_grant` | Soft result (`u8Ok`, `u32WinId`, range) |
| `ddi_door_syscall` | Soft SCAN / GET / OPEN / MAP_BAR / CFG / DMA_NOTE ops |

Greppable markers (stable for smoke; soft ≠ product):

```text
devmgr: soft init PASS
devmgr: soft pci scan PASS n=
devmgr: soft found 10ec:8168
devmgr: soft found 8086:a12f
devmgr: soft dma window note
ddi_door: soft init PASS
ddi_door: soft product surface PASS
ddi_door: soft scan count=
```

### Makefile link line (coordinator)

Do **not** ship this edit in the soft-only drop; coordinator wires:

```make
kernel/drv/devmgr_soft.c \
kernel/drv/ddi_door.c \
```

Object expected:

```text
build/kernel/drv/devmgr_soft.o
build/kernel/drv/ddi_door.o
```

Call site (optional bring-up): after `iommu_probe` / before class soft smoke, `devmgr_soft_init()` then optional `devmgr_soft_pci_scan(…)`.

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
| Id table / probe shape | UDX + `skeleton_pci_driver.c` + class soft hosts | Same; real grants |
| Match | Host inject / soft DDI bind | **devmgr** personality match |
| MMIO | Host inject map / soft MAP_BAR | MMIO_FRAME cap map |
| IRQ | `udx_host_fire_irq` / soft MSI-X | Notification badge from kernel |
| DMA | Host calloc cookie / soft DMA_NOTE | Window cap + IOMMU |
| License gate | CI / review: dual only | Same; **never** GPL product ship |

Out-of-tree trees stay **separate**; GreenJade only consumes dual-license binaries/headers against the published contract. No Linux driver source import.

### Dual DoD A/B (what soft DDI does **not** close)

| # | DoD | Status | Soft DDI role |
|---|-----|--------|---------------|
| **A** | **UDX USB** (laptop Linux-shaped USB host) | **OPEN** | Soft scan/bind for `8086:a12f` is inventory only — not BOT/MSC product |
| **B** | **UDX NIC** (laptop Linux-shaped NIC host) | **OPEN** | Soft scan/bind for `10ec:8168` is inventory only — not TX/RX product |

Close criteria live on **userspace UDX** paths (`xhci_udx` / `rtl8168_udx` + DDI caps), not freestanding stages and not soft door notes. Operator runbook: [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md).

---

## 6. Honesty

| Claim | Status |
|-------|--------|
| Soft DDI types + init/scan/window-note / door lamps | **In tree** (`devmgr_soft.c` / `ddi_door.c` / `gj/devmgr.h`) |
| Product multi-server `devmgr` | **Not claimed** |
| Live cap mint of MMIO/IRQ/DMA to hosts | **Not claimed** |
| Freestanding xHCI/rtl as T1 product | **No** — **SKIP** default; lab residual only; T0 = virtio |
| Dual DoD **A** UDX USB / **B** UDX NIC | **OPEN** — soft ≠ close |
| bar3 / Steam | **OPEN** — unrelated to this stub |
| GPL / `.ko` product | **Forbidden** (**G-AC-1**) |
| Flash bar | Honesty: **v2026.08.04.72** — freestanding SKIP live; Dual DoD OPEN; do not invent .73 |
| Wave inventory cross-ref | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) · [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) |

| Soft status (2026-08 operator) | |
|--------------------------------|--|
| Class | **Soft stub** — lamps + types + door soft ops only |
| Product bar | **Open** — grant graph / host CNode mint later |
| Next under ABI-first | After Linux ABI + virtio apps: real MMIO/IRQ/DMA caps → UDX Dual DoD A/B |
| Laptop soft host | [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) — G752VT UDX path; **G-AC-1** no `.ko`; freestanding **SKIP** |
| Photo IDs | **None** — lamps / serial only |

---

## 7. Door / sys-op notes (soft → product)

Soft DDI is not yet a full door server. Operators and porters should expect this shape:

| Surface | Soft today | Product later |
|---------|------------|---------------|
| **devmgr soft API** | In-kernel `devmgr_soft_*` lamps + tables (`devmgr.h`) | Same policy owner; mint into host CNode |
| **`ddi_door` (soft)** | Greppable door/sys-op path for DDI-shaped ops (`GJ_SYS_DDI=103`) | Live door: match / grant / revoke / quiesce coordination |
| **Sys-op class** | Observation / inventory / soft open-map ops; fail closed on real grant | MMIO_FRAME map · IRQ→Notification bind · DMA window create/destroy |
| **UDX host bind** | Host inject + soft ddi markers (`udx: soft ddi-*`); no cap rights | Spawn host with granted slots only; driver `.c` never mints |
| **Lifecycle sys-ops** | Soft catalog: probe / remove / **quiesce** (DDI spirit) | Server enforces quiesce-before-revoke; crash → Phase-A revoke |

```text
soft:   app/host  →  ddi_door note / soft sys-op  →  lamps only
product: host     →  door_call(DDI_OP_*)          →  devmgr mints caps
                  →  UDX maps MMIO / waits IRQ Notification / DMA in window
```

| Operator grep (soft host path) | Product claim? |
|--------------------------------|----------------|
| `devmgr: soft pci scan…` / `soft found …` | **No** |
| `ddi_door` soft serve/call / open / map | **No** — door note ≠ grant graph ≠ Dual DoD close |
| `udx` bind / `udx: soft ddi-*` | **No** |
| Live window cap + VT-d cover | **Only** when product mint lands |

**Fail closed:** unknown DDI op → error; no ambient bus-master; no Linux `.ko` load path (**G-AC-1**).

G752VT class soft hosts (`rtl8168_udx` / `xhci_udx`) sit **above** this door — they do not replace it. See [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md). Freestanding `rtl8168` / `xhci_msc` are **SKIP** residual, not Dual DoD owners.

### Soft xHCI BOT progress (dual-license host only)

`user/drivers/xhci_udx` deepens the soft host path **without** freestanding `xhci_msc` thrash and **without** GPL:

| Soft stage lamp | Meaning | Product? |
|-----------------|---------|----------|
| `xhci_udx: soft bot stage=1 cap` | CAPLENGTH/HCIVERSION soft snapshot | **No** |
| `xhci_udx: soft bot stage=2 params` | HCSPARAMS1 MaxPorts/MaxSlots | **No** |
| `xhci_udx: soft bot stage=3 ports` | Soft PORTSC CCS walk | **No** |
| `xhci_udx: soft bot stage=4 stub` / `soft bot stub PASS` | BOT **catalog** only (next=slot/addr/enum/bot OPEN) | **No** |

Freestanding bind still uses DDI SCAN/OPEN/MAP_BAR (`udx_host_bind_by_id` → `8086:a12f`) when silicon present. Soft stage=4 **≠** stick access; product BOT remains OPEN; Dual DoD **A** remains **OPEN**.

*Soft confine ≠ product multi-server; soft window note ≠ VT-d enforce; soft ddi_door ≠ live grant; soft bot stub ≠ MSC; soft DDI ≠ Dual DoD A/B closed.*  
*Dual MIT OR Apache-2.0 · **G-AC-1** · **Soft ≠ product** · freestanding **SKIP** · product = **ABI + UDX**.*  
*Dual DoD **A** UDX USB **OPEN** · **B** UDX NIC **OPEN**. Stop freestanding rtl rabbit hole.*  
*Flash bar honesty: **STATUS (static) v2026.08.04.72** — freestanding **SKIP** live · Dual DoD A/B **OPEN** · do not invent .73. No test-panel photo IDs.*
