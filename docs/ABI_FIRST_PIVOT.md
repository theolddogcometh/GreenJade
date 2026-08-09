# GreenJade — ABI-first pivot (no freestanding driver thrash)

| Field | Value |
|-------|--------|
| **Status** | **Accepted** (operator direction 2026-08) |
| **Inventory** | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) — soft-scaffold honesty |
| **Coding backlog** | [TODO.md](TODO.md) § *Current track* (Linux drivers via ABI/UDX only · flash bar **v2026.08.04.93** · freestanding rtl/USB **SKIP**; Dual DoD **A/B UDX product OPEN**; Soft ≠ product · **G-AC-1**; **no invent next N**) |
| **Law** | Dual MIT OR Apache-2.0 only; **no GPL source in tree** |
| **Bar3** | [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) only |
| **Companions** | [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) · [HCL.md](HCL.md) · [DDI_SOFT.md](DDI_SOFT.md) · [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) · [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) · [ASSURANCE_LITE.md](ASSURANCE_LITE.md) (claim classes · L3 test what you fly) |

## One-sentence strategy

**GreenJade wins as a secure capability kernel that runs Linux-shaped userspace and hosts clean-room / out-of-tree dual-license drivers over DMA/IRQ caps — not by reimplementing every Linux PCI class driver in the kernel, not by executing host Linux `.ko` in-kernel as product, and not by shipping GPL.**

## Hard law — **G-AC-1** and product path

| Rule | Meaning |
|------|---------|
| **G-AC-1** | **No Linux `.ko` binary runs in the kernel as product.** No `init_module` / `finit_module`-style **product** acceptance; no GPL in-tree source. Soft/lab soft-loader experiments do **not** rewrite this. |
| **Product** | **Hot + cold Linux ABI** (Option C personality) + **Linux-shaped drivers in userspace** over **UDX / DDI** caps (clean-room or dual-license out of tree). |
| **Soft residual** | `freestanding_no_exec` / **`RUN_INIT=0`** (soft SKIP exec of staged `.ko` init in kernel) is **engineering residual** — “do not execute `.ko` init in-kernel” — **not** “never use Linux drivers.” Linux-shaped **userspace** hosts remain the product path. |
| **Freestanding class drivers** | **Not product.** Default **SKIP**: `GJ_XHCI_MSC_PROBE=0` (USB) · **`GJ_RTL8168_PROBE=0`** (NIC). Same policy both. Operator 2026-08: **stop freestanding rtl rabbit hole**; focus Linux-shaped UDX/ABI only. Opt-in residual only (`=1`). |
| **Product NIC / USB** | **Userspace UDX** (`rtl8168_udx` / `xhci_udx` …) + hot+cold ABI + DDI caps. **Not** freestanding `rtl8168.c` / `xhci_msc.c`. **Not** in-kernel `r8169.ko` / `usb_storage.ko` wire. |
| **Module path** | Collect → stage → soft loader/ksym is an **ABI / hostability eng track**. Soft load, REAL probe, hybrid SOFT lamps ≠ product AC and ≠ G-AC-1 waiver. |

```text
  PRODUCT (G-AC-1 ok)                 NOT PRODUCT / FORBIDDEN as product AC
  ────────────────────────────────    ──────────────────────────────────────
  Hot+cold Linux ABI (apps)           Linux .ko init/exec in kernel as ship story
  UDX/DDI userspace class hosts       “r8169.ko owns the wire” product claim
  Cap-gated MMIO/IRQ/DMA              GPL driver source in knano tree
  virtio T0 until UDX real-HW         Freestanding stage counts as T1 close
  Soft freestanding_no_exec residual  Reading RUN_INIT=0 as “never Linux drivers”
```

## Why

