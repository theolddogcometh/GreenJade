# GreenJade — Laptop Linux-shaped driver host (G752VT operator runbook)

| Field | Value |
|-------|--------|
| **Status** | **Wave D soft host path live** — `GJ_SYS_DDI=103`, real soft PCI scan, UDX SCAN/GET/OPEN/MAP_BAR bind; Dual DoD **A/B OPEN** (UDX USB / UDX NIC); freestanding class **SKIP**; **not** product T1 / **not** bar3 |
| **DUT** | ASUS ROG **G752VT** (RH71-class) |
| **Law** | Dual **MIT OR Apache-2.0** only; **no GPL source in tree**; **Soft ≠ product** |
| **Hard AC** | **G-AC-1** — no Linux `.ko` **runs in kernel** as product; product = hot+cold **Linux ABI** + **Linux-SHAPED UDX/DDI userspace** drivers |
| **Freestanding** | **SKIP** default — `GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`. **Stop freestanding rtl rabbit hole.** Not Dual DoD close. |
| **Image stamp** | GOP **STATUS (static) v0.1.184** from `GJ_IMAGE_VERSION` (`kernel/include/gj/config.h`); packed, not host-probed; confirm cut after flash |
| **Strategy** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · inventory [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) · track [TODO.md](TODO.md) Current track (2026-08-06) |
| **Companions** | [HCL.md](HCL.md) · [DDI_SOFT.md](DDI_SOFT.md) · [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) · [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

## One-sentence goal

**Run GreenJade on the G752VT with a Linux-class driver story: hot+cold ABI + DDI + userspace UDX hosts (product) — not freestanding rewrites of every class driver, not freestanding rtl R-climb as product, and not product AC by executing `.ko` in-kernel.**

### Operator pivot (2026-08) — read first

| Rule | Meaning |
|------|---------|
| **Stop freestanding rtl rabbit hole** | In-kernel `rtl8168.c` is residual **SKIP**, same policy as freestanding USB. Product NIC = **`rtl8168_udx`** + ABI/DDI. |
| **Dual DoD A/B** | **A** OPEN until host USB path (never `USBCMD.RS=1`). **B** OPEN until **interactive SSH login**. Banner / :22 / PK_OK / SUCCESS ≠ close. SUCCESS on **0.1.178** historical. **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical (not live bar). |
| **Soft ≠ product** | Soft DDI bind, soft module lamps, soft REAL+SOFT1 ≠ product TX/RX/BOT. |
| **G-AC-1** | No Linux `.ko` runs in kernel as product. |
| **Product** | Hot + cold Linux ABI + Linux-shaped **UDX/DDI userspace**. |
| **Flash bar** | Honesty: **v0.1.184** packed, not host-probed — Dual DoD **A** park RS-off · **B** exec TX drain after 183 Sending command · login OPEN · freestanding **SKIP** live · Dual DoD A **OPEN** until host USB path · Dual DoD B **OPEN** until interactive SSH login · SUCCESS on **0.1.178** historical · **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical · do not invent next N · **0.2.0** reserved |
| **Evidence** | Lamps / serial only — **no test-panel photo IDs**. |

### Operator / Grok media (2026-08)

| Artifact | Role |
|----------|------|
| `make collect-linux-drivers` | Harvest host `.ko` + firmware + **NEEDED-DRIVERS.txt** → `build/linux-drivers/` |
| `make hwtest-img` | Packs those onto **GJ-PERSIST/linux-drivers/** + ESP **NEEDED-DRIVERS.txt** |
| `make linux-hwtest-img` | Linux inventory USB; **`70-needed-drivers.sh`** writes DUT **NEEDED-DRIVERS** for collect |
| Soft module loader + ksym | **Eng residual** — see [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md); soft ≠ product; **G-AC-1** |
| Module **load** / soft bind (embed r8169) | **SOFT eng** — `INIT=0`, soft lamps possible; **not** Dual DoD B close; product NIC ≠ in-kernel `r8169.ko` |
| Real `.ko` probe / hostish `pci_dev` **0xb40** | Lab soft eng only — soft ≠ product |
| **Primary next** | **UDX Dual DoD A/B** — userspace `xhci_udx` / `rtl8168_udx` + DDI caps + hot/cold ABI |
| Freestanding rtl / freestanding USB | **SKIP default** — residual opt-in only; **not** Dual DoD close |
| **Dual DoD (laptop)** | **A** Linux-shaped **UDX USB** **OPEN**. **B** Linux-shaped **UDX NIC** **OPEN**. Soft ≠ product · **G-AC-1.** |
| Product TX/RX / BOT | **OPEN** — soft ≠ product; product path = **UDX/DDI userspace** |
| Bar3 / Deck Top 50 | **OPEN** — [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) only (not boot spam) |

Honesty: staged `.ko` are **host Linux** binaries (often GPL). GreenJade **source tree** stays dual-license (**no GPL source**); product AC is not “bar3 by shipping GPL.” Soft netdev bind ≠ TX/RX. Public docs use **lamps / serial only** — **no test-panel photo IDs**.

### Module path at a glance (eng residual; normative: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md))

| # | Step | Status |
|---|------|--------|
| 1 | Collect `.ko` (`collect-linux-drivers`) | **DONE** (live) |
| 2 | Stage on media (`GJ-PERSIST/linux-drivers/`) | **DONE** (live) |
| 3 | Soft module loader + ksym | **IN PROGRESS** (eng residual) |
| 4 | Soft load + id match + netdev | **SOFT eng** — soft ≠ product |
| 5 | Hostish real probe | **SOFT eng** — soft ≠ product |
| 6 | Datapath / product wire | **OPEN** — Dual DoD **B** close is host **interactive SSH login**, not freestanding / not in-kernel `.ko` |
| 7 | Iterative ksym / media `finit_module` | **OPEN** (eng) |
| Dual DoD | UDX USB (**A**) / UDX NIC (**B**) | **OPEN** |

Module path does **not** rewrite Dual DoD and does **not** waive **G-AC-1**.

---

## 1. Law first — **G-AC-1** (read before boot)

| Rule | Meaning for operators |
|------|------------------------|
| **G-AC-1** | **No Linux `.ko` binary runs in the kernel as product.** No `init_module`-style product AC; no bar3/Steam ship via GPL `.ko` in-kernel. Product-shaped class hosts = **Linux-SHAPED UDX/DDI in userspace** (clean-room / dual-license out of tree). Inventory under a Linux stick is **oracle only**. |
| **Product** | **Hot + cold Linux ABI** (Option C) + **Linux-shaped drivers in userspace** over **UDX/DDI** caps → virtio T0 apps until real-HW UDX datapath. |
| **Freestanding SKIP** | Default **`GJ_RTL8168_PROBE=0`** · **`GJ_XHCI_MSC_PROBE=0`**. Freestanding class thrash is **not** the product track and **not** Dual DoD close. **Stop freestanding rtl rabbit hole.** |
| **Soft residual** | Soft **`RUN_INIT=0`** / **`freestanding_no_exec`** (SKIP exec of staged `.ko` **init in kernel**) is **engineering residual** — not “never use Linux drivers.” Userspace hosts remain the product path. |
| **License** | GreenJade tree + UDX + out-of-tree product drivers: **MIT OR Apache-2.0** (or BSD dual-license out of tree). **No** Linux driver **source** import. |
| **Two tracks** | **(A)** UDX soft host + hot/cold ABI (this runbook, wave D) — **product path** / Dual DoD A/B. **(B)** Host-collected module path: collect → stage → soft loader/ksym — eng only; does **not** rewrite G-AC-1. |
| **Not product** | In-kernel freestanding stage thrash; freestanding rtl R-climb as Dual DoD B; Linux inventory `RESULT: PASS`; Steam **bar3** (**OPEN**); soft REAL+SOFT1 / module INIT=0 ≠ product datapath; claiming product NIC = in-kernel `r8169.ko`. |
| **Operator choice (2026-08)** | **ABI-first + UDX only:** freestanding panel `xhci stage=…` and `net … t/f/b/r` are **lab residual honesty only** (often **SKIP**). Product-shaped work = **hot+cold ABI + DDI bind + UDX userspace hosts**. Do **not** treat freestanding counters or soft `.ko` lamps as Dual DoD or T0/T1 product close. |

```text
  ALLOWED (G-AC-1 / law)           FORBIDDEN as product AC
  ─────────────────────────────    ────────────────────────────────
  Hot+cold Linux ABI (apps)        Linux .ko runs/exec in kernel as ship story
  Clean-room UDX driver .c         GPL Linux driver *source* in tree
  Out-of-tree dual-license hosts   Claiming bar3 closed via staged .ko
  Cap-gated MMIO/IRQ/DMA (DDI)     Freestanding thrash / rtl rabbit hole as product
  Collect/stage host .ko (eng)     Claiming inventory PASS = T1 done
  Soft loader + ksym (eng)         Claiming soft UDX/.ko probe = TX/RX/BOT
  RUN_INIT=0 residual (eng)        Reading residual as “never Linux drivers”
  linux-hwtest inventory stick     Product NIC = in-kernel r8169.ko
  Dual DoD via UDX USB/NIC         Dual DoD via freestanding :22 / freestanding R
```

**Hard rule:** Linux-shaped **API names** (`probe` / `ioremap` / `dma_*`) in UDX are **clean-room shape** for **userspace** hosts. Staged `.ko` are for **module-path engineering** only — they do **not** make in-kernel `.ko` the product NIC/USB stack (**G-AC-1**).

---

## 2. What “Linux-shaped” means here (post wave D)

| Layer | Role | Wave D reality | Product? |
|-------|------|----------------|----------|
| **Linux ABI Option C** | Syscall/personality so Linux-shaped **apps** run | Primary path | Soft → product |
| **`GJ_SYS_DDI` (103)** | Native door: SCAN / GET / OPEN / MAP_BAR / CFG / DMA_NOTE | **Live in kernel** (`kernel/drv/ddi_door.c`) | Soft surface ≠ full CNode mint |
| **`devmgr_soft`** | Soft policy inventory; **real type-0 PCI CF8/CFC scan** | **Live walk** + G752 ID lamps | Soft table; no product match graph |
| **UDX** (`user/udx/`) | Userspace runtime: Linux-shaped `probe` / `remove` / `ioremap` / IRQ / `dma_*` | Bind via **SCAN→GET→OPEN→MAP_BAR** matching kernel ops | Soft host / inject on Linux host |
| **`vmm_map_user_device`** | UC MMIO map into process user AS | Used by MAP_BAR when current process exists | Soft map path; not full MMIO_FRAME cap mint story |
| **Driver `.c`** | Clean-room class hosts | `rtl8168_udx` + `xhci_udx` **skeletons** | Soft only today — Dual DoD A **OPEN** until host USB path · Dual DoD B **OPEN** until interactive SSH login |
| **In-kernel freestanding** | Residual scaffolds; **SKIP** default | `rtl8168.c`, `xhci_msc.c` | **No** — **SKIP**; product NIC/USB ≠ freestanding / ≠ in-kernel `.ko` |

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
| **rtl8168_udx** | `user/drivers/rtl8168_udx/` | Product residual host — laptop **ARP + ping proven** (2026-08-14); Dual DoD **B OPEN** until host **interactive SSH login** |
| **xhci_udx** | `user/drivers/xhci_udx/` | Clean-room skeleton — soft cap/params/PORTSC + **soft BOT progress stub**; **Dual DoD A OPEN** (no product BOT/MSC) |

### 2.2 Explicitly still **OPEN** (do not claim)

| Bar | Status |
|-----|--------|
| Dual DoD **A** — **UDX USB** datapath / BOT / MSC on `8086:a12f` | **OPEN** |
| Dual DoD **B** — **UDX NIC** + OpenSSH on `10ec:8168` | **OPEN** until host **interactive SSH login** (L3 ARP/ping **proven**; SUCCESS on **0.1.178** was abandoned `sshd_gj`; **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical; fly **0.1.184** packed, not host-probed) |
| Product NIC **TX/RX** datapath / link / PHY (userspace UDX) | **L3 ARP/ping proven** (2026-08-14); login still OPEN |
| Product xHCI **BOT / MSC / HID** (userspace UDX) | **OPEN** |
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
| **Secure Boot** | **Off** for GreenJade USB |
| **Flash bar** | Expect **STATUS (static) v0.1.184** after flash of this cut (packed, not host-probed) |

Linux inventory ground truth (topology oracle only): [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

| Device | PCI ID | Soft UDX class (wave D) | In-kernel freestanding | Product |
|--------|--------|-------------------------|------------------------|---------|
| Wired NIC | **`10ec:8168`** | `rtl8168_udx` clean-room skeleton (**userspace**) — Dual DoD **B** | `rtl8168.c` **SKIP** residual | T0 **virtio-net**; real-HW = UDX NIC TX/RX — **not** freestanding · **not** in-kernel `r8169.ko` |
| USB HC | **`8086:a12f`** | `xhci_udx` clean-room skeleton (**userspace**) — Dual DoD **A** | `xhci_msc.c` **SKIP** residual | T1 via DDI/UDX hosts later; **BOT OPEN** — not product freestanding / not in-kernel HC `.ko` |

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
| `scripts/collect-linux-drivers.sh` (via `hwtest-img`) | **`GJ-PERSIST/linux-drivers/`** | Host-collected `.ko` + firmware + meta — module path eng only |

Module-path layout (eng residual): [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md).

### 4.2 DUT — UEFI boot

1. AC power preferred; insert stick in a **USB3** jack.  
2. Power on → spam **Esc** (or **F8**) → choose **UEFI: … USB**.  
3. Secure Boot **Disabled**; prefer pure UEFI (not CSM-only).  
4. Watch **panel** (no COM1 on this chassis): loader `GJ-EFI` / kernel progress / soft lamps.  
5. Confirm flash identity: **STATUS (static) v0.1.184**.  
6. After run, power off and collect logs on the lab host (below).

Firmware tips: [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md).

### 4.3 Collect evidence

```sh
sudo umount /run/media/$USER/* 2>/dev/null; true
sudo mkdir -p /mnt/gj-esp /mnt/gj-persist

sudo mount -L GREENJADE /mnt/gj-esp
cat /mnt/gj-esp/EFI/GREENJADE/BOOT.LOG
cat /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT   # if MSC stick-log path wrote
# Prefer grepping panel-captured serial tees / KLOG when present:
grep -E 'devmgr:|ddi_door|main: soft ddi|udx |rtl8168_udx|xhci_udx|vmm: map_user|image version' \
  /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT 2>/dev/null || true
ls /mnt/gj-esp/EFI/GREENJADE/drivers 2>/dev/null || true
sudo umount /mnt/gj-esp

sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/logs /mnt/gj-persist/steam 2>/dev/null || true
cat /mnt/gj-persist/steam/STATUS 2>/dev/null || true   # READY = media only
# Module path staging (collect ≠ load; eng only):
ls /mnt/gj-persist/linux-drivers/modules 2>/dev/null || true
cat /mnt/gj-persist/linux-drivers/NEEDED-DRIVERS.txt 2>/dev/null || true
sudo umount /mnt/gj-persist
```

Panel capture is valid evidence when serial is absent. **No test-panel photo IDs** in public docs — lamps / serial text only.

---

## 5. Expected panel / serial lines (wave D)

**Soft ≠ product.** These lamps prove the **Linux-shaped host path is wired and greppable**, not that Dual DoD A/B, T1 NIC/USB product, TX/RX, BOT, live IRQ, or bar3 is done.

### 5.0 Image stamp (first check after flash)

| Lamp | Meaning | Closes product / dual DoD? |
|------|---------|----------------------------|
| **STATUS (static) v0.1.184** | Title bar uses `GJ_IMAGE_VERSION` from `config.h`; packed, not host-probed | **No** — flash identity only |
| **`main: image version=0.1.184`** | Serial echo of the same stamp | **No** |

Honesty: this cut is **v0.1.184** packed, not host-probed. SUCCESS on **0.1.178** historical. **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical (not the live bar). **Stamp bump:** do not invent next N; confirm STATUS title matches `GJ_IMAGE_VERSION` after flash. **0.2.0** reserved.

### 5.1 Kernel / door (expect on freestanding-SKIP laptop boot)

| Soft lamp (grep / panel) | Meaning | Closes product / Dual DoD? |
|--------------------------|---------|----------------------------|
| **`devmgr: soft init PASS`** | Soft DDI/devmgr ready | **No** |
| **`devmgr: soft pci scan PASS n=N`** | **Real** CF8/CFC soft inventory filled | **No** |
| **`devmgr: soft found 10ec:8168`** | G752 wired NIC seen in soft table | **No** — not Dual DoD B / not TX/RX |
| **`devmgr: soft found 8086:a12f`** | G752 xHCI seen in soft table | **No** — not Dual DoD A / not BOT |
| **`devmgr: soft dma window note`** | Soft window table; **no** VT-d mint | **No** |
| **`ddi_door: soft init PASS`** | Door tables up | **No** |
| **`ddi_door: soft product surface PASS`** | `GJ_SYS_DDI` inventory lamp (soft≠product) | **No** |
| **`ddi_door: soft scan count=N`** | Door SCAN via `devmgr_soft_pci_scan` | **No** |
| **`ddi_door: soft open handle=`** | Soft open (when host binds) | **No** — not CNode mint |
| **`ddi_door: soft map bar … user=1`** | MAP_BAR via **`vmm_map_user_device`** | **No** — not product cap story |
| **`main: soft freestanding lab only`** | Explicit: freestanding residual ≠ product | **No** (policy lamp) |
| **`main: soft ddi found 10ec:8168`** / **`8086:a12f`** | Boot smoke ID + BAR notes | **No** |
| **`main: soft ddi bind 10ec:8168 PASS`** | OPEN + MAP BAR0/2 + soft IRQ note | **No** — not Dual DoD B |
| **`main: soft ddi bind 8086:a12f PASS`** | OPEN + MAP BAR0 + soft CFG/IRQ note | **No** — not Dual DoD A |
| **`main: soft ddi laptop smoke PASS … abi_first=1`** | Boot DDI bind path finished | **No** |
| **`main: soft linux_module … SKIP|PASS|FAIL`** | Module-path eng residual | **No** — eng only; **G-AC-1** |
| Freestanding **`xhci: … stage=`** / **`NET … t/f/b/r`** | Residual only if opt-in (`PROBE=1`); default **SKIP** | **No** — never Dual DoD close |

### 5.2 UDX / class hosts (when host process runs bind)

| Soft lamp | Meaning | Closes Dual DoD / product? |
|-----------|---------|----------------------------|
| **`udx: soft ddi bind PASS`** | SCAN/GET/OPEN/MAP_BAR path succeeded | **No** |
| **`udx: soft ddi bind SKIP`** | No GJ_SYS_DDI / no match (host-linux inject fallback) | **No** |
| **`udx: soft ddi host note PASS`** / **`udx: soft ddi-ready`** | Host soft surface lamps | **No** |
| **`rtl8168_udx: soft probe PASS`** | Clean-room NIC skeleton probe+ISR soft path | **No** — Dual DoD **B OPEN** |
| **`xhci_udx: soft probe PASS`** / **`soft cap ver=`** | Clean-room xHCI skeleton cap read | **No** — Dual DoD **A OPEN** |
| **`xhci_udx: soft bot stage=1..4`** / **`soft bot stub PASS`** | Soft BOT **progress** (cap→params→ports→stub) | **No** — catalog only; **no** CBW/MSC |

In-tree greppable anchors (wave D):

```text
STATUS (static) v0.1.184
main: image version=0.1.184
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
1. Confirm **STATUS (static) v0.1.184**.  
2. Ignore freestanding stage/net counters for product judgment (default **SKIP**).  
3. Prefer **`main: soft freestanding lab only`** + **`main: soft ddi bind … PASS`** (when silicon present) + **`main: soft ddi laptop smoke PASS … abi_first=1`**.  
4. Class host soft notes appear when UDX drivers are launched later — still **not** Dual DoD A/B closed, not product TX/RX/BOT, not a license to load GPL `.ko`.

---

## 6. Net and USB honesty (do not over-claim)

| Claim | Status |
|-------|--------|
| **T0 product net** | **virtio-net** (QEMU / CI). Laptop wire is **UDX** (`rtl8168_udx`). |
| **Dual DoD B (UDX NIC)** | **OPEN** until host **interactive SSH login**. Laptop **ARP + ping proven** (2026-08-14). SUCCESS on **0.1.178** historical. **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical (not live bar). Fly **0.1.184** packed, not host-probed. Soft probe ≠ close. |
| **Dual DoD A (UDX USB)** | **OPEN** — `xhci_udx` userspace bind + USB datapath. Soft bot stub ≠ close. |
| **G752 wired NIC** | Hardware **`10ec:8168`**. Product path = `rtl8168_udx`. **L3 ARP + ping proven.** Dual DoD **B OPEN** until host **interactive SSH login**. **Product NIC ≠ freestanding · ≠ in-kernel `r8169.ko`.** |
| **G752 xHCI** | Hardware **`8086:a12f`**. Soft `xhci_udx` cap read / residual freestanding `xhci_msc` ≠ product USB. **Product BOT/MSC OPEN.** |
| **Live IRQ to UDX host** | **OPEN** — host `fire_irq` / soft ISR only; kernel notify product not closed. |
| **Freestanding `rtl8168` / `xhci_msc` kernel** | **SKIP default** — residual opt-in only. **Not** Dual DoD close. **Stop freestanding rtl rabbit hole.** |
| **Linux inventory PASS** | Topology oracle (`a12f`, SS port map, VT-d translated). **≠** freestanding product. **≠** bar3. |
| **Steam bar3** | **OPEN** — media READY on `GJ-PERSIST` ≠ client launch ≠ Deck Top 50. |
| **Linux `.ko` product AC** | **Forbidden** (**G-AC-1**): no `.ko` **runs in kernel** as product. Product = hot+cold ABI + **Linux-SHAPED UDX/DDI userspace**. |
| **Soft `RUN_INIT=0` / `freestanding_no_exec`** | Eng residual: soft SKIP `.ko` init **exec in kernel** — **not** “never use Linux drivers.” |
| **Module path load/probe** | Collect+stage **DONE**; soft eng possible; **not** Dual DoD close — [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md). |

```text
Linux inventory stick  →  map PCI IDs / ports / IOMMU   (oracle)
GreenJade hwtest stick →  wave D soft host path + staged linux-drivers/ (eng)
Module path (track B)  →  collect → stage → loader/ksym → eng only (not product AC)
Product path           →  hot+cold Linux ABI + UDX/DDI userspace hosts
Dual DoD A             →  UDX USB  OPEN
Dual DoD B             →  UDX NIC  OPEN until interactive SSH login (ARP/ping proven)
Product T0             →  virtio apps on QEMU; laptop wire = UDX
Product real-HW        →  DDI caps + dual-license UDX userspace
Freestanding class     →  SKIP default (not Dual DoD)
bar3                   →  OPEN until client + matrix evidence
Flash bar              →  STATUS (static) v0.1.184 packed, not host-probed 
```

---

## 7. Definition of done — Dual DoD A/B vs wave D vs residual

### 7.0 Dual DoD (retargeted 2026-08 — freestanding net/USB out)

| # | DoD | Class | Status | Close when |
|---|-----|-------|--------|------------|
| **A** | **Linux-shaped USB** (laptop) | C1/C2 **UDX** | **OPEN** | Host USB path. RS-off / scratchpad ≠ close. Never `USBCMD.RS=1` unless the operator named that experiment. **Not** freestanding MSC. **Not** `usb_storage.ko` init in kernel. Soft lamps alone ≠ close. |
| **B** | **Linux-shaped NIC** (laptop) | C1/C2 **UDX** | **OPEN** | Host **interactive SSH login**. Banner / :22 / PK_OK / SUCCESS ≠ login. SUCCESS proven on **0.1.178** (historical). **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical. Fly **0.1.184** packed, not host-probed. Wire hop is `rtl8168_udx`. **Not** freestanding rtl R-climb. **Not** in-kernel `r8169.ko` wire. Soft lamps alone ≠ close. |

**Defaults (honesty):**
- `GJ_XHCI_MSC_PROBE=0` · **`GJ_RTL8168_PROBE=0`** — freestanding class **SKIP**.
- Soft r8169 load/ksym may still run eng residual (`INIT=0`); **WIRE product** is UDX Dual DoD **B**, not freestanding.
- STATUS title: **`STATUS (static) v0.1.184`** packed, not host-probed (test what you fly). **0.2.0** reserved.

| # | Work | Status | Note |
|---|------|--------|------|
| **1** | Freestanding rtl / freestanding USB | **SKIP default** | Residual opt-in only; **not** Dual DoD close |
| **2** | **Hot + cold Linux ABI** | **PRIMARY** | Host Linux-shaped apps + drivers in userspace |
| **3** | **`rtl8168_udx` + DDI caps** | **PRIMARY · DoD B** | Product NIC path |
| **4** | **`xhci_udx` + DDI caps** | **PRIMARY · DoD A** | Product USB path |
| **5** | Soft ksym / module path | Eng residual | Soft≠product; G-AC-1 |
| **6** | virtio-net T0 | Product default (QEMU) | Until UDX owns real HW |

**Blocking now:** Linux-shaped **userspace** NIC + USB hosts over ABI/UDX. Soft ≠ product · **G-AC-1.**

### 7.1 Wave D DoD — **soft host path live** (infra; not Dual DoD close)

| # | Checklist item | Done when |
|---|----------------|-----------|
| 1 | **G-AC-1 stamped** | Docs: no Linux `.ko` **runs in kernel** as product; product = hot+cold ABI + **Linux-SHAPED UDX/DDI userspace**; no GPL **source** in tree; freestanding **SKIP** |
| 2 | **`GJ_SYS_DDI=103` live** | Kernel door `ddi_door.c` dispatched; SCAN/GET/OPEN/MAP_BAR present |
| 3 | **Real PCI scan** | `devmgr_soft` CF8/CFC walk + `pci scan PASS n=` |
| 4 | **UDX bind matches kernel** | `udx_host_bind_*` uses SCAN/GET/OPEN/MAP_BAR |
| 5 | **User MMIO path** | MAP_BAR can call `vmm_map_user_device` |
| 6 | **Clean-room skeletons** | `rtl8168_udx` + `xhci_udx` in tree (soft probe only) |
| 7 | **Boot recipe** | `make hwtest-img` → `install-hwtest-usb` → UEFI → panel lamps (§4–5) |
| 8 | **Honesty stamps** | Dual DoD **A/B**, product **TX/RX**, xHCI **BOT**, **live IRQ**, **bar3** remain **OPEN** |
| 9 | **Collect + stage live** | `collect-linux-drivers` + `GJ-PERSIST/linux-drivers/` on hwtest image (eng) |
| 10 | **No freestanding thrash as primary** | Docs point operators at ABI + UDX Dual DoD; freestanding **SKIP** |

Meeting this DoD means: **an operator can flash the stick, boot G752VT, and read wave-D soft DDI/PCI/UDX lamps without mistaking them for Dual DoD close or product drivers.** It does **not** mean Dual DoD A/B closed, product TX/RX, BOT, live IRQ, or bar3.

### 7.2 Module path — eng residual only

Normative table: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md). Collect/stage may be **DONE**; soft load/probe may be eng-stable. **None of that closes Dual DoD A/B** or waives **G-AC-1**. Product NIC/USB remain **UDX userspace**.

### 7.3 Explicitly **not** done (stay OPEN)

| Bar | Status |
|-----|--------|
| Dual DoD **A** UDX USB | **OPEN** |
| Dual DoD **B** UDX NIC | **OPEN** |
| **bar3** (Steam client + Deck Top 50 matrix) | **OPEN** |
| Product NIC **TX/RX** on `10ec:8168` via UDX | **OPEN** |
| Product xHCI **BOT / MSC / HID** on `8086:a12f` via UDX | **OPEN** |
| **Live IRQ** (kernel notify → host ISR product) | **OPEN** |
| Live DDI **cap mint** (MMIO_FRAME / IRQ Notification / DMA window into host CNode) | **OPEN** |
| Product multi-server `devmgr` match → grant graph | **OPEN** |
| Freestanding in-kernel stage count as Dual DoD / T1 close | **Never** |
| Lab freestanding rtl as product NIC | **Never** — **SKIP**; product NIC ≠ freestanding · ≠ in-kernel `r8169.ko` |
| Linux `.ko` **runs in kernel** as product AC | **Forbidden** (**G-AC-1**) |
| Soft `RUN_INIT=0` misread as “never Linux drivers” | **Wrong** — eng residual only; product = userspace UDX/DDI |
| Flash bar **v0.1.184** | **This cut** packed, not host-probed — confirm STATUS title after flash; **0.1.178** SUCCESS historical; **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical |

### 7.4 Short DoD vs remaining (one glance)

```text
WAVE D SOFT HOST PATH (infra)        STILL OPEN / NEVER PRODUCT
──────────────────────────────       ────────────────────────────────
GJ_SYS_DDI=103 in kernel             Dual DoD A UDX USB
Real soft PCI scan (devmgr_soft)     Dual DoD B UDX NIC
UDX SCAN/GET/OPEN/MAP_BAR bind       Product TX/RX (rtl8168_udx userspace)
vmm_map_user_device for user MMIO    Product xHCI BOT/MSC (xhci_udx)
Clean-room rtl8168_udx + xhci_udx    Live IRQ / kernel notify product
collect + stage linux-drivers/ eng   Full CNode MMIO/IRQ/DMA cap mint
G-AC-1: no .ko runs in-kernel product Multi-server devmgr match graph
hot+cold ABI + UDX/DDI userspace     Freestanding as Dual DoD (NEVER)
freestanding SKIP default            bar3 (Steam client + matrix)
STATUS (static) v0.1.184             Claim stamp without matching flash media
```

---

## 8. Two sticks — quick reference

| Stick | Make | Labels | Use |
|-------|------|--------|-----|
| **A — GreenJade** | `make hwtest-img` · `install-hwtest-usb` | **`GREENJADE`** + **`GJ-PERSIST`** | Wave D soft host path + UDX Dual DoD track; freestanding **SKIP**; this runbook |
| **B — Linux inventory** | `make linux-hwtest-img` · `install-linux-hwtest` | **`GJ-LNX-ESP`** + **`GJ-LNX-ROOT`** | PCI/USB/IOMMU map only; not GJ product |

Do not mix labels or claims. Details: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

---

## 9. Related (minimal)

| Doc | Role |
|-----|------|
| [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) | Why ABI-first; freestanding SKIP disposition |
| [TODO.md](TODO.md) | Current track — Dual DoD retarget + flash bar **v0.1.184** packed, not host-probed |
| [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) | Collect → stage → loader/ksym eng residual |
| [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) | Soft inventory matrix |
| [DDI_SOFT.md](DDI_SOFT.md) | Soft devmgr / door notes; Soft ≠ product |
| [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) | Porter contract; Dual DoD A/B OPEN |
| [HCL.md](HCL.md) | Tiers + product-path table |
| [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) | bar3 OPEN ceiling |
| [ASSURANCE_LITE.md](ASSURANCE_LITE.md) | Claim classes · test what you fly |

---

*Dual MIT OR Apache-2.0 spirit: small honest docs, fail-closed claims, no GPL **source** in tree.*  
***G-AC-1:** no Linux `.ko` **runs in kernel** as product — product = hot+cold Linux ABI + Linux-SHAPED UDX/DDI **userspace**.*  
***Soft ≠ product.** Soft DDI / soft module / soft probe ≠ Dual DoD close.*  
*Freestanding class **SKIP** default (`GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`). **Stop freestanding rtl rabbit hole.***  
*Dual DoD **A** UDX USB **OPEN** · **B** UDX NIC **OPEN** until host **interactive SSH login**. Freestanding is not Dual DoD close.*  
*Flash bar honesty: **STATUS (static) v0.1.184** packed, not host-probed — Dual DoD **A** park RS-off (never `USBCMD.RS=1`) · Dual DoD B **OPEN** until interactive SSH login · SUCCESS on **0.1.178** historical · **0.1.183** host FAIL (`Sending command: true` **PASS**; exec 124) historical · test what you fly · do not invent next N · **0.2.0** reserved. No test-panel photo IDs.*  
*Wave D soft host path live ≠ Dual DoD closed ≠ product TX/RX ≠ BOT ≠ live IRQ ≠ bar3 closed.*
