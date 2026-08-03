# xhci_udx — clean-room UDX soft skeleton (Intel PCH `8086:a12f`)

| Field | Value |
|-------|--------|
| **Path** | `user/drivers/xhci_udx/` |
| **License** | **MIT OR Apache-2.0** — dual; **no GPL**, no Linux xHCI paste |
| **Match** | PCI `8086:a12f` (G752VT 100 Series / C230 xHCI) · optional class `0x0c0330` |
| **Runtime** | Links **libudx** (`user/udx/`) — Linux-shaped `probe` / `quiesce` / `remove` |
| **Product claim** | **None** for BOT/MSC/USB host stack — soft host path only |

## Honesty split (read this first)

| Path | Where | Claim |
|------|--------|--------|
| **Freestanding lab stick** | `kernel/drv/xhci_msc.c` | Soft / lab scaffold only — **not** T1 product |
| **This tree** | `user/drivers/xhci_udx/` | **Userspace Linux-shaped driver host** against UDX — soft inject + BAR0 cap read |
| **BOT / stick log / MSC** | Neither path here | **No product claim** |

GreenJade product I/O direction remains ABI-first + virtio T0 + future DDI grants.
In-kernel freestanding xHCI stages stay **soft** (see [docs/G752VT_LINUX_HWTEST.md](../../../docs/G752VT_LINUX_HWTEST.md),
[docs/DDI_SOFT.md](../../../docs/DDI_SOFT.md), [docs/UDX_LINUX_PORTER.md](../../../docs/UDX_LINUX_PORTER.md)).

On many lab hosts `xhci_pci` / `xhci_hcd` are **kernel builtin** (no plain `.ko`
to embed). Collect reports `xhci_pci=BUILTIN`; freestanding STATUS soft-skips
the xHCI module path — [docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md).

## What the soft skeleton does

1. **Match** id table: exact `8086:a12f` and optional PCI class `0x0c0330` (USB xHCI).
2. **probe**: `pci_enable` → `request_regions` → **ioremap BAR0** → soft-read public
   capability fields **CAPLENGTH** / **HCIVERSION** (xHCI Spec offsets only).
3. **quiesce**: mark soft idle (no rings / bus-master in this skeleton).
4. **remove**: `iounmap` → release regions → disable.

Host demo injects a synthetic function, seeds soft BAR0 capability dword0, then
registers the driver so probe sees non-zero cap/version.

## Greppable markers

| Marker | Meaning |
|--------|---------|
| `xhci_udx: soft cap ver=` | CAPLENGTH + HCIVERSION soft snapshot printed |
| `xhci_udx: soft probe PASS` | probe completed with non-zero cap/version |
| `xhci_udx: soft quiesce` | DDI-style quiesce ran |
| `xhci_udx: soft remove` | remove completed |
| `xhci_udx: host soft demo done` | host main verified soft seed values |

```sh
make -C user/drivers/xhci_udx
./user/drivers/xhci_udx/build/xhci_udx | tee /tmp/xhci_udx.log
grep -E 'xhci_udx: soft (probe PASS|cap ver=)' /tmp/xhci_udx.log
```

## Build

Requires host `libudx` first:

```sh
# from repo root
make udx
# or local
make -C user/udx lib

make -C user/drivers/xhci_udx
./user/drivers/xhci_udx/build/xhci_udx
```

`UDX_HOST_LIBC=1` is the default for the host binary. Freestanding shape:
compile without host inject (`xhci_udx_freestanding_register`) when a GJ
driver-host process exists — still no product BOT claim.

## Public registers touched (xHCI Spec capability space)

Offsets are relative to **BAR0** (capability register base). Only these fields
are read on the soft path:

| Offset | Name | Width | Soft use |
|--------|------|-------|----------|
| `0x00` | **CAPLENGTH** | 8-bit | Length of capability registers |
| `0x02` | **HCIVERSION** | 16-bit | Interface Version Number |

Host seed writes one dword at `0x00` packing both fields
(`CAPLENGTH | (HCIVERSION << 16)`). No operational (`op`), runtime (`rt`),
doorbell, or extended capability programming in this skeleton.

## Clean-room rules

- Specs + public PCI IDs / manuals only.
- **Do not** paste Linux `xhci-*.c` / `xhci_hcd` or any GPL tree.
- Caps, IOMMU, hard IRQ stay inside UDX — never in this driver `.c`.
- Soft host inject ≠ product MMIO/IRQ/DMA grants from devmgr.
