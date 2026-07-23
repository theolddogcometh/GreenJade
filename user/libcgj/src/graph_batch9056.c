/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9056: soft HarfBuzz buffer-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hb_buffer_ok_u_9056(void);
 *     - Returns 1 (hb_buffer soft-id continuum ok). Pure-data product
 *       tag; does not allocate buffers or call libharfbuzz.
 *   uint32_t __gj_hb_buffer_ok_u_9056  (alias)
 *   __libcgj_batch9056_marker = "libcgj-batch9056"
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

const char __libcgj_batch9056_marker[] = "libcgj-batch9056";

#define B9056_HB_BUFFER_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9056_buffer_ok(void)
{
	return B9056_HB_BUFFER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hb_buffer_ok_u_9056 - HarfBuzz buffer soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not allocate
 * hb_buffer objects or call libharfbuzz. No parent wires.
 */
uint32_t
gj_hb_buffer_ok_u_9056(void)
{
	(void)NULL;
	return b9056_buffer_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hb_buffer_ok_u_9056(void)
    __attribute__((alias("gj_hb_buffer_ok_u_9056")));
