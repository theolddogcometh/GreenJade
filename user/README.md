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
| `drivers/ddi_host_gj/` | Freestanding soft DDI host (`GJ_SYS_DDI` 103) — G752 inventory |
| `drivers/rtl8168_udx/` | Soft UDX skeleton for **`10ec:8168`** (not product net) |
| `drivers/xhci_udx/` | Soft UDX skeleton for **`8086:a12f`** (not product USB) |
| `vulkan_icd/` · `hda_client/` · `scsi_mid/` | ICD / audio / SCSI mid soft paths |

## ABI-first driver story

Product progress is **Linux-shaped ABI + DDI/UDX hosts**, not in-kernel freestanding class-driver thrash:

1. **UDX** — clean-room Linux-shaped `probe` / `irq` / `dma` / `mmio` / `quiesce` ([udx/README.md](udx/README.md)).
2. **DDI soft** — `GJ_SYS_DDI` scan/open/map; soft ≠ product caps ([docs/DDI_SOFT.md](../docs/DDI_SOFT.md)).
3. **Host Linux modules** — lab soft load of collected `.ko` (kernel path); dual-license tree only, **no GPL source**, **G-AC-1** no `.ko` product AC ([docs/LINUX_MODULE_PATH.md](../docs/LINUX_MODULE_PATH.md)).
4. **T0 product NIC** remains **virtio-net**.

**Best combo for porters:** Linux-shaped probe/irq/dma/mmio + DDI `quiesce` + caps hidden + host inject for bring-up.

See [docs/UDX_LINUX_PORTER.md](../docs/UDX_LINUX_PORTER.md), [docs/ABI_FIRST_PIVOT.md](../docs/ABI_FIRST_PIVOT.md), [docs/LAPTOP_LINUX_DRIVER_HOST.md](../docs/LAPTOP_LINUX_DRIVER_HOST.md).