In-kernel freestanding xHCI MSC and rtl8168 recovery consumed disproportionate effort (stages 11→15, TX/RX OWN thrash) while Linux inventory already maps G752 correctly. Product adoption (desktop / Proton / bar3) is gated by **Linux ABI completeness**, **virtio T0**, and **userspace UDX/DDI hosts** — not freestanding class-driver stage numbers and **not** in-kernel `r8169.ko` product ownership.

## Allowed vs forbidden

| Allowed | Forbidden |
|---------|-----------|
| Clean-room Linux **syscall/ABI** personality (hot + cold) | GPL/Linux **source** in the GreenJade tree |
| Userspace stacks over **cap-gated** PCI/MMIO/IRQ/DMA (**UDX/DDI**) | Claiming “we wrote every class driver in-tree” as product |
| Out-of-tree **dual MIT/Apache or BSD** drivers vs published GJ DDI | Importing Linux driver **source** into knano |
| Operator **linux-hwtest** inventory + **NEEDED-DRIVERS** lists | Claiming inventory PASS = freestanding T1 product |
| Host-collected **`.ko` on install media** for ABI/module-path **eng** | **G-AC-1:** Linux `.ko` **running in kernel** as product AC (Steam/bar3 ship) |
| Goal: **Linux-shaped drivers in userspace** on GJ (UDX + ABI) | Freestanding thrash / in-kernel `r8169.ko` as primary product NIC |
| Soft `freestanding_no_exec` / `RUN_INIT=0` as eng residual | Treating that residual as “never use Linux-shaped drivers” |

## Disposition of soft scaffolds

| Code | Role | Product bar? |
|------|------|----------------|
| `kernel/drv/xhci_msc.c` | Freestanding USB residual; **default SKIP** (`GJ_XHCI_MSC_PROBE=0`) | **No** |
| `kernel/drv/rtl8168.c` | Freestanding NIC residual; **default SKIP** (`GJ_RTL8168_PROBE=0`) | **No** — product NIC = **userspace UDX**; T0 product net = **virtio-net** until UDX owns wire |
| Soft module path / `RUN_INIT=0` / `freestanding_no_exec` | Eng residual: soft SKIP **exec** of `.ko` init in kernel | **No** — not “ban Linux drivers”; product drivers stay **userspace** |
| Panel XHCI/NET stages | Bring-up honesty | **No** |
| `kernel/syscall/linux_cold_net.c` (+ `.h`) | Socket-family cold bridge → `net_tcp` / `net_lo` | **Soft** (wired; not full POSIX) |
| `kernel/drv/devmgr_soft.c` + `gj/devmgr.h` | Soft DDI types / init / scan-deferred / window-note | **Soft** — see [DDI_SOFT.md](DDI_SOFT.md) |
| `user/personality/personality_gj.c` | Userspace G-PERS scaffold (`personality: soft userspace scaffold PASS`) | **Soft** — door loop not product-default |
| `net_tcp_poll_mask` + `vfs_ram_poll_mask` net route | Readiness bits for poll/epoll cold paths | **Soft/partial** |
| `memobj_map_file_fd` | File-backed mmap **snapshot** from vfs_ram regular fd | **Soft** (no live pager) |
| `process_linux_fork` / wait soft | fork/clone + wait4 reaper soft stubs | **Soft** (not full session/COW child) |

## Primary engineering order

