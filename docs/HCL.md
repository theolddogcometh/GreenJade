# GreenJade — Hardware Compatibility List (HCL)

| | |
|--|--|
| **Status** | Living list — start here for real-hw install work |
| **Policy** | Clean-room dual MIT OR Apache-2.0 only (no GPL drivers) |
| **Tiers** | See [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) 13 |

## Tiers

| Tier | Meaning | Product bar |
|------|---------|-------------|
| **T0** | QEMU virtio + UEFI OVMF | CI must pass (`make smoke`) |
| **T1** | Documented NVMe + AHCI + common USB HID | Real-hw install path |
| **T2** | One SAS HBA family (clean-room) | Storage product |
| **T3** | GPU accel opportunistic | Deck/desktop polish |

## T0 — Virtual (done / CI)

| Device class | Implementation | Smoke gate |
|--------------|----------------|------------|
| virtio-blk | `kernel/drv/virtio_blk.c` + store door | `store_door` / `storaged` / `vfsd` |
| mini-FS + shell | `vfs_door` + `shell.elf` echo/cat/ls | `greenjade-shell: vfs PASS` |
| virtio-net | `virtio_net` + net door | `net_door` / `netstackd` |
| virtio-gpu | `virtio_gpu` + session door | `sessiond` / compositor |
| virtio-input | `virtio_input` | session input |
| virtio-scsi | probe path | `INQUIRY ok` |
| UEFI OVMF | `GreenJade.efi` | `GJ-OVMF` / `kmain_uefi` |
| Multiboot2 | GRUB ISO | `M0 OK` |
| x2APIC / SMP | AP bring-up | `ap_run ok` |
| VT-d soft | tables + TE | `iommu: vtd * PASS` |

## T1 — Real hardware (next)

| Class | Target | Notes |
|-------|--------|-------|
| Storage | NVMe (1 documented controller) | Soft CAP via `vmm_map_device_uc` when ctrl present; QEMU `-device nvme` optional (RHEL may omit model) |
| Storage | AHCI SATA | PCI class + soft CAP/GHC via `vmm_map_device_uc` (high UC window) |
| Console | UEFI GOP / simple FB | Already partial via virtio-gpu shape |
| Input | USB HID (kbd/mouse) | Host HC probe (`usb: probe`); HID parse later |
| USB HC | UHCI/OHCI/EHCI/xHCI | PCI class 0C:03 probe; **in-kernel MSC = soft scaffold only** (see below) |
| Input | PS/2 8042 | Soft status probe (`ps2: status=… PASS`) |
| Net | **virtio-net (T0 product)**; real NIC later via userspace/DDI | In-kernel `rtl8168` = **lab soft only** |
| Firmware | UEFI + ACPI | `X86_64_INTEL_PLATFORM.md` |

### Product path vs freestanding soft drivers (2026-08 ABI-first)

| Path | Role | Product bar? |
|------|------|----------------|
| **Linux ABI (Option C)** + doors + virtio | Primary: apps / netstackd / sshd on T0 | **Yes** |
| **Userspace + DMA/IRQ caps (DDI) + UDX** | Real-HW class drivers: clean-room or **out-of-tree dual-license** hosts | **Yes** (when DDI grants ship) |
| **Soft DDI / `devmgr_soft` + soft host path** | Types + init/scan/window-note; ddi_door / udx bind soft lamps | **No** — see [DDI_SOFT.md](DDI_SOFT.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) |
| **In-kernel `xhci_msc` / `rtl8168`** | Bring-up scaffold on G752; panel stages | **No** — lab only; soft ≠ T1 close |
| **G752VT soft UDX class notes** | `rtl8168_udx` (`10ec:8168`) / `xhci_udx` (`8086:a12f`) soft | **No** — soft host path ≠ product NIC/USB |
| **`linux-hwtest` inventory** | Full Linux stack for DUT map | **No** — operator truth only |
| **Linux `.ko` / GPL drivers in tree** | Forbidden ([DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) **G-AC-1**) | **Never** |

| ABI-first soft status (2026-08-02) | Class | Product? |
|------------------------------------|-------|----------|
| `linux_cold_net` socket bridge | Soft bridge (STREAM→`net_tcp`) | Partial |
| `personality_gj` cold personality | Userspace **scaffold** only | No (kthread still default) |
| `net_tcp_poll_mask` / vfs poll route | Soft/partial readiness | Partial |
| T0 net | **virtio-net** | Yes (CI) — remains until UDX NIC product |
| Real NIC / freestanding rtl | Lab soft | No |
| Laptop soft host path (G752VT) | Operator runbook + soft lamps | No — bar3 **OPEN** |

