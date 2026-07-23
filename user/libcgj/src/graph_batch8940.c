/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8940: input/evdev soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8940(void);
 *     - Returns the compile-time graph batch number for this TU (8940).
 *   uint32_t __gj_batch_id_8940  (alias)
 *   __libcgj_batch8940_marker = "libcgj-batch8940"
 *
 * Exclusive continuum CREATE-ONLY (8931-8940: input/evdev id stubs —
 * ev_syn_id_8931, ev_key_id_8932, ev_rel_id_8933, ev_abs_id_8934,
 * ev_msc_id_8935, ev_sw_id_8936, ev_led_id_8937, ev_rep_id_8938,
 * evdev_ready_u_8939, batch_id_8940).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No input device implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8940_marker[] = "libcgj-batch8940";

#define B8940_BATCH_ID  8940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8940_id(void)
{
	return B8940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8940 - report this TU's graph batch number.
 *
 * Always returns 8940.
 */
uint32_t
gj_batch_id_8940(void)
{
	(void)NULL;
	return b8940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8940(void)
    __attribute__((alias("gj_batch_id_8940")));
