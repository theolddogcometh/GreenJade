# Known DUT hardware maps (Linux hwtest)

Operator reference from real `GJ-HWTEST/reports/LATEST.txt` runs.
These are **Linux inventory facts**, not freestanding GreenJade product claims.

---

## ASUS ROG G752VT (2026-08-01 loop PASS)

| Field | Value |
|-------|--------|
| **DMI product** | `G752VT` (ASUSTeK) |
| **BIOS** | American Megatrends `G752VT.307` (2019-04-26) |
| **CPU** | Intel Core i7-6700HQ |
| **RAM observed** | ~15.6 GiB |
| **GPU (PCI)** | NVIDIA GM204M GTX 970M `[10de:1618]` |
| **SATA** | Intel RAID-mode AHCI `[8086:2822]` @ `00:17.0` (HDD seen as `sda`) |

### xHCI controller

| Field | Value |
|-------|--------|
| **PCI BDF** | `0000:00:14.0` |
| **PCI ID** | Intel 100 Series / C230 USB 3.0 xHCI **`[8086:a12f]`** (rev 31) |
| **Class** | `0x0c0330` |
| **Driver** | `xhci_hcd` via `xhci_pci` |
| **Root hubs** | Bus **1** = High-Speed (480 Mbit/s); Bus **2** = SuperSpeed (5000 Mbit/s) |
| **Ports** | HS root hub 16 ports; SS root hub 10 ports (same physical controller) |

**Chassis note:** G752VT/VL family is **USB 3.0 only** on external ports — there is no pure EHCI/OHCI “USB2-only” port fallback. Stick mass-storage for freestanding work must take the **SuperSpeed** path (Linux bus 2).

### External USB stick (Linux hwtest media) on that run

| Field | Value |
|-------|--------|
| **VID:PID** | **`13fe:6400`** (“USB DISK 3.0”) |
| **Topology** | **bus2-port5** → sysfs `2-5` / dmesg `usb 2-5` |
| **Speed** | SuperSpeed **5000M** (`usb-storage`) |
| **Block** | `sdb` (ESP `GJ-LNX-ESP` + root `GJ-LNX-ROOT`) |
| **Serial (example)** | `B2EDE381310E9297` (per-stick; do not hardcode) |

Greppable evidence lines from report:

```text
xhci_bdf=0000:00:14.0
xhci_pci_id=8086:a12f
stick_vid_pid=13fe:6400
stick_bus_port=2-5
stick_speed=SuperSpeed
```

### Internal USB devices (same run; for noise filtering)

| Bus-port | Speed | ID | Device |
|----------|-------|-----|--------|
| 1-4 | HS 480M | `04f2:b414` | Chicony UVC webcam |
| 1-8 | FS 12M | `0b05:1819` | ASUS HID |
| 1-9 | FS 12M | `8087:0a2a` | Intel Bluetooth |
| 1-10 | FS 12M | `0b05:1822` | Sunrex/JME keyboard |

When comparing freestanding stage-11 xHCI MSC, ignore 1-* internal noise; the **stick is SS on 2-5** on this chassis/port choice.

### Operator tips (G752VT)

1. Prefer a **rear/side USB3** port; Linux should show the stick under **Bus 002** at **5000M**.
2. If stick appears only under Bus 001 at 480M, the link negotiated HS-only — try another port / cable / stick; freestanding SS MSC may also fail there.
3. Reports land on **`GJ-LNX-ESP`**: `GJ-HWTEST/reports/LATEST.txt` and `KEY-FACTS.txt`.
4. Cross-check: `lspci -nn \| grep -i xhci` → expect `8086:a12f` @ `00:14.0`.
