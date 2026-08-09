<p align="center">
  <img src="media/GreenJade_Logo.jpg" alt="GreenJade logo" width="280" />
</p>

# GreenJade

**A pure-C microkernel desktop OS** — dual-licensed **MIT OR Apache-2.0**, no GPL in the tree.

GreenJade is a from-scratch OS kernel and userspace personality aimed at a **general-purpose desktop / workstation**. The kernel stays small on purpose: programs only get the access they are given, talk through the OS on purpose, and drivers live in **userspace** over a Linux-shaped ABI (so a flaky NIC driver need not take down the whole machine). Clean-room path toward desktop software and eventually **Steam via Proton** — without pasting a GPL kernel into the product core.

If you just cloned the repo: a normal host toolchain and QEMU are enough to build and smoke. Optional support: **[Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog)**.

---

## At a glance

| | |
|--|--|
| **What it is** | Freestanding pure-C microkernel + hybrid Linux personality |
| **What it is not** | A Linux distro, SteamOS port, or GPL-derived kernel |
| **Priorities** | **1. Security → 2. Performance → 3. Portability → 4. Readability** |
| **License** | **MIT OR Apache-2.0** (dual) — **no GPL/copyleft source** |
| **Lawful use** | **You** are responsible for not using this project where illegal — [docs/LEGAL_DISCLAIMER.md](docs/LEGAL_DISCLAIMER.md) |
| **Language** | Pure C only in-tree (no C++, Rust, …) |
| **Firmware** | **UEFI** product path; Multiboot2 bring-up for QEMU |
| **Adoption bar** | **Steam Deck Top 50** via Proton on real hardware — **target**, not claimed done |
| **Hardware bar** | **≥ 1 TiB RAM**, SMP, SAS/SCSI (product goals; bring-up runs on modest QEMU) |
| **Style** | Hungarian notation — [STYLE.md](STYLE.md) |
| **Assurance** | [docs/ASSURANCE_LITE.md](docs/ASSURANCE_LITE.md) — Soft≠product · G-AC-1 · Dual DoD L3 = host probes on stamped image · `make assurance-check` |
| **Support** | [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog) — optional funding for hardware digs and late-night boots |

---

**Host tools you need:**

- `gcc` or `clang`, `ld` (binutils), `make`
- For QEMU run: `qemu-system-x86_64` / `qemu-kvm`
- Optional: `grub2-mkrescue` (live ISO), OVMF (UEFI smoke), `aarch64-linux-gnu-gcc` (optional aarch64 scaffold)

Clone → build → run. Artifacts land under `build/` (local only).

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

**Bring-up today (QEMU / soft product markers):** Multiboot2 + OVMF UEFI, SMP, virtio, hybrid Linux ABI surface, PE32 Wine int80 path, ELF dynlinker, fork COW, doors/session/ICD, packaging. Kernel smoke aims for **M0 OK** / **UD=0**. Media may stage a Steam tree as **READY**; that is bootstrap only.

**Bar3 / Deck Top 50:** status and definitions live in **one place only** — [docs/STEAM_BAR3_STATUS.md](docs/STEAM_BAR3_STATUS.md) (currently **OPEN** / **NOT-TRIED × 50**). Boot console and soft inventory lamps do **not** restate bar3.

**Soft continuum high-water:** CREATE-ONLY libcgj graph parent wire (verify with `./scripts/gj-continuum-makefile-snippet.sh --max`). Product lamps remain **0**. Soft continuum is not product complete and is not Deck Top 50 progress.

**Still open:** real-hardware UEFI install + Steam client, Deck Top 50 (matrix remains **NOT-TRIED**), full multi-server confine product, full ≥ 1 TiB soak when host allows.

---

## ABI-first + laptop Linux drivers (G752VT)

Product direction is **ABI-first** (Linux-shaped **userspace** drivers over hot+cold ABI + **DDI/UDX**). In-kernel freestanding class drivers are **SKIP by default** — not Dual DoD close.

