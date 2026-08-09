# ddi_host_gj — freestanding soft DDI host (NATIVE ELF)

| Field | Value |
|-------|--------|
| **Status** | **Soft residual lean** — DDI door path for userspace UDX NIC/USB hosts |
| **ELF** | NATIVE freestanding (libgj + `user/init/user.ld`) |
| **Syscall** | **`GJ_SYS_DDI` (103)** via `gj_ddi` / `gj_ddi_*` in `user/libgj/include/gj/syscalls.h` |
| **License** | **MIT OR Apache-2.0** only — **no GPL**, **no Linux `.ko` product AC** |
| **Law** | **Soft != product** ([docs/DDI_SOFT.md](../../../docs/DDI_SOFT.md)); **mint=0** honesty |
| **Product** | **UDX + ABI** Linux-shaped userspace drivers (`rtl8168_udx` / `xhci_udx`) |
| **Dual DoD** | **A/B OPEN** — soft door residual only; Soft≠product |
| **Support** | [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog) · root [README](../../../README.md) |

## One-sentence goal

**Exercise the soft DDI door from a ring-3 NATIVE process so userspace UDX NIC/USB hosts can bind G752 targets (`10ec:8168` / `8086:a12f`) — SCAN → GET → OPEN → MAP_BAR → CFG/IRQ/DMA lifecycle — without claiming product DDI caps (mint=0). Soft bind ≠ product cap mint; product hosts = UDX class drivers; Dual DoD A/B remain OPEN.**

## Product vs freestanding SKIP

| Path | Policy |
|------|--------|
| **Product drivers** | **Linux-shaped** userspace over **hot + cold ABI** and **DDI/UDX** caps (MMIO / IRQ / DMA). This tree is the soft door residual those hosts use. |
| **Freestanding class drivers** | **Not product.** Default **SKIP**: `GJ_RTL8168_PROBE=0` (NIC) · `GJ_XHCI_MSC_PROBE=0` (USB). Residual opt-in only. |
| **Linux `.ko` in-kernel** | **Forbidden as product AC** (**G-AC-1**). Soft module path is eng only. |
| **This ELF** | Soft residual door observe only. Soft PASS != product TX/RX, BOT/MSC, or Dual DoD close. |

Soft != product. No version stamp. No stamp storms.

## What this is / is not

| Claims (soft) | Does **not** claim |
|---------------|--------------------|
| `ddi_host: soft SCAN PASS n=…` | Full `devmgr` match graph |
| Soft OPEN + MAP preferred BARs (`user=` map_note) | Live **MMIO_FRAME** cap mint |
| Soft MAP re-map (idempotent VA match + sticky) | Product user-AS reinstall |
| Soft CFG_READ + CFG_WRITE careful note + BAR reject | Live PCI reprogram / bus-master enable |
| Soft IRQ_BIND note (badge) | Live **IRQ Notification** mint |
| Soft NOTIFY_WAIT poll (non-block) | MSI-X fire / product badge delivery |
| Soft DMA_NOTE / DMA_BUF ALLOC→MAP→cookie idem→FREE | Live **DMA window** CNode mint |
| Soft QUIESCE catalog (before CLOSE) | Product quiesce-before-revoke enforce |
| Soft CLOSE + post_close (CFG/MAP/IRQ/DMA/CLOSE) + REOPEN | Product CNode reinstall / Phase-A |
| Cap mint / window / door residual honesty lamps | Product UDX TX/RX or BOT/MSC |

Product order remains ABI → virtio T0 → **DDI caps** → dual-license userspace hosts ([docs/ABI_FIRST_PIVOT.md](../../../docs/ABI_FIRST_PIVOT.md), [docs/LAPTOP_LINUX_DRIVER_HOST.md](../../../docs/LAPTOP_LINUX_DRIVER_HOST.md)).

Sibling UDX soft skeletons: [`rtl8168_udx`](../rtl8168_udx/), [`xhci_udx`](../xhci_udx/). Kernel soft module path is [docs/LINUX_MODULE_PATH.md](../../../docs/LINUX_MODULE_PATH.md) — not this ELF.

