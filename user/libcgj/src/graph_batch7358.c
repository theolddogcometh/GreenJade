/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7358: posix_fadvise max known advice id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_advice_max_id_7358(void);
 *     - Return soft max known POSIX_FADV_* value (5 / NOREUSE).
 *   uint32_t __gj_fadv_advice_max_id_7358  (alias)
 *   __libcgj_batch7358_marker = "libcgj-batch7358"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_advice_max_id_7358 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7358_marker[] = "libcgj-batch7358";

/* Max known soft POSIX_FADV_* value (POSIX_FADV_NOREUSE). */
#define B7358_FADV_MAX ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7358_fadv_advice_max_id(void)
{
	return B7358_FADV_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_advice_max_id_7358 - soft max known posix_fadvise advice value.
 *
 * Always returns 5 (POSIX_FADV_NOREUSE upper bound of the known catalog).
 * Catalog bound only; does not call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_advice_max_id_7358(void)
{
	(void)NULL;
	return b7358_fadv_advice_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_advice_max_id_7358(void)
    __attribute__((alias("gj_fadv_advice_max_id_7358")));
