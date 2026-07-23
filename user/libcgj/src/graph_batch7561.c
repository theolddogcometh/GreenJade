/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7561: timer_settime TIMER_ABSTIME flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_abstime_id_7561(void);
 *     - Return soft TIMER_ABSTIME constant (1).
 *   uint32_t __gj_ts_abstime_id_7561  (alias)
 *   __libcgj_batch7561_marker = "libcgj-batch7561"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_abstime_id_7561 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7561_marker[] = "libcgj-batch7561";

/* Soft TIMER_ABSTIME for timer_settime(2) flags (value 1). */
#define B7561_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7561_ts_abstime_id(void)
{
	return B7561_TIMER_ABSTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_abstime_id_7561 - soft TIMER_ABSTIME flag constant.
 *
 * Always returns 1 (POSIX TIMER_ABSTIME). Catalog id only; does not
 * call timer_settime. No parent wires.
 */
uint32_t
gj_ts_abstime_id_7561(void)
{
	(void)NULL;
	return b7561_ts_abstime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_abstime_id_7561(void)
    __attribute__((alias("gj_ts_abstime_id_7561")));
