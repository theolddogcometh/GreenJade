# GreenJade Vulkan ICD (A1)

Clean-room **Vulkan ICD** subset for Proton A1 on virtio-gpu 2D present (HCL T0).

Not a full Khronos SDK or GPU driver: a freestanding-friendly, software-raster path that
presents through virtio-gpu (kernel smoke) or a host software present counter (host ICD).

**License:** dual MIT OR Apache-2.0. Pure C. No GPL Mesa paste.

## Layout

| Path | Role |
|------|------|
| `include/vulkan/vulkan_core_gj.h` | Khronos-shaped types and entry decls (subset) |
| `include/vulkan/vk_icd.h` | Loader ICD negotiate / GetInstanceProcAddr |
| `include/vulkan/vk_platform.h` | Platform calling conventions |
| `include/gj_vk_icd.h` | Non-Khronos convenience API (`gj_vk_*`) |
| `src/vulkan_icd.c` | Khronos-shaped entry points (instance → present + deepen stubs) |
| `src/gj_vk_icd.c` | Legacy GreenJade-shaped helpers |
| `tools/icd_host_smoke.c` | Host multi-frame + SPIR-V + UV + product-stub smoke |
| `GreenJade_icd.json` | Loader manifest (`library_path` → `.so`) |

## Implemented

Smoke markers (kernel `kprintf` / host `printf`) expected by `scripts/smoke-all.sh`:

| Marker | Path |
|--------|------|
| `vk: QueuePresentKHR` / `vk: QueueSubmit` | ICD (kernel build) |
| `vk: khronos path PASS` … `vk: uv path PASS` | Kernel driver (`kernel/main.c`) |
| `icd_host_smoke: PASS` (+ `spirv` / `uv` / soft / feature-not-present) | Host smoke binary |

Coverage:

- **Loader:** `vk_icdNegotiateLoaderICDInterfaceVersion`, `vk_icdGetInstanceProcAddr`
- **Core:** instance, physical device enum/properties/format props, device, queue
- **KHR:** surface caps/formats/modes, swapchain create/images/acquire/present
- **GJ ext:** headless surface, map swapchain, texture bind
- **Cmd pool / buffers:** clear, draw, indexed draw, submit, reset
- **Render pass / framebuffer / graphics pipelines** (stages bind modules)
- **SPIR-V:** magic + EntryPoint + SpecId 0 color
- **Software raster:** barycentric triangles; VBO stride 2/3/4 (`x,y` / `+color` / `+u,v`)
- **Texture:** center sample + UV-mapped nearest/bilinear (integer fixed-point)
- Buffers, push constants, viewport/scissor, sampler
- Image create + host-visible memory
- Fence / semaphore / event (soft)
- Pipeline cache / buffer view / descriptor layout+pool (soft handles)
- **Transfers (soft host-visible):** `CmdCopyBuffer`, `CmdCopy*Image*`, `CmdFillBuffer`,
  `CmdUpdateBuffer`, `CmdBlitImage` (1:1 nearest)
- **Host-coherent soft:** flush/invalidate mapped ranges, memory commitment, subresource layout

### Product deepen policy

| Shape | Examples |
|-------|----------|
| **Soft success** | WaitIdle, barriers, flush/invalidate, reset pool/buffer, empty sparse props, events set/reset, dispatch nop, empty pipeline cache data |
| **`VK_ERROR_FEATURE_NOT_PRESENT`** | `vkCreateComputePipelines`, `vkQueueBindSparse`, `vkCreateQueryPool`, `vkGetQueryPoolResults` |
| **`VK_ERROR_FORMAT_NOT_SUPPORTED`** | Image format props for non-RGBA8 formats |

GetProcAddr (`vkGetInstanceProcAddr` / `vkGetDeviceProcAddr` / ICD GIPA) exports both
implemented paths and deepen stubs so loaders/apps can probe without NULL crashes.

## Host build

```bash
make vulkan-icd          # lib + .so + JSON
make vulkan-icd-host     # above + icd_host_smoke
./build/icd_host_smoke
```

Artifacts under `build/`:

- `libgj_vulkan_icd.a` — static host archive
- `libGreenJade_icd.so` — shared ICD for the loader
- `GreenJade_icd.json` — copied loader manifest
- `icd_host_smoke` — host smoke binary

Kernel smoke links the same sources with `GJ_VK_KERNEL_SMOKE` (virtio-gpu + PMM).

## Host smoke checks

1. Loader negotiate + instance version/extensions
2. Multi-frame present (`gj_vk_host_present_count`)
3. SPIR-V module parse + solid triangle draw
4. UV textured triangle
5. Soft path: fence status, flush, reset cmd, buffer copy/fill, GIPA/GDPA
6. Feature-not-present: compute pipelines, sparse bind, query pool

## Not yet

- Real WSI platforms (X11 / Wayland / Win32)
- Full SPIR-V interpreter / multi-texture
- Real sparse binding, multi-queue, timeline semaphores
- Real compute / query timestamps
- Descriptor-driven texturing (still GJ bind-texture path)
