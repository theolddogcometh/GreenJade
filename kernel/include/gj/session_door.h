/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session door — userspace sessiond hand-off surface (A1 / Proton T0).
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * Dispatched via GJ_SYS_SESSION (arg0 = opcode). Ownership token 0 means
 * the kernel interim owns policy; a non-zero claim means sessiond owns
 * scanout policy. The physical BGRA tile still lives in compositor until
 * a full userspace compositor takes over MAP_SCANOUT / PRESENT_FB.
 *
 * Dispatch contract:
 *   session_door_call(op, arg1, arg2, arg3) → i64
 *   Success: 0, 1 (INPUT_POP filled), or soft positive counts
 *   Errors:  negative GJ_ERR_* (BUSY / FAULT / INVAL / NODEV soft)
 *
 * Soft paths (bring-up / product smoke):
 *   CLAIM is idempotent for the same 32-bit token (reclaim soft);
 *   a different token returns GJ_ERR_BUSY. RELEASE when free is soft 0.
 *   PRESENT / PRESENT_FB remain usable without claim for smokes; sessiond
 *   prefers the owned PRESENT_FB path. Multi-frame soft: consecutive
 *   successful PRESENT_FB is counted (frame gen via compositor; STATS bit18).
 *   INPUT_POLL/POP soft-skip cleanly when virtio-input is absent (empty ring).
 *   Dimensions clamped to compositor bring-up tile (≤ 256; session door
 *   matches GJ_SESS_MAX_DIM).
 *
 * User pointers: copy_{to,from}_user when the range is in the user VA
 * window; early kernel smokes may pass HHDM/static buffers.
 *
 * Greppable product markers (keep ABI stable):
 *   session_door: PASS / ownership PASS
 *   session_door claim soft
 *   compositor multi-frame soft (via PRESENT_FB / present_n)
 */
#pragma once

#include <gj/types.h>

/* Session door opcodes (arg0 when using GJ_SYS_SESSION) */
#define GJ_SESS_OP_PRESENT      1u /* present current scanout (compositor) */
#define GJ_SESS_OP_DISPLAY_INFO 2u /* arg1 → user u32[2] {w, h} */
#define GJ_SESS_OP_INPUT_POLL   3u /* drain virtio-input into hub */
#define GJ_SESS_OP_INPUT_POP    4u /* arg1 → gj_input_event; ret 1/0 */
/**
 * STATS: arg1 → user u32[5]
 *   [0] compositor presents (lifetime success)
 *   [1] input events pushed (lifetime)
 *   [2] door call count
 *   [3] flags: bit0 compositor ready, bit1 input ready, bit2 owned,
 *              bits 8..15 pending input count (0..255),
 *              bit16 sticky input drop observed,
 *              bit17 any user PRESENT_FB success,
 *              bit18 multi-frame soft (user presents ≥ 2),
 *              bit19 reclaim soft observed (same-token re-CLAIM)
 *   [4] owner token (0 = kernel interim)
 * Wire size stays 5 for sessiond / smoke ABI stability.
 */
#define GJ_SESS_OP_STATS        5u
/**
 * Present user framebuffer: arg1=w arg2=h arg3=user BGRA ptr (stride=w*4).
 * Clips to scanout; blits top-left with correct source/dest strides.
 * Soft multi-frame: successive successful PRESENT_FB bump user present
 * count and compositor frame gen (second+ frames set STATS bit18).
 */
#define GJ_SESS_OP_PRESENT_FB   6u
/**
 * Claim ownership (sessiond): arg1=token (non-zero, 32-bit). Soft reclaim
 * for same token; BUSY for different owner; INVAL for token 0.
 */
#define GJ_SESS_OP_CLAIM        7u
/** Release ownership: arg1=token matching claim. Soft free when unowned. */
#define GJ_SESS_OP_RELEASE      8u
/**
 * Map scanout FB (interim VA hint, not a true userspace map):
 *   arg1 → user u64 VA hint
 *   arg2 → user u32[3] {w, h, stride_bytes}
 * Soft: reports compositor geometry; full map is product follow-on.
 */
#define GJ_SESS_OP_MAP_SCANOUT  9u

/**
 * Init door ownership/soft counters. Safe to re-call.
 * Does not allocate scanout (session_compositor_init owns that).
 */
void session_door_init(void);

/** Dispatch session opcode; returns 0 / 1 / negative GJ_ERR_*. */
i64 session_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero if userspace sessiond has claimed ownership. */
int session_door_owned(void);

/** Current owner token, or 0 if kernel interim owns policy. */
u32 session_door_owner_token(void);

/**
 * Soft diagnostics: successful first claims + idempotent reclaims.
 * greppable: session_door claim soft
 */
u32 session_door_claim_count(void);

/** Lifetime successful user PRESENT_FB count (multi-frame soft base). */
u32 session_door_user_presents(void);
