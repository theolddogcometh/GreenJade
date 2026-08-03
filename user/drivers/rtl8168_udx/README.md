# rtl8168_udx — clean-room UDX soft skeleton (RTL8168/8111)

| | |
|--|--|
| **PCI** | **`10ec:8168`** (Realtek RTL8168 / RTL8111 class) |
| **Target class** | ASUS G752VT laptop wired NIC (and similar) |
| **License** | **MIT OR Apache-2.0** — **no GPL**, no Linux source |
| **Role** | Userspace **driver-host soft path** vs [libudx](../../udx/) |
| **Product net** | **Still virtio-net (T0)** — this is **not** product networking |

## Honesty (read first)

This tree is a **soft skeleton** for the ABI-first / DDI direction:

- Exercises Linux-shaped `probe` / `remove` / `quiesce`, BAR MMIO, soft IRQ, soft DMA ring alloc.
- Does **not** bring up link, TX/RX datapath, PHY, or NAPI-style product I/O.
- Does **not** replace or promote in-kernel `kernel/drv/rtl8168.c` lab counters to product.
- Greppable soft lamps are **not** HCL product close and **not** bar3.

| Marker | Meaning |
|--------|---------|
| `rtl8168_udx: soft probe PASS` | Host soft path completed (probe + ISR + work) |
| `rtl8168_udx: soft SKIP` | Soft path not completed (no inject, bind missing, etc.) |

T0 product NIC on GreenJade remains **virtio-net**. Real NIC product path is future **DDI + out-of-tree dual-license** hosts — not this soft demo alone.

**Related (kernel lab, not this tree):** soft load of host `r8169.ko` via
`linux_module` + 40-byte `pci_device_id` match for `10ec:8168` — see
[docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md). Soft ≠ product;
**G-AC-1** forbids treating Linux `.ko` as product AC.

## Clean-room rules

- IDs from **public PCI** (`vendor 0x10ec`, `device 0x8168`).
- Register **names** only from **public** Realtek documentation / widely published maps (e.g. `IntrStatus`, `IntrMask`, `ChipCmd`, `TxConfig`, `RxConfig`, `TNPDS`, `RDSAR`).
- **Do not** paste Linux `r8169.c`, Realtek GPL out-of-tree blobs, or GreenJade `kernel/drv/rtl8168.c`.
- Caps / IOMMU / hard IRQ stay inside UDX — never minted in this driver.

## BAR note (soft)

RTL8168/8111-class devices **often** place the primary MMIO window on **BAR2**. This skeleton:

1. Prefers **BAR2** for `ioremap`.
2. Soft-falls back to the first non-empty **memory** BAR if BAR2 is empty (host inject mistakes / odd revs).
3. Logs a warning when falling back — **re-check real silicon BARs** from Linux inventory / config space before any hard programming.

Host inject for the lab demo maps **only BAR2** (`0x1000` soft window).

## Soft lifecycle

```text
udx_init
  → udx_pci_register_driver(rtl8168_udx)
  → [host] udx_host_inject_pci(10ec, 8168, BAR2=0x1000)
  → probe: enable → master → regions → ioremap(BAR2) →
           dma_alloc_coherent (soft ring) → request_irq → IntrMask soft
  → [host] bar writew IntrStatus(ROK|TOK) → fire_irq
  → ISR: read IntrStatus → ack → schedule_work
  → udx_run → work → stop
  → unregister (quiesce → remove) → udx_exit
  → "rtl8168_udx: soft probe PASS"
```

**Freestanding:** call `rtl8168_udx_register()` (or `rtl8168_udx_freestanding_start()`).  
If the platform links **`udx_host_bind_by_id`**, freestanding start will bind `10ec:8168`.  
If that symbol is **absent**, freestanding logs **soft SKIP** and documents host inject (below).

## Build instructions

Prerequisites: host toolchain with `gcc`/`ar`, and **libudx** built for host.

### 1. Build libudx (repo root preferred)

```sh
cd /path/to/knano
make udx
# → build/libudx.a
```

Or local UDX tree:

```sh
make -C user/udx lib
# → user/udx/build/libudx.a
```

### 2. Build this driver (local Makefile)

```sh
make -C user/drivers/rtl8168_udx
# → user/drivers/rtl8168_udx/build/rtl8168_udx
```

Override lib path if you used the top-level UDX build:

```sh
make -C user/drivers/rtl8168_udx UDX_LIB=../../../../build/libudx.a \
  UDX_INC=../../udx/include
```

(Paths are relative to `user/drivers/rtl8168_udx/`; the local Makefile defaults to `user/udx`.)

### 3. Run host soft demo

```sh
./user/drivers/rtl8168_udx/build/rtl8168_udx | tee /tmp/rtl8168_udx.log
grep -E 'rtl8168_udx: soft (probe PASS|SKIP)' /tmp/rtl8168_udx.log
```

Expected greppable success line:

```text
rtl8168_udx: soft probe PASS
```

### One-liner from repo root (after `make udx` or `make -C user/udx lib`)

```sh
make -C user/drivers/rtl8168_udx && ./user/drivers/rtl8168_udx/build/rtl8168_udx
```

## Host inject IDs (lab)

When freestanding bind is unavailable, host test inject:

| Field | Value |
|-------|--------|
| Vendor | `0x10ec` |
| Device | `0x8168` |
| BAR2 | memory, length `0x1000` (soft) |
| IRQ | any unused soft line (demo uses `12`) |

API: `udx_host_inject_pci(0x10ec, 0x8168, bus, devfn, irq, aBarLen, aBarMem, &pPdev)` with `aBarLen[2] = 0x1000`.

## Files

| File | Role |
|------|------|
| `rtl8168_udx.c` | Driver skeleton + host `main` / freestanding stubs |
| `Makefile` | Local host build against libudx |
| `README.md` | This note |

## What is intentionally missing

- No TX/RX descriptor programming to live silicon
- No PHY / MII / EEPROM bring-up
- No NAPI, ethtool, or netstack attachment
- No MSI-X product path
- No claim of G752VT LAN product support

*Soft skeleton for laptop NIC class IDs — product net remains virtio T0.*
