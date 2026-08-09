# UDX (userspace Linux-porter runtime)

Clean-room, Linux-shaped driver API for GreenJade. Porters write
`probe` / `remove` / `ioremap` / `request_irq` / `dma_*` / `schedule_work`
against this library; capabilities, IOMMU, and hard IRQ stay hidden.

| Field | Value |
|-------|--------|
| **Product** | `GREENJADE_UDX` * `UDX_PRODUCT` * v1.3 |
| **License** | MIT OR Apache-2.0 — **no Linux source**, **no GPL** |
| **Guide** | [docs/UDX_LINUX_PORTER.md](../../docs/UDX_LINUX_PORTER.md) |
| **Library** | `make udx` → `build/libudx.a` |
| **Host demo** | `make udx-example` → `build/udx_skeleton` |
| **G-AC-1** | No Linux `.ko` product AC; Soft!=product |
| **Dual DoD** | **A/B OPEN** — hosts: `rtl8168_udx` (NIC), `xhci_udx` (USB) |
| **Support** | [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog) |

## Product direction (normative honesty)

| Class | Status | Meaning |
|-------|--------|---------|
| **Freestanding class drivers** | **SKIP** | Kernel `rtl8168` / `xhci_msc` residual opt-in only — **not** product |
| **Product drivers** | **Linux-shaped userspace** | `rtl8168_udx` / `xhci_udx` over hot+cold ABI + DDI*UDX |
| **Caps (MMIO / IRQ / DMA)** | Soft honesty OPEN | Soft window / table / identity cookie != product mint |
| **Soft inventory** | Observation only | Never gates skeleton PASS or product DoD close |

```text
product path:  userspace UDX host  →  Cap MMIO_FRAME + IRQ Notification + DMA window
               freestanding bind residual (GJ_SYS_DDI 103):
                 SCAN → GET → OPEN → MAP preferred BARs
                   → install_granted → window_register → probe
                 preferred: rtl8168_udx BAR0+BAR2; xhci_udx BAR0; other BAR0
soft today:    inject / fire_irq / identity DMA  (lab; Soft!=product)
freestanding:  kernel class drivers default SKIP  (G-AC-1; not dual DoD close)
product=UDX+ABI: Linux-shaped userspace (rtl8168_udx / xhci_udx), not .ko
```

## ABI-first pivot (DDI host)

UDX is the **userspace driver host** for GreenJade’s ABI-first strategy
([docs/ABI_FIRST_PIVOT.md](../../docs/ABI_FIRST_PIVOT.md),
[docs/DDI_SOFT.md](../../docs/DDI_SOFT.md),
[docs/LAPTOP_LINUX_DRIVER_HOST.md](../../docs/LAPTOP_LINUX_DRIVER_HOST.md)):

Sibling soft skeletons (not product class drivers):

| Tree | PCI | Notes |
|------|-----|--------|
| [`user/drivers/rtl8168_udx/`](../drivers/rtl8168_udx/) | `10ec:8168` | Soft probe/quiesce; T0 net remains virtio |
| [`user/drivers/xhci_udx/`](../drivers/xhci_udx/) | `8086:a12f` | Soft cap/version read; no BOT/MSC claim |
| [`user/drivers/ddi_host_gj/`](../drivers/ddi_host_gj/) | scan + bind | Freestanding `GJ_SYS_DDI` inventory host |

Kernel **soft module path** (load host `r8169.ko` via ksym) is separate and lab-only — see [docs/LINUX_MODULE_PATH.md](../../docs/LINUX_MODULE_PATH.md). **G-AC-1:** no Linux `.ko` product AC.

| Principle | Meaning |
|-----------|---------|
| **ABI-first** | Product progress is Linux-shaped syscall/personality + virtio T0, not freestanding class-driver thrash in the kernel |
| **Out-of-tree drivers** | Real HW drivers live **outside** the kernel TCB as dual-license (MIT/Apache or BSD) code against a published UDX/DDI surface |
| **No GPL** | UDX and GreenJade ship **no GPL** and no Linux source; clean-room C only |
| **DDI (future)** | Cap-gated PCI / IRQ / DMA window attachment will replace host inject; host soft path stays observation-only until then |

### Soft DDI host surface (`host.h`)

Local UDX types mirror kernel soft `gj_devmgr_*` **shape** (no kernel headers
in userspace). Soft != product grants:

