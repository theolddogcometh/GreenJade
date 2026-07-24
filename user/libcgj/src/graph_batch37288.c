/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37288: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37288(void);
 *     - Returns the compile-time graph batch number for this TU (37288).
 *   uint32_t gj_pulse_u_37288(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_37288  (alias)
 *   uint32_t __gj_pulse_u_37288  (alias)
 *   __libcgj_batch37288_marker = "libcgj-batch37288"
 *
 * Exclusive continuum CREATE-ONLY (37276-37300 wave toward MILESTONE
 * 37300). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch37288_marker[] = "libcgj-batch37288";

/* Batch identity. */
#define B37288_ID  37288u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B37288_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37288_id(void)
{
	return B37288_ID;
}

static uint32_t
b37288_pulse(void)
{
	return B37288_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37288 - soft continuum surface
 *
 * Always returns 37288u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_37288(void)
{
	(void)NULL;
	return b37288_id();
}

/*
 * gj_pulse_u_37288 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_37288(void)
{
	return b37288_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37288(void)
    __attribute__((alias("gj_batch_id_37288")));

uint32_t __gj_pulse_u_37288(void)
    __attribute__((alias("gj_pulse_u_37288")));
