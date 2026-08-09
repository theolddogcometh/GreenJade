# Userspace

| Path | Status |
|------|--------|
| `libgj/` | Freestanding syscall stubs — `make userland` |
| `libgj-so/` · `libgj-gnu/` | Shared / GNU-shaped helpers |
| `libcgj/` | Clean-room glibc-shaped `libc.so.6` (continuum soft graph) |
| `init/` | First process → `build/user/init.elf` |
| `ld-gj/` | Dynamic linker |
| `shell/` · `vfsd/` · `session/` · `netstackd/` · `sshd/` · `storaged/` | Product servers / tools |
| `personality/` | Linux personality soft surface (ABI-first) |
| `protonrt-server/` | Ring-3 door server (linked into kernel smoke map) |
| `udx/` | **UDX** Linux-porter runtime — `make udx` → `build/libudx.a` |
| `udx/examples/` | Skeleton PCI driver — `make udx-example` → `./build/udx_skeleton` |
| `drivers/ddi_host_gj/` | Soft DDI host (`GJ_SYS_DDI` 103) for UDX bind residual |
| `drivers/rtl8168_udx/` | Dual DoD **B** product residual NIC host **`10ec:8168`** (OPEN; Soft≠product) |
| `drivers/xhci_udx/` | Dual DoD **A** product residual USB host **`8086:a12f`** (OPEN; Soft≠product) |
| `vulkan_icd/` · `hda_client/` · `scsi_mid/` | ICD / audio / SCSI mid soft paths |

## ABI-first driver story

Product progress is **Linux-shaped ABI + DDI/UDX hosts**, not in-kernel freestanding class-driver thrash:

1. **UDX** — clean-room Linux-shaped `probe` / `irq` / `dma` / `mmio` / `quiesce` ([udx/README.md](udx/README.md)).
2. **DDI soft** — `GJ_SYS_DDI` scan/open/map; soft ≠ product caps ([docs/DDI_SOFT.md](../docs/DDI_SOFT.md)).
3. **Dual DoD B** — `rtl8168_udx` product residual (Own / TE\|RE / thr L2) → netstackd → sshd :22 — **OPEN** until glass L3.
4. **Dual DoD A** — `xhci_udx` product residual — **OPEN**; freestanding MSC **SKIP**.
5. **Host Linux modules** — lab soft load only; **G-AC-1** no `.ko` product AC ([docs/LINUX_MODULE_PATH.md](../docs/LINUX_MODULE_PATH.md)).
6. **T0 product NIC (QEMU)** remains **virtio-net** until UDX owns laptop wire.

**Best combo for porters:** Linux-shaped probe/irq/dma/mmio + DDI `quiesce` + caps hidden + host inject for bring-up.

See [docs/UDX_LINUX_PORTER.md](../docs/UDX_LINUX_PORTER.md), [docs/ABI_FIRST_PIVOT.md](../docs/ABI_FIRST_PIVOT.md), [docs/TODO.md](../docs/TODO.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
