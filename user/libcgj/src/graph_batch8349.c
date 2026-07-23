/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8349: Unix epoch origin identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_time_epoch_id_8349(void);
 *     - Returns 0: soft Unix-epoch origin tag (1970-01-01T00:00:00Z
 *       as the zero point of CLOCK_REALTIME wall time).
 *   uint32_t __gj_time_epoch_id_8349  (alias)
 *   __libcgj_batch8349_marker = "libcgj-batch8349"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Unique gj_time_epoch_id_8349 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8349_marker[] = "libcgj-batch8349";

/* Soft Unix-epoch origin tag (tv_sec == 0 at epoch). */
#define B8349_TIME_EPOCH_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8349_epoch_id(void)
{
	return B8349_TIME_EPOCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_epoch_id_8349 - Unix epoch origin identity tag.
 *
 * Always returns 0. Soft pure data product tag for the continuum
 * (epoch as zero origin of wall-clock seconds). No parent wires.
 */
uint32_t
gj_time_epoch_id_8349(void)
{
	(void)NULL;
	return b8349_epoch_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_time_epoch_id_8349(void)
    __attribute__((alias("gj_time_epoch_id_8349")));
