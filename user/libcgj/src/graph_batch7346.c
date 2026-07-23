/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7346: posix_fadvise advice validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_advice_ok_u_7346(uint32_t advice);
 *     - Return 1 if advice is a known POSIX_FADV_* value (0..5), else 0.
 *   uint32_t __gj_fadv_advice_ok_u_7346  (alias)
 *   __libcgj_batch7346_marker = "libcgj-batch7346"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_fadv_advice_ok_u_7346 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7346_marker[] = "libcgj-batch7346";

/*
 * Soft catalog advice values (POSIX):
 *   NORMAL=0, RANDOM=1, SEQUENTIAL=2, WILLNEED=3, DONTNEED=4, NOREUSE=5
 */
#define B7346_FADV_MAX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7346_fadv_advice_ok(uint32_t u32Advice)
{
	return (u32Advice <= B7346_FADV_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_advice_ok_u_7346 - 1 if advice is a known POSIX_FADV_*.
 *
 * advice: posix_fadvise(3) advice argument
 *
 * Returns 1 when advice is in {0,1,2,3,4,5}, else 0. Soft catalog check;
 * does not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_advice_ok_u_7346(uint32_t u32Advice)
{
	(void)NULL;
	return b7346_fadv_advice_ok(u32Advice);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_advice_ok_u_7346(uint32_t u32Advice)
    __attribute__((alias("gj_fadv_advice_ok_u_7346")));
