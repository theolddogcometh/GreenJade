# GreenJade — Linux module path (laptop / host-collected drivers)

| Field | Value |
|-------|--------|
| **Status** | **Abandoned leftover** (not linked). **Dual DoD (TODO Current track):** **A** `xhci_udx` USB path **OPEN** (this doc is leftover module-path / ksym eng, not A close; never `USBCMD.RS=1`). **B** **OPEN** until host **interactive SSH login** (not this doc’s close). Image: **STATUS (static) v0.1.184** via `GJ_IMAGE_VERSION` — packed, not host-probed. **0.2.0** reserved. Soft ≠ product; **G-AC-1**; dual **MIT OR Apache-2.0**; bar3 only [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |
| **DUT class** | Laptop real-HW (G752VT primary; same steps for other Intel x86_64) |
| **Law** | Dual **MIT OR Apache-2.0** source tree; **no GPL source in tree** |
| **Strategy** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) — not freestanding class-driver thrash |
| **Operator runbook** | [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) (UDX soft host + media) |
| **Inventory oracle** | [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · `make linux-hwtest-img` |
| **Wave status** | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) · [TODO.md](TODO.md) § Current track |
| **Bar3** | [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) only — not module-path kprintf |
| **pci_dev plan** | [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) — Strategy A hostish **0xb40** (real probe soft PASS) |
| **MMIO handoff** | [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) — freestanding → soft r8169 ownership; gate `GJ_SOFT_R8169_MMIO_HANDOFF` **default 0** |

## One-sentence goal

**Host-collected Linux modules staged on install media, via a soft module loader + ksym surface for ABI/hostability engineering — not freestanding rewrites of every class driver, not GPL source in the knano tree, and not product AC by running `.ko` in the kernel.**

### Law stamp (read first)

| Rule | Meaning |
|------|---------|
| **G-AC-1** | **No Linux `.ko` binary runs in the kernel as product.** Soft load / REAL probe / hybrid lamps are eng only — never Steam/bar3 product AC. |
| **Product** | **Hot + cold Linux ABI** + **Linux-shaped drivers in userspace** (**UDX/DDI**). Product NIC is **not** in-kernel `r8169.ko`. |
| **Soft residual** | Soft **`RUN_INIT=0`** / **`freestanding_no_exec`** (SKIP exec of staged `.ko` **init in kernel**) is **engineering residual** — “do not execute `.ko` init in-kernel” — **not** “never use Linux drivers.” Userspace Linux-shaped hosts remain allowed and are the product path. |
| **Lab freestanding rtl** | In-kernel `rtl8168.c` **SKIP** (`./abandoned`). Dual DoD **B** is `rtl8168_udx` + **interactive SSH login**. Not product NIC. |
| **This doc’s track** | Collect → stage → soft loader/ksym → eng load/probe/datapath experiments. **Soft ≠ product.** |

---

## Wave note — 2026-08-04 (leftover MAP · module path ≠ Dual DoD A close)

**Dual DoD (normative: [TODO.md](TODO.md) § Current track):** **A** `xhci_udx` host USB path **OPEN**. This doc is leftover ksym / module-path eng — **not** Dual DoD A close. **B** **OPEN** until host **interactive SSH login**; not closed by module soft lamps; **not** “product = in-kernel `r8169.ko`.” Soft ≠ product · **G-AC-1** · dual **MIT OR Apache-2.0**.

**Lab evidence (G752):** greppable STATUS / serial lamps below. Fly **v0.1.184** packed, not host-probed (Dual DoD **A/B OPEN**; never `USBCMD.RS=1`; **0.2.0** reserved). Host lamps below are **historical (0.1.180)** unless restamped. Confirm image cut via **STATUS (static) v…** / `GJ_IMAGE_VERSION` (no test-panel photo IDs).

| STATUS / lamp | Value |
|---------------|--------|
| **image** | **STATUS (static) v0.1.184** (`GJ_IMAGE_VERSION`) — packed, not host-probed |
| **probe** | `PROBE SOFT` at **gate0** (typical) |
| **NET** | freestanding **ICMP proven earlier** · guest **10.200.125.50** · hybrid **SOFT gate0**; **recent boots R0/R1 regression** |
| **USB freestanding** | Stage 15 GET_CONFIG TO (lab path; ≠ DoD A close) |
| **usb-storage** | soft seed **+33** `usb_*`/`scsi_*`; may still **FAIL KSYM** on `sg_nents` etc. |
| **xHCI** | **SKIP BUILTIN** |

| Item | Fact |
|------|------|
| **DoD A (USB module)** | **OPEN** until host USB path — this doc is abandoned-module history; product USB is **xhci_udx** (RS-off park). Soft usbcore seed ≠ close |
| **NIC hybrid** | Soft eng live; **gate0 SOFT** (no REAL BAR); prior hybrid 4a eng when RX live |
| **USB module** | xHCI **SKIP BUILTIN**; `linux_usb_soft` seed; remaining miss ~`sg_nents` |
| **DoD B (interactive SSH login)** | **OPEN** until host **interactive SSH login** — banner / :22 / ICMP ≠ close ([TODO.md](TODO.md)) |
| **Prior 4a / ICMP** | `HYBRID WIRE=FS SOFT=R8169` · L2 RX live once · freestanding ICMP once |
| **Not proven** | Full `.ko` wire (4b); USB MSC via module; product AC; continuous ICMP every boot |
| **Honesty** | **Soft ≠ product.** Hybrid eng ≠ product. **G-AC-1.** Dual **MIT OR Apache-2.0**. |

