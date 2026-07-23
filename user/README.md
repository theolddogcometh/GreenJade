# Userspace

| Path | Status |
|------|--------|
| `libgj/` | Freestanding syscall stubs — `make userland` |
| `init/` | First process scaffold → `build/user/init.elf` (loader TBD) |
| `protonrt-server/` | Ring-3 door server (linked into kernel smoke map) |
| `udx/` | **UDX** Linux-porter runtime — `make udx` → `build/libudx.a` |
| `udx/examples/` | Skeleton PCI driver — `make udx-example` → `./build/udx_skeleton` |

**Best combo for porters:** Linux-shaped probe/irq/dma/mmio + DDI `quiesce` + caps hidden + host inject for bring-up.

See [docs/UDX_LINUX_PORTER.md](../docs/UDX_LINUX_PORTER.md).
