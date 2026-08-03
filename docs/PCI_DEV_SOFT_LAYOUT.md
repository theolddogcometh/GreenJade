# Soft `struct pci_dev` — probe-shaped field plan (r8169 / 10ec:8168)

| Field | Value |
|-------|--------|
| **Status** | **PLAN only** — EMU bind stays live; real `.ko` probe **not** enabled |
| **Host oracle** | RHEL 9.8-class **5.14.0-687** x86_64 (`/lib/modules/…/build`, `vmlinux.h`) |
| **Soft code** | [`kernel/include/gj/linux_pci_soft.h`](../kernel/include/gj/linux_pci_soft.h) · [`kernel/mm/linux_pci_soft.c`](../kernel/mm/linux_pci_soft.c) |
| **Law** | Dual **MIT OR Apache-2.0**; **no GPL source** in tree; host headers / staged `.ko` are **oracle only** |
| **Honesty** | **Soft ≠ ABI-stable** — do **not** claim binary layout match to any Linux kver |

Companion: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · `build/linux-drivers/meta/R8169-UNRESOLVED-PLAN.txt`.

---

## 1. Why EMU bind exists

GreenJade soft `struct pci_dev` is a **small clean-room snapshot** (vendor/device/BDF/BARs/irq/drvdata) used by soft helpers and inventory fill.

Linux `struct pci_dev` (host RHEL 5.14) is a **multi-kilobyte** object: list links, `struct pci_bus *`, capability caches, **embedded `struct device`**, `struct resource resource[17]`, enable refcounts, MSI bookkeeping, RH KABI pads, …

Calling real `r8169` `probe(pdev, id)` with the soft object would read/write **wrong offsets** (fault or memory corruption). Current path:

```text
id_table match → lpcis_soft_emu_bind → soft netdev register
                 (NO .ko probe; lamp: soft probe emu …)
```

Goal of this doc: list **early probe requirements** and a **recommended soft field order** so a future “probe-shaped” buffer can be filled **before** re-enabling real probe. Until then EMU remains correct.

---

## 2. How r8169 touches `pci_dev` early (API / macro shape)

No Linux driver source is imported. Early touch set is inferred from:

- Host `include/linux/pci.h` macros / inlines (`pci_resource_*`, `pci_set_drvdata`, `pci_name`)
- Host `include/linux/netdevice.h` (`SET_NETDEV_DEV`)
- Host `include/linux/device.h` (`driver_data`, `dma_mask`, `parent`)
- Staged `r8169.ko` unresolved surface (`dma_set_mask*`, `pcim_*`, `devm_alloc_etherdev_mqs`, `_dev_err`, … — see R8169 plan)

Typical **early** NIC probe shape (logical order, soft names):

| Step | What probe expects | Linux field / API path | Soft today |
|------|--------------------|------------------------|------------|
| 1 | Enable MMIO/IO decode | `pci_enable_device` / `pcim_enable_device` → `enable_cnt`, command via cfg | Soft **function** body on soft layout |
| 2 | Bus master | `pci_set_master` → cmd bit + `is_busmaster` | Soft function |
| 3 | Claim BARs | `pci_request_regions` / selected regions | Soft flag only |
| 4 | BAR phys/len | **`pci_resource_start/len` macros** → `pdev->resource[bar].start/end` | Soft **arrays** + optional ksym **functions** (macros **do not** call ksym) |
| 5 | Map MMIO | `ioremap` / `pcim_iomap*` with BAR phys | Soft ioremap identity |
| 6 | IRQ | `pci_alloc_irq_vectors` + `pci_irq_vector` / `pdev->irq` | Soft line IRQ **11** |
| 7 | DMA mask | `dma_set_mask(_and_coherent)(&pdev->dev, …)` → `dev.dma_mask` / `coherent_dma_mask` | Soft no-op ksym; **no** embedded `dev` |
| 8 | Netdev parent | `SET_NETDEV_DEV(nd, &pdev->dev)` → `nd->dev.parent = &pdev->dev` | Soft netdev has `pParentDev` only if set by soft path |
| 9 | Drvdata | **`pci_set_drvdata` inline** → `pdev->dev.driver_data` | Soft field `driver_data` at wrong offset vs Linux |
| 10 | Logs / name | `pci_err` / `dev_err` / `pci_name` → `&pdev->dev` / `kobj.name` | Missing |
| 11 | Config | `pci_read_config_*` needs correct **BDF** | Soft `bus`+`devfn` filled from inventory; works for **soft** helpers |

### Critical: macros vs ksym functions

On Linux headers used by the staged module:

```c
/* host oracle — not in knano tree */
#define pci_resource_start(dev, bar)  ((dev)->resource[(bar)].start)
static inline void pci_set_drvdata(struct pci_dev *pdev, void *data)
{ dev_set_drvdata(&pdev->dev, data); }
```

So a host-built `r8169.ko` typically has **inlined** loads/stores into:

