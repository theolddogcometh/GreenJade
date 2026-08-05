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
| **Freestanding lab stick** | `kernel/drv/xhci_msc.c` | Soft / lab scaffold only — **not** T1 product; **do not thrash** from this tree |
| **This tree** | `user/drivers/xhci_udx/` | **Userspace Linux-shaped driver host** against UDX — soft inject + BAR0 cap/params/ports + **soft BOT progress stub** |
| **BOT / stick log / MSC** | Neither path here as product | Soft stage lamps **≠** CBW/CSW / READ CAPACITY / stick log |

GreenJade product I/O direction remains ABI-first + virtio T0 + future DDI grants.
In-kernel freestanding xHCI stages stay **soft** (see [docs/G752VT_LINUX_HWTEST.md](../../../docs/G752VT_LINUX_HWTEST.md),
[docs/DDI_SOFT.md](../../../docs/DDI_SOFT.md), [docs/UDX_LINUX_PORTER.md](../../../docs/UDX_LINUX_PORTER.md),
[docs/LAPTOP_LINUX_DRIVER_HOST.md](../../../docs/LAPTOP_LINUX_DRIVER_HOST.md) wave D).

On many lab hosts `xhci_pci` / `xhci_hcd` are **kernel builtin** (no plain `.ko`
to embed). Collect reports `xhci_pci=BUILTIN`; freestanding STATUS soft-skips
the xHCI module path — [docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md).

## What the soft skeleton does (deepen)

1. **Match** id table: exact `8086:a12f` and optional PCI class `0x0c0330` (USB xHCI).
2. **probe stage=1 cap**: `pci_enable` → `request_regions` → **ioremap BAR0** → soft-read
   **CAPLENGTH** / **HCIVERSION** (xHCI Spec offsets only).
3. **probe stage=2 params**: soft-read **HCSPARAMS1** → MaxSlots / MaxIntrs / MaxPorts.
4. **probe stage=3 ports**: soft PORTSC walk at `op + 0x400 + (n-1)*0x10` (op = CAPLENGTH);
   count CCS; lamp first connected port + speed field.
5. **probe stage=4 stub**: soft BOT **progress** lamp only — catalogs next product steps
   (`slot/addr/enum/bot`) without rings, CBW/CSW, or DMA.
6. **quiesce**: mark soft idle (no rings / bus-master in this skeleton).
7. **remove**: `iounmap` → release regions → disable.

Host demo injects a synthetic function, seeds soft BAR0 capability dword0 + HCSPARAMS1 +
PORTSC[1] (CCS|PED|PP|SS), then registers the driver so probe walks stages 1→4.

## Greppable markers

| Marker | Meaning |
|--------|---------|
| `xhci_udx: soft cap ver=` | CAPLENGTH + HCIVERSION soft snapshot |
| `xhci_udx: soft bot stage=1 cap` | Soft BOT progress: capability ok |
| `xhci_udx: soft params … maxports=` | HCSPARAMS1 soft snapshot |
| `xhci_udx: soft bot stage=2 params` | Soft BOT progress: params ok |
| `xhci_udx: soft port N ccs=1 …` | Per-port PORTSC soft CCS hit |
| `xhci_udx: soft ports n=… ccs=…` | Soft port walk summary |
| `xhci_udx: soft bot stage=3 ports` | Soft BOT progress: ports walked |
| `xhci_udx: soft bot would_try_port=` | Soft next-step note (no Enable Slot) |
| `xhci_udx: soft bot stage=4 stub` | Soft BOT catalog complete |
| `xhci_udx: soft bot stub PASS` | Soft progress path finished — **≠ product BOT** |
| `xhci_udx: soft probe PASS` | probe completed (bot_stage=4 on host demo) |
| `xhci_udx: soft quiesce` | DDI-style quiesce ran |
| `xhci_udx: soft remove` | remove completed |
| `xhci_udx: host soft demo done` | host main verified soft seed values |

```sh
make -C user/drivers/xhci_udx
./user/drivers/xhci_udx/build/xhci_udx | tee /tmp/xhci_udx.log
grep -E 'xhci_udx: soft (probe PASS|bot stage=|bot stub PASS|ports |params )' /tmp/xhci_udx.log
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
`xhci_udx_freestanding_register` registers the driver and, when linked,
`udx_host_bind_by_id(8086, a12f)` via **GJ_SYS_DDI** SCAN/OPEN/MAP_BAR —
still **no** product BOT claim.

## Public registers touched (xHCI Spec)

Offsets relative to **BAR0** unless noted. Soft path only:

| Offset | Name | Width | Soft use |
|--------|------|-------|----------|
| `0x00` | **CAPLENGTH** | 8-bit | Length of capability registers |
| `0x02` | **HCIVERSION** | 16-bit | Interface Version Number |
| `0x04` | **HCSPARAMS1** | 32-bit | MaxSlots / MaxIntrs / MaxPorts |
| `CAPLENGTH + 0x400 + (n-1)*0x10` | **PORTSC[n]** | 32-bit | Soft CCS/PED/PP/speed snapshot |

Host seed writes:

- dword0 at `0x00`: `CAPLENGTH | (HCIVERSION << 16)`
- dword at `0x04`: soft MaxSlots/MaxIntrs/MaxPorts
- dword at PORTSC[1]: CCS|PED|PP|speed=SS

No operational run/stop, runtime, doorbell, extended capability, or ring program
in this skeleton. Soft BOT stub does **not** issue CBW/CSW.

## Clean-room rules

- Specs + public PCI IDs / manuals only.
- **Do not** paste Linux `xhci-*.c` / `xhci_hcd` or any GPL tree.
- **Do not** edit `kernel/drv/xhci_msc.c` from this dual-license host path.
- Caps, IOMMU, hard IRQ stay inside UDX — never in this driver `.c`.
- Soft host inject ≠ product MMIO/IRQ/DMA grants from devmgr.
- Soft BOT stage=4 **PASS** ≠ product stick access.
