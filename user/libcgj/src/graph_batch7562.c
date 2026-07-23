/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7562: timer_settime relative flags id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_relative_id_7562(void);
 *     - Return soft relative-time flags value (0).
 *   uint32_t __gj_ts_relative_id_7562  (alias)
 *   __libcgj_batch7562_marker = "libcgj-batch7562"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_relative_id_7562 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7562_marker[] = "libcgj-batch7562";

/* Soft relative-time flags for timer_settime(2) (value 0). */
#define B7562_TIMER_RELATIVE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7562_ts_relative_id(void)
{
	return B7562_TIMER_RELATIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_relative_id_7562 - soft relative-time flags constant.
 *
 * Always returns 0 (it_value/it_interval are relative to now). Catalog
 * id only; does not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_relative_id_7562(void)
{
	(void)NULL;
	return b7562_ts_relative_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_relative_id_7562(void)
    __attribute__((alias("gj_ts_relative_id_7562")));
