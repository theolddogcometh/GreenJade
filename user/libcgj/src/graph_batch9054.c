/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9054: soft HarfBuzz LTR direction id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_dir_ltr_id_9054(void);
 *     - Returns 4 (HB_DIRECTION_LTR soft catalog value). Pure-data
 *       direction id stub; does not set buffer direction or call
 *       libharfbuzz.
 *   uint32_t __gj_hb_dir_ltr_id_9054  (alias)
 *   __libcgj_batch9054_marker = "libcgj-batch9054"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9054_marker[] = "libcgj-batch9054";

/* Soft HarfBuzz LTR direction catalog id (HB_DIRECTION_LTR = 4). */
#define B9054_HB_DIR_LTR  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9054_dir_ltr(void)
{
	return B9054_HB_DIR_LTR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_dir_ltr_id_9054 - soft HarfBuzz LTR direction catalog id.
 *
 * Always returns 4. Soft continuum constant; does not set buffer
 * direction or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_dir_ltr_id_9054(void)
{
	(void)NULL;
	return b9054_dir_ltr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_dir_ltr_id_9054(void)
    __attribute__((alias("gj_hb_dir_ltr_id_9054")));
