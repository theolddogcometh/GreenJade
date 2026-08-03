# GreenJade — ABI-first pivot (no freestanding driver thrash)

| Field | Value |
|-------|--------|
| **Status** | **Accepted** (operator direction 2026-08) |
| **Inventory** | [ABI_WAVE_STATUS.md](ABI_WAVE_STATUS.md) — soft-scaffold honesty |
| **Law** | Dual MIT OR Apache-2.0 only; **no GPL source in tree** |
| **Companions** | [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) · [HCL.md](HCL.md) · [DDI_SOFT.md](DDI_SOFT.md) · [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) · [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) · [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) |

## One-sentence strategy

**GreenJade wins as a secure capability kernel that runs Linux-shaped userspace and hosts clean-room / out-of-tree dual-license drivers over DMA/IRQ caps — not by reimplementing every Linux PCI class driver in the kernel, and not by shipping GPL.**

## Why

In-kernel freestanding xHCI MSC and rtl8168 recovery consumed disproportionate effort (stages 11→15, TX/RX OWN thrash) while Linux inventory already maps G752 correctly. Product adoption (desktop / Proton / bar3) is gated by **Linux ABI completeness** and **virtio T0**, not freestanding class-driver stage numbers.

## Allowed vs forbidden

| Allowed | Forbidden |
|---------|-----------|
| Clean-room Linux **syscall/ABI** personality | GPL/Linux **source** in the GreenJade tree |
| Userspace stacks over **cap-gated** PCI/MMIO/IRQ/DMA | Claiming “we wrote every class driver in-tree” as product |
| Out-of-tree **dual MIT/Apache or BSD** drivers vs published GJ DDI | Importing Linux driver **source** into knano |
| Operator **linux-hwtest** inventory + **NEEDED-DRIVERS** lists | Claiming inventory PASS = freestanding T1 product |
| Host-collected **`.ko` on install media** for ABI/module-path testing (operator/dev) | **G-AC-1:** treating “ship GPL modules” as Steam/bar3 product AC without dual-license product story |
| Goal: **run Linux-shaped drivers** on GJ (module/host path) | Freestanding thrash as primary driver strategy |

## Disposition of soft scaffolds

| Code | Role | Product bar? |
|------|------|----------------|
| `kernel/drv/xhci_msc.c` | Soft scaffold only — optional stick log experiments | **No** |
| `kernel/drv/rtl8168.c` | Lab soft only | **No** — T0 product net = **virtio-net** |
| Panel XHCI/NET stages | Bring-up honesty | **No** |
| `kernel/syscall/linux_cold_net.c` (+ `.h`) | Socket-family cold bridge → `net_tcp` / `net_lo` | **Soft** (wired; not full POSIX) |
| `kernel/drv/devmgr_soft.c` + `gj/devmgr.h` | Soft DDI types / init / scan-deferred / window-note | **Soft** — see [DDI_SOFT.md](DDI_SOFT.md) |
| `user/personality/personality_gj.c` | Userspace G-PERS scaffold (`personality: soft userspace scaffold PASS`) | **Soft** — door loop not product-default |
| `net_tcp_poll_mask` + `vfs_ram_poll_mask` net route | Readiness bits for poll/epoll cold paths | **Soft/partial** |
| `memobj_map_file_fd` | File-backed mmap **snapshot** from vfs_ram regular fd | **Soft** (no live pager) |
| `process_linux_fork` / wait soft | fork/clone + wait4 reaper soft stubs | **Soft** (not full session/COW child) |

## Primary engineering order

1. Linux ABI Option C (hot + cold doors; personality → **userspace**) so **Linux drivers** can be hosted (not reimplemented freestanding).  
2. **Collect + stage** needed host modules (`scripts/collect-linux-drivers.sh` → `GJ-PERSIST/linux-drivers/`); linux-hwtest emits **NEEDED-DRIVERS** for operators.  
3. **Module path** (see [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md)): soft module loader + **ksym** (**in progress**) → `finit_module` / boot smoke → iterative ksym resolve for large surfaces (`r8169`, `xhci_hcd`).  
4. App path on virtio T0: netstackd / sshd / shell.  
5. DDI: PCI / IRQ / DMA window **caps** + UDX clean-room / dual-license hosts (parallel product-shaped track).  
6. Real HW DoD: module **loads**, probe **binds PCI**, net/usb **datapath** — mark each OPEN until evidence; **not** freestanding class thrash.  
7. bar3 remains OPEN until Steam client + matrix evidence.

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
| Linux `.ko` product AC | **Forbidden** as Steam/bar3 ship story (**G-AC-1**) | Never as product AC |
| Module path (collect/stage/loader) | Collect+stage **live**; soft loader+ksym **in progress**; load/probe/datapath **OPEN** | See [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) |
| DDI / devmgr | Soft stub: `devmgr_soft_*` lamps + types ([DDI_SOFT.md](DDI_SOFT.md)) | No live MMIO/IRQ/DMA mint |

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
- **Linux module path (collect → stage → loader/ksym → finit → DoD):** [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md).  
- **G752VT laptop operator runbook (UDX soft host + media):** [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md).  
- OpenBSD spirit: small TCB, fail-closed — not a second authority model.
