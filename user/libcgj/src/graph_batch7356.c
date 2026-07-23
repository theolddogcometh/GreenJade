/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7356: posix_fadvise POSIX_FADV_NOREUSE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_noreuse_id_7356(void);
 *     - Return soft POSIX_FADV_NOREUSE constant (5).
 *   uint32_t __gj_fadv_noreuse_id_7356  (alias)
 *   __libcgj_batch7356_marker = "libcgj-batch7356"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_noreuse_id_7356 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7356_marker[] = "libcgj-batch7356";

/* POSIX_FADV_NOREUSE: data will be accessed once only (5). */
#define B7356_FADV_NOREUSE ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7356_fadv_noreuse_id(void)
{
	return B7356_FADV_NOREUSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_noreuse_id_7356 - soft POSIX_FADV_NOREUSE advice constant.
 *
 * Always returns 5 (POSIX_FADV_NOREUSE). Catalog id only; does not
 * call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_noreuse_id_7356(void)
{
	(void)NULL;
	return b7356_fadv_noreuse_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_noreuse_id_7356(void)
    __attribute__((alias("gj_fadv_noreuse_id_7356")));