Inventory detail: [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) · strategy: [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) · laptop UDX host: [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md).

**Strategy one-liner:** GreenJade is a capability kernel that runs **Linux-shaped userspace** and hosts **clean-room / out-of-tree** drivers over caps — not a reimplementation of every Linux PCI class driver in-kernel, and not a ship vehicle for GPL.

## T2 — SAS / enterprise

| Class | Target | Notes |
|-------|--------|-------|
| HBA | One documented SAS family | `scsi_mid` + userspace host |
| Multipath | Deferred | After single path stable |

## T3 — GPU

| Class | Target | Notes |
|-------|--------|-------|
| Scanout | GOP + optional KMS-shaped | Compositor already present path |
| 3D | Vulkan ICD bring-up | Host ICD smoke exists; real HW later |

## Install checklist (real-hw)

1. `make install-img` → write GPT ESP to USB  
2. Boot UEFI → `KERNEL.ELF` → serial `M0 OK`  
3. Rootfs: `init` / `sh` / `vfsd` / `storaged` / `sessiond`  
4. Storage: format via vfs door / store door  
5. Input + display for interactive shell  
6. Net for package/Steam fetch (later)

## Hardware-test live media (logs + remote debug)

Two **different** USB images — do not mix labels or claims:

| Media | Make target | Labels | Role |
|-------|-------------|--------|------|
| **GreenJade freestanding** | `make hwtest-img` · `install-hwtest-usb` | ESP **`GREENJADE`** · **`GJ-PERSIST`** | Product path: UEFI → `KERNEL.ELF` → stick logs / Steam media |
| **Linux inventory** | `make linux-hwtest-img` · `install-linux-hwtest` | **`GJ-LNX-ESP`** · **`GJ-LNX-ROOT`** | Full Linux driver stack; auto-report then poweroff |

### A) Freestanding dual-partition USB (product path)

