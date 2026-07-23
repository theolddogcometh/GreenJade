/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6250: color-space continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cs_bt_continuum_6250(void);
 *     - Returns 1 (soft compile-time product tag: color space BT.709 /
 *       BT.2020 stubs continuum 6241-6250 is complete / ready).
 *   uint32_t gj_batch_id_6250(void);
 *     - Returns the compile-time graph batch number for this TU (6250).
 *   uint32_t __gj_cs_bt_continuum_6250  (alias)
 *   uint32_t __gj_batch_id_6250  (alias)
 *   __libcgj_batch6250_marker = "libcgj-batch6250"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs — bt709_luma8_6241, bt2020_luma8_6242, bt709_cb_u8_6243,
 * bt2020_cb_u8_6244, bt709_cr_u8_6245, bt2020_cr_u8_6246,
 * y_limited8_ok_6247, c_limited8_ok_6248, matrix_id_ok_6249,
 * continuum + batch_id_6250). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6250_marker[] = "libcgj-batch6250";

/* Continuum-complete lamp for the 6241-6250 exclusive wave. */
#define B6250_CONTINUUM_READY  1u
#define B6250_BATCH_ID         6250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6250_continuum(void)
{
	return B6250_CONTINUUM_READY;
}

static uint32_t
b6250_id(void)
{
	return B6250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt_continuum_6250 - continuum-ready tag for 6241-6250.
 *
 * Always returns 1. Soft pure-data product tag that the color space
 * BT.709 / BT.2020 stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_cs_bt_continuum_6250(void)
{
	(void)NULL;
	return b6250_continuum();
}

/*
 * gj_batch_id_6250 - report this TU's graph batch number.
 *
 * Always returns 6250.
 */
uint32_t
gj_batch_id_6250(void)
{
	return b6250_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_cs_bt_continuum_6250(void)
    __attribute__((alias("gj_cs_bt_continuum_6250")));

uint32_t __gj_batch_id_6250(void)
    __attribute__((alias("gj_batch_id_6250")));