1. Linux ABI Option C (**hot + cold** doors; personality → **userspace**) so **Linux-shaped apps and drivers** can be hosted in userspace (not reimplemented freestanding, not product in-kernel `.ko` exec).  
2. **DDI + UDX product track:** PCI / IRQ / DMA window **caps** + clean-room / dual-license **userspace** class hosts (`rtl8168_udx` / `xhci_udx` and successors). This is the **product** driver story under **G-AC-1**.  
3. **Collect + stage** needed host modules (`scripts/collect-linux-drivers.sh` → `GJ-PERSIST/linux-drivers/`); linux-hwtest emits **NEEDED-DRIVERS** for operators.  
4. **Module path** (see [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md)): soft module loader + **ksym** (**in progress**) → soft load / probe smokes for **ABI hostability eng** only. Soft `RUN_INIT=0` / `freestanding_no_exec` = do not exec `.ko` init **in kernel** as product — eng residual, not a ban on Linux-shaped userspace drivers.  
5. App path on virtio T0: netstackd / sshd / shell.  
6. Real HW product DoD: UDX hosts + DDI caps bind PCI and close net/usb **datapath** — mark each OPEN until evidence; **not** freestanding class thrash; **not** “`r8169.ko` owns the wire” product claim.  
7. **Dual DoD A/B (laptop)** — **A** Linux-shaped USB UDX · **B** Linux-shaped NIC UDX — both **OPEN** until DUT proof ([TODO.md](TODO.md) Current track). Soft lamps ≠ close.  
8. **Freestanding rtl / freestanding USB are SKIP by default** — demoted; **not** dual DoD close criteria (2026-08 operator).  
9. bar3 remains OPEN until Steam client + matrix evidence.

### Laptop Linux-driver path (short)

| Step | Action | Status |
|------|--------|--------|
| 1 | `make collect-linux-drivers` → `build/linux-drivers/` | **Live** |
| 2 | `make hwtest-img` → **`GJ-PERSIST/linux-drivers/`** | **Live** |
| 3 | Soft module loader + ksym table | **In progress** |
| 4 | `finit_module` / boot smoke | **OPEN** |
| 5 | Full `r8169` / `xhci_hcd` ksym surface (iterative) | **OPEN** |

Normative checklist: [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) **DoD D1–D10**. Collect/stage/policy **DONE**; load / probe / datapath **OPEN**.

## ABI gap snapshot (inventory 2026-08-02)

| Surface | Status | Soft ≠ product |
|---------|--------|----------------|
| Hot Linux NRs | ~110 registered (`linux_dispatch.c` / `linux_hot.c`) | Depth varies by path |
| Cold Linux NRs | ~201 classified; many soft-stub depth only | Soft continuum ≠ product |
| Cold personality | **Kernel thread** still default; `personality_gj` = userspace **scaffold only** | G-PERS product open |
| Freestanding sshd/netstackd | Prefer **native `GJ_SYS_NET` doors** on virtio T0 | Live spawn soft on T0 |
| Linux `socket`/`bind`/`accept`/… | **Soft bridge in tree**: `linux_cold_net` → STREAM/`net_tcp`, else `net_lo`; wired from `protonrt_cold_link` | Half-close / name / flags incomplete |
| File-backed `mmap` | **Soft path**: hot → `memobj_map_file_fd` (vfs_ram regular fd snapshot); else ENOSYS | No live pager / no door-backed file |
| poll/epoll | Cold handlers + `vfs_ram_poll_mask`; **net route** via `net_tcp_poll_mask` / `net_lo` ready bits | Blocking timeout / full daemon loops partial |
| clone/fork/exec/wait | Cold → `process_linux_fork` + soft wait (`process: soft fork` / `process: soft wait`); PE32 smokes exist | Not full AS isolation / COW product child |
| Linux `.ko` **in-kernel** product AC | **Forbidden** (**G-AC-1**): no `.ko` **runs in kernel** as product | Never as product AC; product = userspace UDX/DDI |
| Soft `freestanding_no_exec` / `RUN_INIT=0` | Eng residual: soft SKIP `.ko` **init exec** in kernel | **Not** “never use Linux drivers” |
| Lab freestanding rtl (`rtl8168.c`) | **Demoted** residual; default **SKIP** (`GJ_RTL8168_PROBE=0`) — **not** Dual DoD **B** | **No** — Dual DoD **B** = userspace UDX NIC; product NIC ≠ freestanding rtl · ≠ in-kernel `r8169.ko` |
| Module path (collect/stage/loader) | Collect+stage **live**; soft loader+ksym **in progress**; eng load/probe **OPEN** | See [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md); soft ≠ product |
| DDI / devmgr | Soft stub: `devmgr_soft_*` lamps + types ([DDI_SOFT.md](DDI_SOFT.md)) | No live MMIO/IRQ/DMA mint; product surface = userspace hosts |

