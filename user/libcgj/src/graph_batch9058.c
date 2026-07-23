/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9058: soft HarfBuzz shape-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_shape_ok_u_9058(void);
 *     - Returns 1 (hb_shape soft-id continuum ok). Pure-data product
 *       tag; does not run shaping or call libharfbuzz.
 *   uint32_t __gj_hb_shape_ok_u_9058  (alias)
 *   __libcgj_batch9058_marker = "libcgj-batch9058"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surface only; no multi-def. Distinct from buffer/font ok
 * flags. No parent wires. No __int128. No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9058_marker[] = "libcgj-batch9058";

#define B9058_HB_SHAPE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9058_shape_ok(void)
{
	return B9058_HB_SHAPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_shape_ok_u_9058 - HarfBuzz shape soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not run
 * hb_shape or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_shape_ok_u_9058(void)
{
	(void)NULL;
	return b9058_shape_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_shape_ok_u_9058(void)
    __attribute__((alias("gj_hb_shape_ok_u_9058")));
