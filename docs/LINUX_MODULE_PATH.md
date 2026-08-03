# GreenJade — Linux module path (laptop / host-collected drivers)

| Field | Value |
|-------|--------|
| **Status** | **Path documented** — collect + stage **live**; soft module loader + ksym **in progress**; load soft-landed on DUT (r8169 INIT=0); soft probe stride fixed 2026-08-03; product probe / datapath **OPEN** |
| **DUT class** | Laptop real-HW (G752VT primary; same steps for other Intel x86_64) |
| **Law** | Dual **MIT OR Apache-2.0** source tree; **no GPL source in tree** |
| **Strategy** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) — not freestanding class-driver thrash |
| **Operator runbook** | [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) (UDX soft host + media) |
| **Inventory oracle** | [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · `make linux-hwtest-img` |
| **Wave status** | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) |

## One-sentence goal

**Run GreenJade on a laptop with host-collected Linux modules staged on install media, via a soft module loader + ksym surface and `finit_module` smokes — not by freestanding rewrites of every class driver, and not by importing GPL source into the knano tree.**

---

## Wave note — 2026-08-03 (NETDEV SOFT 0 after LOAD OK)

**Photos:** `1000003238` / `1000003240` — `MOD R8169 LOAD OK INIT=0` then `NETDEV SOFT 0`.

| Item | Fact |
|------|------|
| **Root cause** | Soft `struct pci_device_id` was **32 bytes**; RHEL **9.8** `r8169.ko` table rows are **40 bytes** (`driver_data` + `override_only` + pad) |
| **Effect** | Soft `id_table` walk **false-ended** before `10ec:8168` → no soft netdev bind |
| **Fix** | Soft `sizeof(pci_device_id) == 40`; force-EMU bind safety net; STATUS lamp `probe 10ec:8168 soft\|miss` |
| **Honesty** | **Soft ≠ product.** Load/init OK + soft probe ≠ TX/RX. **G-AC-1:** no Linux `.ko` product claim. Dual **MIT OR Apache-2.0** tree only. |

---

## Honesty first

| Rule | Meaning |
|------|---------|
| **No GPL source in tree** | Never paste Linux `r8169.c` / `xhci-*.c` / kernel trees into knano. |
| **Staged `.ko` are host binaries** | Often GPL-bound; operator/dev media for **ABI + module-path** work. **Not** dual-license product source. |
| **G-AC-1 (product AC)** | Steam/bar3 / shipping product acceptance is **not** “we ship GPL `.ko`.” Product class-driver story remains **Linux-SHAPED UDX** + dual-license out-of-tree when claims are made. |
| **Module path = engineering track** | Collect → stage → soft loader/ksym → `finit_module` → iterative ksym resolve → probe → datapath. Most of this is still **OPEN**. |
| **Not freestanding thrash** | In-kernel `rtl8168` / `xhci_msc` stage numbers are **lab honesty only**, not the primary laptop driver strategy. |

```text
  ALLOWED                                      FORBIDDEN
  ─────────────────────────────────────────    ──────────────────────────────────
  Collect host .ko + firmware for media        Import GPL driver *source* into knano
  Stage under GJ-PERSIST/linux-drivers/        Claim freestanding stage=15 = T1 product
  Soft module loader + ksym (dev)              Claim bar3 closed because .ko staged
  finit_module / boot smoke (when wired)       Treat inventory PASS as GJ product
  Iterative unresolved-symbol resolve          "Just insmod r8169 and we're done"
  Parallel: clean-room UDX hosts (wave D)      Claiming soft probe PASS = TX/RX / BOT
```

---

## Path overview (five steps)

```text
  ┌──────────────────────────────────────────────────────────────────┐
  │ 1. COLLECT   make collect-linux-drivers / collect-linux-drivers.sh │
  │              → build/linux-drivers/{modules,firmware,meta}/        │
  ├──────────────────────────────────────────────────────────────────┤
  │ 2. STAGE     make hwtest-img → GJ-PERSIST/linux-drivers/           │
  │              + ESP NEEDED-DRIVERS.txt                              │
  ├──────────────────────────────────────────────────────────────────┤
  │ 3. LOADER    Soft module loader + ksym table          IN PROGRESS  │
  │              (ELF .ko parse, symbol resolve, live surface)         │
  ├──────────────────────────────────────────────────────────────────┤
  │ 4. SMOKE     finit_module / init_module + boot smoke     OPEN      │
  │              (syscall stubs exist in graph; kernel load OPEN)      │
  ├──────────────────────────────────────────────────────────────────┤
  │ 5. RESOLVE   Full r8169 / xhci_hcd need large ksym     ITERATIVE   │
  │              surface — resolve missing exports one by one          │
  └──────────────────────────────────────────────────────────────────┘
         │
         ▼  DoD “can run with Linux drivers” (see §DoD)
    module loads → probe binds PCI → net/usb datapath
         (each step marked DONE or OPEN below)
```

