/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7349: msync flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_sync_flags_ok_u_7349(uint32_t flags);
 *     - Return 1 if flags uses only known MS_* bits
 *       (0x1|0x2|0x4), else 0.
 *   uint32_t __gj_ms_sync_flags_ok_u_7349  (alias)
 *   __libcgj_batch7349_marker = "libcgj-batch7349"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_ms_sync_flags_ok_u_7349 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7349_marker[] = "libcgj-batch7349";

/*
 * Known msync flag bits for this continuum:
 *   MS_ASYNC (0x1) | MS_INVALIDATE (0x2) | MS_SYNC (0x4) = 0x7
 */
#define B7349_MS_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7349_ms_sync_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7349_MS_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_sync_flags_ok_u_7349 - 1 if flags has no unknown bits.
 *
 * flags: msync(2) flags argument
 *
 * Returns 1 when only bits in {0x1, 0x2, 0x4} are set (including 0),
 * else 0. Soft catalog check; does not call msync. No parent wires.
 */
uint32_t
gj_ms_sync_flags_ok_u_7349(uint32_t u32Flags)
{
	(void)NULL;
	return b7349_ms_sync_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_sync_flags_ok_u_7349(uint32_t u32Flags)
    __attribute__((alias("gj_ms_sync_flags_ok_u_7349")));
