# GreenJade — Hardware Compatibility List (HCL)

| | |
|--|--|
| **Status** | Living list — start here for real-hw install work |
| **Policy** | Clean-room dual MIT OR Apache-2.0 only (no GPL drivers) |
| **Tiers** | See [SOLARIS_STYLE_REMAINING.md](SOLARIS_STYLE_REMAINING.md) §13 |

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
| USB HC | UHCI/OHCI/EHCI/xHCI | PCI class 0C:03 probe done |
| Input | PS/2 8042 | Soft status probe (`ps2: status=… PASS`) |
| Net | virtio-net on KVM + 1 real NIC later | Clean-room |
| Firmware | UEFI + ACPI | `X86_64_INTEL_PLATFORM.md` |

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

Prefer the **dual-partition USB image** for real hardware (persistent logs):

| Step | Command / action |
|------|------------------|
| 1. Build | `make hwtest-img` → `build/greenjade-hwtest.img` |
| 2. Write USB | `sudo make install-hwtest-usb DEV=/dev/sdX` (wipes device) |
| 3. Layout | **p1** ESP `GREENJADE` (boot) · **p2** FAT32 **`GJ-PERSIST`** (logs + ssh + **steam/**) |
| 4. Boot DUT | UEFI → `BOOTX64.EFI` → serial `GJ-EFI` / `M0 OK` |
| 5. Collect logs | On lab host: `sudo mount -L GJ-PERSIST /mnt/gj-persist` → `logs/` |
| 6. Remote SSH | Product **sshd** is on by default (`sshd.elf`: **TCP :22** on `net_tcp`/virtio-net + loopback; **real X25519-SHA256 KEX**). Live-spawned at boot. Operator/Grok bench access still: `sudo make hwtest-ssh-setup` then `ssh -i build/hwtest-keys/id_ed25519 root@<lab-host>` + serial. Host smoke: `make sshd && ./build/sshd --once`. QEMU: hostfwd `2222→22`. |
| 7. Steam (options **2 + 3**) | **No dpkg.** **Opt2:** pack tree at image build — `make steam-host-prep-all` (or `steam-fetch` + `steam-stage` + `hwtest-img`). **Opt3:** host prep onto existing stick — `sudo make steam-to-persist` / `./scripts/steam-host-prep.sh --to-mount …`. Stick: `GJ-PERSIST/steam/STATUS`. See [STEAM_HWTEST.md](STEAM_HWTEST.md). |

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

## Status — 2026-07-23

Parallel waves **soft shipped** (kernel / media / continuum graph only). **Open bars stay open.**

**Soft stamp (Wave 9, historical):** continuum **makefile_max=15100** soft only (`graph_batch15100.c` parent wire / CREATE-ONLY decades when present on tree). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 12, historical):** continuum **makefile_max=15400** soft only (`graph_batch15400.c` parent wire / CREATE-ONLY decades when present on tree). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. Soft probes and continuum growth remain **≠ full HCL product**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

**Soft stamp (Wave 14):** continuum high-water **advancing toward 15600** soft only (parent wires / CREATE-ONLY; soft deepen on non-w13 surfaces). Host `./scripts/steam-bar3-check.sh` → media inventory **READY** (stage/rootfs STATUS agree) — **media bootstrap only**. Soft probes and continuum growth remain **≠ full HCL product**. **Soft ≠ product complete.** Product lamps remain **0**. **bar3: OPEN.** Matrix **NOT-TRIED × 50**. **No bar3 claim.**

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

### Soft probes ≠ full HCL product (Wave 14 honesty)

**Additive only.** Wave 9/12 tables and hard rules above stay historical ledger. This section is Wave 14 honesty: continuum high-water **advancing toward 15600** soft only (parent wires). Soft deepen on non-w13 surfaces. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Product lamps remain **0**. **bar3 remains OPEN.**

| Soft surface | What it is | What it is **not** |
|--------------|------------|--------------------|
| Continuum **toward 15600** | CREATE-ONLY soft graph high-water advance (parent wires; tree may lag prior decade) | Runtime ABI product; Steam client; Deck Top 50; bar3; **full HCL product** |
| Soft deepen (non-w13) | Honesty / bring-up surface deepen | Product complete; product lamps lit |
| Media `STATUS=READY` / `steam-bar3-check` | Host/media bootstrap inventory | Client launch; title try; HCL product bar |
| Product lamps **0** | Soft score / bar3-ready stubs remain zero | Product-score claim |

**Hard rule (Wave 14):** never promote soft probes, continuum advancing toward **15600**, or media READY to “HCL product complete,” T1+ certified, product lamps > 0, or **bar3 closed**. Soft continuum ≠ full HCL product. Soft probes ≠ full HCL product. **Soft ≠ product complete.** Open bars below stay open.

### Explicit non-claims (Wave 12 historical · Wave 14 restates)

| Claim | Allowed? |
|-------|----------|
| “Continuum high-water advancing toward **15600** (CREATE-ONLY parent wire)” | **Yes** — soft only |
| “Media `STATUS=READY` / steam-bar3-check inventory READY” | **Yes** — media bootstrap only |
| “Soft probes / T0 CI greppable” | **Yes** — probe / virtual gate only |
| “Full HCL product / T1+ certified real-hw matrix closed” | **No** |
| “Product lamps lit / product complete from soft continuum” | **No** — lamps remain **0** |
| “bar3 closed / Steam client on DUT / Deck Top 50 leave NOT-TRIED” | **No** |

### Soft shipped (not product bars)

- **Live daemons:** `sessiond` / `netstackd` / `sshd` / `storaged` / `vfsd` / shell / `scsi_mid` / `hda_client`
- Residual **#UD** closed
- Continuum high-water **advancing toward 15600** (soft graph only — not Steam client; parent-wire CREATE-ONLY; **not** full HCL product; Wave 12 historical high-water was **15400**)
- **io_uring** soft surface shipped (`io_uring_min.c`; `linux: io_uring min rings PASS`, `mmap PASS`, `SQE I/O PASS`) — vfs_ram/kernel smoke, **not** game I/O
- **GJ_MEM=768G** hierarchical `soak_tib` **PASS** on host class **≥1TiB** — product bar still **≥ 1 TiB** full path when host allows
- **aarch64 M0 scaffold** smoke **PASS** (`make aarch64` / `aarch64-smoke`; shared C + PSCI smokes) — separate arch tree; not Deck/x86 product kernel
- **HDA multi-stream** kernel **PASS** — not Steam/game audio
- Media `STATUS=READY` / host soft check READY = bootstrap tree on media only — **not** client run
- T1 soft probes (NVMe CAP, AHCI GHC, USB HC class, PS/2 status) = **probe only** — **not** T1 product HCL close
- Product lamps remain **0** — soft continuum ≠ product complete

### Open bars (do not claim done)

- **bar3** — Steam **client** launch on DUT + Deck Top 50 leave `NOT-TRIED` (**OPEN**; soft probes / continuum toward **15600** do **not** close)
- Deck Top 50 title runs — all **NOT-TRIED** (no titles tried; claim **targeting only**)
- Product RAM **≥ 1 TiB** full path — **open** when host allows (768G soak ≠ TiB bar)
- Steam/game audio / title matrix fill — **open** (blocked on bar3 client run)
- **Full HCL product** (documented T1+ real-hw install matrix beyond soft probes) — **open** (soft probes ≠ full HCL product)

## Related docs

- [X86_64_INTEL_PLATFORM.md](X86_64_INTEL_PLATFORM.md) — platform rules  
- [GLIBC_COMPAT.md](GLIBC_COMPAT.md) — libcgj / host glibc policy  
- [TODO.md](TODO.md) — coding checklist  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 honesty / READY vs NOT-TRIED  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — hwtest Steam media options 2+3  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md) — product matrix  
