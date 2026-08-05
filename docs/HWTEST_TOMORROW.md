# Hardware test — operator pack (2026-08-01)

**Goal:** boot GreenJade freestanding **and/or** Linux inventory on real DUT; capture evidence; do not over-claim.  
**Product tip:** `makefile_max=26800` (Wave 126). **bar3 remains OPEN** (Steam client / Top-50 still not claimed).  
**G752VT Linux inventory (2026-08-01):** **PASS** on `GJ-LNX-ESP`. Freestanding in-kernel xHCI/rtl = **soft scaffold** (high-water ~stage 15); **product priority = Linux ABI + virtio T0**, not freestanding class-driver thrash. Map: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

## DUT #1 — ASUS ROG G752VT-RH71

| Field | Value |
|-------|--------|
| **Model** | ASUS ROG **G752VT-RH71** (17.3″ gaming laptop, ~2015–2016) |
| **BIOS** | **G752VT.307** (confirmed on DUT run) |
| **CPU** | Intel **Core i7-6700HQ** (Skylake, 4C/8T) — **x86_64** OK |
| **RAM** | **16 GiB** DDR4 (upgradeable to 64 GiB) |
| **GPU** | NVIDIA **GeForce GTX 970M** 3 GiB + Intel HD 530 (Optimus-era) |
| **Display** | 17.3″ FHD — firmware **GOP** only for freestanding console |
| **Storage** | M.2 NVMe + 2.5″ SATA common — HCL **T1 soft probes** only, **not** full product drivers |
| **USB HC** | PCI **`0000:00:14.0`** · **`8086:a12f`** Intel 100 Series USB 3.0 xHCI |
| **USB ports** | **USB 3.0 only** (no pure USB 2.0 jacks) — SuperSpeed path required for stick log |
| **IOMMU** | **Translated** under Linux (VT-d) — freestanding DMA must match |
| **Net** | Realtek/Intel wired + Wi‑Fi — **not** virtio; no LAN product path on first freestanding boot |
| **Audio** | Realtek HDA — freestanding multi-stream smoke ≠ laptop codec product |
| **Firmware** | **UEFI** (Aptio); Secure Boot present — **disable** for GreenJade / Linux sticks |

### USB topology (Linux inventory PASS — 2026-08-01)

| Hub | Layout | Devices of interest |
|-----|--------|---------------------|
| **Bus 1 HS** 16 ports | Internal HS tree | **1-4** webcam · **1-8** ASUS HID · **1-9** Intel BT · **1-10** keyboard |
| **Bus 2 SS** 10 ports | SuperSpeed tree | Stick on **SS port 5**: **`13fe:6400`**, **usb-storage BOT** |

Prefer a physical USB3 jack that enumerates as **SS port 5** for freestanding MSC log media. Full note: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

### Firmware keys (ASUS ROG G752)

| Action | Key (typical) |
|--------|----------------|
| Boot menu (one-shot) | **Esc** (or **F8** on some ROG SKUs) at power-on |
| Setup (BIOS/UEFI) | **F2** at power-on |
| Boot override | Setup → **Boot** → USB / UEFI: USB name |

**Required settings before USB boot:**

1. **Secure Boot → Disabled** (neither GreenJade nor the Alpine inventory image is MS-signed for this path).
2. Prefer **UEFI** boot of the stick (not CSM-only “Legacy USB” unless Multiboot ISO needs it).
3. If present: **Fast Boot → Disabled** (so Esc/F2 work reliably).
4. **Launch CSM** / Legacy: leave **Off** for pure UEFI USB; if freestanding UEFI stalls, try **live ISO** with CSM **Enabled** for Multiboot hybrid.
5. Discrete GPU: leave default; first bring-up only needs **UEFI GOP** (do not expect GTX 970M 3D).

### Freestanding vs Linux (honest)

| Surface | Status on G752VT (2026-08-01) |
|---------|-------------------------------|
| **Linux inventory stick** | **PASS** — probes write report to **`GJ-LNX-ESP`**, machine poweroffs |
| **Freestanding xHCI/net** | **Soft scaffold** (~stage 15 GET_CONFIG; rtl8168 lab) — **not** T1 product bar |
| **Product direction** | **Linux ABI + virtio T0** + later userspace/DDI drivers (no GPL ship) |
| **IOMMU** | Linux: **translated**; product DMA = **window caps** (P-DMA) |
| **UEFI / M0** | Freestanding pack ready; confirm on panel / ESP logs per run |
| **Serial** | **No DB9** on this chassis — use panel bars + remount stick logs |
| **Steam / bar3** | Media **READY** on freestanding stick only — **bar3 OPEN**; Linux inventory **≠** bar3 |

**Hard rule:** Linux `RESULT: PASS` and freestanding soft greps are **not** Steam client, **not** Deck Top 50, **not** full T1 product HCL close.