| Soft API / type | Behavior today |
|-----------------|----------------|
| `struct udx_host_bdf` / `udx_host_bar` / `udx_host_window_grant` | Soft inventory snapshots (BDF + BAR PA/len) |
| `udx_host_soft_init` / `udx_host_soft_ready` | Idempotent; first call logs **PASS** |
| `udx_host_soft_lifecycle_note` | probe / remove / quiesce catalog lamp |
| `udx_host_soft_bdf_from_pdev` / `udx_host_soft_bar_snapshot[_all]` | Fill soft structs from inject/bound `udx_pci_dev` |
| `udx_host_soft_dma_window_request` | Always **not-granted** (`UDX_ERR_NOSYS`, `u8Ok=0`) until product |
| `udx_host_bind_scan` / `udx_host_bind_by_id` | Freestanding **GJ_SYS_DDI (103)** residual **SCAN→GET→OPEN→MAP preferred BARs** → install → window; soft **SKIP** on host-linux |
| `udx_host_install_granted_pci` | Install grant + wire BAR0 VA; bind residual then window-registers each preferred BAR VA (rtl BAR2 etc.) for `ioremap` |
| `include/udx/ddi.h` | Soft ABI shapes (SCAN/GET/OPEN/MAP_BAR ops) + G752 IDs + preferred BAR catalog |

**Lifecycle (DDI spirit, Linux names):** `probe` attach → run with threaded
IRQ + DMA inside future window → **`quiesce`** (stop DMA / mask IRQ / cancel
work) → `remove`. Driver `.c` never mints/revokes caps.

### Freestanding bind (not host-inject-only)

On GreenJade freestanding hosts (`!UDX_HOST_LIBC` or `GJ_FREESTANDING`):

```text
udx_init()
udx_pci_register_driver(&drv)          # id table includes G752 targets
udx_host_bind_by_id(0x10ec, 0x8168)    # or 8086:a12f / udx_host_bind_scan()
  → GJ_SYS_DDI (103) residual (host.c host_ddi_open_map_install*):
      SCAN     → device count
      GET      → gj_ddi_dev_info (vend/dev/BARs; no CF8/CFC in UDX)
      OPEN     → soft handle id
      MAP_BAR  → preferred BARs only (map_note VA/PA/len per BAR)
                   rtl8168_udx: BAR0 + BAR2
                   xhci_udx:    BAR0
                   other:       BAR0
  → udx_host_install_granted_pci       # BAR0 VA wired
  → udx_host_window_register each preferred BAR VA  # BAR2 etc.
  → pci match / probe
  → probe: udx_ioremap(udx_pci_resource_start(pdev, bar), len)  # granted PA
```

| Piece | Soft today |
|-------|------------|
| Syscall | `GJ_SYS_DDI` **103** (`UDX_DDI_OP_SCAN` / `GET` / `OPEN` / `MAP_BAR`; CLOSE reserved) |
| Bind residual | **SCAN → GET → OPEN → MAP preferred BARs → install_granted → window_register** (`host_ddi_open_map_install*`) |
| Preferred BARs | **rtl8168_udx** BAR0+BAR2 · **xhci_udx** BAR0 · other BAR0 (`ddi.h` / `host_ddi_preferred_bars`) |
| Kernel | May soft-stub → **SKIP** until product dispatch |
| Host-linux | Bind APIs **SKIP**; keep `udx_host_inject_pci` for lab tests (Soft!=product) |
| G752 first targets | **`10ec:8168`** (RTL8111/8168 → `rtl8168_udx`), **`8086:a12f`** (xHCI → `xhci_udx`) |
| Product direction | **freestanding class SKIP**; **product=UDX+ABI** (Linux-shaped userspace) |

Soft != product multi-server confine / live VT-d mint. Dual MIT OR Apache-2.0 only; G-AC-1.

Soft markers (never gate skeleton PASS):

| Marker | Where |
|--------|--------|
| `udx: soft ddi host note PASS` | `host.c` `udx_host_soft_init` (first host init path) |
| `udx: soft ddi-ready` | `host.c` `udx_host_soft_note_ddi` + soft inventory |
| `udx: soft ddi lifecycle note` | `host.c` `udx_host_soft_lifecycle_note` |
| `udx: soft ddi dma not-granted` | `host.c` `udx_host_soft_dma_window_request` |
| `udx: soft ddi bind residual` | `host.c` SCAN/GET/OPEN/MAP_BAR residual tallies + product hosts |
| `udx: soft ddi bind residual path` | `host.c` per-install residual path lamp (maps/pref/host) |
| `udx: soft ddi bind PASS` | `host.c` freestanding open+map preferred+install success |
| `udx: soft ddi bind SKIP` | `host.c` no syscall / empty scan / host-linux fallback |

