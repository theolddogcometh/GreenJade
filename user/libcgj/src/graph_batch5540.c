/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5540: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5540(void);
 *     - Returns the compile-time graph batch number for this TU (5540).
 *   uint32_t __gj_batch_id_5540  (alias)
 *   __libcgj_batch5540_marker = "libcgj-batch5540"
 *
 * Exclusive continuum CREATE-ONLY (5531-5540: gamescope compositor —
 * gscope_comp_stub_5531, nested_refresh_hz_5532, fsr_sharpness_5533,
 * integer_scale_5534, deck_ui_chrome_5535, gscope_layer_z_5536,
 * nested_refresh_us_5537, fsr_sharp_q8_5538, integer_scale_fit_5539,
 * batch_id_5540). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5540_marker[] = "libcgj-batch5540";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5540_id(void)
{
	return 5540u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5540 - report this TU's graph batch number.
 *
 * Always returns 5540. Closes the 5531-5540 gamescope compositor wave.
 */
uint32_t
gj_batch_id_5540(void)
{
	(void)NULL;
	return b5540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5540(void)
    __attribute__((alias("gj_batch_id_5540")));