### Next engineering targets (do not claim done)

1. **Linux ABI Option C** — apps on virtio (sshd/netstackd/shell); cold personality to userspace.  
2. **DDI** — PCI/IRQ/DMA caps for userspace / out-of-tree dual-license drivers.  
3. **G752 inventory** — keep Linux stick as topology oracle (SS port 5, a12f, translated IOMMU).  
4. **bar3** — remains **OPEN** until client + matrix evidence.  
5. Freestanding xHCI/rtl — optional soft only; stop treating panel stages as product DoD.

---

## Two sticks — clear operator paths

**Use separate sticks or re-image between modes.** Labels differ; do not expect `GJ-PERSIST` on the Linux image or `GJ-LNX-ESP` on freestanding.

### Stick A — GreenJade freestanding (`GREENJADE` + `GJ-PERSIST`)

```sh
cd /home/jay/Documents/knano   # or your clone
ls -lh build/greenjade-hwtest.img
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL   # pick stick; NOT sda/sdb RAID
# DESTROYS the entire device:
sudo make install-hwtest-usb DEV=/dev/sdX
```

**On DUT:**

1. AC power preferred; insert stick (USB3 jack).  
2. Power on → spam **Esc** → **UEFI: … USB**.  
3. Watch panel: `GJ-EFI` / progress bars; keyboard may be soft-only.  
4. Expect freestanding **stage 11** hang on xHCI control until that target lands — still collect whatever logs exist.

**After run — collect freestanding logs:**

```sh
sudo umount /run/media/$USER/* 2>/dev/null; true
sudo mkdir -p /mnt/gj-esp /mnt/gj-persist
sudo mount -L GREENJADE /mnt/gj-esp    # or /dev/sdX1
cat /mnt/gj-esp/EFI/GREENJADE/BOOT.LOG   # UEFI loader tee (pre-EBS)
cat /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT   # kernel xHCI MSC stick log (if path up)
sudo umount /mnt/gj-esp
sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/logs /mnt/gj-persist/steam
cat /mnt/gj-persist/steam/STATUS         # READY = media only — bar3 OPEN
sudo umount /mnt/gj-persist
```

Optional lab-host SSH enable (serial bridge host, not GreenJade LAN on this laptop):

```sh
sudo make hwtest-ssh-setup
```

### Stick B — Linux inventory (`GJ-LNX-ESP` + `GJ-LNX-ROOT`)

When freestanding is stuck (stage 11) or you need a fresh PCI/USB map:

```sh
cd /home/jay/Documents/knano
sudo make linux-hwtest-img                 # build/linux-hwtest.img (~2 GiB)
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL
sudo make install-linux-hwtest DEV=/dev/sdX
```

**On DUT:**

1. Secure Boot **off** → UEFI boot stick.  
2. Console: `GreenJade Linux HWTEST — starting full loop` (or similar).  
3. Wait until machine **powers off** (leave stick inserted).  

**After poweroff — collect Linux report:**

```sh
sudo mkdir -p /mnt/esp
sudo mount -L GJ-LNX-ESP /mnt/esp
ls /mnt/esp/GJ-HWTEST/reports/
cat /mnt/esp/GJ-HWTEST-RESULT.txt
cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt
# Note: xHCI 8086:a12f, Bus1 HS map, Bus2 SS port 5 stick 13fe:6400 BOT, IOMMU translated
sudo umount /mnt/esp
```

This is **Linux dmesg/lspci/lsusb evidence**, not freestanding product PASS. See `scripts/linux-hwtest/README.md`.

### Fallback — Multiboot live ISO (if freestanding UEFI stalls)

```sh
sudo dd if=build/greenjade-live.iso of=/dev/sdX bs=4M status=progress conv=fsync
# or QEMU check first:
./scripts/run-live-iso.sh
```

---

## What is ready on this lab host

