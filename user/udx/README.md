# UDX (userspace Linux-porter runtime)

Clean-room, Linux-shaped driver API for GreenJade. Porters write
`probe` / `remove` / `ioremap` / `request_irq` / `dma_*` / `schedule_work`
against this library; capabilities, IOMMU, and hard IRQ stay hidden.

| Field | Value |
|-------|--------|
| **Product** | `GREENJADE_UDX` · `UDX_PRODUCT` · v1.3 |
| **License** | MIT OR Apache-2.0 — **no Linux source** |
| **Guide** | [docs/UDX_LINUX_PORTER.md](../../docs/UDX_LINUX_PORTER.md) |
| **Library** | `make udx` → `build/libudx.a` |
| **Host demo** | `make udx-example` → `build/udx_skeleton` |

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

```sh
make udx-example && ./build/udx_skeleton | tee /tmp/udx.log
grep -E 'GREENJADE_UDX|UDX_PRODUCT|skeleton PASS|probe ok|work \(irq' /tmp/udx.log
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
    host.h      inject/remove PCI + multi-width BAR poke + fire_irq
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
| License | Clean-room MIT/Apache C only |

## Example path (`udx-example`) — probe / irq soft path

`examples/skeleton_pci_driver.c` is the canonical porter template.

1. `udx_init` → `udx_pci_register_driver`
2. Host: `udx_host_inject_pci` → match + `probe`
3. `probe` soft phases:
   - A: `enable` → `set_master` → `request_regions`
   - B: `ioremap` BAR0 + `init_work` / spinlock
   - C: `dma_alloc_coherent` + optional `dma_iommu_grant`
   - D: `request_irq` (table bind) → `enable_irq` → device ctrl IRQEN → `set_drvdata`
4. Host IRQ soft path: `udx_host_bar_writel(status)` → `udx_host_fire_irq`
   - `my_isr`: read status → ack → `schedule_work` → `UDX_IRQ_HANDLED`
5. `udx_run` flushes work → `udx_request_stop`
6. unregister → `quiesce` (`disable_irq` / cancel work / clear master) → `remove` → `udx_exit`
7. `udx: skeleton PASS` if irq_hits≥1 and work_hits≥1

Replace `MY_VEND` / `MY_DEV` and the register program from **public**
hardware docs only. Do not paste Linux kernel source.

Expected host demo log (greppable):

```text
udx: init GREENJADE_UDX UDX_PRODUCT v1.3 (Linux-porter surface)
udx: pci register my_drv
udx: inject pci 1af4:1001 bus 0 devfn 0 irq 11
my_drv: probe ok bar0=… dma=… dev=my_drv
udx: pci my_drv probe ok 1af4:1001
my_drv: work (irq_hits=1)
my_drv: removed
my_drv: host demo done
udx: skeleton PASS
```

## Host vs full GreenJade

| Piece | Host (`UDX_HOST_LIBC`) | Full GJ |
|-------|------------------------|---------|
| PCI match/probe | inject_ex + id/subsys/class + soft cfg | devmgr grants |
| MMIO | host window table + multi-width BAR poke | window table + freestanding iomem pool |
| IRQ | `udx_host_fire_irq` + nested soft mask/pending | Notification cap + soft mask |
| DMA | malloc cookie | freestanding slab + IOMMU grant |
| Work | queue + `udx_run` flush / cancel | same shape |
| Virtq | header attach/avail/used_reap | MAP_RING + door ops |
| Caps | hidden (none) | hidden (real) |

## Rules for porters

- No Linux **source** paste — specs and vendor manuals only
- No kernel modules — one **host process** per device/function
- No raw caps, IOMMU tables, or kernel endpoints in driver `.c`
- Hard IRQ is not in the driver — UDX runs the threaded handler
- Always implement **quiesce** (stop DMA / soft-mask IRQs / cancel work) before remove/crash

*UDX = Linux-shaped surface, DDI quiesce, GreenJade security underneath.*