## Layout

| Path | Role |
|------|------|
| `ddi_host_gj.c` | Freestanding `_start` → `main`; soft scan + residual bind lamps |
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

## Runtime path (soft residual)

```text
_start → main
  → ddi_host: soft scaffold PASS
  → gj_ddi_inventory()                    (optional door once lamp)
  → gj_ddi_scan()  → n devices
  → ddi_host: soft SCAN PASS n=…
  → for each row: gj_ddi_get → soft GET / soft found …
  → soft residual lean (product=UDX+ABI · fs_skip)
  → match 10ec:8168:
       OPEN → CFG_READ → CFG_WRITE(note+BAR reject) → MAP bar0+bar2
       → MAP_REMAP(idem VA + sticky) → DMA_NOTE → IRQ_BIND → notify poll
       → DMA_BUF ALLOC/MAP/MAP_REMAP(cookie idem)/FREE → grant → QUIESCE → CLOSE
       → post_close forget (CFG/MAP/IRQ/DMA/CLOSE) → REOPEN/CLOSE
  → match 8086:a12f: (same chain; preferred BAR0)
  → residual life rollup (door SCAN/GET/OPEN/MAP_BAR/CFG/IRQ/DMA_BUF/CLOSE)
  → residual / cap mint / window / door honesty · mint claim=0
  → ddi_host: soft done Soft!=product
  → gj_exit(0)
```

Lifecycle vs kernel `ddi_door` (functional residual preferred over lamp spam):

| Door surface | Host residual |
|--------------|---------------|
| SCAN / GET / OPEN / MAP_BAR | Inventory walk + G752 bind |
| CFG_READ / CFG_WRITE | Identity snap; careful soft-note + BAR PERM reject |
| MAP grant slot / idempotent re-MAP | `map_note.u8User` + VA match + **sticky** third MAP |
| IRQ_BIND + NOTIFY_WAIT | Soft badge note + non-block poll (`fire=0`) |
| DMA_NOTE / DMA_BUF_* | Soft window note + ALLOC→MAP→**MAP_REMAP cookie**→FREE |
| CLOSE (forget maps/IRQ/DMA) | CLOSE + **post_close** CFG/MAP/IRQ/DMA/CLOSE fail |

G752 targets (inventory / HCL):

| PCI ID | Class intent | Preferred BARs |
|--------|----------------|----------------|
| **`10ec:8168`** | Realtek RTL8111/8168 Gigabit Ethernet | BAR0 + BAR2 (prefer silicon-common BAR2; DDI often grants BAR0) |
| **`8086:a12f`** | Intel 100 Series / C230 USB 3.0 xHCI | BAR0 |

## Greppable lamps (expected)

Prefix-stable for smoke / serial. Lean set only (no deepen / no wave stamp):

