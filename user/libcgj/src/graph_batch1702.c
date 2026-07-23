/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1702: FNV-1a 64-bit stream finalizer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fnv1a64_final(uint64_t h);
 *     — Identity finalize: FNV-1a has no padding or residual block;
 *       the running state from gj_fnv1a64_update is already complete.
 *   uint64_t __gj_fnv1a64_final  (alias)
 *   __libcgj_batch1702_marker = "libcgj-batch1702"
 *
 * Companion to gj_fnv1a64_update (batch1701). Does NOT redefine
 * gj_fnv1a64 / fnv1a_64 / gj_fnv1a64_update.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1702_marker[] = "libcgj-batch1702";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1702_identity(uint64_t u64H)
{
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fnv1a64_final — finalize a streaming FNV-1a 64-bit hash.
 *
 * u64H: running state from gj_fnv1a64_update (or offset basis if empty).
 *
 * Returns u64H unchanged (FNV-1a needs no final mix).
 */
uint64_t
gj_fnv1a64_final(uint64_t u64H)
{
	return b1702_identity(u64H);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fnv1a64_final(uint64_t u64H)
    __attribute__((alias("gj_fnv1a64_final")));
