/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7353: posix_fadvise POSIX_FADV_SEQUENTIAL id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_sequential_id_7353(void);
 *     - Return soft POSIX_FADV_SEQUENTIAL constant (2).
 *   uint32_t __gj_fadv_sequential_id_7353  (alias)
 *   __libcgj_batch7353_marker = "libcgj-batch7353"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_sequential_id_7353 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7353_marker[] = "libcgj-batch7353";

/* POSIX_FADV_SEQUENTIAL: expect sequential page references (2). */
#define B7353_FADV_SEQUENTIAL ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7353_fadv_sequential_id(void)
{
	return B7353_FADV_SEQUENTIAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_sequential_id_7353 - soft POSIX_FADV_SEQUENTIAL advice constant.
 *
 * Always returns 2 (POSIX_FADV_SEQUENTIAL). Catalog id only; does not
 * call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_sequential_id_7353(void)
{
	(void)NULL;
	return b7353_fadv_sequential_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_sequential_id_7353(void)
    __attribute__((alias("gj_fadv_sequential_id_7353")));
