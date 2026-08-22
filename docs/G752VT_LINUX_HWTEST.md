# ASUS G752VT — Linux inventory vs freestanding xHCI (2026-08-01)

| | |
|--|--|
| **DUT** | ASUS ROG **G752VT** (RH71-class) |
| **BIOS** | **G752VT.307** |
| **CPU / RAM / GPU** | i7-6700HQ · **16 GiB** · GTX **970M** |
| **Source** | Linux auto-test stick (`make linux-hwtest-img` → label **`GJ-LNX-ESP`**) |
| **bar3** | **OPEN** (inventory ≠ Steam client / Top-50) |

This note is **ground truth from Linux drivers**, not a GreenJade freestanding product PASS.
Use it to debug freestanding xHCI stages and port choice. Full HCL tiers: [HCL.md](HCL.md).
Operator pack: [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md).

---

## Honesty split

| Path | Result (2026-08-01) | What it means |
|------|---------------------|---------------|
| **Linux inventory** (`linux-hwtest.img`) | **PASS** — full USB/PCI/storage report written to stick | Host Linux (`xhci_hcd`, `usb-storage`, IOMMU) sees the machine correctly |
| **GreenJade freestanding** (`greenjade-hwtest.img`) | **Stuck stage 11** — control after address | Freestanding xHCI control path not yet product-complete on this HC |
| **bar3 / Deck Top 50** | **OPEN** | Linux inventory and freestanding stick log **do not** close Steam client |

**Hard rule:** `RESULT: PASS` on `GJ-LNX-ESP` = probe scripts finished under Linux. It is **not** freestanding MSC/HID product done, **not** T1 HCL product close, **not** bar3.

---

## xHCI host (from Linux)

| Field | Value |
|-------|--------|
| **PCI** | `0000:00:14.0` |
| **ID** | **`8086:a12f`** — Intel 100 Series / C230 Series USB 3.0 xHCI |
| **IOMMU** | Devices **translated** under Linux (VT-d active) — freestanding must either honor IOMMU or keep DMA identity/compatible with firmware tables |
| **Class** | USB xHCI (PCI class `0C:03`) |

G752VT/VL family: **USB 3.0 ports only** (no pure USB 2.0 jack). Stick log/MSC path is **SuperSpeed-capable** on the physical ports; Linux still enumerates high-speed devices on the HS root hub.

---

## USB topology map (Linux inventory)

### Bus 1 — High-Speed root hub (16 ports)

| Port | Device (typical report) | Notes for freestanding |
|------|-------------------------|------------------------|
| **1-4** | Webcam | Internal HS; ignore for stick log |
| **1-8** | ASUS HID | Laptop control / HID path |
| **1-9** | Intel Bluetooth | Internal |
| **1-10** | Keyboard | Internal HID — soft input later; **≠** MSC stick |

Other HS ports: empty or unused on this SKU unless external HS-only gadgets are attached.

### Bus 2 — SuperSpeed root hub (10 ports)

| Port | Device (typical report) | Notes for freestanding |
|------|-------------------------|------------------------|
| **SS port 5** | USB stick | **`13fe:6400`**, **`usb-storage`**, **BOT** (Bulk-Only Transport) |
| Others | empty | Prefer this SS port class for MSC log media |

**Operator takeaway:** for freestanding stick logging, prefer a rear/side **USB3** jack that Linux maps to **Bus2 SS port 5** when the known stick is inserted. Port priority matters — stage 11 control after address is freestanding HC bring-up, not “wrong stick VID:PID only.”

---

## Freestanding vs Linux (stage focus)

```
Linux:   PCI a12f → xhci_hcd → HS + SS roots → stick @ SS:5 (13fe:6400 BOT)  → inventory PASS
GJ free: soft xhci_msc scaffold (high-water ~stage 15 GET_CONFIG, 2026-08) → not product T1
GJ product direction: Linux ABI (Option C) + collect/stage needed host modules
  (`scripts/collect-linux-drivers.sh` → `GJ-PERSIST/linux-drivers/`) + DDI/host path;
  freestanding class thrash is lab only; module load on GJ still OPEN;
  GPL **source** not imported into knano (operator-collected .ko on media for ABI work)
```

