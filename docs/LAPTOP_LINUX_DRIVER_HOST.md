# GreenJade — Laptop Linux-shaped driver host (G752VT operator runbook)

| Field | Value |
|-------|--------|
| **Status** | **Wave D soft host path live** — `GJ_SYS_DDI=103`, real soft PCI scan, UDX SCAN/GET/OPEN/MAP_BAR bind; **not** product T1 / **not** bar3 |
| **DUT** | ASUS ROG **G752VT** (RH71-class) |
| **Law** | Dual **MIT OR Apache-2.0** only; **no GPL source in tree** |
| **Hard AC** | **G-AC-1** — **not** Linux `.ko`; **Linux-SHAPED UDX** userspace drivers only |
| **Strategy** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · inventory [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) |
| **Companions** | [HCL.md](HCL.md) · [DDI_SOFT.md](DDI_SOFT.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) · [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

## One-sentence goal

**Run GreenJade on the G752VT with a Linux-class driver story: ABI + DDI + host/module path so operators collect Linux modules for their install — not freestanding rewrites of every class driver.**

### Operator / Grok media (2026-08)

| Artifact | Role |
|----------|------|
| `make collect-linux-drivers` | Harvest host `.ko` + firmware + **NEEDED-DRIVERS.txt** → `build/linux-drivers/` |
| `make hwtest-img` | Packs those onto **GJ-PERSIST/linux-drivers/** + ESP **NEEDED-DRIVERS.txt** |
| `make linux-hwtest-img` | Linux inventory USB; **`70-needed-drivers.sh`** writes DUT **NEEDED-DRIVERS** for collect |
| Soft module loader + ksym | **In progress** — see [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) |
| Module **load** / soft bind (embed r8169) | **SOFT DONE** on G752 — `INIT=0`, `REG=1` `MATCH=1` `NETDEV SOFT 1`; force-EMU netdev-only fix when bound without netdev |
| Real `.ko` probe / hostish `pci_dev` **0xb40** | **REAL+SOFT1 stable** — lab: `PROBE … REAL` `ST=0` `NETDEV SOFT 1` (prior first REAL same lamps); soft ≠ product |
| **Next** | **D7** datapath / MMIO handoff — freestanding still owns MMIO; soft open/TX · hold14 refresh · soft NAPI (in progress) |
| xHCI module path | **SKIP builtin** — no `xhci_pci.ko` on many hosts → dual **DoD A OPEN** ([TODO.md](TODO.md)) |
| **Dual DoD (laptop)** | **A** Linux USB drivers path **OPEN** (stage-15 / builtin not enough). **B** freestanding **sshd TCP :22 OPEN** (ICMP ping ≠ :22). Soft ≠ product · **G-AC-1.** |
| Product TX/RX / BOT | **OPEN** — soft ≠ product; **G-AC-1** |
| Bar3 / Deck Top 50 | **OPEN** — [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) only (not boot spam) |

Honesty: staged `.ko` are **host Linux** binaries (often GPL). GreenJade **source tree** stays dual-license (**no GPL source**); product AC is not “bar3 by shipping GPL.” Soft netdev bind ≠ TX/RX. Full `r8169` / `xhci_hcd` need a **large ksym surface** — iterative resolve, not freestanding thrash.

### Module path at a glance (normative: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md))

| # | Step | Status |
|---|------|--------|
| 1 | Collect `.ko` (`collect-linux-drivers`) | **DONE** (live) |
| 2 | Stage on media (`GJ-PERSIST/linux-drivers/`) | **DONE** (live) |
| 3 | Soft module loader + ksym | **IN PROGRESS** (r8169 embed path soft-working; ksym **N=289**) |
| 4 | Soft load + id match + netdev | **SOFT DONE** — `NETDEV SOFT 1` (EMU + REAL) |
| 5 | Hostish real probe (`pci_dev` **0xb40**) | **SOFT DONE** — REAL+SOFT1 stable |
| 6 | **D7** datapath / MMIO handoff | **NEXT** — freestanding owns MMIO; soft open/TX · hold14 · soft NAPI |
| 7 | Iterative ksym / media `finit_module` | **OPEN** |
| DoD | Real probe → net/usb datapath | **OPEN** (D6 soft-done; D7–D8) |

---

## 1. Law first — **G-AC-1** (read before boot)

| Rule | Meaning for operators |
|------|------------------------|
| **G-AC-1** | **Not** Linux `.ko` as GreenJade **shipping product** acceptance (bar3/Steam). Product-shaped class hosts = **Linux-SHAPED UDX** (clean-room / dual-license out of tree). Inventory under a Linux stick is **oracle only**. |
| **License** | GreenJade tree + UDX + out-of-tree product drivers: **MIT OR Apache-2.0** (or BSD dual-license out of tree). **No** Linux driver **source** import. |
| **Two tracks** | **(A)** UDX soft host (this runbook, wave D). **(B)** Host-collected module path: collect → stage → soft loader/ksym → `finit_module` — [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md). Track B is ABI/engineering; does **not** rewrite G-AC-1 for bar3. |
| **Product path** | **ABI** (Linux-shaped syscall/personality) → **virtio T0** apps → **DDI** (`GJ_SYS_DDI` / door) → **UDX** clean-room / dual-license **userspace** driver hosts; module path deepens hostability of collected drivers. |
| **Not product** | In-kernel freestanding `rtl8168` / `xhci_msc` stage thrash; Linux inventory `RESULT: PASS`; Steam **bar3** (stays **OPEN**); product NIC **TX/RX**, xHCI **BOT/MSC**, **live IRQ**, REAL+SOFT1 / soft `NETDEV SOFT 1` ≠ product datapath. |
| **Operator choice (2026-08)** | **Option 1 — ABI-first:** freestanding panel `xhci stage=15 GET_CONFIG cc=0` and `net … t/f/b/r link=1` are **lab honesty only**. Product-shaped work = **DDI bind + UDX hosts**; module path = **collect/stage + loader/ksym**. Do **not** treat stage/net counters as T0/T1 close. |

```text
  ALLOWED (G-AC-1 / law)           FORBIDDEN
  ─────────────────────────────    ────────────────────────────────
  Clean-room UDX driver .c         GPL Linux driver *source* in tree
  Out-of-tree dual-license hosts   Claiming bar3 closed via staged .ko
  Cap-gated MMIO/IRQ/DMA (DDI)     Freestanding thrash as primary path
  Collect/stage host .ko (dev)     Claiming inventory PASS = T1 done
  Soft loader + ksym (in progress) Claiming soft UDX probe = TX/RX/BOT
  linux-hwtest inventory stick     Silent load with unresolved ksym
```

**Hard rule:** Linux-shaped **API names** (`probe` / `ioremap` / `dma_*`) in UDX are **clean-room shape**. Staged `.ko` are for **module-path development** — full `r8169`/`xhci_hcd` need iterative **ksym** resolve; they are **not** product NIC/USB stacks until DoD D5–D8 close.

---

## 2. What “Linux-shaped” means here (post wave D)

| Layer | Role | Wave D reality | Product? |
|-------|------|----------------|----------|
| **Linux ABI Option C** | Syscall/personality so Linux-shaped **apps** run | Primary path | Soft → product |
| **`GJ_SYS_DDI` (103)** | Native door: SCAN / GET / OPEN / MAP_BAR / CFG / DMA_NOTE | **Live in kernel** (`kernel/drv/ddi_door.c`) | Soft surface ≠ full CNode mint |
| **`devmgr_soft`** | Soft policy inventory; **real type-0 PCI CF8/CFC scan** | **Live walk** + G752 ID lamps | Soft table; no product match graph |
| **UDX** (`user/udx/`) | Userspace runtime: Linux-shaped `probe` / `remove` / `ioremap` / IRQ / `dma_*` | Bind via **SCAN→GET→OPEN→MAP_BAR** matching kernel ops | Soft host / inject on Linux host |
| **`vmm_map_user_device`** | UC MMIO map into process user AS | Used by MAP_BAR when current process exists | Soft map path; not full MMIO_FRAME cap mint story |
| **Driver `.c`** | Clean-room class hosts | `rtl8168_udx` + `xhci_udx` **skeletons** | Soft only — no TX/RX / BOT |
| **In-kernel freestanding** | Lab scaffolds | `rtl8168.c`, `xhci_msc.c` | **No** — lab only |

```text
┌─────────────────────────────────────────────────────────────┐
│  clean-room UDX driver .c  (rtl8168_udx / xhci_udx skeleton) │
├─────────────────────────────────────────────────────────────┤
│  UDX runtime — Linux-shaped API, caps hidden                │
│    udx_host_bind_by_id → SCAN → GET → OPEN → MAP_BAR        │
├─────────────────────────────────────────────────────────────┤
│  driver host process — user AS; MMIO via vmm_map_user_device │
├─────────────────────────────────────────────────────────────┤
│  GJ_SYS_DDI=103  (ddi_door.c) + devmgr_soft real PCI scan   │
│    soft open / map / cfg notes — not full product CNode mint │
└─────────────────────────────────────────────────────────────┘
```

Porter contract: [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md). Soft DDI surface: [DDI_SOFT.md](DDI_SOFT.md).

### 2.1 Wave D — what is live in tree

| Item | Path / NR | Greppable / note |
|------|-----------|------------------|
| **DDI syscall** | `GJ_SYS_DDI` **= 103** | `kernel/include/gj/syscall.h` · dispatched in `native.c` → `ddi_door_syscall` |
| **DDI door** | `kernel/drv/ddi_door.c` · `gj/ddi_door.h` | Ops: **SCAN / GET / OPEN / MAP_BAR / CFG_READ / DMA_NOTE / INVENTORY / CFG_WRITE** |
| **Real PCI scan** | `kernel/drv/devmgr_soft.c` | CF8/CFC walk; `devmgr: soft pci scan PASS n=N`; `devmgr: soft found 10ec:8168` / `8086:a12f` when present |
| **UDX bind** | `user/udx/src/host.c` | `udx_host_bind_by_id` / `bind_scan` → same SCAN/GET/OPEN/MAP_BAR opcodes as kernel |
| **User MMIO** | `vmm_map_user_device` in `kernel/mm/vmm.c` | MAP_BAR prefers process user-AS UC map; falls back to kernel UC for same-AS smoke |
| **rtl8168_udx** | `user/drivers/rtl8168_udx/` | Clean-room skeleton — soft probe / ISR / work; **no product TX/RX** |
| **xhci_udx** | `user/drivers/xhci_udx/` | Clean-room skeleton — soft cap/params/PORTSC + **soft BOT progress stub**; **no product BOT/MSC** |

### 2.2 Explicitly still **OPEN** (do not claim)

| Bar | Status |
|-----|--------|
| Product NIC **TX/RX** datapath / link / PHY on `10ec:8168` | **OPEN** |
| Product xHCI **BOT / MSC / HID** on `8086:a12f` | **OPEN** |
| **Live IRQ** path (kernel notify → userspace host ISR product) | **OPEN** (host-sim / soft fire only) |
| Live DDI **cap mint** (MMIO_FRAME / IRQ Notification / DMA window into host CNode) | **OPEN** |
| Product multi-server `devmgr` match → grant graph | **OPEN** |
| Steam **bar3** | **OPEN** |

---

## 3. G752VT hardware facts (operator)

| Field | Value |
|-------|--------|
| **Model / BIOS** | ASUS ROG **G752VT**, BIOS **G752VT.307** (typical) |
| **CPU / RAM / GPU** | i7-6700HQ · ~16 GiB · GTX 970M + HD 530 |
| **NIC (wired)** | Realtek **`10ec:8168`** (RTL8168/8111 family; often `03:00.0`) |
| **USB xHCI** | Intel **`8086:a12f`** — `0000:00:14.0` 100 Series / C230 USB 3.0 |
| **IOMMU** | **Translated** under Linux (VT-d) — product DMA must use **windows**, not assume identity forever |
| **USB ports** | USB **3.0 only** on this chassis — prefer SS stick path for MSC media |
| **Serial** | **No DB9** — use **panel** bars/text + remount stick logs |
| **Secure Boot** | **Off** for freestanding GreenJade USB |

Linux inventory ground truth (topology oracle only): [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

| Device | PCI ID | Soft UDX class (wave D) | In-kernel freestanding | Product T0 |
|--------|--------|-------------------------|------------------------|------------|
| Wired NIC | **`10ec:8168`** | `rtl8168_udx` clean-room skeleton | `rtl8168.c` lab | **virtio-net** until UDX NIC product TX/RX |
| USB HC | **`8086:a12f`** | `xhci_udx` clean-room skeleton | `xhci_msc.c` lab | T1 via DDI hosts later; **BOT OPEN** |

---

## 4. Operator checklist — flash USB and boot

Do **not** confuse with the **Linux inventory** stick (`linux-hwtest-img` / labels `GJ-LNX-*`). This section is **GreenJade** only.

### 4.1 Lab host — build and write USB

```sh
cd /home/jay/Documents/knano   # or your clone

# Optional: host UDX + clean-room class skeletons (lab Linux, not DUT)
make udx udx-example
make -C user/drivers/rtl8168_udx
make -C user/drivers/xhci_udx

# Build dual-partition hardware-test image (stages ESP + drivers when present)
make hwtest-img
ls -lh build/greenjade-hwtest.img

# Identify stick carefully (NOT internal RAID/root disks)
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL

# DESTROYS entire device DEV:
sudo make install-hwtest-usb DEV=/dev/sdX
```

| Partition | Label | Role |
|-----------|-------|------|
| ESP (FAT) | **`GREENJADE`** | UEFI `BOOTX64.EFI`, `EFI/GREENJADE/BOOT.LOG`, `KLOG.TXT`, optional `drivers/` |
| Data (ext4) | **`GJ-PERSIST`** | `logs/`, `steam/` media (media READY ≠ bar3) |

Staging helpers:

| Helper | Destination | Contents |
|--------|-------------|----------|
| `scripts/stage-udx-drivers.sh` | ESP `EFI/GREENJADE/drivers/*` + `DRIVERS.txt` / `LAPTOP.txt` | Clean-room UDX host binaries — **not** `.ko` |
| `scripts/collect-linux-drivers.sh` (via `hwtest-img`) | **`GJ-PERSIST/linux-drivers/`** | Host-collected `.ko` + firmware + meta — module path |

Module-path layout and DoD: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md).

### 4.2 DUT — UEFI boot

1. AC power preferred; insert stick in a **USB3** jack.  
2. Power on → spam **Esc** (or **F8**) → choose **UEFI: … USB**.  
3. Secure Boot **Disabled**; prefer pure UEFI (not CSM-only).  
4. Watch **panel** (no COM1 on this chassis): loader `GJ-EFI` / kernel progress / soft lamps.  
5. After run, power off and collect logs on the lab host (below).

Firmware tips: [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md).

### 4.3 Collect evidence

```sh
sudo umount /run/media/$USER/* 2>/dev/null; true
sudo mkdir -p /mnt/gj-esp /mnt/gj-persist

sudo mount -L GREENJADE /mnt/gj-esp
cat /mnt/gj-esp/EFI/GREENJADE/BOOT.LOG
cat /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT   # if MSC stick-log path wrote
# Prefer grepping panel-captured serial tees / KLOG when present:
grep -E 'devmgr:|ddi_door|main: soft ddi|udx |rtl8168_udx|xhci_udx|vmm: map_user' \
  /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT 2>/dev/null || true
ls /mnt/gj-esp/EFI/GREENJADE/drivers 2>/dev/null || true
sudo umount /mnt/gj-esp

sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/logs /mnt/gj-persist/steam 2>/dev/null || true
cat /mnt/gj-persist/steam/STATUS 2>/dev/null || true   # READY = media only
# Module path staging (collect ≠ load):
ls /mnt/gj-persist/linux-drivers/modules 2>/dev/null || true
cat /mnt/gj-persist/linux-drivers/NEEDED-DRIVERS.txt 2>/dev/null || true
sudo umount /mnt/gj-persist
```

Panel capture is valid evidence when serial is absent.

---

## 5. Expected panel / serial lines (wave D)

**Soft ≠ product.** These lamps prove the **Linux-shaped host path is wired and greppable**, not that T1 NIC/USB product, TX/RX, BOT, live IRQ, or bar3 is done.

### 5.1 Kernel / door (expect on freestanding laptop boot)

| Soft lamp (grep / panel) | Meaning | Closes product? |
|--------------------------|---------|-----------------|
| **`devmgr: soft init PASS`** | Soft DDI/devmgr ready | **No** |
| **`devmgr: soft pci scan PASS n=N`** | **Real** CF8/CFC soft inventory filled | **No** |
| **`devmgr: soft found 10ec:8168`** | G752 wired NIC seen in soft table | **No** — not TX/RX |
| **`devmgr: soft found 8086:a12f`** | G752 xHCI seen in soft table | **No** — not BOT |
| **`devmgr: soft dma window note`** | Soft window table; **no** VT-d mint | **No** |
| **`ddi_door: soft init PASS`** | Door tables up | **No** |
| **`ddi_door: soft product surface PASS`** | `GJ_SYS_DDI` inventory lamp (soft≠product) | **No** |
| **`ddi_door: soft scan count=N`** | Door SCAN via `devmgr_soft_pci_scan` | **No** |
| **`ddi_door: soft open handle=`** | Soft open (when host binds) | **No** — not CNode mint |
| **`ddi_door: soft map bar … user=1`** | MAP_BAR via **`vmm_map_user_device`** | **No** — not product cap story |
| **`main: soft freestanding lab only`** | Explicit: freestanding stage/net ≠ product | **No** (policy lamp) |
| **`main: soft ddi found 10ec:8168`** / **`8086:a12f`** | Boot smoke ID + BAR notes | **No** |
| **`main: soft ddi bind 10ec:8168 PASS`** | OPEN + MAP BAR0/2 + soft IRQ note | **No** — not TX/RX |
| **`main: soft ddi bind 8086:a12f PASS`** | OPEN + MAP BAR0 + soft CFG/IRQ note | **No** — not BOT / not stage15 |
| **`main: soft ddi laptop smoke PASS … abi_first=1`** | Boot DDI bind path finished | **No** |
| **`main: soft linux_module xhci path SKIP builtin`** | No `xhci_pci` embed (host often builtin) — **expected** on G752 class | **No** — not BOT; collect plain `.ko` later |
| **`main: soft linux_module xhci path PASS\|FAIL`** | Optional weak embed load of `xhci_pci.ko` | **No** — load ≠ probe ≠ BOT |
| **STATUS `netdev soft 1`** / **`pci reg=1 match=1`** | Soft r8169 register + id match | **No** — soft bind ≠ TX/RX |
| **STATUS `probe 10ec:8168 real`** | Hostish real `.ko` probe (lab REAL+SOFT1) | **No** — REAL+SOFT1 ≠ product datapath |
| **STATUS `probe 10ec:8168 soft`** | Soft EMU probe lamp (prior path) | **No** |
| **`xhci: … stage=15`** / **`NET … t/f/b/r`** | Freestanding lab panel (ignore for product) | **No** |

### 5.2 UDX / class hosts (when host process runs bind)

| Soft lamp | Meaning | Closes product? |
|-----------|---------|-----------------|
| **`udx: soft ddi bind PASS`** | SCAN/GET/OPEN/MAP_BAR path succeeded | **No** |
| **`udx: soft ddi bind SKIP`** | No GJ_SYS_DDI / no match (host-linux inject fallback) | **No** |
| **`udx: soft ddi host note PASS`** / **`udx: soft ddi-ready`** | Host soft surface lamps | **No** |
| **`rtl8168_udx: soft probe PASS`** | Clean-room NIC skeleton probe+ISR soft path | **No** — **TX/RX OPEN** |
| **`xhci_udx: soft probe PASS`** / **`soft cap ver=`** | Clean-room xHCI skeleton cap read | **No** — **BOT OPEN** |
| **`xhci_udx: soft bot stage=1..4`** / **`soft bot stub PASS`** | Soft BOT **progress** (cap→params→ports→stub) | **No** — catalog only; **no** CBW/MSC |
| **`xhci_udx: soft params … maxports=`** / **`soft ports … ccs=`** | HCSPARAMS1 + soft PORTSC walk | **No** — not live enum / reset |

In-tree greppable anchors (wave D — prefer these over old “scan deferred” only):

```text
devmgr: soft init PASS
devmgr: soft pci scan PASS n=
devmgr: soft found 10ec:8168
devmgr: soft found 8086:a12f
devmgr: soft dma window note
ddi_door: soft init PASS
ddi_door: soft product surface PASS
ddi_door: soft scan count=
main: soft freestanding lab only
main: soft ddi bind 10ec:8168 PASS
main: soft ddi bind 8086:a12f PASS
main: soft ddi laptop smoke PASS
main: soft linux_module path PASS|FAIL|SKIP
main: soft linux_module xhci path PASS|FAIL|SKIP
udx: soft ddi bind PASS
udx: soft ddi bind SKIP
rtl8168_udx: soft probe PASS
xhci_udx: soft probe PASS
xhci_udx: soft bot stage=
xhci_udx: soft bot stub PASS
xhci_udx: soft ports
xhci_udx: soft params
```

Host UDX demo (lab Linux, not laptop DUT — inject path; DDI bind expected **SKIP** without GreenJade kernel):

```sh
make udx-example
./build/udx_skeleton | tee /tmp/udx.log
grep -E 'GREENJADE_UDX|skeleton PASS|soft ddi' /tmp/udx.log

make -C user/drivers/rtl8168_udx && ./user/drivers/rtl8168_udx/build/rtl8168_udx
make -C user/drivers/xhci_udx && ./user/drivers/xhci_udx/build/xhci_udx
```

**Operator reading of a good ABI-first laptop boot:**  
1. Ignore freestanding **`stage=15`** / **`t/f/b/r`** for product judgment.  
2. Prefer **`main: soft freestanding lab only`** + **`main: soft ddi bind … PASS`** (when silicon present) + **`main: soft ddi laptop smoke PASS … abi_first=1`**.  
3. Class host soft notes appear when UDX drivers are launched later — still **not** product TX/RX, BOT, live IRQ, or a license to load GPL `.ko`.

---

## 6. Net and USB honesty (do not over-claim)

| Claim | Status |
|-------|--------|
| **T0 product net** | **virtio-net** (QEMU / CI). Remains virtio until a **UDX NIC product** path with real DDI grants **and** TX/RX. |
| **G752 wired NIC** | Hardware **`10ec:8168`**. Soft `rtl8168_udx` probe / lab in-kernel `rtl8168` ≠ product LAN. **Product TX/RX OPEN.** |
| **Product sshd on laptop NIC** | **Not** product AC (**G-AC-1** / Soft ≠ product). Lab dual **DoD B:** freestanding **sshd TCP :22** from host — **OPEN** until proven ([TODO.md](TODO.md)); ICMP ≠ :22. |
| **G752 xHCI** | Hardware **`8086:a12f`**. Soft `xhci_udx` cap read / lab `xhci_msc` ≠ product USB. **Product BOT/MSC OPEN.** |
| **Live IRQ to UDX host** | **OPEN** — host `fire_irq` / soft ISR only; kernel notify product not closed. |
| **Freestanding `rtl8168` / `xhci_msc` kernel** | **Lab only** — stage numbers are **not** T0/T1 product bars under ABI-first. |
| **Linux inventory PASS** | Topology oracle (`a12f`, SS port map, VT-d translated). **≠** freestanding product. **≠** bar3. |
| **Steam bar3** | **OPEN** — media READY on `GJ-PERSIST` ≠ client launch ≠ Deck Top 50. |
| **Linux `.ko` product AC** | **Forbidden** for bar3/Steam ship story (**G-AC-1**). Dual-license product-shaped hosts remain **Linux-SHAPED UDX**. |
| **Module path load/probe/datapath** | Collect+stage **DONE**; REAL+SOFT1 **stable**; **D7** TX/RX / MMIO handoff **OPEN** (freestanding owns MMIO) — [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md). |

```text
Linux inventory stick  →  map PCI IDs / ports / IOMMU   (oracle)
GreenJade hwtest stick →  wave D soft host path + staged linux-drivers/
Module path (track B)  →  collect → stage → loader/ksym → finit → probe
Product T0             →  virtio apps on QEMU
Product real-HW        →  DDI caps + dual-license UDX and/or module DoD
bar3                   →  OPEN until client + matrix evidence
```

---

## 7. Definition of done — wave D vs module path vs remaining

### 7.1 Wave D DoD — **soft host path live** (this wave)

| # | Checklist item | Done when |
|---|----------------|-----------|
| 1 | **G-AC-1 stamped** | Docs: no bar3-by-GPL-`.ko`; **Linux-SHAPED UDX** for product-shaped class hosts; no GPL **source** in tree |
| 2 | **`GJ_SYS_DDI=103` live** | Kernel door `ddi_door.c` dispatched; SCAN/GET/OPEN/MAP_BAR present |
| 3 | **Real PCI scan** | `devmgr_soft` CF8/CFC walk + `pci scan PASS n=` (not deferred-only story) |
| 4 | **UDX bind matches kernel** | `udx_host_bind_*` uses SCAN/GET/OPEN/MAP_BAR |
| 5 | **User MMIO path** | MAP_BAR can call `vmm_map_user_device` |
| 6 | **Clean-room skeletons** | `rtl8168_udx` + `xhci_udx` in tree (soft probe only) |
| 7 | **Boot recipe** | `make hwtest-img` → `install-hwtest-usb` → UEFI → panel lamps (§4–5) |
| 8 | **Honesty stamps** | Product **TX/RX**, xHCI **BOT**, **live IRQ**, **bar3**, module load/probe remain **OPEN** |
| 9 | **Collect + stage live** | `collect-linux-drivers` + `GJ-PERSIST/linux-drivers/` on hwtest image |
| 10 | **No freestanding thrash as primary** | Docs point operators at ABI + UDX + module path, not stage counts |

Meeting this DoD means: **an operator can flash the stick, boot G752VT, and read wave-D soft DDI/PCI/UDX lamps without mistaking them for product drivers; linux-drivers are staged for the module track.** It does **not** mean T1 product TX/RX, BOT, live IRQ, module load, or bar3.

### 7.2 Module path DoD — “can run with Linux drivers”

Normative table: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) **§ DoD checklist (D1–D10)**.

| # | Criterion | Status |
|---|-----------|--------|
| **D1** | Collect `.ko` (`make collect-linux-drivers`) | **DONE** |
| **D2** | Stage on media (`GJ-PERSIST/linux-drivers/`) | **DONE** |
| **D3** | Soft module loader + ksym | **OPEN** (in progress) |
| **D4** | `finit_module` / boot smoke | **OPEN** |
| **D5** | Module loads (init returns 0) | **SOFT DONE** (r8169 INIT=0 embed) |
| **D6** | Probe binds PCI (`10ec:8168` hostish real) | **SOFT DONE** — REAL+SOFT1 stable; soft ≠ product |
| **D7** | Net datapath (TX/RX) / MMIO handoff | **OPEN** (**next**); freestanding owns MMIO |
| **D8** | USB datapath (HC + device path) | **OPEN** (xhci **SKIP builtin**) |
| **D9** | Iterative ksym honesty for full r8169/xhci_hcd | **OPEN** |
| **D10** | No GPL source in tree; no bar3-by-`.ko` claim | **DONE** (policy) |

**“Can run with Linux drivers” closes only when D5–D8 have greppable laptop evidence.** Staged media alone is **not** that close.

### 7.3 Explicitly **not** done (stay OPEN)

| Bar | Status |
|-----|--------|
| **bar3** (Steam client + Deck Top 50 matrix) | **OPEN** |
| Product NIC **TX/RX** on `10ec:8168` | **OPEN** |
| Product xHCI **BOT / MSC / HID** on `8086:a12f` | **OPEN** |
| **Live IRQ** (kernel notify → host ISR product) | **OPEN** |
| Live DDI **cap mint** (MMIO_FRAME / IRQ Notification / DMA window into host CNode) | **OPEN** |
| Product multi-server `devmgr` match → grant graph | **OPEN** |
| Product UDX NIC laptop LAN / sshd | **OPEN** |
| Soft module loader + ksym product surface | **OPEN** (in progress; r8169 embed soft-working; ksym N=289) |
| Hostish real `.ko` probe (`pci_dev` **0xb40**) | **SOFT DONE** — REAL+SOFT1 stable; soft ≠ product |
| Media `finit_module` load of staged `.ko` | **OPEN** |
| Net/usb datapath (DoD D7–D8) / MMIO handoff | **OPEN** (**next**); freestanding owns MMIO |
| Freestanding in-kernel stage count as T1 close | **Never** a product bar |
| Linux `.ko` as bar3/Steam product AC | **Forbidden** (**G-AC-1**) |

### 7.4 Short DoD vs remaining (one glance)

```text
WAVE D + MEDIA + REAL+SOFT1          STILL OPEN
──────────────────────────────       ────────────────────────────────
GJ_SYS_DDI=103 in kernel             Product TX/RX (rtl8168_udx / .ko)
Real soft PCI scan (devmgr_soft)     Product xHCI BOT/MSC (xhci_udx)
UDX SCAN/GET/OPEN/MAP_BAR bind       Live IRQ / kernel notify product
vmm_map_user_device for user MMIO    Full CNode MMIO/IRQ/DMA cap mint
Clean-room rtl8168_udx + xhci_udx    Multi-server devmgr match graph
collect + stage linux-drivers/       D7 net datapath / MMIO handoff
REAL+SOFT1 stable                    Freestanding still owns MMIO
r8169 INIT=0 + NETDEV SOFT 1         media finit_module; soft open/TX
G-AC-1: no bar3-by-.ko; no GPL src   bar3 (Steam client + matrix)
```

---

## 8. Two sticks — quick reference

| Stick | Make | Labels | Use |
|-------|------|--------|-----|
| **A — GreenJade** | `make hwtest-img` · `install-hwtest-usb` | **`GREENJADE`** + **`GJ-PERSIST`** | Wave D soft host path + lab freestanding; this runbook |
| **B — Linux inventory** | `make linux-hwtest-img` · `install-linux-hwtest` | **`GJ-LNX-ESP`** + **`GJ-LNX-ROOT`** | PCI/USB/IOMMU map only; not GJ product |

Do not mix labels or claims. Details: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

---

## 9. Related (minimal)

| Doc | Role |
|-----|------|
| [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) | Why ABI-first; freestanding disposition |
| [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) | Collect → stage → loader/ksym → finit → **DoD D1–D10** |
| [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) | Soft inventory matrix (wave D updated) |
| [DDI_SOFT.md](DDI_SOFT.md) | Soft devmgr / door notes |
| [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) | Porter contract |
| [HCL.md](HCL.md) | Tiers + product-path table |
| [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) | bar3 OPEN ceiling |

---

*Dual MIT OR Apache-2.0 spirit: small honest docs, fail-closed claims, no GPL **source** in tree.*  
***G-AC-1:** no bar3/Steam product AC via shipping GPL `.ko` — product-shaped class hosts = Linux-SHAPED UDX.*  
*Module path: REAL+SOFT1 stable; soft ≠ product; next = D7 datapath / MMIO handoff (freestanding owns MMIO).*  
*Wave D soft host path live ≠ product TX/RX ≠ BOT ≠ live IRQ ≠ bar3 closed.*