- `resource[N].start` / `.end` / `.flags`
- `dev.driver_data`
- `&pdev->dev` for DMA / parent / printk

GreenJade ksym entries for `pci_resource_start` / `pci_set_drvdata` help **soft modules** and any **out-of-line** unds; they **do not** fix inlined offsets inside the `.ko`. Real probe therefore needs either:

1. a **layout-approximating** soft `pci_dev` blob sized/offset for the host kver, **or**
2. a **non-`.ko`** path (UDX / soft-only probe) forever.

This plan assumes (1) for module-path engineering only (G-AC-1 still forbids bar3-by-GPL-`.ko` product claim).

---

## 3. Host field order (RHEL 5.14.0-687 `vmlinux.h` — oracle)

Soft≠ABI-stable: offsets below are **host facts for one kver**, not a GreenJade product ABI. CONFIG and RH_KABI change them across rebuilds.

### 3.1 Early `pci_dev` header (stable prefix on this host)

Computed from host `vmlinux.h` field order (x86_64 LP64, natural alignment):

| Offset (host) | Field | Size / notes | Needed early for r8169? |
|---------------|-------|--------------|-------------------------|
| `0x00` | `bus_list` | `list_head` 16 B | Low (list walk) |
| `0x10` | `bus` | `struct pci_bus *` | **Yes** if anything walks bus; soft uses **u8 bus number** instead |
| `0x18` | `subordinate` | ptr | No for EP NIC |
| `0x20` | `sysdata` | ptr | No |
| `0x28` | `procent` | ptr | No |
| `0x30` | `slot` | ptr | No |
| `0x38` | `devfn` | `u32` | **Yes** (config / naming) |
| `0x3c` | `vendor` | `u16` | **Yes** |
| `0x3e` | `device` | `u16` | **Yes** |
| `0x40` | `subsystem_vendor` | `u16` | Maybe (quirks) |
| `0x42` | `subsystem_device` | `u16` | Maybe |
| `0x44` | `class` | `u32` | Low after match |
| `0x48` | `revision` | `u8` | **Yes** often (chip rev) |
| `0x49` | `hdr_type` | `u8` | Low |

Soft today: `vendor` at **offset 0**, `devfn` as **u8**, no `bus*` — **cannot** satisfy inlined host offsets.

### 3.2 Mid fields (before embedded `dev`)

| Host field | Role | Early? |
|------------|------|--------|
| `driver` | bound `pci_driver *` | Bookkeeping |
| `dma_mask` | `u64` on `pci_dev` (often mirrored by `dev.dma_mask` ptr) | **Yes** for DMA setup |
| `dma_parms` | segment limits | Sometimes |
| `current_state` / PM bits | D0 bookkeeping | Soft no-op OK initially |
| capability offsets (`pcie_cap`, `msi_cap`, …) | MSI / PCIe helpers | When MSI path used |

### 3.3 Embedded `struct device dev` (largest early dependency)

`pci_set_drvdata`, `SET_NETDEV_DEV`, `dma_set_mask`, `dev_err`, `pci_name` all go through **`&pdev->dev`**.

Host `struct device` front (order oracle):

| Field | Early probe use |
|-------|-----------------|
| `kobj` (esp. `kobj.name`) | `dev_name` / `pci_name` |
| `parent` | bus device parent; netdev parent chain |
| `init_name` | fallback name |
| `driver_data` | **drvdata** |
| `dma_mask` (`u64 *`) | points at mask storage |
| `coherent_dma_mask` | coherent alloc mask |
| `dma_parms` | optional |

Soft has **no** embedded `device`. Recommended: carve a **soft device front** inside a future probe-shaped blob (not full Linux `device`).

### 3.4 After `dev`: irq + resources

| Host field | Layout note | Early? |
|------------|-------------|--------|
| `cfg_size` | config space size | Low |
| `irq` | line / first vector bookkeeping | **Yes** |
| `resource[17]` | each `struct resource` ≈ **56 B** on x86_64 (`start`,`end`,`name`,`flags`,`desc`,`parent`,`sibling`,`child`) | **Yes** BAR0/2 for RTL8168 |
| `enable_cnt` | `pci_enable_device` refcount | **Yes** if enable path is real |
| `msi_enabled` / `msix_enabled` | after vector alloc | Soft bookkeeping |

`pci_resource_start(pdev, 0)` expands to `pdev->resource[0].start` — host offset of `resource` is **after** the full `struct device` (hundreds of bytes). Soft parallel arrays at the **end of a tiny struct** are **not** at that offset.

---

## 4. Soft today vs probe needs

| Capability | Soft `pci_dev` / helpers | Real `.ko` early probe |
|------------|--------------------------|-------------------------|
| vendor/device | filled from devmgr | needs host offsets `0x3c/0x3e` |
| BDF | `u8 bus` + `u8 devfn` | host: `bus*` + `u32 devfn` |
| BAR phys/len | `resource_start[]` / `resource_len[]` + soft functions | inlined `resource[N].start/end` |
| irq | `int irq = 11` | host `unsigned int irq` after `dev` |
| drvdata | soft `driver_data` | `dev.driver_data` |
| DMA | ignore on soft pci_dev | `dev.dma_mask` / coherent |
| parent | none | `&pdev->dev` for netdev |
| size | ~O(100) bytes | multi-KB |