### Lean residual Cap/MMIO/IRQ/DMA soft API honesty

Soft residual lean markers (Soft!=product * dual MIT OR Apache-2.0 * G-AC-1).
Few lamps only -- no stamp storms. Never product DoD close.

| Marker | Unit | Soft claim |
|--------|------|------------|
| `udx: soft residual lean` | `core.c` | Cap/MMIO/IRQ/DMA rollup + freestanding SKIP |
| `udx: soft residual freestanding class SKIP` | `core.c` / `host.c` | Kernel class drivers not product |
| `udx: soft residual product=UDX+ABI` | `core.c` / `host.c` | Linux-shaped userspace hosts |
| `udx: soft residual api honesty` | `core.c` | Cap hidden; MMIO window; IRQ table; DMA cookie |
| `udx: host soft residual lean` | `host.c` | Cap mint OPEN; soft DDI grant fail-closed |
| `udx: host soft api honesty` | `host.c` | Cap/MMIO/IRQ/DMA soft surface catalog |
| `udx: mmio soft residual lean` | `mmio.c` | ioremap via window != MMIO_FRAME cap |
| `udx: mmio soft api honesty` | `mmio.c` | product MMIO_FRAME OPEN |
| `udx: notify soft residual lean` | `irq.c` | table bind != IRQ Notification cap |
| `udx: notify soft api honesty` | `irq.c` | product IRQ Notification OPEN |
| `udx: dma soft residual lean` | `dma.c` | identity cookie != DMA window cap |
| `udx: dma soft api honesty` | `dma.c` | product DMA window OPEN |

```sh
# After host demo / soft inventory:
grep -F 'udx: soft ddi host note PASS' /tmp/udx.log
grep -F 'udx: soft ddi-ready' /tmp/udx.log
grep -E 'udx: soft ddi bind (PASS|SKIP)' /tmp/udx.log
# Lean residual Cap/MMIO/IRQ/DMA honesty:
grep -E 'udx: soft residual|soft residual lean|soft api honesty' /tmp/udx.log
```

## Greppable product markers

| Marker | Where |
|--------|--------|
| `GREENJADE_UDX` | `types.h` `UDX_PRODUCT_NAME`, `udx_init` log |
| `UDX_PRODUCT` | `types.h` `UDX_PRODUCT_TAG`, `udx_init` log |
| `udx: init GREENJADE_UDX UDX_PRODUCT` | runtime init line |
| `my_drv: probe ok` | skeleton probe success |
| `my_drv: work (irq_hits=` | ISR → schedule_work soft path |
| `my_drv: removed` | quiesce + remove |
| `my_drv: host demo done` | host main completed soft checks |
| `udx: skeleton PASS` | full host demo green (exit 0) |
| `udx: skeleton FAIL` | host demo soft path missed IRQ/work |
| `udx: soft ddi-ready` | host soft note — future DDI attach (ABI-first) |
| `udx: soft ddi host note PASS` | soft DDI host init (types + lifecycle seed) |
| `udx: soft ddi lifecycle note` | probe/remove/quiesce soft catalog |
| `udx: soft ddi dma not-granted` | DMA window request soft fail until product |
| `udx: soft ddi bind residual` | freestanding SCAN→GET→OPEN→MAP preferred BARs residual tallies |
| `udx: soft ddi bind residual path` | per-install residual path (maps/pref/host; Soft!=product) |
| `udx: soft ddi bind PASS` | freestanding residual PASS + preferred BAR windows granted soft |
| `udx: soft ddi bind SKIP` | soft fallback (host inject still valid; Soft!=product) |
| `udx: soft residual lean` | Cap/MMIO/IRQ/DMA lean residual rollup |
| `udx: soft residual freestanding class SKIP` | freestanding class not product |
| `udx: soft residual product=UDX+ABI` | product = Linux-shaped UDX hosts |
| `udx: soft residual api honesty` | Cap/MMIO/IRQ/DMA soft != product mint |
| `udx: skeleton soft honesty` | skeleton residual: class SKIP, product=UDX+ABI, bind + pref_bars |
| `udx: skeleton residual` | skeleton observation lamp (chain + hosts; Soft!=product; never DoD close) |

