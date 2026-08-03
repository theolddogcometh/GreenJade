# aarch64 product kernel (shared C)

QEMU `virt` freestanding product ELF with **shared freestanding C**:
`string` / `kprintf` / `pmm_freelist` / `sched_coop` + arch switch.

**Product laptop / ABI-first work (G752VT, soft Linux module path, UDX/DDI)
lives on x86_64** — see root [README.md](../../../README.md) and
[docs/ABI_FIRST_PIVOT.md](../../../docs/ABI_FIRST_PIVOT.md). This aarch64
tree remains a QEMU `virt` scaffold (virtio-mmio T0-shaped), not the G752
hwtest path.

## Status

| Component | State |
|-----------|--------|
| Shared lib | `string.c`, `stdio_k.c` via `console_*` |
| Shared PMM | `kernel/shared/pmm_freelist.c` (order-0 core) |
| Shared sched | `sched_coop` + `arch/aarch64/switch.S` |
| PSCI | HVC first, SMC fallback; exception recovery if no firmware |
| GIC / timer / MMU / SVC / virtio-mmio | product scaffold |

## Greppable markers

```
aarch64: shared C kernel PASS (string+kprintf)
aarch64: exceptions PASS
aarch64: cpu PASS
aarch64: psci PASS conduit=hvc|smc …   # or soft SKIP
aarch64: GIC PASS
aarch64: timer PASS
aarch64: pmm PASS (shared core …)
aarch64: mmu PASS
aarch64: shared sched PASS
aarch64: svc PASS
aarch64: virtio-mmio PASS
aarch64: mem probe PASS
M0 OK
```

## Build

```sh
make aarch64 AARCH64_CROSS=tools/aarch64-linux-musl-cross/bin/aarch64-linux-musl-
./scripts/gj-aarch64-podman-smoke.sh
```

## Next

- Share more of hierarchical PMM / VMM under arch shims
- Full Linux ABI SVC dispatch
- UEFI PE boot
