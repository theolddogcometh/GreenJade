# ddi_host_gj — freestanding soft DDI host (NATIVE ELF)

| Field | Value |
|-------|--------|
| **Status** | **Soft scaffold** — inventory + OPEN/MAP_BAR0 probes only |
| **ELF** | NATIVE freestanding (libgj + `user/init/user.ld`) |
| **Syscall** | **`GJ_SYS_DDI` (103)** via `gj_ddi` / `gj_ddi_*` in `user/libgj/include/gj/syscalls.h` |
| **License** | **MIT OR Apache-2.0** only — **no GPL**, **no Linux `.ko` product AC** |
| **Law** | **Soft ≠ product** ([docs/DDI_SOFT.md](../../../docs/DDI_SOFT.md)) |

## One-sentence goal

**Exercise the soft DDI door from a ring-3 NATIVE process: SCAN inventory, log devices, and soft-bind G752 first targets (`10ec:8168`, `8086:a12f`) with OPEN + MAP_BAR0 — without claiming product DDI caps.**

## What this is / is not

| Claims (soft) | Does **not** claim |
|---------------|--------------------|
| `ddi_host: soft scan PASS n=…` | Full `devmgr` match graph |
| Soft OPEN + MAP_BAR0 for matched IDs | Live MMIO_FRAME / IRQ / DMA window **cap mint** |
| Greppable PASS \| SKIP per G752 ID | Product UDX class driver complete |
| Freestanding link like `personality_gj` | Boot embed / default spawn (coordinator later) |

Product order remains ABI → virtio T0 → **DDI caps** → dual-license userspace hosts ([docs/ABI_FIRST_PIVOT.md](../../../docs/ABI_FIRST_PIVOT.md), [docs/LAPTOP_LINUX_DRIVER_HOST.md](../../../docs/LAPTOP_LINUX_DRIVER_HOST.md)).

Sibling UDX soft skeletons: [`rtl8168_udx`](../rtl8168_udx/), [`xhci_udx`](../xhci_udx/). Kernel soft module path (embed/load host `.ko`) is documented in [docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md) — not this ELF.

## Layout

| Path | Role |
|------|------|
| `ddi_host_gj.c` | Freestanding `_start` → `main`; soft scan + bind lamps |
| `Makefile` | Local freestanding build → `build/ddi_host.elf` |
| `README.md` | This file |

Source of truth under this tree: `ddi_host_gj.c` only. Kernel/ is **not** edited here. Reuses [`user/init/user.ld`](../../init/user.ld) like other `*-gj` daemons.

## Build

From this directory:

```sh
make
# → user/drivers/ddi_host_gj/build/ddi_host.elf
```

Root-style output path (optional):

```sh
make OUT=../../../build/user/ddi_host.elf
# → build/user/ddi_host.elf
```

`Makefile` builds `libgj.a` under `$(ROOT)/build/user/` if missing (same freestanding flags as the coordinator `USER_CFLAGS`).

## Runtime path (soft)

```text
_start → main
  → ddi_host: soft scaffold PASS          (DEBUG_LOG)
  → gj_ddi_inventory()                    (optional door once lamp)
  → gj_ddi_scan()  → n devices
  → ddi_host: soft scan PASS n=…
  → for each row: gj_ddi_get → ddi_host: soft found …
  → match 10ec:8168 → OPEN + MAP_BAR0 → soft bind … PASS|SKIP
  → match 8086:a12f → OPEN + MAP_BAR0 → soft bind … PASS|SKIP
  → ddi_host: soft done soft≠product
  → gj_exit(0)
```

G752 targets (inventory / HCL):

| PCI ID | Class intent |
|--------|----------------|
| **`10ec:8168`** | Realtek RTL8111/8168 Gigabit Ethernet |
| **`8086:a12f`** | Intel 100 Series / C230 USB 3.0 xHCI (`00:14.0`) |

## Greppable lamps (expected)

Prefix-stable for smoke / serial:

```text
ddi_host: soft scaffold PASS
ddi_host: soft scan PASS n=<count>
ddi_host: soft found idx=… bdf=…:….… id=vvvv:dddd   (0..n rows)
ddi_host: soft inventory empty …                     (when n=0 / no rows)
ddi_host: soft bind 10ec:8168 PASS|SKIP soft≠product
ddi_host: soft bind 8086:a12f PASS|SKIP soft≠product
ddi_host: soft done soft≠product gpl=0
```

| Lamp | When |
|------|------|
| **scan PASS** | Always after SCAN (even if `n=0` — deferred soft table) |
| **bind PASS** | Matching vend:dev found **and** OPEN + MAP_BAR0 soft-ok |
| **bind SKIP** | No match, open fail, empty BAR, or MAP soft-fail |

Related kernel lamps (not this ELF): `ddi_door: soft scan …`, `ddi_door: soft product surface PASS`, `devmgr: soft found 10ec:8168` / `8086:a12f`.

## API surface used

From `user/libgj/include/gj/syscalls.h`:

| Symbol | Role |
|--------|------|
| `GJ_SYS_DDI` / `gj_ddi` | Multiplex soft DDI door |
| `gj_ddi_scan` | Soft PCI inventory → count |
| `gj_ddi_get` | Index → packed dev info |
| `gj_ddi_open` | Soft handle for index |
| `gj_ddi_map_bar` / `GJ_DDI_OP_MAP_BAR` | Soft BAR0 map note |
| `gj_ddi_inventory` | Optional once surface lamp |
| `gj_debug_log` / `gj_exit` | Soft markers + terminate |

Local packed layouts match `kernel/include/gj/ddi_door.h` (`gj_ddi_dev_info`, `gj_ddi_map_note`).

## Coordinator note (optional)

Prefer this local Makefile. If a one-liner phony is desired at repo root later:

```make
.PHONY: ddi_host-gj
ddi_host-gj:
	$(MAKE) -C user/drivers/ddi_host_gj
```

No kernel embed or boot spawn is wired by this drop — soft host scaffold only.

## License

Dual **MIT OR Apache-2.0**. Clean-room; **no GPL** source. Soft path is **not** product DDI authority.
