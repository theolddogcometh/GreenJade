# Project GreenJade

**GreenJade** is a pure-C, capability-based microkernel aimed at a **general-purpose desktop / workstation OS**.

| | |
|--|--|
| **Codename** | GreenJade |
| **Priorities** | **1. Security → 2. Performance → 3. Portability → 4. Readability** (**unchanged** by games) |
| **Product** | General-purpose **desktop** |
| **Adoption games** | **Steam Deck Top 50** via Proton ([docs/PROTON_PERSONALITY.md](docs/PROTON_PERSONALITY.md)) — when installable on **real hardware**, install Steam and run the Top 50 matrix |
| **Firmware** | **UEFI** (primary) |
| **Hardware bar** | **≥ 1 TiB RAM**, **SMP / multi-CPU**, **SAS/SCSI** |
| **Language** | Pure C only in-tree (no C++, Rust, …) |
| **License** | **MIT OR Apache-2.0** (dual) — **no GPL/copyleft source** |
| **Linux ABI** | **Clean-room reimplementation OK** — grows with Deck Top 50 needs |
| **Security** | OpenBSD-inspired defaults (reimplemented); priorities unchanged by games |
| **Drivers** | Userspace UDX; clean-room (no Linux **source** paste) |
| **Not a goal** | OOP; importing copyleft; SteamOS **distro port**; loading Linux `.ko` |
| **Variables** | **Hungarian notation** (see [STYLE.md](STYLE.md)) |

## Docs

| Doc | Purpose |
|-----|---------|
| [Architecture](docs/GREENJADE_KERNEL_SPEC.md) | Project law, product bars, milestones |
| [**Design complete freeze**](docs/DESIGN_SPEC_COMPLETE.md) | **Accepted** — isolation, doors, AC, matrix, locks, clean-room |
| [Security core](docs/SECURITY_CORE_DESIGN.md) | Caps, revoke, IPC, SMP, quotas |
| [Cap addressing](docs/CAP_ADDRESSING.md) | Scheme A; root meta; pager |
| [Proton personality](docs/PROTON_PERSONALITY.md) | Deck Top 50; clean-room Linux ABI |
| [glibc compat](docs/GLIBC_COMPAT.md) | Clean-room **libcgj** → `libc.so.6` (no GNU glibc in tree) |
| [Linux ABI hybrid](docs/LINUX_ABI_HYBRID.md) | Option C hot/cold SYSCALL |
| [Apple channel](docs/APPLE_CHANNEL_REMAINING.md) | VM objects, task ports, QoS, session |
| [Solaris remaining](docs/SOLARIS_STYLE_REMAINING.md) | Untyped, CDT, map cookie |
| [x86_64 Intel platform](docs/X86_64_INTEL_PLATFORM.md) | UEFI, VT-d, x2APIC, TSC |
| [HCL](docs/HCL.md) | Hardware compatibility tiers T0–T3 + install checklist |
| [UDX Linux porter](docs/UDX_LINUX_PORTER.md) | Userspace driver API |
| [Implementation](docs/IMPLEMENTATION.md) / [TODO](docs/TODO.md) | Coding phases |
| [Deck Top 50 matrix template](matrix/deck-top50-TEMPLATE.md) | Adoption tracking |
| [STYLE](STYLE.md) · [LICENSE](LICENSE) | Style · dual MIT/Apache |

## Build & run (M0 interim — Multiboot2)

Dependencies: `gcc` or `clang`, `ld`, `qemu-system-x86_64` (for run).

```sh
make            # -> build/greenjade.elf
make userland   # -> init.elf shell.elf ld-gj + libcgj/libc.so.6 + servers
make libcgj     # clean-room glibc-shaped libc (docs/GLIBC_COMPAT.md)
make uefi-stub  # freestanding UEFI handoff object (not Multiboot)
make greenjade.efi  # -> build/GreenJade.efi (PE32+ EFI app, serial GJ-EFI)
make ovmf           # QEMU + OVMF GPT ESP boot of GreenJade.efi
make stage-esp      # build/esp: BOOTX64.EFI + KERNEL.ELF + user/*.elf (real-hw copy)
make stage-rootfs   # build/rootfs: sbin/init, bin/sh, etc. stubs + server ELFs
make install-img    # build/greenjade-install.img — GPT ESP (dd to USB / QEMU raw)
sudo make install-usb DEV=/dev/sdX   # wipe + write install-img to USB (root)
make live-iso       # build/greenjade-live.iso — hybrid Multiboot2+EFI hardware-test ISO
make hwtest-img     # build/greenjade-hwtest.img — ESP + GJ-PERSIST (logs+steam) for USB
sudo make install-hwtest-usb DEV=/dev/sdX  # wipe + write dual-partition hwtest image
sudo make hwtest-ssh-setup   # lab-host OpenSSH + ed25519 key (Grok remote debug)
make steam-fetch    # lab host: Valve Steam bootstrap → build/steam-tree (gitignored)
make steam-stage    # stage tree (options 2+3; docs/STEAM_HWTEST.md)
make steam-host-prep-all  # opt2: fetch+stage+hwtest-img
sudo make steam-to-persist  # opt3: copy tree onto mounted GJ-PERSIST stick
make sshd-gj        # freestanding product sshd.elf (port 22, on by default)
make sshd           # host POSIX sshd smoke (127.0.0.1:2222)
make smoke          # Multiboot + OVMF + stage-esp + install-img + license gate
make udx        # -> build/libudx.a  (Linux-porter driver runtime, host)
make udx-example # -> build/udx_skeleton  (PCI probe/mmio/irq/work demo)
make run        # QEMU q35 + virtio net/gpu/kbd/blk, serial on stdio
GJ_SMP=4 make run   # MADT sees 4 local APICs (APs deferred until trampoline)
make license    # coarse GPL guard
make clean
```

Driver hosts use **UDX** (`user/udx/`, see [UDX guide](docs/UDX_LINUX_PORTER.md)) — Linux-shaped `probe`/`irq`/`dma`/`mmio` + DDI `quiesce`, caps hidden; host inject for bring-up.

**Bring-up progress:** Multiboot2 + OVMF UEFI, SMP, virtio, hybrid Linux ABI, PE32 Wine int80, **ELF dynlinker** (INTERP-first, multi-SO×4, SysV/GNU hash+bloom), **fork COW** + private/low-band reclaim, fanotify soft path, doors/session/ICD, install rootfs/ESP packaging. Smoke-all green (~275+ gates).  

**Product still open (out of freestanding bring-up scope):** host glibc/Steam client, real-hardware install try, Deck Top 50 title matrix fill.

## Start coding

1. **docs/TODO.md** — M0 finish, then large PMM / UEFI / SMP  
2. **docs/IMPLEMENTATION.md**  
3. **STYLE.md** — Hungarian + dual-license SPDX  

## Heritage

Structural inspiration from the classic Pink/Taligent **Opus** microkernel idea (tiny core, services outside). Product name: **GreenJade**.