```sh
make udx-example && ./build/udx_skeleton | tee /tmp/udx.log
grep -E 'GREENJADE_UDX|UDX_PRODUCT|skeleton PASS|probe ok|work \(irq|soft ddi|soft residual' /tmp/udx.log
```

## Layout

```text
user/udx/
  include/udx/
    udx.h       umbrella + init/run/exit/printk
    types.h     status, product markers, basic types
    device.h    device / drvdata / dev_name
    pci.h       pci_driver, id table, soft cfg, enable/master/regions
    irq.h       request_irq / free_irq / nested soft mask + pending
    dma.h       coherent + map/unmap + sync + iommu grant
    mmio.h      ioremap / read[bwlq] / memcpy_io / flush
    work.h      init_work / schedule_work / cancel / pending
    host.h      inject/remove PCI + BAR poke + soft DDI + freestanding bind
    ddi.h       GJ_SYS_DDI soft ABI shapes + G752 10ec:8168 / 8086:a12f
    virtq.h     virtio queue attach / avail / used_reap (header)
  src/          runtime (host-libc objects with -DUDX_HOST_LIBC=1)
  examples/
    skeleton_pci_driver.c   full soft-path demo (probe → irq → work)
  Makefile      optional local build (does not replace top Makefile)
```

## Build

From repo root (preferred; uses top Makefile):

```sh
make udx           # → build/libudx.a
make udx-example   # → build/udx_skeleton  (links skeleton + libudx)
./build/udx_skeleton
```

Local only (does **not** change the top Makefile):

```sh
make -C user/udx          # lib + example under user/udx/build/
make -C user/udx example
./user/udx/build/udx_skeleton
```

Host objects and the example compile with `-DUDX_HOST_LIBC=1`. Freestanding
GJ driver hosts link the same public headers without host inject; DMA/MMIO
use static freestanding pools (no libc heap). See the skeleton’s `#else`
register stub.

## Best combo (locked)

| Layer | Choice |
|-------|--------|
| API names | Linux-shaped (`probe`/`remove`, `ioremap`, `request_irq`, `dma_*`, work) |
| Teardown | DDI-style **`quiesce`** + `remove` |
| IRQ | Threaded only + nested soft `disable_irq` + pending latch on fire |
| Authority | Caps hidden — never mint/revoke from driver `.c` |
| Develop | `udx_host_inject_pci[_ex]` / `udx_host_bar_*` / `udx_host_fire_irq` |
| License | Clean-room dual **MIT OR Apache-2.0** only — **no GPL** |
| Drivers | Out-of-tree dual-license vs UDX/DDI; host inject is lab soft path |

## Example path (`udx-example`) — probe / irq soft path

`examples/skeleton_pci_driver.c` is the canonical porter template.

**Honesty:** host demo is **lab soft** (`inject` / `fire_irq`). Product freestanding
attach residual is **SCAN→GET→OPEN→MAP preferred BARs→install_granted→window_register**
via `udx_host_bind_*` (not BAR0-only). Freestanding class drivers **SKIP**;
**product=UDX+ABI** (Linux-shaped userspace). Soft!=product · G-AC-1 · dual MIT OR Apache-2.0.

1. `udx_init` → `udx_pci_register_driver` (also emits soft residual lean /
   freestanding class SKIP / product=UDX+ABI from core)
2. Host: `udx_host_bind_by_id(UDX_DDI_G752_RTL8168_*)` → expected **SKIP**
   (no GJ_SYS_DDI on host-linux; residual honesty lamp still runs)
3. Host lab: `udx_host_inject_pci` → match + `probe` (Soft!=product)
4. `probe` soft phases:
   - A: `enable` → `set_master` → `request_regions`
   - B: `ioremap` BAR0 + `init_work` / spinlock
   - C: `dma_alloc_coherent` + optional `dma_iommu_grant`
   - D: `request_irq` (table bind) → `enable_irq` → device ctrl IRQEN → `set_drvdata`
5. Host IRQ soft path: `udx_host_bar_writel(status)` → `udx_host_fire_irq`
   - `my_isr`: read status → ack → `schedule_work` → `UDX_IRQ_HANDLED`
