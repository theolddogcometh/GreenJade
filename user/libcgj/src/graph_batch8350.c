/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8350: time/clock unit stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8350(void);
 *     - Returns the compile-time graph batch number for this TU (8350).
 *   uint32_t __gj_batch_id_8350  (alias)
 *   __libcgj_batch8350_marker = "libcgj-batch8350"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs —
 * sec_ok, nsec_ok, timespec_norm, ms_to_ns, us_to_ns, ns_to_ms,
 * clock_mono_id, clock_realtime_id, time_epoch_id, batch_id). Unique
 * gj_batch_id_8350 surface only; does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8350_marker[] = "libcgj-batch8350";

#define B8350_BATCH_ID  8350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8350_id(void)
{
	return B8350_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8350 - report this TU's graph batch number.
 *
 * Always returns 8350. Soft continuum lamp for the 8341-8350
 * time/clock unit stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_8350(void)
{
	(void)NULL;
	return b8350_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8350(void)
    __attribute__((alias("gj_batch_id_8350")));