```text
ddi_host: soft scaffold PASS
ddi_host: soft SCAN PASS n=<count>
ddi_host: soft GET idx=… id=… PASS|SKIP
ddi_host: soft found idx=… bdf=…:….… id=vvvv:dddd
ddi_host: soft residual lean … product=UDX+ABI … Soft!=product
ddi_host: soft OPEN 10ec:8168|8086:a12f … PASS|SKIP mint=0
ddi_host: soft MAP_BAR … barN user=0|1 PASS|SKIP mint=0
ddi_host: soft MAP_REMAP … barN PASS|SKIP idempotent=1 va_match=0|1 sticky=0|1 mint=0
ddi_host: soft CFG … PASS|SKIP
ddi_host: soft CFG_WRITE … off=0x4 PASS soft_note=1 live_poke=0 mint=0
ddi_host: soft CFG_WRITE reject … off=0x10 PASS why=unsafe_bar …
ddi_host: soft DMA_NOTE … mint=0
ddi_host: soft IRQ_BIND … mint=0 irq_notify=0
ddi_host: soft notify poll … block=0 fire=0 mint=0
ddi_host: soft DMA_BUF … mint=0
ddi_host: soft DMA_BUF_MAP … soft_cookie=1 mint=0
ddi_host: soft DMA_BUF_MAP_REMAP … cookie_match=0|1 dma_idem=0|1 mint=0
ddi_host: soft DMA_BUF_FREE … soft_forget=1 mint=0
ddi_host: soft dma_buf chain … alloc= map= free= dma_idem= mint=0
ddi_host: soft grant surface … open= maps= irq= mint=0
ddi_host: soft QUIESCE … order=before_close mint=0
ddi_host: soft CLOSE … mint=0
ddi_host: soft post_close … cfg= map= irq= dma= close2= PASS forget=1 mint=0
ddi_host: soft REOPEN … reuse=1 mint=0
ddi_host: soft residual path id=… open= map= irq= grant= life=0x… mint=0
ddi_host: soft bind 10ec:8168 PASS|SKIP|MISS Soft!=product
ddi_host: soft bind 8086:a12f PASS|SKIP|MISS Soft!=product
ddi_host: soft residual rtl=… xhci=…
ddi_host: soft residual life rtl=0x… xhci=0x… idem_ok= sticky= dma_idem= post_cl= post_map= post_irq= post_dma= door=SCAN/GET/OPEN/MAP_BAR/CFG/IRQ/DMA_BUF/CLOSE …
ddi_host: soft cap mint honesty … soft_bind_ne_mint=1 mint=0
ddi_host: soft window residual honesty …
ddi_host: soft door residual honesty … product=UDX+ABI
ddi_host: soft mint claim=0 …
ddi_host: soft done Soft!=product soft!=product gpl=0 G-AC-1 product_mint=0
```

| Lamp | When |
|------|------|
| **SCAN PASS** | Always after SCAN (even if `n=0` — deferred soft table) |
| **bind PASS** | Matching vend:dev found **and** OPEN + ≥1 preferred BAR MAP soft-ok |
| **bind SKIP** | Match found but OPEN/MAP soft-failed |
| **bind MISS** | No matching ID in inventory |
| **residual path** | Per-ID OPEN/MAP/IRQ/grant/life counters (mint=0) |
| **residual life** | Host life= mask vs door SCAN/GET/OPEN/MAP_BAR/CFG/IRQ/DMA_BUF/CLOSE |
| **CFG_WRITE / reject** | Careful soft-note + BAR PERM residual (functional policy) |
| **MAP_REMAP sticky** | Second re-MAP still matches prior VA (grant_live residual) |
| **DMA_BUF_MAP_REMAP** | Re-MAP same pa/cb cookie match (door dma_idem) |
| **post_close** | Dead-handle forget: CFG+MAP+IRQ+DMA_ALLOC+CLOSE all fail |
| **cap mint honesty** | Soft only: mmio_frame=0 irq_notify=0 dma_window=0 cnode=0; soft_bind_ne_mint=1 |
| **window residual honesty** | Soft DMA note/buf only; product window OPEN |
| **door residual honesty** | Soft `GJ_SYS_DDI` observe; product multi-cap grant OPEN |
| **mint claim=0** | Explicit no product MMIO_FRAME / IRQ Notification / DMA window mint |

Related kernel lamps (not this ELF): `ddi_door: soft scan …`, `ddi_door: soft product surface PASS`, `ddi: soft irq note PASS`, `devmgr: soft found 10ec:8168` / `8086:a12f`.

## Residual surfaces (rtl8168_udx + xhci_udx)

