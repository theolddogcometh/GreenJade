/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel interim session compositor (A1 / Proton T0).
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * Owns one BGRA scanout tile backed by PMM + HHDM, presented through
 * virtio-gpu. Policy ownership lives on the session door (CLAIM); this
 * module still holds the physical buffer until a full userspace
 * compositor takes over MAP_SCANOUT / PRESENT_FB.
 *
 * Soft multi-frame (v1 deepen, no redesign):
 *   - One physical scanout; a soft double-buffer index (0/1) flips on each
 *     successful present for swapchain-shaped sequencing (metadata only).
 *   - Frame generation bumps on each ok present (wrap OK).
 *   - session_compositor_present_n() drains up to N soft frames in one
 *     call (capped by GJ_COMP_MULTI_MAX) for multi-frame bring-up without
 *     a second alloc. Stops the soft batch on first backend reject.
 *
 * Geometry (compositor.c):
 *   clamp ≤ GJ_COMP_MAX_DIM (256); fallback 64×64; min 32×32
 *   stride = width * 4 (BGRA); prefer session_compositor_stride() for
 *   MAP_SCANOUT consistency over recomputing from size
 *
 * Invariants while ready:
 *   - session_compositor_fb() is a non-NULL HHDM pointer
 *   - width/height/stride describe a contiguous BGRA buffer (stride = w*4)
 *   - never returns a dangling FB pointer after a failed init
 *
 * Greppable:
 *   compositor: scanout … ready (multi-frame soft)
 *   compositor: multi-frame soft
 *   compositor multi-frame soft
 */
#pragma once

#include <gj/types.h>

/** Soft multi-frame present batch ceiling (per present_n call). */
#define GJ_COMP_MULTI_MAX 8u

/**
 * Allocate scanout (if needed), paint a bring-up gradient, mark ready.
 * Idempotent: returns 0 when already ready. Returns -1 if virtio-gpu or
 * PMM cannot back a tile.
 */
int  session_compositor_init(void);

/**
 * Flip current scanout to virtio-gpu.
 * Returns 0 on success, -1 if not ready or the backend rejects the frame.
 * Soft multi-frame: bumps frame gen + soft buffer index on success.
 */
int  session_compositor_present(void);

/**
 * Soft multi-frame present: flip up to @u32N times (clamped to
 * GJ_COMP_MULTI_MAX, minimum 1). Returns 0 if at least one frame ok,
 * -1 if none succeeded. Lifetime present_count increases per ok frame.
 * greppable: compositor multi-frame soft
 */
int  session_compositor_present_n(u32 u32N);

/** Lifetime successful present count (kernel + PRESENT_FB via scanout). */
u32  session_compositor_present_count(void);

/**
 * Soft multi-frame: presents after the first successful one (0 until
 * the second ok present). Sticky for diagnostics.
 */
u32  session_compositor_multi_count(void);

/** Soft frame generation (bumps on each ok present; wrap OK). */
u32  session_compositor_frame_gen(void);

/** Soft double-buffer index (0/1), metadata only — one physical scanout. */
u32  session_compositor_soft_index(void);

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