### Soft scaffolds already in tree (do not re-claim as “next invent”)

| Path | Greppable / note |
|------|------------------|
| `kernel/syscall/linux_cold_net.c` · `kernel/include/gj/linux_cold_net.h` | `linux_cold_net: soft …` |
| `kernel/drv/devmgr_soft.c` · `kernel/include/gj/devmgr.h` | `devmgr: soft init PASS` / scan deferred / dma window note |
| `user/personality/personality_gj.c` | `personality: soft userspace scaffold PASS` |
| `kernel/net/net_tcp.c` (`net_tcp_poll_mask`) + `kernel/fs/vfs_ram.c` (`vfs_ram_poll_mask` net route) | TCP/lo readiness into poll/epoll |
| `kernel/mm/memobj.c` (`memobj_map_file_fd`) | `memobj: file map soft` / `linux_hot: mmap file soft ENOSYS` fallback |
| `kernel/proc/process.c` + `spawn.c` | `process: soft fork` · `process: soft wait` |

### Recommended next five (honest — deepen soft → product)

1. **G-PERS product default:** spawn/attach `personality_gj` (or equivalent) as cold-door owner; drop kthread as default; greppable live door serve.  
2. **Blocking poll/ppoll/select/epoll product depth** on net + vfs FDs (timeouts, multi-fd loops for real daemons) — readiness masks already soft-routed.  
3. **Process session product:** runnable `clone`/`fork` child + `execve` + `wait4`/`waitid` reaper beyond soft stub AS (usable pid already soft).  
4. **File-backed mmap product:** live pager / cold open-read via vfs door beyond ramfs **snapshot** `memobj_map_file_fd`.  
5. **DDI product caps:** real MMIO_FRAME / IRQ→Notification / DMA window mint from `devmgr` (soft window-note is **not** VT-d enforce).

*Prior “wire socket family” item is **soft-landed** via `linux_cold_net`; remaining work is fidelity and product apps on that bridge, not inventing the bridge again.*

## Related

- Spec already: *“Userspace drivers; Linux-shaped APIs written from scratch; no Linux source”* — [GREENJADE_KERNEL_SPEC.md](GREENJADE_KERNEL_SPEC.md) 0.x driver strategy.  
- Security: [SECURITY_CORE_DESIGN.md](SECURITY_CORE_DESIGN.md).  
- Soft DDI: [DDI_SOFT.md](DDI_SOFT.md).  
- Wave inventory: [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md).  
- **Linux module path (collect → stage → loader/ksym → eng DoD):** [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md).  
- **G752VT laptop operator runbook (UDX soft host + media):** [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md).  
- OpenBSD spirit: small TCB, fail-closed — not a second authority model.

---

***G-AC-1:** no Linux `.ko` **runs in kernel** as product.*  
*Product = hot+cold Linux ABI + Linux-shaped drivers in **userspace** (UDX/DDI).*  
*`freestanding_no_exec` / `RUN_INIT=0` = eng residual (no in-kernel `.ko` init exec), **not** “never use Linux drivers.”*  
*Freestanding rtl/USB = **SKIP default** — **not** Dual DoD close. Dual DoD **A/B** = **UDX product OPEN** (Linux-shaped USB/NIC hosts).*  
*Product NIC ≠ freestanding rtl · ≠ in-kernel `r8169.ko`.*  
*DUT STATUS (fly): freestanding rtl/USB **SKIP** · Dual DoD **A/B UDX OPEN** · Soft ≠ product · **G-AC-1** · **no invent next N**.*  
*Dual MIT OR Apache-2.0. Soft ≠ product. Flash bar **v2026.08.04.93**. No test-panel photo IDs.*