### Prior lamps (hybrid 4a eng)

| STATUS / lamp | Value |
|---------------|--------|
| **netdev** | `NETDEV SOFT 1` |
| **probe** | `PROBE 10EC:8168 REAL` · `REG=1 MATCH=1 ST=0` |
| **ksym** | **N=295** |
| **mod** | `MOD R8169 INIT=0` |
| **hold14** | **`L2 BR RX=2 TX=0`** |
| **hold15** | **`HYBRID WIRE=FS SOFT=R8169`** |
| **NET** | `10.200.125.50 UP T4/F0/B924/R2` |
| **xHCI** | `MOD XHCI PCI SKIP BUILTIN` · hold13 `USB linux path OPEN builtin` or **`usb_storage need=usbcore`** when MSC leaf embed fails ksym (this lab class) |

### D7 MMIO handoff ([R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md))

| Phase | State |
|-------|--------|
| **0** bridge copy-only | **LIVE** — freestanding owns wire; soft L2 copies frames |
| **1** freestanding quiesce | **Stub** — gate0 → SKIP; gate1 → PREPARE |
| **2** hostish sole-owner | **Stub** — ready codes; no dual-drive; `g_fMmioHandoff` still 0 until phase3 try_open |
| **3** soft-open | **Stub Option A** — soft-open only when gate1 + sole-owner; **no** `.ko` ndo_open (Option B separate) |
| **4** soft wire TX/RX | **OPEN** — or hybrid freestanding wire + soft control |

Greppable (gate 0): `… handoff SKIP (gate off)`. Soft safety: `soft open skip .ko ops` · `soft napi poll skip .ko` · `soft softirq kick` · `soft l2 bridge first tx` · `soft dev_queue_xmit`.

### Gate0 soft load health (zero-touch freestanding BAR)

| Gate / symbol | Default | Effect |
|--------------|---------|--------|
| **`GJ_SOFT_R8169_MMIO_HANDOFF`** | **0** | gate0 hybrid: **no REAL** `.ko` probe for `10ec:8168` → EMU bind only |
| **`GJ_SOFT_R8169_LOAD`** | **1** | boot load/init embed/media `r8169.ko`; set **0** = freestanding-only net isolation |
| **`GJ_SOFT_R8169_KO_NDO_OPEN`** | **0** | Option B `.ko` ndo_open (needs handoff=1) |
| **`linux_pci_soft_hw_touch_ok`** | — | gate0: **always 0** for `10ec:8168` (CF8 write + `pci_iomap` NOOP); gate1: NOOP while `rtl8168_ready()` |
| **rtnl / eth_*** | soft bodies | nesting lock; eth_platform **-EOPNOTSUPP**; mac copy/validate; eth_type_trans |
| **ethtool_op_get_link** | soft body | carrier or freestanding ready (empty stub was always-down) |
| **ethtool_op_get_ts_info** | soft body | zero info head (no PHC) |
| **net_ratelimit** | soft body | allow (1); empty stub suppressed |
| **synchronize_net** | soft no-op | post-open safe |

**Freestanding-only isolation:** rebuild with `-DGJ_SOFT_R8169_LOAD=0`. Boot skips soft r8169 load/init (hold8 `mod r8169 SKIP load=0`). Soft ksyms remain registered; no EMU netdev. Grep: `main: soft linux_module path SKIP name=r8169 reason=GJ_SOFT_R8169_LOAD=0`.

**Zero-touch serial (once):** `linux_pci_soft: soft hybrid zero-touch PASS gate0 REAL=skip cf8/iomap=NOOP_8168 always` · `soft hostish probe SKIP hybrid` · `soft cf8 write NOOP hybrid` · `soft pci_iomap NOOP hybrid`. Soft ≠ product.

**Prior lab milestones (same day):** first REAL probe; EMU bind; first SOFT 1; force-EMU netdev-only; id_table 40 B stride.

### Real probe outcomes (hostish `pci_dev` 0xb40)

| Serial / STATUS outcome | Meaning | Laptop evidence |
|-------------------------|---------|-----------------|
| **PASS** | `probe()` returned **0** | stable: `PROBE … REAL` `ST=0` `NETDEV SOFT 1`; prior first REAL same lamps |
| **FAIL** | `probe()` returned **nonzero** | `… FAIL st=N (fallback EMU)`; may still show **NETDEV SOFT 1** |
| **FAULT** | Ring-0 **#PF** mid-probe | `… FAULT` then **kernel halt** — no EMU fallback |

Flag: `g_u32SoftHostishProbeInflight` around `.ko` probe; grep `soft hostish probe`. Soft≠product.

### Force-EMU netdev-only (lab fix)

| Symptom | `REG=1 MATCH=1` but `NETDEV SOFT 0` / `PROBE MISS` |
|---------|------------------------------------------------------|
| **Cause** | Slot already soft-bound without `register_netdev`; force path skipped as “already bound” |
| **Fix** | EMU bind fails unless netdev registers; force-EMU can mint **netdev-only** when bound but count is 0 |

### Prior wave (fixed) — id_table stride

Lab evidence: soft `pci_device_id` was **32 B**; RHEL **9.8** rows are **40 B** → walk false-ended before `10ec:8168`. Fix: 40-byte stride. **Superseded** by SOFT 1 evidence above.

---

## Honesty first