| Step | Command / action |
|------|------------------|
| 1. Build | `make hwtest-img` → `build/greenjade-hwtest.img` |
| 2. Write USB | `sudo make install-hwtest-usb DEV=/dev/sdX` (wipes device) |
| 3. Layout | **p1** ESP FAT **`GREENJADE`** (UEFI) · **p2** **ext4** **`GJ-PERSIST`** (logs + ssh + **steam/**) |
| 4. Boot DUT | UEFI → `BOOTX64.EFI` → serial/panel `GJ-EFI` / `M0 OK` |
| 5. Collect logs | Lab host: `sudo mount -L GJ-PERSIST /mnt/gj-persist` → `logs/`; ESP: `EFI/GREENJADE/BOOT.LOG`, `KLOG.TXT` |
| 6. Remote SSH | Product **sshd** default-on on virtio-net (`sshd.elf` **TCP :22**). Real laptop NIC **not** virtio. Lab: `sudo make hwtest-ssh-setup`. QEMU hostfwd `2222→22`. |
| 7. Steam (options **2 + 3**) | **No dpkg.** Media only on `GJ-PERSIST/steam/STATUS`. See [STEAM_HWTEST.md](STEAM_HWTEST.md). **bar3 OPEN.** |

### B) Linux inventory USB (DUT map / xHCI evidence)

| Step | Command / action |
|------|------------------|
| 1. Build | `sudo make linux-hwtest-img` → `build/linux-hwtest.img` |
| 2. Write USB | `sudo make install-linux-hwtest DEV=/dev/sdX` (wipes device) |
| 3. Boot DUT | UEFI, Secure Boot off → auto probes → **poweroff** |
| 4. Collect | `sudo mount -L GJ-LNX-ESP /mnt/esp` → `GJ-HWTEST/reports/LATEST.txt`, `GJ-HWTEST-RESULT.txt` |

Linux `RESULT: PASS` = probe scripts finished under **Linux**. **≠** freestanding product PASS. **≠** bar3. Details: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md), `scripts/linux-hwtest/README.md`.

Also available:

| Target | Output | Use |
|--------|--------|-----|
| `make live-iso` | `build/greenjade-live.iso` | QEMU `-cdrom` / optical Multiboot2+EFI hybrid |
| `./scripts/run-live-iso.sh` | QEMU boot of live ISO | Serial on stdio; hostfwd `2222→22` for product sshd path |
| `make sshd-gj` / `make sshd` | `build/user/sshd.elf` / `build/sshd` | Freestanding product daemon + host POSIX smoke |

QEMU ISO smoke:

```sh
make live-iso
./scripts/run-live-iso.sh
```

## Real DUT — ASUS G752VT (2026-08-01 Linux inventory)

| Field | Value |
|-------|--------|
| **Model / BIOS** | ASUS ROG **G752VT**, BIOS **G752VT.307** |
| **CPU / RAM / GPU** | **i7-6700HQ**, **16 GiB**, GTX **970M** |
| **xHCI** | PCI **`0000:00:14.0`** · **`8086:a12f`** Intel 100 Series USB 3.0 |
| **IOMMU** | Devices **translated** under Linux (VT-d) |
| **Linux inventory** | **PASS** — `make linux-hwtest-img`, reports on **`GJ-LNX-ESP`** |
| **Freestanding xHCI/net** | **Soft scaffold only** (high-water ~stage 15 GET_CONFIG; rtl8168 lab counters) — **not** T1 product close |
| **Product direction** | **Linux ABI + virtio T0** first; real-HW class drivers via userspace/DDI later (no GPL ship) |
| **bar3** | **OPEN** |

### USB map (Linux)

| Hub | Ports | Notable devices |
|-----|-------|-----------------|
| **Bus 1 HS** (16p) | 1-4 webcam · 1-8 ASUS HID · 1-9 Intel BT · 1-10 keyboard | Internal HS; not MSC stick log |
| **Bus 2 SS** (10p) | **SS port 5** stick **`13fe:6400`**, **usb-storage BOT** | Prefer this SS path for freestanding MSC media |

Full map + operator dual-stick steps: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md).

### Freestanding vs Linux (honesty)

| Claim | Allowed? |
|-------|----------|
| “Linux inventory PASS on G752VT; full lsusb/lspci/IOMMU map” | **Yes** — Linux helper only |
| “Freestanding stuck stage 11 control after address on `a12f`” | **Yes** — known freestanding gap |
| “Linux PASS ⇒ freestanding xHCI / MSC product done” | **No** |
| “bar3 closed / Steam client on DUT” | **No** — **bar3 OPEN** |

### Next engineering targets (G752VT / xHCI)

1. **SS control on `8086:a12f`** — freestanding control after SET_ADDRESS (stage 11).  
2. **IOMMU** — DMA correct under **translated** VT-d (QEMU soft VT-d ≠ this laptop).  
3. **Port priority** — prefer SuperSpeed root (inventory stick @ **SS port 5**); then BOT MSC stick log.  
4. **bar3** stays **OPEN** until client + Top-50 evidence on GreenJade.

## Status — 2026-08-01

**G752VT Linux inventory PASS** (map above); freestanding still **stage 11** on xHCI control.  
Operator packs: [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md), [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).  
Artifacts (local `build/`, not in git): `greenjade-hwtest.img` (ESP+GJ-PERSIST, Steam media READY), `linux-hwtest.img` (**GJ-LNX-ESP** reports), `greenjade-live.iso`, Multiboot smoke **PASS**. **bar3 OPEN.**

Parallel waves **soft shipped** (kernel / media / continuum graph only; tip **makefile_max=26800**). **Open bars stay open.**

**Soft stamp (Wave 9, historical):** continuum **makefile_max=15100** soft only (`graph_batch15100.c` parent wire / CREATE-ONLY decades when present on tree). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 12, historical):** continuum **makefile_max=15400** soft only (`graph_batch15400.c` parent wire / CREATE-ONLY decades when present on tree). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. Soft probes and continuum growth remain **≠ full HCL product**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 14, historical):** continuum high-water **advancing toward 15600** soft only (parent wires / CREATE-ONLY; soft deepen on non-w13 surfaces). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. Soft probes and continuum growth remain **≠ full HCL product**. **Soft ≠ product complete.** Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 16, historical):** continuum high-water **advancing toward 15800** soft only (parent wires / CREATE-ONLY). Honest scan may still have reported **makefile_max=15700** until parent wires. Soft probes and continuum growth remain **≠ full HCL product**. **Soft ≠ product complete.** Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 111):** continuum high-water **advancing toward 25300** soft only (parent wires / CREATE-ONLY). Honest scan may still report **makefile_max=25200** until parent wires — **do not hardcode false 25300**. Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. Soft probes and continuum growth remain **≠ full HCL product**. **Soft ≠ product complete.** Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

### Soft probes ≠ full HCL product (Wave 9 honesty — retained)

HCL soft probes and continuum growth are **not** a product HCL certification:

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| PCI class / CAP / GHC / status **soft probes** (NVMe, AHCI, USB HC, PS/2, virtio-scsi INQUIRY, …) | Bring-up greppable path; optional map via `vmm_map_device_uc` when present | Documented T1+ **product** install matrix; full driver stack; certified real-hw support |
| T0 virtio + OVMF CI (`make smoke`) | Virtual HCL gate | Real-DUT HCL product close |
| Continuum **makefile_max=15100** (Wave 9 high-water, historical) | CREATE-ONLY soft graph high-water only | Runtime ABI product; Steam client; Deck Top 50; bar3 |
| Media `STATUS=READY` / `steam-bar3-check` | Host/media bootstrap inventory | Client launch; title try; HCL product bar |
| Kernel smokes (io_uring min, HDA multi-stream, 768G soak, aarch64 M0, VT-d soft, …) | Surface soft ship | Full HCL tier product; bar3; game I/O/audio |

**Hard rule (Wave 9):** never promote soft probes, continuum **15100**, or media READY to “HCL product complete,” T1+ certified, or **bar3 closed**. Open bars below stay open.

### Soft probes ≠ full HCL product (Wave 12 honesty)

**Additive only.** Wave 9 table and hard rule above stay historical ledger. This section is Wave 12 honesty: continuum high-water **makefile_max=15400** soft only. Soft probes ≠ full HCL product. **bar3 remains OPEN.**

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| PCI class / CAP / GHC / status **soft probes** (NVMe, AHCI, USB HC, PS/2, virtio-scsi INQUIRY, …) | Bring-up greppable path; optional map via `vmm_map_device_uc` when present | Documented T1+ **product** install matrix; full driver stack; certified real-hw support |
| T0 virtio + OVMF CI (`make smoke`) | Virtual HCL gate | Real-DUT HCL product close |
| Continuum **makefile_max=15400** | CREATE-ONLY soft graph high-water only (`graph_batch15400.c` when present) | Runtime ABI product; Steam client; Deck Top 50; bar3; **full HCL product** |
| Media `STATUS=READY` / `steam-bar3-check` | Host/media bootstrap inventory | Client launch; title try; HCL product bar |
| Kernel smokes (io_uring min, HDA multi-stream, 768G soak, aarch64 M0, VT-d soft, …) | Surface soft ship | Full HCL tier product; bar3; game I/O/audio |

**Hard rule (Wave 12):** never promote soft probes, continuum **makefile_max=15400**, or media READY to “HCL product complete,” T1+ certified, or **bar3 closed**. Soft continuum ≠ full HCL product. Soft probes ≠ full HCL product. Open bars below stay open.

### Soft probes ≠ full HCL product (Wave 14 honesty — historical)

**Additive only.** Wave 9/12 tables and hard rules above stay historical ledger. Wave 14 honesty: continuum high-water **advancing toward 15600** soft only (parent wires). Soft deepen on non-w13 surfaces. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Product lamps remain **0**. **bar3 remains OPEN.**

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 15600** | CREATE-ONLY soft graph high-water advance (parent wires; tree may lag prior decade) | Runtime ABI product; Steam client; Deck Top 50; bar3; **full HCL product** |
| Soft deepen (non-w13) | Honesty / bring-up surface deepen | Product complete; product lamps lit |
| Media `STATUS=READY` / `steam-bar3-check` | Host/media bootstrap inventory | Client launch; title try; HCL product bar |
| Product lamps **0** | Soft score / bar3-ready stubs remain zero | Product-score claim |

**Hard rule (Wave 14, historical):** never promote soft probes, continuum advancing toward **15600**, or media READY to “HCL product complete,” T1+ certified, product lamps > 0, or **bar3 closed**. Soft continuum ≠ full HCL product. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Open bars below stay open.

### Soft probes ≠ full HCL product (Wave 111 honesty)

**Additive only.** Wave 9/12/14/16 tables and hard rules above stay historical ledger. This section is Wave 111 honesty: continuum high-water **advancing toward 25300** soft only (parent wires). Honest scan may still report **makefile_max=25200** — **do not hardcode false 25300**. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Product lamps remain **0**. **bar3 remains OPEN.**

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 25300** | CREATE-ONLY soft graph high-water advance (parent wires; scan may lag at prior tip) | Runtime ABI product; Steam client; Deck Top 50; bar3; **full HCL product** |
| Honest scan **makefile_max=25200** (prior tip when greppable) | Makefile wire truth until parent wires 25300 basenames | Claim that **25300** is greppable when scan still lags at prior tip |
| Soft deepen | Honesty / bring-up surface deepen | Product complete; product lamps lit |
| Media `STATUS=READY` / `steam-bar3-check` | Host/media bootstrap inventory | Client launch; title try; HCL product bar |
| Product lamps **0** | Soft score / bar3-ready stubs remain zero | Product-score claim |

**Hard rule (Wave 111):** never promote soft probes, continuum advancing toward **25300**, or media READY to “HCL product complete,” T1+ certified, product lamps > 0, or **bar3 closed**. Soft continuum ≠ full HCL product. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Open bars below stay open.

### Explicit non-claims (Wave 12/14 historical · Wave 111 restates)

| Claim | Allowed? |
|-------|----------|
| “Continuum high-water advancing toward **25300** (CREATE-ONLY parent wire)” | **Yes** — soft only |
| “Honest scan may still be `makefile_max=25200` until parent wires 25300” | **Yes** — scan is source of truth |
| “Media `STATUS=READY` / steam-bar3-check inventory READY” | **Yes** — media bootstrap only |
| “Soft probes / T0 CI greppable” | **Yes** — probe / virtual gate only |
| “Full HCL product / T1+ certified real-hw matrix closed” | **No** |
| “Product lamps lit / product complete from soft continuum” | **No** — lamps remain **0** |
| “bar3 closed / Steam client on DUT / Deck Top 50 leave NOT-TRIED” | **No** |

### Soft shipped (not product bars)

- **Live daemons:** `sessiond` / `netstackd` / `sshd` / `storaged` / `vfsd` / shell / `scsi_mid` / `hda_client`
- Residual **#UD** closed
- Continuum high-water **advancing toward 25300** (soft graph only — not Steam client; parent-wire CREATE-ONLY; honest scan may still be **25200**; **not** full HCL product; Wave 14 historical target was **15600**)
- **io_uring** soft surface shipped (`io_uring_min.c`; `linux: io_uring min rings PASS`, `mmap PASS`, `SQE I/O PASS`) — vfs_ram/kernel smoke, **not** game I/O
- **GJ_MEM=768G** hierarchical `soak_tib` **PASS** on host class **≥1TiB** — product bar still **≥ 1 TiB** full path when host allows
- **aarch64 M0 scaffold** smoke **PASS** (`make aarch64` / `aarch64-smoke`; shared C + PSCI smokes) — separate arch tree; not Deck/x86 product kernel
- **HDA multi-stream** kernel **PASS** — not Steam/game audio
- Media `STATUS=READY` / host soft check READY = bootstrap tree on media only — **not** client run
- T1 soft probes (NVMe CAP, AHCI GHC, USB HC class, PS/2 status) = **probe only** — **not** T1 product HCL close
- Product lamps remain **0** — soft continuum ≠ product complete

### Open bars (do not claim done)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` (**OPEN**; soft probes / continuum / **Linux inventory PASS** do **not** close)
- Deck Top 50 title runs — all **NOT-TRIED** (no titles tried; claim **targeting only**)
- **Freestanding xHCI on G752VT `8086:a12f`** — stage 11 control after address (**open**; Linux inventory ≠ freestanding product)
- **IOMMU translated DMA on real DUT** — open (Linux shows translated; freestanding product path incomplete)
- **Port priority / SS MSC stick log** — open (prefer inventory SS port 5 path)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Steam/game audio / title matrix fill — **open** (blocked on bar3 client run)
- **Full HCL product** (documented T1+ real-hw install matrix beyond soft probes + inventory helper) — **open** (soft probes ≠ full HCL product; Linux map ≠ freestanding close)

## Related docs

- [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) — platform rules  
- [GLIBC_COMPAT.md](GLIBC_COMPAT.md) — libcgj / host glibc policy  
- [TODO.md](TODO.md) — coding checklist  
- [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md) — operator pack (freestanding + Linux sticks)  
- [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) — G752VT USB/xHCI map · freestanding vs Linux  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 honesty / READY vs NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — hwtest Steam media options 2+3  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — product matrix  
