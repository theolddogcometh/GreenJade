/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu control queue + 2D present (OASIS; Proton A1 T0).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 only. No Linux virtio source.
 *
 * Soft path (control q0 only — no cursor / 3D / EDID product depth yet):
 *   display  — GET_DISPLAY_INFO, first enabled scanout (default 1024x768)
 *   resource — CREATE_2D / ATTACH_BACKING (page-walk+coalesce) / DETACH / UNREF
 *   scanout  — SET_SCANOUT (enable) / resource_id 0 (disable)
 *   flush    — TRANSFER_TO_HOST_2D + RESOURCE_FLUSH (full or dirty rect)
 *
 * Control command IDs (OASIS public; driver-local, not re-exported):
 *   0x0100 GET_DISPLAY_INFO … 0x0107 RESOURCE_DETACH_BACKING
 *   resp OK_NODATA 0x1100, OK_DISPLAY_INFO 0x1101
 * Format soft: B8G8R8X8_UNORM (2) for product present.
 *
 * Consumers: session door / compositor / ICD smoke / present_stub test pattern.
 *
 * Bring-up lifecycle:
 *   scan → first KIND_GPU → setup → negotiate VERSION_1 → control q0
 *   → driver_ok → ready PASS → present (create/attach/scanout/xfer/flush)
 *
 * Greppable product markers (prefix-stable):
 *   virtio-gpu: ready PASS
 *   virtio-gpu: present PASS
 *
 * greppable: virtio_gpu_probe virtio_gpu_present virtio_gpu_flush
 */
#pragma once

#include <gj/types.h>

/**
 * Probe first virtio-gpu; init control virtqueue (q0) + DRIVER_OK.
 * Returns 0 on success, -1 if no device / setup / negotiate / queue fail.
 */
int virtio_gpu_probe(void);

/**
 * Present a guest framebuffer: create/reuse resource, attach, set scanout,
 * transfer, flush. pFb must be page-table-reachable or identity-mapped guest
 * memory. Backing is walked page-by-page and coalesced into host mem_entries
 * (fragmentation cap soft-fails attach). Stride is bytes per row.
 *
 * On geometry change vs active resource, soft path recreates as needed.
 * Returns 0 on host ACK, -1 on failure (timeout / bad resp / not ready).
 */
int virtio_gpu_present(u32 u32Width, u32 u32Height, void *pFb, u32 u32Stride);

/**
 * Re-transfer + flush a dirty rectangle of the active resource without
 * re-create/attach/scanout. Coordinates are resource-relative; clipped to
 * the active geometry. Zero width or height means the full resource.
 * Requires a prior successful present (active resource id live).
 * Returns 0 on host ACK, -1 else.
 */
int virtio_gpu_flush(u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height);

/**
 * Rebind scanout to the active resource (or disable when fEnable==0).
 * Soft path helper for mode switches after present. u32ScanoutId is the
 * host scanout index from GET_DISPLAY_INFO (usually 0).
 * Returns 0 on success, -1 on fail / not ready / no active resource.
 */
int virtio_gpu_scanout(u32 u32ScanoutId, int fEnable);

/**
 * Legacy name used by smoke: present a test-pattern FB when u64GuestAddr==0,
 * else present guest memory at that address. Width/height as present().
 * Prefer virtio_gpu_present for new product code.
 */
int virtio_gpu_present_stub(u32 u32Width, u32 u32Height, u64 u64GuestAddr);

/** Lifetime successful present count since probe (greppable smoke). */
u32 virtio_gpu_present_count(void);

/** Non-zero when control queue is DRIVER_OK live. */
int virtio_gpu_ready(void);

/**
 * Query host display geometry via GET_DISPLAY_INFO soft path.
 * Picks the first enabled scanout; falls back to 1024x768 when none enabled
 * (still returns 0 so compositor can paint a default). pOutW/pOutH required.
 * Returns 0 on success, -1 if not ready / cmd fail / null outs.
 */
int virtio_gpu_get_display(u32 *pOutW, u32 *pOutH);

/**
 * Extended display query: scanout id + enabled flag from last refresh.
 * Any of pOut* may be NULL. Returns 0 on success, -1 if not ready / cmd fail.
 * Does not invent a default enabled=1 when host reports none — use
 * get_display for soft default geometry.
 */
int virtio_gpu_get_display_ex(u32 *pOutScanoutId, u32 *pOutW, u32 *pOutH,
                              u32 *pOutEnabled);
