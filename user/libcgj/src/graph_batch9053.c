/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9053: soft HarfBuzz Han (Hani) script id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_script_hani_id_9053(void);
 *     - Returns 0x48616E69 (HB_TAG 'Hani' soft catalog value). Pure-data
 *       script id stub; does not shape text or call libharfbuzz.
 *   uint32_t __gj_hb_script_hani_id_9053  (alias)
 *   __libcgj_batch9053_marker = "libcgj-batch9053"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surface only; no multi-def. Distinct from
 * gj_hb_script_latn_id_9051 / gj_hb_script_arab_id_9052. No parent
 * wires. No __int128. No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9053_marker[] = "libcgj-batch9053";

/* Soft HarfBuzz Han script catalog id (HB_TAG('H','a','n','i')). */
#define B9053_HB_HANI_ID  0x48616E69u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9053_hani_id(void)
{
	return B9053_HB_HANI_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_script_hani_id_9053 - soft HarfBuzz Han (Hani) script catalog id.
 *
 * Always returns 0x48616E69. Soft continuum constant; does not shape
 * text or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_script_hani_id_9053(void)
{
	(void)NULL;
	return b9053_hani_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_script_hani_id_9053(void)
    __attribute__((alias("gj_hb_script_hani_id_9053")));