Greppable incompleteness (fill path):

```text
linux_pci_soft: soft pci_dev incomplete field=…
```

---

## 5. Recommended next soft fields (priority)

Do **not** flip EMU→real probe until a dedicated “probe-shaped” buffer exists and is validated. Suggested fill order:

### P0 — must approximate for first real probe smoke

1. **Host-shaped prefix** through `vendor`/`device`/`devfn`/`class`/`revision`  
   - Either pad soft object to host offsets for those fields, **or** maintain a separate `struct pci_dev_hostish` blob only passed to `.ko` probe.
2. **`struct resource resource[6]`** (std BARs) with `start`/`end`/`flags` (`IORESOURCE_MEM`/`IO`) from inventory  
   - Place at **host offset of `resource`** for the target kver, **or** accept that only non-macro BAR access works.
3. **Embedded soft `device` front**: at least `driver_data`, `dma_mask` pointer, `coherent_dma_mask`, `parent` (may be NULL), `init_name` / `kobj.name` string for `"0000:BB:DD.F"`.
4. **`irq`** at host offset (or ensure all IRQ use is via out-of-line `pci_irq_vector` ksym — currently not true for all drivers).
5. **`dma_mask` u64** storage + `dev.dma_mask = &dma_mask` (default `DMA_BIT_MASK(64)` soft accept).
6. **`enable_cnt`** = 1 after soft enable (if enable is called from probe).

### P1 — soon after first non-fault probe entry

7. `subsystem_*` from config space read  
8. `driver` pointer → registered soft view / `.ko` driver object  
9. `msi_enabled` / soft MSI state after `pci_alloc_irq_vectors`  
10. `is_busmaster` bit after `pci_set_master`  
11. Minimal `struct pci_bus` stub so `pdev->bus` is non-NULL (number + ops soft)

### P2 — later (probe may limp without)

12. Full PM / ASPM / AER / VPD / SR-IOV tails  
13. `saved_config_space`, res sysfs attrs  
14. RH reserved pads to match exact `sizeof(pci_dev)` for a given kver

### Explicit non-goals

- **ABI-stable product `pci_dev`** across all distros  
- Shipping GPL `.ko` as bar3 / product AC (**G-AC-1**)  
- Changing **`pci_device_id` stride** (40-byte RHEL row — separate work; do not regress)

---

## 6. Implementation strategies (pick one later)

| Strategy | Pros | Cons |
|----------|------|------|
| **A. Dual object** — soft inventory `pci_dev` + hostish probe blob | Soft helpers stay simple; EMU unchanged | Two fills; offset table per kver |
| **B. Grow soft struct** toward host offsets with explicit pads | One object | Fragile; RH_KABI; huge pads; easy to get wrong |
| **C. Stay EMU forever** + UDX product NIC | Law-friendly product path | Module-path D6 “real probe” stays OPEN |

**Recommendation:** **A** for module-path experiments; product laptop NIC remains UDX/DDI ([LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md)).

When real probe is wired:

```text
match → fill hostish blob from inventory
      → optional: still log soft probe emu OFF
      → call pView->probe(pHostish, pId)
      → on fault/nonzero: fall back EMU + FAIL lamp
```

Gate behind an explicit flag so default boot **keeps EMU** until proven.

---

## 7. Offset table maintenance

Host offsets are **not** checked into GreenJade as a product ABI. When deepening:

1. Re-read host `vmlinux.h` / `pahole -C pci_dev` for the **same** kver as staged `r8169.ko`.  
2. Update the table in §3 (date + kver stamp).  
3. Re-run soft fill incompleteness lamps.  
4. Never paste GPL driver source to “learn” fields.

Operator one-liner (lab host, not freestanding):

```sh
# Example — requires pahole / matching debuginfo; optional
pahole -C pci_dev /lib/modules/$(uname -r)/build/vmlinux 2>/dev/null | head
```

---

## 8. Greppable lamps

| Lamp | Meaning |
|------|---------|
| `linux_pci_soft: soft probe emu (no .ko probe; …)` | Current safe path |
| `linux_pci_soft: soft pci_dev incomplete field=…` | Fill notes missing host-shaped pieces |
| `linux_pci_soft: soft probe 10ec:8168 PASS\|FAIL\|SKIP` | Bind outcome (EMU or future real) |
| `linux_pci_soft: soft force emu 10ec:8168 PASS\|SKIP` | Safety-net bind without `.ko` register |

---

*Soft ≠ product. Soft ≠ ABI-stable. Host layout is oracle for one kver only.*
*EMU bind remains correct until a probe-shaped blob is intentionally enabled.*
