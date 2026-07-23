/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu control queue + 2D present (OASIS; Proton A1 T0).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
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
 * Present a guest framebuffer: create/reuse resource, attach, set scanout, flush.
 * pFb must be identity-mapped or page-table-reachable guest memory.
 * Returns 0 on host ACK, -1 on failure.
 */
int virtio_gpu_present(u32 u32Width, u32 u32Height, void *pFb, u32 u32Stride);

/** Legacy name used by smoke (test-pattern FB when u64GuestAddr==0). */
int virtio_gpu_present_stub(u32 u32Width, u32 u32Height, u64 u64GuestAddr);

u32 virtio_gpu_present_count(void);
int virtio_gpu_ready(void);

/** Query host display 0 enabled size (GET_DISPLAY_INFO). */
int virtio_gpu_get_display(u32 *pOutW, u32 *pOutH);