| Surface | Soft residual | Product (OPEN) |
|---------|---------------|----------------|
| **OPEN** | Soft handle id | CNode install into host |
| **MAP** | Soft MAP_BAR note (UC VA / map_note) | MMIO_FRAME cap map |
| **MAP_REMAP** | Soft re-MAP; VA match (`idem=`) + sticky third MAP | Product user-AS reinstall |
| **CFG_WRITE** | Soft-note safe off; BAR/identity reject | Live CF8 / bus-master enable |
| **IRQ_BIND** | Soft badge note; notify_wait OPEN | IRQ → Notification mint |
| **notify poll** | Non-block `GJ_SYS_NOTIFY_WAIT`; `fire=0` | Live MSI-X badge delivery |
| **grant** | Catalog mmio_frame=0 irq_notify=0 dma_window=0 | Live multi-cap grant |
| **DMA_BUF** | Soft ALLOC → MAP → MAP_REMAP cookie → FREE | DMA window + IOMMU program |
| **QUIESCE** | Catalog stop DMA / mask IRQ / cancel work | Product quiesce-before-revoke |
| **CLOSE** | Soft forget handle / slots | Phase-A revoke |
| **post_close** | Dead-handle CFG/MAP/IRQ/DMA/CLOSE must fail | Product revoke enforce |
| **REOPEN** | Soft OPEN again after CLOSE | Product CNode reinstall |

Soft OPEN+MAP != product TX/RX (rtl) or BOT/MSC (xhci). Soft bind != product cap mint. **G-AC-1.** **mint=0** on every grant lamp. Product hosts = UDX class drivers.

### Residual lean (door path for UDX hosts)

```text
OPEN → CFG_READ → CFG_WRITE(note+reject) → MAP_BAR* → MAP_REMAP(idem+sticky)
  → DMA_NOTE → IRQ_BIND → notify_poll
  → DMA_BUF_ALLOC → DMA_BUF_MAP → DMA_BUF_MAP_REMAP(cookie) → DMA_BUF_FREE
  → grant surface → QUIESCE → CLOSE
  → post_close(CFG+MAP+IRQ+DMA+CLOSE fail) → REOPEN/CLOSE
```

Honesty lamps (soft only; lean; no stamp storm):

```text
ddi_host: soft residual lean … product=UDX+ABI …
ddi_host: soft cap mint honesty …
ddi_host: soft window residual honesty …
ddi_host: soft door residual honesty …
ddi_host: soft mint claim=0 …
```

## API surface used

From `user/libgj/include/gj/syscalls.h` + raw `gj_ddi` for ops not yet wrapped:

| Symbol / op | Role |
|-------------|------|
| `GJ_SYS_DDI` / `gj_ddi` | Multiplex soft DDI door |
| `gj_ddi_scan` / `gj_ddi_get` | Soft PCI inventory |
| `gj_ddi_open` | Soft handle for index |
| `gj_ddi_map_bar` / `GJ_DDI_OP_MAP_BAR` | Soft BAR map note |
| `gj_ddi_cfg_read` | Soft config dword snap |
| `gj_ddi_cfg_write` / op **16** | Careful soft-note + BAR reject residual |
| `gj_ddi_dma_note` | Soft DMA window note |
| `gj_ddi_inventory` | Optional once surface lamp |
| raw op **8** CLOSE | Soft lifecycle forget |
| raw op **9** IRQ_BIND | Soft IRQ badge note |
| raw op **10** DMA_BUF_ALLOC | Soft page residual |
| raw op **11** DMA_BUF_FREE | Soft free after map |
| raw op **12** DMA_BUF_MAP | Soft bus cookie residual |
| post_close (CFG+CLOSE after CLOSE) | Functional dead-handle forget check |
| `gj_notify_wait` (poll) | Soft notify residual; fire not claimed |
| `gj_debug_log` / `gj_exit` | Soft markers + terminate |

Local packed layouts match `kernel/include/gj/ddi_door.h` (`gj_ddi_dev_info`, `gj_ddi_map_note`).

## Coordinator note (optional)

Prefer this local Makefile. If a one-liner phony is desired at repo root later:

```make
.PHONY: ddi_host-gj
ddi_host-gj:
	$(MAKE) -C user/drivers/ddi_host_gj
```

No kernel embed or boot spawn is wired by this drop — soft host residual only.

## License

Dual **MIT OR Apache-2.0** (Soft!=product dual license). Clean-room; **no GPL** source. Soft path is **not** product DDI authority. Soft != product. mint=0. No kernel stamp. No version stamp.