| Rule | Meaning |
|------|---------|
| **No GPL source in tree** | Never paste Linux `r8169.c` / `xhci-*.c` / kernel trees into knano. |
| **Staged `.ko` are host binaries** | Often GPL-bound; operator/dev media for **ABI + module-path eng**. **Not** dual-license product source. |
| **G-AC-1 (product AC)** | **No Linux `.ko` runs in the kernel as product.** Steam/bar3 / shipping acceptance is **not** “we ship or exec GPL `.ko` in-kernel.” Product class-driver story = **hot+cold Linux ABI** + **Linux-SHAPED UDX/DDI userspace** (+ dual-license out-of-tree). |
| **Product NIC** | **Not** in-kernel `r8169.ko` owning the wire. Dual DoD **B** is `rtl8168_udx` until host **interactive SSH login**. |
| **`RUN_INIT=0` / `freestanding_no_exec`** | Soft SKIP **exec** of `.ko` init **in kernel** — eng residual aligned with G-AC-1. **Not** “never use Linux drivers”; product still hosts Linux-shaped drivers in **userspace**. |
| **Module path = engineering track** | Collect → stage → soft loader/ksym → soft load/probe → iterative ksym → eng datapath experiments. Most of this is still **OPEN**. Soft ≠ product. |
| **Not freestanding thrash** | In-kernel `rtl8168` / `xhci_msc` stage numbers are **lab honesty only**, not the primary laptop **product** driver strategy. |

