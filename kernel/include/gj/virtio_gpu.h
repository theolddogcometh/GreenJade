/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu control queue + 2D present (OASIS; Proton A1 T0).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Soft path (control q0, 2D only):
 *   display  — GET_DISPLAY_INFO, first enabled scanout (default 1024x768)
 *   resource — CREATE_2D / ATTACH_BACKING (page-walk+coalesce) / DETACH / UNREF
 *   scanout  — SET_SCANOUT (enable) / resource_id 0 (disable)
 *   flush    — TRANSFER_TO_HOST_2D + RESOURCE_FLUSH (full or dirty rect)
 *
 * Greppable product markers (prefix-stable):
 *   virtio-gpu: ready PASS
 *   virtio-gpu: present PASS
 */
#pragma once

#include <gj/types.h>

/** Probe + init control virtqueue. Returns 0 on success. */
int virtio_gpu_probe(void);

/**
 * Present a guest framebuffer: create/reuse resource, attach, set scanout,
 * transfer, flush. pFb must be page-table-reachable or identity-mapped guest
 * memory. Backing is walked page-by-page and coalesced into host mem_entries.
 * Returns 0 on host ACK, -1 on failure.
 */
int virtio_gpu_present(u32 u32Width, u32 u32Height, void *pFb, u32 u32Stride);

/**
 * Re-transfer + flush a dirty rectangle of the active resource without
 * re-create/attach/scanout. Coordinates are resource-relative; clipped to
 * the active geometry. Zero width or height means the full resource.
 * Requires a prior successful present. Returns 0 on host ACK, -1 else.
 */
int virtio_gpu_flush(u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height);

/**
 * Rebind scanout to the active resource (or disable when fEnable==0).
 * Soft path helper for mode switches after present. Returns 0 / -1.
 */
int virtio_gpu_scanout(u32 u32ScanoutId, int fEnable);

/** Legacy name used by smoke (test-pattern FB when u64GuestAddr==0). */
int virtio_gpu_present_stub(u32 u32Width, u32 u32Height, u64 u64GuestAddr);

u32 virtio_gpu_present_count(void);
int virtio_gpu_ready(void);

/**
 * Query host display geometry via GET_DISPLAY_INFO soft path.
 * Picks the first enabled scanout; falls back to 1024x768 when none.
 */
int virtio_gpu_get_display(u32 *pOutW, u32 *pOutH);

/**
 * Extended display query: scanout id + enabled flag from last refresh.
 * pOut* may be NULL. Returns 0 on success, -1 if not ready / cmd fail.
 */
int virtio_gpu_get_display_ex(u32 *pOutScanoutId, u32 *pOutW, u32 *pOutH,
                              u32 *pOutEnabled);
