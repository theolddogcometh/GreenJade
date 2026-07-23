/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7354: posix_fadvise POSIX_FADV_WILLNEED id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_willneed_id_7354(void);
 *     - Return soft POSIX_FADV_WILLNEED constant (3).
 *   uint32_t __gj_fadv_willneed_id_7354  (alias)
 *   __libcgj_batch7354_marker = "libcgj-batch7354"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_willneed_id_7354 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7354_marker[] = "libcgj-batch7354";

/* POSIX_FADV_WILLNEED: will need these pages soon (3). */
#define B7354_FADV_WILLNEED ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7354_fadv_willneed_id(void)
{
	return B7354_FADV_WILLNEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_willneed_id_7354 - soft POSIX_FADV_WILLNEED advice constant.
 *
 * Always returns 3 (POSIX_FADV_WILLNEED). Catalog id only; does not
 * call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_willneed_id_7354(void)
{
	(void)NULL;
	return b7354_fadv_willneed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_willneed_id_7354(void)
    __attribute__((alias("gj_fadv_willneed_id_7354")));
