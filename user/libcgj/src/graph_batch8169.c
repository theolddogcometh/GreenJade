/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8169: strerror_r minimum buffer size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strerror_r_min_buf_8169(void);
 *     - Return soft minimum buffer bytes for strerror_r-style fill (64).
 *   uint32_t __gj_strerror_r_min_buf_8169  (alias)
 *   __libcgj_batch8169_marker = "libcgj-batch8169"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_strerror_r_min_buf_8169 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8169_marker[] = "libcgj-batch8169";

/*
 * Soft min buffer for strerror_r-style copies of catalog strings
 * (covers "Unknown error" and typical named messages with margin).
 */
#define B8169_STRERROR_R_MIN_BUF  ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8169_min_buf(void)
{
	return B8169_STRERROR_R_MIN_BUF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strerror_r_min_buf_8169 - soft strerror_r minimum buffer size.
 *
 * Always returns 64. Catalog constant only; does not call strerror_r(3).
 * No parent wires.
 */
uint32_t
gj_strerror_r_min_buf_8169(void)
{
	(void)NULL;
	return b8169_min_buf();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strerror_r_min_buf_8169(void)
    __attribute__((alias("gj_strerror_r_min_buf_8169")));
