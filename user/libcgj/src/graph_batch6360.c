/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6360: Xbox pad continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_continuum_6360(void);
 *     - Returns 1 (soft compile-time product tag: Xbox controller
 *       feature stubs continuum 6351-6360 is complete / ready).
 *   uint32_t gj_batch_id_6360(void);
 *     - Returns the compile-time graph batch number for this TU (6360).
 *   uint32_t __gj_xpad_continuum_6360  (alias)
 *   uint32_t __gj_batch_id_6360  (alias)
 *   __libcgj_batch6360_marker = "libcgj-batch6360"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs — face_mask_6351, sys_mask_6352, dpad_hat_6353, lstick_dz_6354,
 * rstick_dz_6355, lt_scale_6356, rt_scale_6357, rumble_clamp_6358,
 * battery_ok_6359, continuum + batch_id_6360). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6360_marker[] = "libcgj-batch6360";

/* Continuum-complete lamp for the 6351-6360 exclusive wave. */
#define B6360_CONTINUUM_READY  1u
#define B6360_BATCH_ID         6360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6360_continuum(void)
{
	return B6360_CONTINUUM_READY;
}

static uint32_t
b6360_id(void)
{
	return B6360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_continuum_6360 - continuum-ready tag for 6351-6360.
 *
 * Always returns 1. Soft pure-data product tag that the Xbox controller
 * feature stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_xpad_continuum_6360(void)
{
	(void)NULL;
	return b6360_continuum();
}

/*
 * gj_batch_id_6360 - report this TU's graph batch number.
 *
 * Always returns 6360.
 */
uint32_t
gj_batch_id_6360(void)
{
	return b6360_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_xpad_continuum_6360(void)
    __attribute__((alias("gj_xpad_continuum_6360")));

uint32_t __gj_batch_id_6360(void)
    __attribute__((alias("gj_batch_id_6360")));
