<p align="center">
  <img src="media/GreenJade_Logo.jpg" alt="GreenJade logo" width="280" />
</p>

# GreenJade

**A pure-C microkernel desktop OS** — dual-licensed **MIT OR Apache-2.0**, no GPL in the tree.

GreenJade is a from-scratch, capability-based kernel and userspace personality aimed at a **general-purpose desktop / workstation**. Think: small trusted core, doors and caps for isolation, a clean-room Linux ABI path so desktop software (and eventually Steam via Proton) can run without pasting copyleft kernel code.

If you just cloned the repo: you only need a normal toolchain and QEMU.

---

## At a glance

| | |
|--|--|
| **What it is** | Freestanding pure-C microkernel + hybrid Linux personality |
| **What it is not** | A Linux distro, SteamOS port, or GPL-derived kernel |
| **Priorities** | **1. Security → 2. Performance → 3. Portability → 4. Readability** |
| **License** | **MIT OR Apache-2.0** (dual) — **no GPL/copyleft source** |
| **Language** | Pure C only in-tree (no C++, Rust, …) |
| **Firmware** | **UEFI** product path; Multiboot2 bring-up for QEMU |
| **Adoption bar** | **Steam Deck Top 50** via Proton when installable on real hardware |
| **Hardware bar** | **≥ 1 TiB RAM**, SMP, SAS/SCSI (product goals; bring-up runs on modest QEMU) |
| **Style** | Hungarian notation — [STYLE.md](STYLE.md) |

---

## Do third parties need the gitignored files?

**No.** `.gitignore` deliberately omits **generated** artifacts so the repo stays source-only:

| Pattern (examples) | What it is | Third-party need? |
|--------------------|------------|-------------------|
| `build/` | Entire output tree | **No** — `make` recreates it |
| `*.o`, `*.a`, `*.so` | Object files / archives | **No** — compiler/linker produce them |
| `*.elf`, `*.bin` | Kernel / user ELFs, raw blobs | **No** — linked from source |
| `*.img`, `*.iso` | Install / live images | **No** — `make install-img` / `live-iso` |
| `tools/`, `*.rpm` | Local toolchains / packages | **No** — optional host tooling |
| Steam stage trees | Lab-host Valve fetch | **No** — optional; see [STEAM_HWTEST](docs/STEAM_HWTEST.md) |

**What you *do* need on the host:**

- `gcc` or `clang`, `ld` (binutils), `make`
- For QEMU run: `qemu-system-x86_64` / `qemu-kvm`
- Optional: `grub2-mkrescue` (live ISO), OVMF (UEFI smoke), `aarch64-linux-gnu-gcc` (optional aarch64 product)

Clone → build → run. Nothing under `build/` has to be committed or downloaded.

```sh
git clone git@github.com:theolddogcometh/GreenJade.git
cd GreenJade
make            # → build/greenjade.elf
make run        # QEMU, serial on stdio
# or: make smoke
```

---

## Quick start (M0 — Multiboot2 + QEMU)

```sh
make            # → build/greenjade.elf
make userland   # init, shell, ld-gj, libcgj, servers
make run        # QEMU q35 + virtio, serial on stdio
GJ_SMP=4 make run
make smoke      # Multiboot + OVMF + packaging + license gate
make clean
```

### More targets (when you want them)

```sh
make libcgj         # clean-room glibc-shaped libc
make uefi-stub      # freestanding UEFI handoff object
make greenjade.efi  # → build/GreenJade.efi
make ovmf           # QEMU + OVMF GPT ESP boot
make stage-esp      # ESP layout for real-hw copy
make stage-rootfs   # rootfs layout
make install-img    # GPT install image (local build only)
make live-iso       # hybrid Multiboot2+EFI test ISO (local build only)
make hwtest-img     # dual-partition hardware-test image
make sshd-gj        # freestanding product sshd
make udx            # host UDX driver runtime
make license        # coarse GPL guard
```

USB / lab helpers (`install-usb`, `steam-fetch`, …) need root or lab host setup — see [docs/STEAM_HWTEST.md](docs/STEAM_HWTEST.md) and [docs/HCL.md](docs/HCL.md).

**Bring-up today:** Multiboot2 + OVMF UEFI, SMP, virtio, hybrid Linux ABI, PE32 Wine int80, ELF dynlinker, fork COW, doors/session/ICD, packaging. Product smoke aims for **M0 OK** / **UD=0**.

**Still open:** real-hardware Steam client, Deck Top 50 matrix fill, full multi-server confine product.

---

## Docs

| Doc | Purpose |
|-----|---------|
| [Architecture](docs/GREENJADE_KERNEL_SPEC.md) | Project law, product bars, milestones |
| [**Design complete freeze**](docs/DESIGN_SPEC_COMPLETE.md) | Isolation, doors, AC, matrix, locks, clean-room |
| [Security core](docs/SECURITY_CORE_DESIGN.md) | Caps, revoke, IPC, SMP, quotas |
| [Cap addressing](docs/CAP_ADDRESSING.md) | Scheme A; root meta; pager |
| [Proton personality](docs/PROTON_PERSONALITY.md) | Deck Top 50; clean-room Linux ABI |
| [glibc compat](docs/GLIBC_COMPAT.md) | Clean-room **libcgj** → `libc.so.6` |
| [Linux ABI hybrid](docs/LINUX_ABI_HYBRID.md) | Option C hot/cold SYSCALL |
| [Apple channel](docs/APPLE_CHANNEL_REMAINING.md) | VM objects, task ports, QoS, session |
| [Solaris remaining](docs/SOLARIS_STYLE_REMAINING.md) | Untyped, CDT, map cookie |
| [x86_64 Intel platform](docs/X86_64_INTEL_PLATFORM.md) | UEFI, VT-d, x2APIC, TSC |
| [HCL](docs/HCL.md) | Hardware tiers T0–T3 + install checklist |
| [UDX Linux porter](docs/UDX_LINUX_PORTER.md) | Userspace driver API |
| [Implementation](docs/IMPLEMENTATION.md) / [TODO](docs/TODO.md) | Coding phases |
| [Deck Top 50 matrix](matrix/deck-top50-TEMPLATE.md) | Adoption tracking |
| [STYLE](STYLE.md) · [LICENSE](LICENSE) | Style · dual MIT/Apache |

Driver hosts use **UDX** (`user/udx/`) — Linux-shaped `probe` / `irq` / `dma` / `mmio` with caps hidden; see the UDX guide.

---

## Start coding

1. [docs/TODO.md](docs/TODO.md) — backlog and phases  
2. [docs/IMPLEMENTATION.md](docs/IMPLEMENTATION.md) — how pieces fit  
3. [STYLE.md](STYLE.md) — Hungarian + dual-license SPDX  

---

## Heritage

Structural inspiration from the classic Pink/Taligent **Opus** microkernel idea (tiny core, services outside). Product name: **GreenJade**.
