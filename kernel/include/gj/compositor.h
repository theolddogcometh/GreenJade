/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel interim session compositor (A1 / Proton T0).
 *
 * Owns one BGRA scanout tile backed by PMM + HHDM, presented through
 * virtio-gpu. Policy ownership lives on the session door (CLAIM); this
 * module still holds the physical buffer until a full userspace
 * compositor takes over MAP_SCANOUT / PRESENT_FB.
 *
 * Invariants while ready:
 *   - session_compositor_fb() is a non-NULL HHDM pointer
 *   - width/height/stride describe a contiguous BGRA buffer (stride = w*4)
 *   - dimensions are clamped to a small bring-up tile (≤ 256)
 */
#pragma once

#include <gj/types.h>

/**
 * Allocate scanout (if needed), paint a bring-up gradient, mark ready.
 * Idempotent: returns 0 when already ready. Returns -1 if virtio-gpu or
 * PMM cannot back a tile.
 */
int  session_compositor_init(void);

/**
 * Flip current scanout to virtio-gpu.
 * Returns 0 on success, -1 if not ready or the backend rejects the frame.
 */
int  session_compositor_present(void);

/** Lifetime successful present count (kernel + PRESENT_FB via scanout). */
u32  session_compositor_present_count(void);

/** Non-zero when scanout is allocated and presentable. */
int  session_compositor_ready(void);

/**
 * Scanout virtual address, or NULL if not ready.
 * Never returns a dangling pointer after a failed init.
 */
void *session_compositor_fb(void);

/**
 * Write current width/height (0,0 if not ready). Null out-params ignored.
 */
void session_compositor_size(u32 *pW, u32 *pH);

/**
 * BGRA row stride in bytes (width * 4), or 0 if not ready.
 * Prefer this over recomputing from size so MAP_SCANOUT stays consistent.
 */
u32  session_compositor_stride(void);
