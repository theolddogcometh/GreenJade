/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9055: soft HarfBuzz RTL direction id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_dir_rtl_id_9055(void);
 *     - Returns 5 (HB_DIRECTION_RTL soft catalog value). Pure-data
 *       direction id stub; does not set buffer direction or call
 *       libharfbuzz.
 *   uint32_t __gj_hb_dir_rtl_id_9055  (alias)
 *   __libcgj_batch9055_marker = "libcgj-batch9055"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surface only; no multi-def. Distinct from
 * gj_hb_dir_ltr_id_9054 (batch9054). No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9055_marker[] = "libcgj-batch9055";

/* Soft HarfBuzz RTL direction catalog id (HB_DIRECTION_RTL = 5). */
#define B9055_HB_DIR_RTL  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9055_dir_rtl(void)
{
	return B9055_HB_DIR_RTL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_dir_rtl_id_9055 - soft HarfBuzz RTL direction catalog id.
 *
 * Always returns 5. Soft continuum constant; does not set buffer
 * direction or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_dir_rtl_id_9055(void)
{
	(void)NULL;
	return b9055_dir_rtl();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_dir_rtl_id_9055(void)
    __attribute__((alias("gj_hb_dir_rtl_id_9055")));