---

## Step 1 — Collect `.ko` (`collect-linux-drivers`)

| Item | Detail |
|------|--------|
| **Script** | `/home/jay/Documents/knano/scripts/collect-linux-drivers.sh` |
| **Make** | `make collect-linux-drivers` → `build/linux-drivers/` |
| **Input** | Host `/lib/modules/$(uname -r)` (+ firmware); optional DUT `NEEDED-DRIVERS` from linux-hwtest |
| **Output** | `modules/*.ko*` · `firmware/**` · `meta/*` · **`NEEDED-DRIVERS.txt`** |
| **Greppable** | `collect-linux-drivers: PASS` |
| **Status** | **Live** |

G752VT-class minimum set (from script map):

| PCI / class | Preferred modules |
|-------------|-------------------|
| `8086:a12f` xHCI | `xhci_pci`, `xhci_hcd` (+ `usbcore`, `usb_common`, …) |
| `10ec:8168` NIC | `r8169` (+ phy / mdio deps) |
| USB MSC stick | `usb_storage` / `uas` |
| Storage (as present) | `ahci` / `nvme` / `sd_mod` / … |

**xHCI presence honesty:** many hosts build `xhci_pci` / `xhci_hcd` / `usbcore` as **builtin** (no `.ko`). Collect writes `meta/XHCI-STATUS.txt` and greps `collect-linux-drivers: xhci 8086:a12f xhci_pci=BUILTIN|PRESENT|MISSING`. Soft boot path then: `main: soft linux_module xhci path SKIP builtin` until a plain `.ko` is staged and optionally `./scripts/embed-linux-mod.sh xhci_pci` (weak embed; Makefile links only if `kernel/proc/xhci_pci_mod_blob.S` exists).

Operator recipe:

```sh
cd /home/jay/Documents/knano

# Prefer same kver/HW as the DUT when possible
make collect-linux-drivers
# or: ./scripts/collect-linux-drivers.sh build/linux-drivers

ls build/linux-drivers/modules
cat build/linux-drivers/NEEDED-DRIVERS.txt
# greppable: collect-linux-drivers: PASS
```

From a **Linux inventory** stick (`make linux-hwtest-img`), `70-needed-drivers.sh` emits a DUT-specific **NEEDED-DRIVERS** list so collect can be aimed at real IDs — still inventory only, not GreenJade product.

---

## Step 2 — Stage on media

| Item | Detail |
|------|--------|
| **Image** | `make hwtest-img` (depends on `collect-linux-drivers`) |
| **Persist** | **`GJ-PERSIST/linux-drivers/`** — full tree (`modules/`, `firmware/`, `meta/`, checklist) |
| **ESP** | **`EFI/GREENJADE/NEEDED-DRIVERS.txt`** (checklist copy when present) |
| **Flash** | `sudo make install-hwtest-usb DEV=/dev/sdX` |
| **Status** | **Live** (staging ≠ load) |

On-media layout after a good pack:

```text
GJ-PERSIST/
  linux-drivers/
    modules/          — .ko / .ko.xz / .ko.gz / .ko.zst
    firmware/         — soft firmware blobs (e.g. rtl_nic/*)
    meta/             — G752VT-MAP, COPIED, MISSING, BUILTIN, HOST-LSPCI-K
    NEEDED-DRIVERS.txt
EFI/GREENJADE/
  NEEDED-DRIVERS.txt  — ESP mirror of checklist (when staged)
```

Verify on lab host after write:

```sh
sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/linux-drivers/modules
cat /mnt/gj-persist/linux-drivers/NEEDED-DRIVERS.txt
sudo umount /mnt/gj-persist
```

**Honesty:** staged modules sit on media for **module-path development**. GreenJade **does not yet** load them at boot.

---

## Step 3 — Soft module loader + ksym (**in progress**)