| Stage theme | Linux | Freestanding (soft scaffold) | Product direction |
|-------------|-------|------------------------------|-------------------|
| HC present `8086:a12f` | Yes | Soft PCI probe | Thin bus + caps |
| IOMMU translated DMA | Linux configures VT-d | Soft identity / open honesty | DMA **windows** (P-DMA) |
| Enumerate + address | Full stack | Reached (past address) | Userspace `usb_hostd` later |
| Control / config | Works | High-water **~stage 15** GET_CONFIG | Clean-room host or out-of-tree dual-license |
| BOT MSC / stick log | `usb-storage` | Soft only — not T1 bar | Store door + host |
| Net on laptop NIC | r8169 etc. | Soft `rtl8168` counters | **virtio-net T0**; real NIC via DDI later |
| Port map | Stick @ SS port 5 | Prefer SS; soft panel `pN/sN` | Inventory remains oracle |

---

## Next engineering targets (ordered) — ABI-first pivot

**Primary (product):**

1. **Linux ABI Option C** — hot/cold completeness for apps (`sshd` / `netstackd` / shell) on **virtio T0**.  
2. **Cold personality → userspace door server** (G-PERS) — personality holds **no** IOMMU/IRQ.  
3. **DDI**: PCI/IRQ/DMA caps for userspace driver hosts — clean-room or **out-of-tree dual MIT/Apache** (never GPL in tree; **no** Linux `.ko` product AC).  
4. **bar3 remains OPEN** until Steam **client** + matrix evidence.

**Secondary (soft only — do not thrash as product bar):**

5. Freestanding `xhci_msc` / `rtl8168` may improve opportunistically; stages are **scaffold**, not T1 close.  
6. This inventory map remains **ground truth** for G752 topology (SS stick port, IOMMU translated).

---

## Operator: two sticks (do not confuse)

### Stick A — GreenJade hwtest (UDX Dual DoD; fly **v0.1.184** packed, not host-probed)

```sh
# Lab host
ls -lh build/greenjade-hwtest.img
sudo make install-hwtest-usb DEV=/dev/sdX   # wipes device; labels GREENJADE + GJ-PERSIST
```

| | |
|--|--|
| Labels | ESP **`GREENJADE`** · persist **`GJ-PERSIST`** |
| DUT | Secure Boot **off** → UEFI USB → `BOOTX64.EFI` |
| Collect | Mount ESP: `EFI/GREENJADE/BOOT.LOG`, `KLOG.TXT`; mount **`GJ-PERSIST`**: `logs/`, `steam/STATUS` |
| Expect | Panel `STATUS (static) v0.1.184` packed, not host-probed; Dual DoD A **OPEN** until host USB path; Dual DoD B **OPEN** until interactive SSH login; GOP isolate ON; xhci RS-off (never `USBCMD.RS=1`); **0.1.183** host FAIL historical (`Sending command: true` **PASS**; exec 124) |
| **bar3** | Media READY ≠ client — **OPEN** |

### Stick B — Linux inventory (this map’s source)

```sh
# Lab host
sudo make linux-hwtest-img                 # → build/linux-hwtest.img
sudo make install-linux-hwtest DEV=/dev/sdX
# DUT: UEFI boot → probes run → poweroff (leave stick in)
sudo mkdir -p /mnt/esp
sudo mount -L GJ-LNX-ESP /mnt/esp
cat /mnt/esp/GJ-HWTEST-RESULT.txt
cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt
sudo umount /mnt/esp
```

| | |
|--|--|
| Labels | **`GJ-LNX-ESP`** (reports) · **`GJ-LNX-ROOT`** (Alpine) |
| Expect | Full loop, **poweroff**, report on ESP — **Linux PASS inventory** (2026-08-01) |
| Use for | PCI ID, port map, IOMMU, dmesg vs freestanding stage bars |
| **Not** | Freestanding product PASS / bar3 |

Details of image build: `scripts/linux-hwtest/README.md` (scripts tree; do not treat as freestanding HCL close).

---

## Related

- [HCL.md](HCL.md) — tiers, real-hw checklist, open bars  
- [HWTEST_TOMORROW.md](HWTEST_TOMORROW.md) — full operator pack  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 **OPEN** ceiling  
