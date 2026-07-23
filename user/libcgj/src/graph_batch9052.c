/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9052: soft HarfBuzz Arabic script id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_script_arab_id_9052(void);
 *     - Returns 0x41726162 (HB_TAG 'Arab' soft catalog value). Pure-data
 *       script id stub; does not shape text or call libharfbuzz.
 *   uint32_t __gj_hb_script_arab_id_9052  (alias)
 *   __libcgj_batch9052_marker = "libcgj-batch9052"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surface only; no multi-def. Distinct from
 * gj_hb_script_latn_id_9051 (batch9051). No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9052_marker[] = "libcgj-batch9052";

/* Soft HarfBuzz Arabic script catalog id (HB_TAG('A','r','a','b')). */
#define B9052_HB_ARAB_ID  0x41726162u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9052_arab_id(void)
{
	return B9052_HB_ARAB_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_script_arab_id_9052 - soft HarfBuzz Arabic script catalog id.
 *
 * Always returns 0x41726162. Soft continuum constant; does not shape
 * text or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_script_arab_id_9052(void)
{
	(void)NULL;
	return b9052_arab_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_script_arab_id_9052(void)
    __attribute__((alias("gj_hb_script_arab_id_9052")));