| Piece | Intent | Status |
|-------|--------|--------|
| **Soft module loader** | Read staged `.ko` (ELF relocatable), allocate core, apply relocs, call init | **In progress** — not product boot path |
| **ksym table** | Export a GreenJade / Linux-shaped symbol surface modules can resolve against | **In progress** — surface starts small |
| **Compression** | Decompress `.ko.xz` / `.gz` / `.zst` before load | **OPEN** (collect may leave compressed) |
| **modversions / vermagic** | Host kver vs GJ surface mismatch handling | **OPEN** — expect iterative fail-closed |
| **Dependency order** | Load `usbcore` before `xhci_hcd`, phy before `r8169`, etc. | **OPEN** (meta/deps from collect help operators) |

Design constraints:

1. **Not freestanding thrash** — do not grow in-kernel `rtl8168.c` / `xhci_msc.c` as the product NIC/USB path while this track exists.  
2. **Not GPL source in tree** — implement clean-room loader/ksym; modules remain **operator-collected binaries**.  
3. **Fail closed** — unresolved symbol → refuse load with greppable reason; no silent half-init.  
4. **Parallel track** — wave D **UDX** hosts (`rtl8168_udx` / `xhci_udx`) remain the dual-license **product-shaped** class story; this module path is the **ABI/host experiment** for “run with Linux drivers.”

---

## Step 4 — `finit_module` / boot smoke

| Surface | Where | Status |
|---------|-------|--------|
| Linux NR defs | `kernel/include/gj/linux_abi.h` — `LINUX_NR_init_module` (175), `delete_module` (176), `finit_module` (313) | **Present** |
| Userspace graph wrappers | `user/libcgj/src/graph_batch3.c` — `init_module` / `finit_module` / `delete_module` syscalls | **Present** (call into kernel) |
| Kernel load implementation | `init_module` / `finit_module` → soft loader | **OPEN** |
| Boot smoke | Early or init-time: open staged `.ko` → `finit_module` → greppable PASS/FAIL | **OPEN** |
| Panel / serial lamps (target) | e.g. `module: soft finit …` / `module: load FAIL unresolved=…` | **Not landed** |

Target smoke sequence (when loader lands):

```text
1. Mount / find GJ-PERSIST/linux-drivers/modules (or ESP staging)
2. Order deps (usbcore → xhci_hcd → xhci_pci; phy → r8169; …)
3. open(2) each .ko → finit_module(fd, params, flags)
4. Grep: module load PASS/FAIL + first unresolved ksym on fail
5. Do not claim PCI probe or datapath from load-alone
```

Until the kernel path exists, **expect ENOSYS / soft fail** — that is honest, not a silent pass.

---

## Step 5 — Large ksym surface (r8169 / xhci_hcd) — iterative

Full **`r8169`** and **`xhci_hcd`** (plus `xhci_pci`, `usbcore`, …) pull a **large** export set: PCI core, DMA API, IRQ, workqueues, timers, netdev/USB core, firmware loader, PM, etc.

| Reality | Implication |
|---------|-------------|
| One `insmod` on Linux resolves against a full kernel | On GreenJade, **every** unresolved symbol is a gap |
| Host modules may use **modversions** / specific kver | Bit-identical host kver helps; still not a guarantee |
| Completing the surface is **iterative** | Load → list missing → add ksym / shim → reload; **not** one PR |
| Do not thrash freestanding as substitute | Keep freestanding lab-only; invest in ksym + loader fidelity |

Recommended loop:

```text
  load candidate .ko
       │
       ├─ PASS init → try next dep / probe
       │
       └─ FAIL unresolved SYM
              → implement clean-room ksym or document BLOCKED
              → re-smoke
              → never copy Linux source to “fix” the symbol
```

**Minimal first targets** (smoke before full NIC/USB):

1. Tiny / leaf modules with tiny dep graphs (if any staged).  
2. `libphy` / `mdio_devres`-class helpers before full `r8169`.  
3. `usb_common` / core fragments before full `xhci_hcd`.  
4. Only then attempt `r8169` / `xhci_pci`+`xhci_hcd` with eyes open on ksym debt.

---

## Relationship to UDX soft host (wave D)

Two tracks — do not collapse claims:

| Track | What | Product AC? |
|-------|------|-------------|
| **A — UDX soft host** | `GJ_SYS_DDI=103`, `rtl8168_udx` / `xhci_udx` clean-room skeletons | Dual-license **shape**; TX/RX / BOT still **OPEN** |
| **B — Linux module path** (this doc) | Collect / stage / soft loader / ksym / `finit_module` | **Engineering / ABI** track; GPL binaries on media ≠ tree source; shipping-as-product still **G-AC-1** careful |

