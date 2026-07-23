/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7359: posix_fadvise advice errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_advice_errorish_u_7359(uint32_t advice);
 *     - Return 1 if advice is outside the known POSIX_FADV_* range
 *       (0..5), else 0.
 *   uint32_t __gj_fadv_advice_errorish_u_7359  (alias)
 *   __libcgj_batch7359_marker = "libcgj-batch7359"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_advice_errorish_u_7359 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7359_marker[] = "libcgj-batch7359";

/*
 * Known soft posix_fadvise advice range:
 *   NORMAL(0) .. NOREUSE(5).
 */
#define B7359_FADV_MAX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7359_fadv_advice_errorish(uint32_t u32Advice)
{
	return (u32Advice > B7359_FADV_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_advice_errorish_u_7359 - 1 if advice is unknown / invalid.
 *
 * advice: posix_fadvise(3) advice argument (soft catalog)
 *
 * Returns 1 when advice > 5 (outside NORMAL..NOREUSE), else 0. Pure
 * integer check; does not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_advice_errorish_u_7359(uint32_t u32Advice)
{
	(void)NULL;
	return b7359_fadv_advice_errorish(u32Advice);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_advice_errorish_u_7359(uint32_t u32Advice)
    __attribute__((alias("gj_fadv_advice_errorish_u_7359")));
