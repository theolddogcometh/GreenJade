/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7355: posix_fadvise POSIX_FADV_DONTNEED id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fadv_dontneed_id_7355(void);
 *     - Return soft POSIX_FADV_DONTNEED constant (4).
 *   uint32_t __gj_fadv_dontneed_id_7355  (alias)
 *   __libcgj_batch7355_marker = "libcgj-batch7355"
 *
 * Exclusive continuum CREATE-ONLY (7351-7360: posix_fadvise advise stubs —
 * normal_id, random_id, sequential_id, willneed_id, dontneed_id,
 * noreuse_id, advice_ok, advice_max, advice_errorish, batch_id_7360).
 * Unique gj_fadv_dontneed_id_7355 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7355_marker[] = "libcgj-batch7355";

/* POSIX_FADV_DONTNEED: data will not be accessed soon (4). */
#define B7355_FADV_DONTNEED ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7355_fadv_dontneed_id(void)
{
	return B7355_FADV_DONTNEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fadv_dontneed_id_7355 - soft POSIX_FADV_DONTNEED advice constant.
 *
 * Always returns 4 (POSIX_FADV_DONTNEED). Catalog id only; does not
 * call posix_fadvise. No parent wires.
 */
uint32_t
gj_fadv_dontneed_id_7355(void)
{
	(void)NULL;
	return b7355_fadv_dontneed_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fadv_dontneed_id_7355(void)
    __attribute__((alias("gj_fadv_dontneed_id_7355")));
