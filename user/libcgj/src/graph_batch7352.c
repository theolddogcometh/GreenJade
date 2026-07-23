/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7352: posix_fadvise POSIX_FADV_RANDOM id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_random_id_7352(void);
 *     - Return soft POSIX_FADV_RANDOM constant (1).
 *   uint32_t __gj_fadv_random_id_7352  (alias)
 *   __libcgj_batch7352_marker = "libcgj-batch7352"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_random_id_7352 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7352_marker[] = "libcgj-batch7352";

/* POSIX_FADV_RANDOM: expect random page references (1). */
#define B7352_FADV_RANDOM ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7352_fadv_random_id(void)
{
	return B7352_FADV_RANDOM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_random_id_7352 - soft POSIX_FADV_RANDOM advice constant.
 *
 * Always returns 1 (POSIX_FADV_RANDOM). Catalog id only; does not
 * call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_random_id_7352(void)
{
	(void)NULL;
	return b7352_fadv_random_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_random_id_7352(void)
    __attribute__((alias("gj_fadv_random_id_7352")));
