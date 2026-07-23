/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9073: soft pixman r5g6b5 format catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pixman_r5g6b5_id_9073(void);
 *     - Returns 0x52353635 (fourCC "R565") as the soft r5g6b5 format
 *       catalog id for the pixman soft-id continuum. Integer product
 *       tag only; does not pack RGB565 or link libpixman.
 *   uint32_t __gj_pixman_r5g6b5_id_9073  (alias)
 *   __libcgj_batch9073_marker = "libcgj-batch9073"
 *
 * Exclusive continuum CREATE-ONLY (9071-9080: pixman soft id stubs —
 * a8r8g8b8_id_9071, x8r8g8b8_id_9072, r5g6b5_id_9073, a8_id_9074,
 * op_over_id_9075, op_src_id_9076, region_ok_u_9077, image_ok_u_9078,
 * ready_u_9079, batch_id_9080). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9073_marker[] = "libcgj-batch9073";

/* Soft "R565" fourCC-style r5g6b5 format catalog id. */
#define B9073_PIXMAN_R5G6B5_ID  0x52353635u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9073_fmt_id(void)
{
	return B9073_PIXMAN_R5G6B5_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pixman_r5g6b5_id_9073 - soft r5g6b5 format catalog id.
 *
 * Always returns 0x52353635 ("R565"). Soft pure-data only; does not
 * pack 16-bit RGB or call libc. No parent wires.
 */
uint32_t
gj_pixman_r5g6b5_id_9073(void)
{
	(void)NULL;
	return b9073_fmt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pixman_r5g6b5_id_9073(void)
    __attribute__((alias("gj_pixman_r5g6b5_id_9073")));
