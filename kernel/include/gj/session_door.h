/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session door — userspace sessiond hand-off surface (A1).
 *
 * Dispatched via GJ_SYS_SESSION (arg0 = opcode). Ownership token 0 means
 * the kernel interim owns policy; a non-zero claim means sessiond owns
 * scanout policy. Claim is idempotent for the same token; a different
 * token returns GJ_ERR_BUSY.
 *
 * User pointers: copy_{to,from}_user when the range is in the user VA
 * window; early kernel smokes may pass HHDM/static buffers.
 */
#pragma once

#include <gj/types.h>

/* Session door opcodes (arg0 when using GJ_SYS_SESSION) */
#define GJ_SESS_OP_PRESENT      1u /* present current scanout */
#define GJ_SESS_OP_DISPLAY_INFO 2u /* arg1 → user u32[2] {w, h} */
#define GJ_SESS_OP_INPUT_POLL   3u /* drain virtio-input into hub */
#define GJ_SESS_OP_INPUT_POP    4u /* arg1 → gj_input_event; ret 1/0 */
/**
 * STATS: arg1 → user u32[5]
 *   [0] compositor presents (lifetime success)
 *   [1] input events pushed (lifetime)
 *   [2] door call count
 *   [3] flags: bit0 compositor ready, bit1 input ready, bit2 owned,
 *              bits 8..15 pending input count (0..255)
 *   [4] owner token (0 = kernel interim)
 */
#define GJ_SESS_OP_STATS        5u
/**
 * Present user framebuffer: arg1=w arg2=h arg3=user BGRA ptr (stride=w*4).
 * Clips to scanout; blits top-left with correct source/dest strides.
 */
#define GJ_SESS_OP_PRESENT_FB   6u
/** Claim ownership (sessiond): arg1=token (non-zero, 32-bit). */
#define GJ_SESS_OP_CLAIM        7u
/** Release ownership: arg1=token matching claim. */
#define GJ_SESS_OP_RELEASE      8u
/**
 * Map scanout FB (interim VA hint, not a true userspace map):
 *   arg1 → user u64 VA hint
 *   arg2 → user u32[3] {w, h, stride_bytes}
 */
#define GJ_SESS_OP_MAP_SCANOUT  9u

void session_door_init(void);

/** Dispatch session opcode; returns 0 / 1 / negative GJ_ERR_*. */
i64 session_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero if userspace sessiond has claimed ownership. */
int session_door_owned(void);

/** Current owner token, or 0 if kernel interim owns policy. */
u32 session_door_owner_token(void);