```text
  ALLOWED                                      FORBIDDEN
  ─────────────────────────────────────────    ──────────────────────────────────
  Collect host .ko + firmware for media        Import GPL driver *source* into knano
  Stage under GJ-PERSIST/linux-drivers/        Claim freestanding stage=15 = T1 product
  Soft module loader + ksym (dev/eng)          Claim bar3 closed because .ko staged
  Soft load/probe smokes (eng residual)        Product AC = “.ko runs in kernel”
  RUN_INIT=0 / freestanding_no_exec residual   Reading residual as “never Linux drivers”
  Iterative unresolved-symbol resolve          "Just insmod r8169 and we're done"
  Parallel: clean-room UDX hosts (wave D)      Claiming soft probe PASS = TX/RX / BOT
  Product: userspace UDX/DDI + hot/cold ABI    Product NIC = in-kernel r8169.ko
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

**xHCI / USB presence honesty (host lab reality):** many hosts (all RHEL9/el9 kvers on this lab) build `xhci_pci` / `xhci_hcd` / `usbcore` / `usb_common` as **builtin** (no `.ko`). **`usb-storage` / `uas` are often modular** and collect stages plain `.ko`. Collect writes:

| Meta | Greppable |
|------|-----------|
| `meta/XHCI-STATUS.txt` | `collect-linux-drivers: xhci 8086:a12f xhci_pci=BUILTIN\|PRESENT\|MISSING` · `usb_storage=…` |
| `meta/USB-STATUS.txt` | `collect-linux-drivers: usb path OPEN builtin hc=… msc=…` |

Soft boot: `main: soft linux_module xhci path SKIP builtin` + `main: soft usb multi-mod order …` until an HC `.ko` is staged. Optional MSC leaf: `./scripts/embed-linux-mod.sh usb-storage` → `usb_storage_mod_blob.S` (auto from collect when PRESENT; Makefile weak link). Optional HC: `./scripts/embed-linux-mod.sh xhci_pci` only if a modular kver/distro supplies the file.

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
| **ESP** | **`EFI/GREENJADE/NEEDED-DRIVERS.txt`** + **`/linux-drivers/modules/r8169.ko`** + **`/linux-drivers/firmware/rtl_nic/rtl8168*.fw`** (UEFI soft media; D4) |
| **Flash** | `sudo make install-hwtest-usb DEV=/dev/sdX` |
| **Status** | **Live** (staging ≠ load) |

On-media layout after a good pack:

```text
GJ-PERSIST/
  linux-drivers/
    modules/          — .ko / .ko.xz / .ko.gz / .ko.zst
    firmware/         — host blobs as collected (often rtl_nic/*.fw.xz)
    firmware_plain/   — decompressed rtl8168*.fw for soft embed / ESP
    meta/             — G752VT-MAP, COPIED, MISSING, BUILTIN, HOST-LSPCI-K
    NEEDED-DRIVERS.txt
ESP (FAT):
  /linux-drivers/modules/r8169.ko
  /linux-drivers/firmware/rtl_nic/rtl8168*.fw   — media honesty (~33 KiB)
EFI/GREENJADE/
  NEEDED-DRIVERS.txt  — ESP mirror of checklist (when staged)
```

Verify on lab host after write:

```sh
sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/linux-drivers/modules
cat /mnt/gj-persist/linux-drivers/NEEDED-DRIVERS.txt
sudo umount /mnt/gj-persist
sudo mount -L GREENJADE /mnt/gj-esp   # or GJ-LNX-ESP label per stick
ls /mnt/gj-esp/linux-drivers/firmware/rtl_nic/
sudo umount /mnt/gj-esp
```

**Honesty:** D4 **PARTIAL** — ESP stages `r8169.ko` + plain `rtl8168*.fw`; UEFI soft media handoff is **single-blob** for the `.ko` (`source=media`). Multi-blob firmware LoadFile is **not** wired — runtime `request_firmware` uses the **in-kernel embed table** (see soft firmware below). **GJ-PERSIST** ext4 still unread at freestanding boot. Soft ≠ product. Await lab panel/serial for `source=media` + fw HIT.

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

1. **Not freestanding thrash** — do not grow in-kernel `rtl8168.c` / `xhci_msc.c` as the **product** NIC/USB path. Freestanding rtl **SKIP**; Dual DoD **B** is UDX + **interactive SSH login**; product NIC ≠ in-kernel `r8169.ko`.  
2. **Not GPL source in tree** — implement clean-room loader/ksym; modules remain **operator-collected binaries**.  
3. **Fail closed** — unresolved symbol → refuse load with greppable reason; no silent half-init.  
4. **G-AC-1** — soft path may **SKIP exec** of `.ko` init in kernel (`freestanding_no_exec` / `RUN_INIT=0`). That residual is **eng**, not a ban on Linux-shaped **userspace** drivers.  
5. **Parallel product track** — wave D **UDX/DDI** hosts (`rtl8168_udx` / `xhci_udx`) + hot/cold **Linux ABI** are the dual-license **product** class story; this module path is the **ABI/host eng experiment** only — “can host Linux-shaped surfaces,” not “product = in-kernel `.ko`.”

---

## Step 4 — `finit_module` / media boot path (**D4 PARTIAL**)

| Surface | Where | Status |
|---------|-------|--------|
| Linux NR defs | `linux_abi.h` — init/finit/delete_module | **Present** |
| Kernel soft load (memory) | `linux_module_load_mem` / `_src` | **SOFT DONE** (lab) |
| Boot **media** (ESP/UEFI) | SimpleFS → `boot_info` soft media → `source=media` preferred | **SOFT PATH LIVE** (code); **await lab panel/serial** |
| Boot **embed** fallback | `gj_r8169_ko_blob` → `source=embed` | **SOFT DONE** (G752 proven path) |
| `finit_module` | vfs_ram fd only (`source=finit`) | **SOFT**; not GJ-PERSIST |
| **GJ-PERSIST ext4** | staged tree on USB | **Unread** at freestanding boot |

| Source tag | Meaning |
|------------|---------|
| **`media`** | UEFI soft media handoff blob (ESP `r8169.ko`) |
| **`embed`** | Linked blob fallback |
| **`finit`** | Cold finit from vfs_ram fd |

**D4 CLOSE remaining:** GJ-PERSIST ext4 reader and/or multi-module deps; Soft≠product.

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

### D9 — ksym post-probe gaps (r8169 REAL ST=0)

After load-time resolve (r8169 **180** strong und names all hit ksym; G752 lab evidence **INIT=0** / **REAL probe ST=0** / ksym **N≈289**), the remaining debt is **post-probe datapath quality**: empty return-0 ksym stubs vs soft bodies that fail closed / bookkeep / copy.

**Unresolved logging (fail-closed):**

| Path | Lamp / API |
|------|------------|
| Reloc strong `SHN_UNDEF` miss | `lmod_set_unres` → `g_szLastUnres` |
| Load fail | `linux_module: soft load FAIL name=… missing=SYM` |
| Reader | `linux_module_last_unresolved()` (boot panel uses this) |
| Reloc range soft-zero | `linux_module: soft reloc … range soft-zero` / `soft reloc skip` |
| Inventory | `linux_ksym: soft init PASS n=` / `soft inventory n=` |

**ksym categories (register sites):**

| TU | Role |
|----|------|
| `abandoned/kernel/mm/linux_ksym.c` | Leftover empty stubs (~272 names); **not linked** |
| `linux_pci_soft` | PCI register/config/irq/resource soft bodies |
| `linux_dma_soft` | DMA / ioremap / pcim / kmalloc / firmware HIT\|MISS |
| `linux_netdev_soft` | netdev / NAPI / skb / (post-probe) rtnl + eth_* |
| `linux_phy_soft` | phy_* / mdiobus_* soft-complete |
| `linux_time_soft` | jiffies / delay / IRQ request / printk |

**Added this pass (soft bodies re-register over empty stubs; Soft≠product):**

| Symbol | Soft behavior | File |
|--------|---------------|------|
| `rtnl_lock` / `rtnl_unlock` | nesting counter + once log | `linux_netdev_soft` |
| `eth_platform_get_mac_address` | **-EOPNOTSUPP** + zero addr (fail closed; driver EEPROM fallback) | `linux_netdev_soft` |
| `eth_mac_addr` | copy 6 B from sockaddr into soft `dev_addr` | `linux_netdev_soft` |
| `eth_validate_addr` | soft is_valid_ether_addr on soft netdev | `linux_netdev_soft` |
| `eth_type_trans` | ethertype from soft skb offset 12, else 0 | `linux_netdev_soft` |
| `synchronize_net` | no-op + once log | `linux_netdev_soft` |
| `ethtool_op_get_link` | soft carrier or freestanding ready (1=up) | `linux_netdev_soft` |
| `ethtool_op_get_ts_info` | soft zero info head (no PHC) | `linux_netdev_soft` |
| `net_ratelimit` | allow (1); empty stub suppressed | `linux_netdev_soft` |
| `usleep_range_state` | alias `usleep_range` (state ignored) | `linux_time_soft` |
| `memcpy_fromio` | real volatile byte-copy (empty stub wrote nothing) | `linux_dma_soft` |
| `skb_copy_bits` | copy soft `abData` → buffer; **-EFAULT** if not soft pool skb | `linux_netdev_soft` |
| `__skb_pad` | zero pad after soft data if room; free on fail if flagged; **-ENOMEM** else | `linux_netdev_soft` |
| `skb_put` | grow soft `uLen`; return tail ptr; NULL fail closed | `linux_netdev_soft` |
| `pskb_may_pull` | soft linear: 1 if `uLen` ≥ pull; 0 fail closed | `linux_netdev_soft` |
| `__pskb_pull_tail` | soft no frags; return `abData` or NULL | `linux_netdev_soft` |
| `pskb_expand_head` | soft fixed slab; 0 if fits (`nhead==0`); **-ENOMEM** else | `linux_netdev_soft` |
| `__netdev_alloc_skb` / `dev_alloc_skb` | soft pool; logical len starts 0 (TX put path) | `linux_netdev_soft` |
| `__napi_alloc_skb` | soft pool RX-shaped (`uLen` preset); alias of soft pool | `linux_netdev_soft` |

Soft skb helpers: greppable `linux_netdev_soft: soft skb_copy_bits|__skb_pad|skb_put|pskb_may_pull|__netdev_alloc_skb`. Reverse TX: `dev_queue_xmit` → freestanding L2 when freestanding owns wire (`soft dev_queue_xmit`). Soft≠product; **no** `.ko` `ndo_start_xmit` product path.

### Soft firmware (`request_firmware` embed HIT)

| Piece | Detail |
|-------|--------|
| **Collect** | `make collect-linux-drivers` → `build/linux-drivers/firmware/rtl_nic/*.fw.xz` (+ full rtl_nic set) |
| **Embed** | `scripts/embed-linux-fw.sh` (also from collect) decompresses **`rtl8168*.fw`** (~33 KiB total) → `firmware_plain/` + `kernel/proc/rtl_nic_fw_blob.S` + `kernel/include/gj/linux_fw_soft_tab.inc` |
| **Lookup** | `linux_dma_soft` `request_firmware` / `firmware_request_nowarn` / `request_firmware_direct` match exact name (`rtl_nic/rtl8168h-2.fw`, …) |
| **HIT** | Soft Linux-shaped `{ size, data, priv=NULL }` over `.rodata` `.incbin`; return **0** |
| **MISS** | `*fw=NULL`, **-ENOENT** (non-rtl8168 names, empty table, slot full) |
| **ESP** | Plain `rtl8168*.fw` staged under `/linux-drivers/firmware/rtl_nic/` for operator honesty; **not** loaded by UEFI multi-blob handoff |
| **Cap** | Soft embed gate **2 MiB** total (`GJ_LINUX_FW_MAX`) |

```text
# greppable
linux_dma_soft: soft firmware HIT name=rtl_nic/rtl8168h-2.fw size=… api=…
linux_dma_soft: soft firmware MISS name=… api=…
embed-linux-fw: PASS
make-hwtest-img: esp stage PASS rtl_nic fw n=…
```

**Expected MISS:** any name outside the embed table (e.g. `rtl_nic/rtl8125*.fw`, `i915/*`, phy-only names not embedded). r8169 on **10ec:8168** typically requests one of the embedded `rtl8168*.fw` variants — that path should **HIT** after collect+rebuild. Soft ≠ product.

**Still open (high priority):**

| Gap | Why |
|-----|-----|
| Soft firmware beyond `rtl8168*` | Only 8168 family embedded (~33 KiB); other rtl_nic / i915 remain MISS |
| Option B `.ko` `ndo_open` | After gate1 sole-owner + hostish layout confidence |
| Phase 4 soft wire TX/RX | Or hybrid freestanding wire + soft control |
| Gate1 sole-owner lab | Prove no dual-drive on G752 |
| xhci_hcd full surface | **SKIP builtin** on lab host; no local modular HC `.ko` |
| usb_storage soft leaf | Embed path live when staged; soft **usb+scsi seed** (`linux_usb_soft`); remaining FAIL on non-USB generics until deepened; INIT=0 ≠ stick |

Soft ≠ product. Do not claim bar3 / product AC from soft ksym deepen.

---

## Relationship to UDX soft host (wave D)

Two tracks — do not collapse claims:

| Track | What | Product AC? |
|-------|------|-------------|
| **A — UDX + Linux ABI (product)** | Hot+cold ABI; `GJ_SYS_DDI=103`; `rtl8168_udx` / `xhci_udx` clean-room / dual-license **userspace** hosts | **Yes, product path** under **G-AC-1**; TX/RX / BOT still **OPEN** until evidence |
| **B — Linux module path** (this doc) | Collect / stage / soft loader / ksym / soft load-probe eng | **Engineering / ABI** only; GPL binaries on media ≠ tree source; **no** `.ko` **in-kernel** product AC (**G-AC-1**) |
| **Lab freestanding rtl** | In-kernel `rtl8168.c` wire / hybrid SOFT gate0 | **SKIP** — **not** Dual DoD B; product NIC = `rtl8168_udx` |
| **Soft residual** | `RUN_INIT=0` / `freestanding_no_exec` | Eng: SKIP `.ko` init **exec in kernel** — **not** “never use Linux drivers” |

Operators may run **both** eng tracks: wave D lamps prove DDI/UDX wiring; module path proves hostability surfaces. Product ship story stays **userspace UDX/DDI + ABI**. Neither module soft lamps nor freestanding stages close bar3.

Details: [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [DDI_SOFT.md](DDI_SOFT.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md).

---

## Definition of done — “can run with Linux drivers”

Use this checklist for the **module path**. Mark each row honestly.

### DoD checklist

| # | Criterion | Meaning of DONE | Status |
|---|-----------|-----------------|--------|
| **D1** | **Collect** | `make collect-linux-drivers` produces modules + `NEEDED-DRIVERS.txt`; greppable `collect-linux-drivers: PASS` | **DONE** |
| **D2** | **Stage** | `make hwtest-img` packs tree to **`GJ-PERSIST/linux-drivers/`** (+ ESP checklist) | **DONE** |
| **D3** | **Soft loader + ksym** | In-tree soft loader can parse/relocate a `.ko` against a documented ksym table; fail-closed on missing symbols | **OPEN** (in progress) |
| **D4** | **`finit_module` / media boot path** | Staged media load or finit; greppable PASS/FAIL | **PARTIAL** — ESP `r8169.ko` + UEFI → `source=media`; embed fallback; GJ-PERSIST ext4 unread; finit vfs_ram-only. Soft≠product. §D4 media honesty |
| **D5** | **Module loads** | Staged module `init` return 0 | **SOFT DONE** — r8169 INIT=0 on G752 (embed proven; media code path live, await lab panel/serial) |
| **D6** | **Probe binds PCI** | Real `.ko` probe hostish `pci_dev` | **SOFT DONE** (REAL+SOFT1 stable; soft ≠ product) |
| **D7** | **Net datapath** | Link + TX/RX; freestanding ↔ soft handoff | **OPEN** — freestanding **ICMP proven earlier**; **recent boots R0/R1** regression under hybrid **SOFT gate0**; handoff gate1 · Option B · phase4 still OPEN ([R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md)). Dual DoD **B** (interactive SSH login) still OPEN. |
| **D8** | **USB datapath** | Bound xHCI: host controller runs; device path (e.g. MSC/HID) functional beyond soft lamp | **OPEN** — xHCI **SKIP BUILTIN**; soft **usbcore+scsi seed** (+33 stubs) advances MSC reloc past `usb_*`/`scsi_*`; first remaining miss **`sg_nents`** (or next non-USB generic). Soft load INIT=0 still ≠ stick. Freestanding **GET_CONFIG FAIL TO p21/s4**. Soft ≠ product. §D8 |
| **D9** | **ksym honesty** | Docs/logs list remaining unresolved surface for full `r8169` / `xhci_hcd`; iterative resolve, no freestanding thrash substitute | **OPEN** (process) |
| **D10** | **License honesty** | No GPL **source** in knano; staged `.ko` not claimed as dual-license product AC / bar3 close | **DONE** (policy) |

### One-glance

```text
DONE / SOFT-DONE             OPEN / PARTIAL
─────────────────────────    ────────────────────────────────────────
D1 Collect .ko               D3 Soft loader + ksym (broaden)
D2 Stage on media            D4 PARTIAL: source=media code; GJ-PERSIST ext4 OPEN
D5 r8169 init=0 (G752)       D7 Net OPEN — ICMP earlier; recent R0/R1; gate0 hybrid
D6 PROBE REAL+SOFT1 stable   D8 USB OPEN — soft usb seed; miss=sg_nents; ≠ stick (DoD A)
D10 No GPL / no bar3.ko      D9 ksym post-probe (firmware HIT rtl8168* …)
```

### D8 — USB linux path honesty (2026-08-04 lab host; soft usbcore seed)

| Item | Fact |
|------|------|
| **PCI** | G752VT-class xHCI **`8086:a12f`** |
| **Host kver sample** | RHEL **9.8** `5.14.0-687.15.1.el9_8` (all installed el9 kvers same shape) |
| **HC stack** | `xhci_pci` · `xhci_hcd` · `usbcore` · `usb_common` → **BUILTIN** (no `.ko` on any local kver) |
| **MSC leaf** | `usb-storage.ko` · `uas.ko` → **MODULAR PRESENT** (~222 KiB / ~75 KiB plain) |
| **Collect** | `meta/XHCI-STATUS.txt` + `meta/USB-STATUS.txt`; greps `xhci_pci=BUILTIN` · `usb_storage=PRESENT` |
| **Soft multi-mod order** | `usb_common → usbcore → xhci_hcd → xhci_pci → usb_storage` (serial stub always) |
| **Soft xHCI** | `main: soft linux_module xhci path SKIP builtin` · hold12 `mod xhci_pci SKIP builtin` |
| **Soft MSC leaf** | Optional embed `usb_storage_mod_blob.S` (collect auto when PRESENT) → `main: soft linux_module usb_storage path PRESENT\|PASS\|FAIL` |
| **Soft usbcore seed** | `kernel/mm/linux_usb_soft.c` — fail-closed `usb_*` + `scsi_*` ksym bodies (cap 20–40). Grep: `linux_usb_soft: soft init PASS n=` |
| **MSC fail lamp** | hold13 **`usb_storage need=usbcore`** + serial `need=usbcore OPEN unresolved=…` while non-USB generics remain (e.g. `sg_nents`) |
| **STATUS hold13** | No MSC embed: `USB linux path OPEN builtin`. MSC FAIL KSYM: **`usb_storage need=usbcore`**. Load ok: `mod usb_storage LOAD ok`. |
| **What D8 is not** | Soft leaf load / INIT=0 ≠ stick write; freestanding stage 15 ≠ Linux module path; Soft≠product; **G-AC-1** |
| **Lab block (parallel)** | Freestanding **stage 15** stick (GET_CONFIG) · module path: remaining **generic** ksym (`sg_miter_*`, `kthread_*`, …) then real HC |
| **Next for stick via Linux modules** | (1) Clear remaining ~34 non-USB/SCSI UND (or accept FAIL on first generic); (2) modular HC stack `.ko` from a distro/kver that builds `xhci_pci`/`xhci_hcd`/`usbcore` as modules (none of the local el9 trees do), or deepen soft HC; (3) multi-mod load; (4) soft probe `8086:a12f`; (5) bind MSC + BOT/UAS; (6) store door. Until then freestanding `xhci_msc` remains the only lab stick path. |

#### D8 MSC leaf — soft ksym surface honesty (lab class)

Host `usb-storage.ko` (el9 plain ~222 KiB) has **108** `SHN_UNDEF`. Prior soft ksym resolved **~41** (generics + prior surface); **~67** remained (of which **20** `usb_*` + **13** `scsi_*` + **~34** other).

| Class | Examples | Soft stance |
|-------|----------|-------------|
| **Generic (safe empty)** | `dma_max_mapping_size`, `sprintf`, `seq_printf`/`putc`, `complete` / `wait_for_completion*`, `_raw_spin_lock_irq` / `_unlock_irq`, `schedule` / `schedule_timeout*`, `queue_limits_commit_update_frozen`, `finish_wait` | **+15 empty stubs** in `linux_ksym.c`. Grep: `linux_ksym: soft usb_storage leaf stubs n=15` |
| **usbcore seed** | `usb_register_driver`, `usb_alloc_urb`, `usb_submit_urb`, `usb_control_msg`, `usb_sg_*`, `usb_autopm_*`, `usb_find_common_endpoints`, `usb_reset_*`, … (**20**) | **`linux_usb_soft.c`** fail-closed / no-op (register soft-success 0; submit/control → `-ENODEV`) |
| **scsi mid seed** | `scsi_host_alloc`, `scsi_add_host_with_dma`, `scsi_scan_host`, `scsi_is_host_device`, `scsi_report_*`, `scsi_eh_*`, … (**13**) | **Same TU** — load-only soft host blob; `scsi_add_host_with_dma` → `-ENODEV` |
| **Other (still OPEN)** | `sg_nents`, `sg_miter_*`, `kthread_*`, `param_ops_*`, `pcpu_hot`, wait/workqueue helpers, … (**~34**) | First remaining reloc miss expected **`sg_nents`** |

**Reloc miss timeline (`.rela.text` order, el9 ko):**  
1. Pre leaf generics: **`dma_max_mapping_size`**  
2. Post +15 generics: **`scsi_is_host_device`**  
3. Post soft usb+scsi seed (**+33**): expected **`sg_nents`**

**usb_* UND (all seeded):**  
`usb_alloc_coherent` `usb_alloc_urb` `usb_autopm_get_interface_no_resume` `usb_autopm_put_interface` `usb_autopm_put_interface_no_suspend` `usb_control_msg` `usb_deregister` `usb_find_common_endpoints` `usb_free_coherent` `usb_free_urb` `usb_kill_urb` `usb_lock_device_for_reset` `usb_register_driver` `usb_reset_device` `usb_reset_endpoint` `usb_sg_cancel` `usb_sg_init` `usb_sg_wait` `usb_submit_urb` `usb_unlink_urb`

**scsi_* UND (all seeded):**  
`scsi_add_host_with_dma` `scsi_done_direct` `scsi_eh_prep_cmnd` `scsi_eh_restore_cmnd` `scsi_host_alloc` `scsi_host_put` `scsi_is_host_device` `scsi_normalize_sense` `scsi_remove_host` `scsi_report_bus_reset` `scsi_report_device_reset` `scsi_scan_host` `scsi_sense_desc_find`

**Verdict:** Soft **usbcore + scsi mid seed** closes the class gap that made MSC **MUST FAIL** on `usb_*`/`scsi_*` alone. Remaining FAIL (if any) is **generic** surface (`sg_*` / workqueue / kthread) — still honest **need=usbcore OPEN** lamp until load PASSes, and even **LOAD/INIT=0 without real HC ≠ stick datapath**. Soft≠product. Multi-mod path still documents order `usb_common → usbcore → xhci_hcd → xhci_pci → usb_storage`; host **BUILTIN** means soft seed substitutes for missing `.ko` exports only.

```text
# greppable (boot)
main: soft linux_module xhci path SKIP builtin
main: soft usb multi-mod order need=usb_common,usbcore,xhci_hcd,xhci_pci,usb_storage …
main: soft linux_module usb_storage path PRESENT|PASS|FAIL|SKIP …
main: soft usb_storage need=usbcore OPEN unresolved=…
linux_ksym: soft usb_storage leaf stubs n=15 …
linux_usb_soft: soft init PASS n=…

# greppable (collect)
collect-linux-drivers: xhci 8086:a12f xhci_pci=BUILTIN
collect-linux-drivers: usb_storage=PRESENT
collect-linux-drivers: usb path OPEN builtin hc=BUILTIN msc=PRESENT
```

**Waves 1–3 eng (not D7 close):** hold14 code · open/NAPI skip `.ko` · MMIO 0+1–3 stubs gate0 · skb + `dev_queue_xmit` reverse TX · D4 PARTIAL. Freestanding **owns MMIO** at gate0. Soft ≠ product.

### Next blockers — “run with Linux drivers”

1. Lab enable handoff **gate1** + prove **sole-owner** without dual-drive  
2. **Option B** real `.ko` `ndo_open` after hostish layout confidence  
3. **Phase 4** soft wire TX/RX (or hybrid freestanding wire + soft control)  
4. **Firmware** soft embed HIT for `rtl8168*` (MISS for other names)  

5. **Lab panel/serial** for hold14 live + `source=media`

### D4 media honesty (2026-08-04, ESP handoff landed)

| Item | Fact |
|------|------|
| **Stage (D2)** | **DONE** — `make collect-linux-drivers` + `make hwtest-img` → **`GJ-PERSIST/linux-drivers/`** + **ESP** `/linux-drivers/modules/r8169.ko` |
| **ESP stage lamp** | `make-hwtest-img: esp stage PASS r8169.ko=…B path=/linux-drivers/modules/r8169.ko` |
| **UEFI handoff** | Stub LoadFile `\linux-drivers\modules\r8169.ko` → `AllocatePages(LOADER_DATA)` → `gj_boot_info.u64SoftMediaPhys/Bytes` + `GJ_BOOT_F_SOFT_MEDIA` |
| **Boot load media** | **SOFT PATH LIVE** when handoff present: soft probe prefers `boot_info` blob → `linux_module_load_mem_src(…, "media")` before embed; PMM reserves soft media pages |
| **GJ-PERSIST ext4** | Still **unread** at freestanding boot (no ext4 reader) |
| **vfs_ram soft probe** | Still tries seed paths; host `.ko` does **not** fit (≤32 KiB cap) |
| **Lab fallback** | **Embed** — `gj_r8169_ko_blob` → `source=embed` if no handoff / media load fails |
| **`finit_module`** | Soft cold path can load from a **vfs_ram** fd (`source=finit`); still not GJ-PERSIST |
| **Greppable** | `make-hwtest-img: esp stage PASS` · `GJ-EFI: soft media PASS\|SKIP\|FAIL` · `boot: soft media PASS\|SKIP` · `pmm: soft media reserve PASS` · `linux_module: soft media path PRESENT … reason=esp_uefi` · `linux_module: soft load source=media\|embed name=` · `linux_module: soft media status D4` / `TODO D4` |
| **Do not claim** | Soft media load = product NIC (G-AC-1); stage on USB = bar3; Soft = product |

**Operator path (collect → stage ESP + persist → boot source=media):**

```sh
cd /home/jay/Documents/knano
make collect-linux-drivers          # build/linux-drivers/modules/r8169.ko …
make hwtest-img                     # packs ESP + GJ-PERSIST
# greppable: make-hwtest-img: esp stage PASS
sudo make install-hwtest-usb DEV=/dev/sdX

# Host-side verify ESP soft media:
sudo mkdir -p /mnt/gj-esp && sudo mount -L GREENJADE /mnt/gj-esp
ls -la /mnt/gj-esp/linux-drivers/modules/r8169.ko
cat /mnt/gj-esp/linux-drivers/ESP-STAGE.txt
sudo umount /mnt/gj-esp

# After GJ UEFI boot serial (media path):
#   GJ-EFI: soft media PASS name=r8169 phys=… bytes=…
#   boot: soft media PASS phys=… bytes=…
#   pmm: soft media reserve PASS …
#   linux_module: soft media path PRESENT name=r8169 path=boot_info reason=esp_uefi …
#   linux_module: soft load source=media name=r8169 …
#   linux_module: soft media status D4 name=r8169 status=MEDIA …
```

**Next for D4 CLOSE:** GJ-PERSIST ext4 reader and/or multi-module dependency load from media; keep Soft≠product honesty.

### What “can run with Linux drivers” does **not** mean yet

- Staged modules on USB ≠ loaded modules.  
- Soft UDX probe PASS ≠ Linux `.ko` probe.  
- Freestanding `xhci stage=15` / `net t/f/b/r` ≠ module-path product.  
- bar3 / Steam client ≠ module load.  
- **G-AC-1:** product acceptance is **not** “`.ko` runs in kernel.” Product = hot+cold ABI + **userspace** UDX/DDI.  
- Soft `RUN_INIT=0` / `freestanding_no_exec` ≠ “never use Linux drivers.”  
- Lab freestanding rtl / dual DoD B ≠ product NIC (product NIC ≠ in-kernel `r8169.ko`).

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

# After boot: media preferred, embed fallback (D4 PARTIAL)
# grep -E 'linux_module: soft (load source|media path|media status)|soft linux_module path' SERIAL
# expect: source=media … OR source=embed PASS; Soft≠product
```

---

## Related

| Doc | Role |
|-----|------|
| [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) | Strategy: ABI + host path |
| [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) | G752VT UDX host runbook |
| [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) | Soft inventory honesty |
| [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) | Hostish `pci_dev` **0xb40** |
| [R8169_MMIO_HANDOFF.md](R8169_MMIO_HANDOFF.md) | MMIO handoff phases + gate |
| [TODO.md](TODO.md) | Current track |
| [DDI_SOFT.md](DDI_SOFT.md) | Soft DDI |
| [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) | Inventory oracle |

---

*Dual MIT OR Apache-2.0. Soft ≠ product. **G-AC-1:** no Linux `.ko` **runs in kernel** as product. Bar3 only STEAM_BAR3_STATUS.*  
*Product = hot+cold Linux ABI + Linux-shaped drivers in **userspace** (UDX/DDI). Product NIC ≠ in-kernel `r8169.ko`.*  
*`RUN_INIT=0` / `freestanding_no_exec` = eng residual (no in-kernel `.ko` init exec), **not** “never use Linux drivers.”*  
*Lab freestanding rtl = SKIP (not Dual DoD B). Module path = eng only.*  
*Fly **v0.1.184**: packed, not host-probed. Dual DoD **A** USB OPEN (never `USBCMD.RS=1`) · **B** OPEN until host **interactive SSH login**. **0.2.0** reserved. Soft ≠ product. No test-panel photo IDs.*
