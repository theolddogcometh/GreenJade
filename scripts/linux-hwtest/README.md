# GreenJade Linux hardware-test OS

A **complete automatic loop** for DUT bring-up (e.g. ASUS G752VT):

1. **Boot** this USB (UEFI, Secure Boot off)  
2. **Run** hardware probe scripts (CPU, PCI, USB/xHCI, storage, dmesg, GreenJade media)  
3. **Write** a report to the stick ESP (`GJ-LNX-ESP`)  
4. **Power off** the machine  

This is a **Linux inventory helper**, not GreenJade freestanding product firmware.  
It does **not** claim bar3 / Deck Top 50 / freestanding xHCI MSC.

## Layout

| Partition | Label | FS | Role |
|-----------|--------|-----|------|
| p1 ESP | `GJ-LNX-ESP` | FAT32 | GRUB EFI, kernel, initramfs, **reports/** |
| p2 root | `GJ-LNX-ROOT` | ext4 | Alpine Linux + `/opt/gj-hwtest` |

## Build (lab host)

Needs root, network (first time), and usual disk tools (`sgdisk`, `mkfs.vfat`, `mkfs.ext4`, `curl`).

```sh
cd /path/to/knano
sudo ./scripts/linux-hwtest/build-img.sh
# → build/linux-hwtest.img  (~2 GiB)
```

Optional:

```sh
sudo GJ_LINUX_HWTEST_MB=4096 GJ_ALPINE_VERSION=3.20 ./scripts/linux-hwtest/build-img.sh
```

## Write to USB

```sh
lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINT   # pick the stick carefully
sudo umount /run/media/$USER/* 2>/dev/null || true
sudo ./scripts/linux-hwtest/install-usb.sh /dev/sdX
```

## Labels (do not confuse with product media)

| Label | FS | This image |
|-------|-----|------------|
| **`GJ-LNX-ESP`** | FAT32 | Linux hwtest ESP (GRUB + reports) — **mount this** after a loop |
| **`GJ-LNX-ROOT`** | ext4 | Alpine root (`/opt/gj-hwtest`) |
| `GREENJADE` | FAT | Product freestanding ESP — **not** used by this image |
| `GJ-PERSIST` | ext4 | Product persist partition — soft-probed only if present |

## DUT procedure

1. Insert stick, power on, spam **Esc** (or F8) → **UEFI: … USB**  
2. Secure Boot **Disabled**  
3. Watch console: `GreenJade Linux HWTEST — starting full loop`  
4. Machine **powers off** when done (leave stick inserted until off)  
5. On lab host:

```sh
sudo mkdir -p /mnt/esp
sudo mount -L GJ-LNX-ESP /mnt/esp
ls /mnt/esp/GJ-HWTEST/reports/
cat /mnt/esp/GJ-HWTEST-RESULT.txt
cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt
# Greppable one-liners (xhci_bdf, stick_bus_port, stick_vid_pid, …):
cat /mnt/esp/GJ-HWTEST/reports/KEY-FACTS.txt
# or: cat /mnt/esp/GJ-HWTEST-KEY-FACTS.txt
# USB3 / xHCI section is especially useful for freestanding stage-11 debugging
sudo umount /mnt/esp
```

## Known DUT maps

See **[KNOWN-HW.md](./KNOWN-HW.md)** for captured xHCI topology (e.g. G752VT: Intel `8086:a12f` @ `00:14.0`, stick SuperSpeed `bus2-port5` / `13fe:6400`).

## Scripts (on image)

| Path | Role |
|------|------|
| `/opt/gj-hwtest/run-all.sh` | Mount **GJ-LNX-ESP**, run probes, write report + KEY-FACTS, poweroff |
| `10-system.sh` | uname, CPU, RAM, DMI |
| `20-pci.sh` | lspci inventory |
| `30-usb-xhci.sh` | lsusb, bus-port speed map (SS vs HS), xHCI PCI, dmesg USB |
| `40-storage.sh` | lsblk, blkid, nvme, smartctl best-effort |
| `50-dmesg.sh` | dmesg tail + error sample |
| `60-greenjade-stick.sh` | Soft inventory of GREENJADE / GJ-PERSIST / GJ-LNX labels |

Auto-start: OpenRC `local` → `/etc/local.d/gj-hwtest.start`

## Makefile

```sh
sudo make linux-hwtest-img
sudo make install-linux-hwtest DEV=/dev/sdX
```

## Relation to GreenJade freestanding USB

| Media | Purpose |
|-------|---------|
| `greenjade-hwtest.img` | GreenJade UEFI + kernel + GJ-PERSIST (product/lab path) |
| `linux-hwtest.img` | **Linux** auto inventory of the same DUT |

Use Linux media when you need a full driver stack (`xhci_hcd`, `lsusb -t`, dmesg) to compare against freestanding stage bars (e.g. stage 11 control).

## Host driver collect (feeds GreenJade soft module path)

On a lab host that already runs the DUT’s drivers (e.g. RHEL 9.x on G752VT):

```sh
# From knano root — stages plain .ko + NEEDED-DRIVERS (not into this Alpine image)
make collect-linux-drivers
# → build/linux-drivers/modules/…  meta/XHCI-STATUS.txt  NEEDED-DRIVERS.txt
# greppable: collect-linux-drivers: PASS
#            collect-linux-drivers: xhci 8086:a12f xhci_pci=BUILTIN|PRESENT|MISSING
```

`make hwtest-img` packs that tree onto **GJ-PERSIST/linux-drivers/** for the freestanding GreenJade image. Soft in-kernel load of embedded `r8169` is **lab only** (Soft ≠ product; G-AC-1). See [docs/LINUX_MODULE_PATH.md](../../docs/LINUX_MODULE_PATH.md) and [docs/LAPTOP_LINUX_DRIVER_HOST.md](../../docs/LAPTOP_LINUX_DRIVER_HOST.md).

Script `70-needed-drivers.sh` (when present on this media) inventories which modules the DUT’s PCI IDs need so the collect list stays honest.

## Honesty

- Reports are **host Linux observations**, not freestanding product PASSes.  
- `RESULT: PASS` means probe scripts finished without shell errors, not “USB product done.”  
- Host `.ko` collect / stage is **inventory for the soft module path**, not a product claim that GreenJade runs GPL drivers.  
- Top50 / bar3 remain **OPEN** until GreenJade client evidence.
