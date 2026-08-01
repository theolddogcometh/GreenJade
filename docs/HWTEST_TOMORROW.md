# Hardware test — operator pack (2026-08-01)

**Goal tomorrow:** boot GreenJade on real DUT, capture serial, confirm product markers, note hardware.  
**Product tip:** `makefile_max=26800` (Wave 126). **bar3 remains OPEN** (Steam client / Top-50 still not claimed).

## What is ready on this lab host

| Artifact | Path | Notes |
|----------|------|--------|
| Product kernel | `build/greenjade.elf` (~8.5 MiB) | Multiboot **smoke PASS** (M0 OK, UD=0, quick-keys) |
| UEFI loader | `build/GreenJade.efi` (~55 KiB) | Loads `KERNEL.ELF` (GJUEFI1); OVMF may #UD after handoff — real DUT TBD |
| **HW test USB image** | `build/greenjade-hwtest.img` (**2.0 GiB**) | GPT: p1 ESP `GREENJADE` + p2 `GJ-PERSIST` (logs/ssh/**steam READY**) |
| Hybrid live ISO | `build/greenjade-live.iso` (~92 MiB) | Multiboot2 + EFI fallback / optical / QEMU |
| ESP stage tree | `build/esp/` | `EFI/BOOT/BOOTX64.EFI` + `EFI/GREENJADE/KERNEL.ELF` + user ELFs |
| SSH keys | `build/hwtest-keys/id_ed25519` | Packed onto GJ-PERSIST; also for lab-host access |
| Steam media | `build/steam-stage/steam/STATUS` = **READY** | Media bootstrap only — **≠ client launch** |

**Preflight (this host, 2026-08-01):**

- Multiboot: `timeout 180 scripts/run-qemu.sh build/greenjade.elf` → **gj-quick-keys PASS** (M0, sshd, scsi_mid, shell, multi-stream/seg, UD=0).
- Steam: `./scripts/steam-bar3-check.sh` → **READY** (bar3 still **OPEN**).
- Image pack: ESP contains full **KERNEL.ELF** (8 856 176 B) + user daemons; p2 Steam **READY**.
- UEFI/OVMF: loader reaches **KERNEL.ELF loaded** + handoff PASS; OVMF then often hits **#UD** (firmware/QEMU layout). Prefer Multiboot path if UEFI stalls on DUT.

## What you need to bring / plug in

1. **USB stick ≥ 4 GiB** (will be **wiped**).
2. **DUT** with UEFI (or legacy Multiboot if using live ISO + GRUB path).
3. **USB serial adapter** + cable to DUT COM/header (115200 8N1).
4. Optional: Ethernet to DUT + lab host on same LAN (product **sshd :22** default-on).
5. This tree checked out, or a laptop with the image + keys copied.

**No USB stick is plugged into this lab box right now** (`lsblk` shows only RAID `sda`/`sdb`). Plug the stick tomorrow before write.

## Tomorrow — step by step

### A) Write the hwtest USB (primary)

```sh
cd /home/jay/Documents/knano   # or your clone

# Confirm image still present
ls -lh build/greenjade-hwtest.img

# Identify stick carefully (must NOT be sda/sdb system disks)
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL
# e.g. /dev/sdc once the stick is plugged in

# DESTROYS the entire device:
sudo make install-hwtest-usb DEV=/dev/sdX
# same as: sudo ./scripts/install-hwtest-usb.sh /dev/sdX
```

Optional lab-host SSH enable (for remote serial bridge, not GreenJade itself):

```sh
sudo make hwtest-ssh-setup
# or after stick is mounted:
# sudo mount -L GJ-PERSIST /mnt/gj-persist
# sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh
```

### B) Boot DUT

1. Enter firmware boot menu → boot **UEFI: USB** / `BOOTX64.EFI` (ESP label **GREENJADE**).
2. Attach serial **before** reset:

```sh
mkdir -p /tmp/gj-hw-logs
picocom -b 115200 /dev/ttyUSB0 | tee /tmp/gj-hw-logs/serial-$(date -u +%Y%m%dT%H%MZ).txt
# Ctrl-A X to quit picocom
```

3. **Expect (serial greps):**
   - UEFI path: `GJ-EFI` then ideally `KERNEL.ELF loaded` / `M0 OK`
   - Multiboot path (if ISO): `source=MULTIBOOT2` then `M0 OK`
   - Product: `sshd: live spawn PASS`, `scsi_mid: live spawn PASS`, `shell: live spawn PASS`
   - Soft: `TRAP #UD` count **0** (or note any #UD)

4. Soft / hard scan on the capture:

```sh
./scripts/gj-product-summary.sh /tmp/gj-hw-logs/serial-….txt   # always exit 0
./scripts/gj-quick-keys.sh /tmp/gj-hw-logs/serial-….txt        # exit 1 on hard miss
```

### C) If UEFI USB does not reach M0

Use the **live ISO** (Multiboot proven on this host):

```sh
# Write ISO to stick (alternate; overwrites stick):
sudo dd if=build/greenjade-live.iso of=/dev/sdX bs=4M status=progress conv=fsync
# or boot ISO from optical / another machine’s QEMU first
./scripts/run-live-iso.sh   # QEMU check on lab host
```

Then re-try DUT boot (firmware “USB CDROM” / hybrid).

### D) After a run — collect logs

```sh
sudo mkdir -p /mnt/gj-persist
sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/logs /mnt/gj-persist/steam
cat /mnt/gj-persist/steam/STATUS    # READY expected
# Copy serial captures into persist if you want them on the stick:
# sudo cp /tmp/gj-hw-logs/serial-*.txt /mnt/gj-persist/logs/
sudo umount /mnt/gj-persist
```

### E) Steam honesty (do not over-claim)

| Media | Meaning |
|-------|---------|
| `steam/STATUS=READY` | Bootstrap tree on stick |
| **bar3 OPEN** | Client not launched; Deck Top 50 still **NOT-TRIED × 50** |
| Product lamps | **0** |

Do **not** mark matrix rows PASS from media READY alone. See `docs/STEAM_HWTEST.md`.

## Success criteria (minimum bar for “DUT boot worked”)

- [ ] Stick boots firmware (UEFI or Multiboot hybrid)
- [ ] Serial shows **`M0 OK`**
- [ ] **`gj-quick-keys` miss=0** on the capture (or document which keys miss)
- [ ] **`TRAP #UD` = 0** (or file issue with snippet)
- [ ] Note DUT model, CPU, RAM, storage, NIC, GPU in a short log note under `GJ-PERSIST/logs/` or `docs/`

Stretch (still open product):

- [ ] `sshd` reachable on LAN `:22` (if net works on DUT)
- [ ] Steam launcher path visible under persist (still not a bar3 close)

## Rebuild commands (if image deleted)

```sh
make greenjade.efi
make build/greenjade.elf
make userland
make steam-stage          # STATUS=READY if build/steam-tree present
make hwtest-img           # → build/greenjade-hwtest.img
make live-iso             # → build/greenjade-live.iso
./scripts/steam-bar3-check.sh
timeout 180 scripts/run-qemu.sh build/greenjade.elf
```

## Honesty / open bars

- Soft continuum high-water **makefile_max=26800** ≠ runtime product complete  
- Soft deepen **retgradientangle** / **retblendangle** ≠ bar3  
- Real-DUT HCL T1+ and Steam client path remain **open**  
- Priorities: Security → Performance → Portability → Readability  

## Related docs

- [HCL.md](HCL.md) — tiers + install checklist  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — Steam option 2/3 media  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 ceiling  