| Layer | Role | Claim |
|-------|------|--------|
| **Linux-shaped userspace** | Option C ABI, libcgj, servers | Product path for apps |
| **DDI / UDX hosts** | `rtl8168_udx` · `xhci_udx` · `ddi_host_gj` | Dual DoD **B** (NIC) / **A** (USB) product direction |
| **Soft module path** | Host-collected `.ko` via ksym (eng) | **Soft ≠ product**; **G-AC-1** no `.ko` product AC |
| **Freestanding class** | `rtl8168` / `xhci_msc` | **SKIP** default (`GJ_RTL8168_PROBE=0` · `GJ_XHCI_MSC_PROBE=0`) |
| **T0 product net (QEMU)** | **virtio-net** | Until UDX owns laptop wire |
| **Fly bar** | `GJ_IMAGE_VERSION` | Fly what you flash — e.g. **STATUS (static) v2026.08.04.93** |

**First DUT:** ASUS ROG **G752VT** — NIC `10ec:8168`, xHCI `8086:a12f`, lab static **10.200.125.50**.

### Dual DoD (honest — both OPEN)

| # | Goal | Path | Status |
|---|------|------|--------|
| **A** | Linux-shaped USB | `xhci_udx` + DDI | **OPEN** — soft residual + program_gate honesty; freestanding MSC SKIP |
| **B** | Linux-shaped NIC + stack + sshd | `rtl8168_udx` → netstackd → sshd | **OPEN** — product residual *writes* TE\|RE / Own on real DDI; glass dig FOVW/RER / Own stuck; inject/arping L3 not closed |

**Lab status (honest):** Product residual on glass programs TNPDS/RDSAR/TE\|RE, Own handoff, thr-poll L2 bridge, lab IP pin under UDX ready (`LAB_MAC_UDX=02:00:00:47:4a:50`). Historical freestanding ICMP is **not** the product track (freestanding rtl **SKIP**). Soft listen **:22** ≠ product host banner on the wire. Soft ≠ product; **G-AC-1**. Backlog: [docs/TODO.md](docs/TODO.md) · [docs/ASSURANCE_LITE.md](docs/ASSURANCE_LITE.md) · [docs/PURE_C_CONCURRENCY_AND_OPS.md](docs/PURE_C_CONCURRENCY_AND_OPS.md) · [docs/MEM_PLACE_CHANNEL.md](docs/MEM_PLACE_CHANNEL.md).

```sh
make collect-linux-drivers   # host .ko → build/linux-drivers/ (+ NEEDED-DRIVERS)
make hwtest-img              # → build/greenjade-hwtest.img (ESP + GJ-PERSIST)
sudo ./scripts/install-hwtest-usb.sh /dev/sdX
# after boot (lab): panel STATUS (static) v… ; arping / ping 10.200.125.50 ; nc -v -w 3 10.200.125.50 22
```

On boot, GOP **STATUS (STATIC)** holds track module path (soft):

| Hold | Example |
|------|---------|
| 7 | `ksym n=…` |
| 8 | `mod r8169 … init=0` (or `SKIP load=0`) |
| 9 | `netdev soft N` (want ≥1) |
| 10 | `probe 10ec:8168 soft` \| `real` \| `miss` |
| 11 | `pci reg=… match=…` |
| 12–13 | xHCI soft SKIP when host `xhci_pci` is **builtin**; USB MSC / `usb_storage need=usbcore` |
| 14–15 | soft L2 bridge · hybrid wire=fs soft=r8169 |

Gate0 hybrid skips real `r8169` probe on the live BAR (EMU soft netdev + freestanding wire). Real hostish probe is gated ([docs/PCI_DEV_SOFT_LAYOUT.md](docs/PCI_DEV_SOFT_LAYOUT.md) · [docs/R8169_MMIO_HANDOFF.md](docs/R8169_MMIO_HANDOFF.md)).

---

## Docs

