/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7357: posix_fadvise advice validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_advice_ok_u_7357(uint32_t advice);
 *     - Return 1 if advice is a known POSIX_FADV_* value (0..5), else 0.
 *   uint32_t __gj_fadv_advice_ok_u_7357  (alias)
 *   __libcgj_batch7357_marker = "libcgj-batch7357"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_advice_ok_u_7357 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7357_marker[] = "libcgj-batch7357";

/*
 * Known soft posix_fadvise advice range:
 *   NORMAL(0) .. NOREUSE(5). Values outside this range are invalid.
 */
#define B7357_FADV_MAX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7357_fadv_advice_ok(uint32_t u32Advice)
{
	return (u32Advice <= B7357_FADV_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_advice_ok_u_7357 - 1 if advice is a known POSIX_FADV_* value.
 *
 * advice: posix_fadvise(3) advice argument (soft catalog 0..5)
 *
 * Returns 1 when advice is in [0, 5] inclusive, else 0. Pure integer
 * check; does not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_advice_ok_u_7357(uint32_t u32Advice)
{
	(void)NULL;
	return b7357_fadv_advice_ok(u32Advice);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_advice_ok_u_7357(uint32_t u32Advice)
    __attribute__((alias("gj_fadv_advice_ok_u_7357")));