6. `udx_run` flushes work → `udx_request_stop`
7. unregister → `quiesce` (`disable_irq` / cancel work / clear master) → `remove` → `udx_exit`
8. `udx: skeleton PASS` if irq_hits≥1 and work_hits≥1 (+ residual honesty lamps)

Freestanding shape (not host inject): `udx_skeleton_freestanding_bind_g752()`
registers then `bind_by_id` (rtl → xhci G752 ids) / `bind_scan`
(SCAN→GET→OPEN→MAP preferred BARs residual).

Replace `MY_VEND` / `MY_DEV` and the register program from **public**
hardware docs only. Do not paste Linux kernel source.

Expected host demo log (greppable):

```text
udx: init GREENJADE_UDX UDX_PRODUCT v1.3 (Linux-porter surface)
udx: soft residual freestanding class SKIP …
udx: soft residual product=UDX+ABI …
udx: soft ddi bind SKIP …
udx: pci register my_drv
udx: inject pci 1af4:1001 bus 0 devfn 0 irq 11
my_drv: probe ok bar0=… dma=… dev=my_drv
udx: pci my_drv probe ok 1af4:1001
my_drv: work (irq_hits=1)
my_drv: removed
my_drv: host demo done
udx: skeleton soft honesty freestanding_class=SKIP product=UDX+ABI bind=SCAN_GET_OPEN_MAP_BAR pref_bars=product_host …
udx: skeleton residual freestanding class SKIP product=UDX+ABI chain=SCAN,GET,OPEN,MAP_BAR hosts=rtl8168_udx,xhci_udx …
udx: skeleton PASS
```

## Host vs full GreenJade

| Piece | Host (`UDX_HOST_LIBC`) | Full GJ freestanding |
|-------|------------------------|----------------------|
| PCI match/probe | inject_ex + id/subsys/class + soft cfg | `udx_host_bind_*` **SCAN→GET→OPEN→MAP preferred BARs→install→window** |
| MMIO | host window table + multi-width BAR poke | window table from **granted preferred BAR PA** + iomem pool |
| IRQ | `udx_host_fire_irq` + nested soft mask/pending | Notification cap + soft mask |
| DMA | malloc cookie | freestanding slab + IOMMU grant |
| Work | queue + `udx_run` flush / cancel | same shape |
| Virtq | header attach/avail/used_reap | MAP_RING + door ops |
| Caps | hidden (none) | hidden (real) |
| Soft bind fail | **SKIP** → keep inject for tests | **SKIP** until kernel DDI live |

## Rules for porters

- No Linux **source** paste — specs and vendor manuals only
- No GPL in UDX or in tree-hosted drivers — dual **MIT OR Apache-2.0** (or BSD out-of-tree)
- No kernel modules — one **host process** per device/function (out-of-tree dual-license drivers)
- No raw caps, IOMMU tables, or kernel endpoints in driver `.c`
- Hard IRQ is not in the driver — UDX runs the threaded handler
- Always implement **quiesce** (stop DMA / soft-mask IRQs / cancel work) before remove/crash
- Treat host inject as soft lab path; freestanding product attach is **DDI residual**
  (`udx_host_bind_by_id` / `bind_scan` → **SCAN→GET→OPEN→MAP preferred BARs→install_granted→window_register** via GJ_SYS_DDI 103) over caps
- Preferred BAR residual is product-host aware (**rtl8168_udx** BAR0+BAR2, **xhci_udx** BAR0) — not BAR0-only honesty
- Soft DMA window request returns **not-granted** until product — do not claim VT-d
- Soft DDI bind **SKIP** is expected on host-linux and on kernels without DDI
- **Freestanding class drivers SKIP** — product NIC/USB = `rtl8168_udx` / `xhci_udx` (Linux-shaped userspace), not kernel class thrash
- **product=UDX+ABI** — userspace UDX hosts + personality/syscall surface; no Linux `.ko` product (G-AC-1)
- Cap/MMIO/IRQ/DMA soft API honesty: soft inventory != product cap mint (G-AC-1 * Soft!=product)

*UDX = ABI-first host for out-of-tree dual-license drivers, DDI residual (SCAN/GET/OPEN/MAP preferred BARs), GreenJade security underneath — no GPL. Freestanding class SKIP; product=UDX+ABI (Linux-shaped userspace). Soft!=product. Dual MIT OR Apache-2.0. G-AC-1.*