Operators may run **both**: wave D lamps prove DDI/UDX wiring; module path proves hostability of collected drivers. Neither alone closes bar3.

Details: [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [DDI_SOFT.md](DDI_SOFT.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md).

---

## Definition of done — “can run with Linux drivers”

Use this checklist for the **module path**. Mark each row honestly.

### DoD checklist

| # | Criterion | Meaning of DONE | Status |
|---|-----------|-----------------|--------|
| **D1** | **Collect** | `make collect-linux-drivers` produces modules + `NEEDED-DRIVERS.txt`; greppable `collect-linux-drivers: PASS` | **DONE** |
| **D2** | **Stage** | `make hwtest-img` packs tree to **`GJ-PERSIST/linux-drivers/`** (+ ESP checklist) | **DONE** |
| **D3** | **Soft loader + ksym** | In-tree soft loader can parse/relocate a `.ko` against a documented ksym table; fail-closed on missing symbols | **OPEN** (in progress) |
| **D4** | **`finit_module` / boot smoke** | Kernel implements load path; boot or init smoke greppable PASS/FAIL for at least one staged module | **OPEN** |
| **D5** | **Module loads** | At least one real staged module reaches `init` return 0 on GJ (deps satisfied) | **OPEN** |
| **D6** | **Probe binds PCI** | Loaded driver probe matches DUT ID (`10ec:8168` and/or `8086:a12f`); device bound (not mere module init) | **OPEN** |
| **D7** | **Net datapath** | Bound NIC: link + TX/RX (or equivalent product net I/O) on laptop silicon | **OPEN** |
| **D8** | **USB datapath** | Bound xHCI: host controller runs; device path (e.g. MSC/HID) functional beyond soft lamp | **OPEN** |
| **D9** | **ksym honesty** | Docs/logs list remaining unresolved surface for full `r8169` / `xhci_hcd`; iterative resolve, no freestanding thrash substitute | **OPEN** (process) |
| **D10** | **License honesty** | No GPL **source** in knano; staged `.ko` not claimed as dual-license product AC / bar3 close | **DONE** (policy) |

### One-glance

```text
DONE                         OPEN
─────────────────────────    ────────────────────────────────────────
D1 Collect .ko               D3 Soft module loader + ksym (in progress)
D2 Stage on media            D4 finit_module / boot smoke
D10 No GPL source in tree    D5 Module loads (init returns 0)
  / no bar3-by-.ko claim     D6 Probe binds PCI (8168 / a12f)
                             D7 Net datapath (TX/RX)
                             D8 USB datapath (HC + device path)
                             D9 Full r8169/xhci_hcd ksym surface
```

### What “can run with Linux drivers” does **not** mean yet

- Staged modules on USB ≠ loaded modules.  
- Soft UDX probe PASS ≠ Linux `.ko` probe.  
- Freestanding `xhci stage=15` / `net t/f/b/r` ≠ module-path product.  
- bar3 / Steam client ≠ module load.  
- G-AC-1 still applies to **shipping product acceptance**.

---

## Operator quick commands

```sh
cd /home/jay/Documents/knano

# 1–2 Collect + stage + flash
make collect-linux-drivers
make hwtest-img
sudo make install-hwtest-usb DEV=/dev/sdX   # careful: whole device

# Inventory stick (optional — DUT NEEDED-DRIVERS oracle)
make linux-hwtest-img
# sudo make install-linux-hwtest DEV=/dev/sdY

# After boot (when loader exists): look for module lamps — today expect OPEN
# grep -E 'module:|finit_module|ksym|collect-linux' KLOG / panel capture
```

---

## Related

| Doc | Role |
|-----|------|
| [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) | Strategy: ABI + host path, not freestanding thrash |
| [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) | G752VT UDX host runbook + media labels |
| [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) | Soft inventory honesty |
| [DDI_SOFT.md](DDI_SOFT.md) | Soft DDI / cap notes |
| [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) | Linux inventory oracle |
| [HCL.md](HCL.md) | Tiers + product-path table |

---

*Dual MIT OR Apache-2.0 source tree. Operator-collected `.ko` on media = module-path engineering, not GPL import, not freestanding thrash, not bar3.*  
*DoD D1–D2–D10 live; D3–D9 OPEN until loader, load, probe, and datapath evidence exist.*
