/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9060: harfbuzz soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9060(void);
 *     - Returns the compile-time graph batch number for this TU (9060).
 *   uint32_t __gj_batch_id_9060  (alias)
 *   __libcgj_batch9060_marker = "libcgj-batch9060"
 *
 * Exclusive continuum CREATE-ONLY (9051-9060: harfbuzz soft id stubs —
 * hb_script_latn_id_9051, hb_script_arab_id_9052,
 * hb_script_hani_id_9053, hb_dir_ltr_id_9054, hb_dir_rtl_id_9055,
 * hb_buffer_ok_u_9056, hb_font_ok_u_9057, hb_shape_ok_u_9058,
 * harfbuzz_ready_u_9059, batch_id_9060).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9060_marker[] = "libcgj-batch9060";

#define B9060_BATCH_ID  9060u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9060_id(void)
{
	return B9060_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9060 - report this TU's graph batch number.
 *
 * Always returns 9060. Link-time presence tags the exclusive
 * harfbuzz soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9060(void)
{
	(void)NULL;
	return b9060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9060(void)
    __attribute__((alias("gj_batch_id_9060")));