| Artifact | Path | Notes |
|----------|------|--------|
| Product kernel | `build/greenjade.elf` (~8.5 MiB) | Multiboot **smoke PASS** (M0 OK, UD=0, quick-keys) |
| UEFI loader | `build/GreenJade.efi` (~55 KiB) | Loads `KERNEL.ELF`; OVMF may #UD after handoff — real DUT TBD |
| **Freestanding HW USB image** | `build/greenjade-hwtest.img` (**2.0 GiB**) | GPT: **`GREENJADE`** + **`GJ-PERSIST`** (logs/ssh/**steam READY**) |
| **Linux inventory image** | `build/linux-hwtest.img` | GPT: **`GJ-LNX-ESP`** + **`GJ-LNX-ROOT`** — G752VT loop **works** |
| Hybrid live ISO | `build/greenjade-live.iso` (~92 MiB) | Multiboot2 + EFI fallback |
| ESP stage tree | `build/esp/` | `BOOTX64.EFI` + `KERNEL.ELF` + user ELFs |
| SSH keys | `build/hwtest-keys/id_ed25519` | Freestanding GJ-PERSIST / lab host |
| Steam media | `build/steam-stage/steam/STATUS` = **READY** | Media bootstrap only — **≠ client launch** |

**Preflight (this host, 2026-08-01):**

- Multiboot: `timeout 180 scripts/run-qemu.sh build/greenjade.elf` → **gj-quick-keys PASS**.
- Steam: `./scripts/steam-bar3-check.sh` → **READY** (**bar3 still OPEN**).
- Linux hwtest loop: `make linux-hwtest-img` → DUT reports on **`GJ-LNX-ESP`** (**PASS inventory** on G752VT).
- Freestanding on DUT: **stage 11** control after address still the freestanding gap.

**No USB stick is plugged into this lab box by default.** Plug the stick before write; never target RAID `sda`/`sdb`.

---

## Optional serial (rare on this laptop)

G752VT has **no convenient COM**. If you later have USB-TTL on another DUT:

```sh
mkdir -p /tmp/gj-hw-logs
picocom -b 115200 /dev/ttyUSB0 | tee /tmp/gj-hw-logs/serial-$(date -u +%Y%m%dT%H%MZ).txt
./scripts/gj-product-summary.sh /tmp/gj-hw-logs/serial-….txt
./scripts/gj-quick-keys.sh /tmp/gj-hw-logs/serial-….txt
```

### Expect (freestanding greps, when path runs)

- UEFI: `GJ-EFI` then `KERNEL.ELF loaded` / `M0 OK`
- Multiboot ISO: `source=MULTIBOOT2` then `M0 OK`
- Product: `sshd: live spawn PASS`, `scsi_mid: live spawn PASS`, `shell: live spawn PASS` (virtio lab; laptop NIC may miss)
- Soft: `TRAP #UD` count **0** (or note any #UD)
- xHCI: stage markers through address; **stage 11 control** is the known freestanding fail point on this HC

---

## Steam honesty (do not over-claim)

| Media / result | Meaning |
|----------------|---------|
| `steam/STATUS=READY` | Bootstrap tree on freestanding stick |
| Linux inventory **PASS** | Host Linux saw hardware — **≠** freestanding MSC/HID product |
| **bar3 OPEN** | Client not launched; Deck Top 50 still **NOT-TRIED × 50** |
| Product lamps | **0** |

Do **not** mark matrix rows PASS from media READY or Linux inventory alone. See [STEAM_HWTEST.md](STEAM_HWTEST.md), [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md).

## Success criteria

**Minimum — freestanding “DUT boot attempted”:**

- [ ] Stick boots firmware on **G752VT** (UEFI USB or Multiboot hybrid)
- [ ] Evidence of loader/kernel progress (panel STATUS capture and/or ESP `BOOT.LOG` / `KLOG.TXT`)
- [ ] Note hang stage if any (**stage 11** expected until `a12f` control lands)
- [ ] Record: model **G752VT**, BIOS **G752VT.307** (or current F2 → Main), RAM, boot path, USB port used

**Minimum — Linux inventory (already achieved 2026-08-01; re-run after HW changes):**

- [ ] `linux-hwtest-img` written; DUT poweroffs after loop
- [ ] `GJ-LNX-ESP` has `GJ-HWTEST-RESULT.txt` + `reports/LATEST.txt`
- [ ] Report still shows **`8086:a12f`**, SS stick port, IOMMU translated (or document deltas)

Stretch (product still open):

- [ ] Freestanding past stage 11 control on **`a12f`**
- [ ] Soft greps NVMe / AHCI / USB HC (≠ product storage)
- [ ] BOT MSC stick log under freestanding
- [ ] Steam tree on GJ-PERSIST (media only — **not** bar3)

## Rebuild commands

```sh
make greenjade.efi
make build/greenjade.elf
make userland
make steam-stage          # STATUS=READY if build/steam-tree present
make hwtest-img           # → build/greenjade-hwtest.img
make live-iso             # → build/greenjade-live.iso
sudo make linux-hwtest-img
./scripts/steam-bar3-check.sh
timeout 180 scripts/run-qemu.sh build/greenjade.elf
```

## Honesty / open bars

- Soft continuum high-water **makefile_max=26800** ≠ runtime product complete  
- Soft deepen **retgradientangle** / **retblendangle** ≠ bar3  
- Linux inventory PASS ≠ freestanding xHCI product  
- Real-DUT freestanding targets: **SS control a12f**, **IOMMU**, **port priority**  
- **bar3 OPEN** — Steam client / Top-50 still not claimed  
- Priorities: Security → Performance → Portability → Readability  

## Related docs

- [HCL.md](HCL.md) — tiers + install checklist + G752VT map summary  
- [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) — USB/xHCI map · freestanding vs Linux · dual sticks  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — Steam option 2/3 media  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 ceiling  