| Doc | Purpose |
|-----|---------|
| [Architecture](docs/GREENJADE_KERNEL_SPEC.md) | Project law, product bars, milestones |
| [**Design complete freeze**](docs/DESIGN_SPEC_COMPLETE.md) | Isolation, doors, AC, matrix, locks, clean-room |
| [**ABI-first pivot**](docs/ABI_FIRST_PIVOT.md) | Strategy: ABI + DDI + host drivers over freestanding thrash |
| [ABI wave status](docs/ABI_WAVE_STATUS.md) | Soft wave lamps / module path status |
| [Linux module path](docs/LINUX_MODULE_PATH.md) | Soft `.ko` loader, ksym, r8169 embed, xHCI builtin |
| [Laptop Linux driver host](docs/LAPTOP_LINUX_DRIVER_HOST.md) | G752VT collect / stage / STATUS expectations |
| [DDI soft](docs/DDI_SOFT.md) | Soft DDI door / devmgr inventory |
| [PCI soft layout](docs/PCI_DEV_SOFT_LAYOUT.md) | Soft vs host `pci_dev` for real probe |
| [G752VT Linux hwtest](docs/G752VT_LINUX_HWTEST.md) | DUT map + freestanding vs Linux inventory |
| [Security core](docs/SECURITY_CORE_DESIGN.md) | Caps, revoke, IPC, SMP, quotas |
| [Cap addressing](docs/CAP_ADDRESSING.md) | Scheme A; root meta; pager |
| [Proton personality](docs/PROTON_PERSONALITY.md) | Deck Top 50; clean-room Linux ABI |
| [Steam bar3 status](docs/STEAM_BAR3_STATUS.md) | Honest product ceiling (READY ≠ titles) |
| [glibc compat](docs/GLIBC_COMPAT.md) | Clean-room **libcgj** → `libc.so.6` |
| [Linux ABI hybrid](docs/LINUX_ABI_HYBRID.md) | Option C hot/cold SYSCALL |
| [Apple channel](docs/APPLE_CHANNEL_REMAINING.md) | VM objects, task ports, QoS, session |
| [Solaris remaining](docs/SOLARIS_STYLE_REMAINING.md) | Untyped, CDT, map cookie |
| [x86_64 Intel platform](docs/X86_64_INTEL_PLATFORM.md) | UEFI, VT-d, x2APIC, TSC |
| [HCL](docs/HCL.md) | Hardware tiers T0–T3 + install checklist |
| [HW test tomorrow](docs/HWTEST_TOMORROW.md) | Operator checklist for DUT media |
| [UDX Linux porter](docs/UDX_LINUX_PORTER.md) | Userspace driver API |
| [Implementation](docs/IMPLEMENTATION.md) / [TODO](docs/TODO.md) | Coding phases |
| [Deck Top 50 matrix](matrix/deck-top50-TEMPLATE.md) | Adoption tracking |
| [STYLE](STYLE.md) · [LICENSE](LICENSE) | Style · dual MIT/Apache |
| [**Legal disclaimer**](docs/LEGAL_DISCLAIMER.md) | Lawful use · U.S. Constitution / speech notice · author fee schedule claim |

Driver hosts use **UDX** (`user/udx/`) and soft DDI (`user/drivers/`) — Linux-shaped `probe` / `irq` / `dma` / `mmio` with caps hidden; see the UDX guide.

---

## Legal disclaimer (lawful use · speech · rights)

**Users:** You are solely responsible for ensuring that obtaining, building, running, redistributing, or productizing GreenJade is **legal** where you operate. **Do not use** this software where that use is illegal or prohibited. Age/ID and platform rules (including **California** and other governments) are **your** compliance problem if you ship a product—not a warranty from this tree.

**Principal author (rights notice):** The lead author is a **U.S. citizen**, governed under the **U.S. Constitution**, and asserts that this **code and project writing are protected speech**. Dual open-source licensing is a **copyright license**, not a waiver of constitutional rights against government actors. The author asserts a **claimed liability schedule** (see full doc) against **oath-sworn officers / government bodies** for **color-of-law** violations of those rights related to this Software—including **4×** financial penalties attempted and **USD $1,000,000 per hour** of incarceration. That schedule is the author’s **public claim/notice**, not a court award.

Full text: **[docs/LEGAL_DISCLAIMER.md](docs/LEGAL_DISCLAIMER.md)** (§0 rights · §1+ user duties) · **[LICENSE](LICENSE)** (MIT OR Apache-2.0, AS IS). **Not legal advice.**

---

## Start coding

1. [docs/TODO.md](docs/TODO.md) — backlog and phases  
2. [docs/IMPLEMENTATION.md](docs/IMPLEMENTATION.md) — how pieces fit  
3. [STYLE.md](STYLE.md) — Hungarian + dual-license SPDX  

---

## Heritage

Structural inspiration from the classic Pink/Taligent **Opus** microkernel idea (tiny core, services outside). Product name: **GreenJade**.
