/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9078: soft pixman image-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pixman_image_ok_u_9078(void);
 *     - Returns 1 (pixman image soft-id continuum ok). Pure-data
 *       product tag; does not create image surfaces or map bits.
 *   uint32_t __gj_pixman_image_ok_u_9078  (alias)
 *   __libcgj_batch9078_marker = "libcgj-batch9078"
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

const char __libcgj_batch9078_marker[] = "libcgj-batch9078";

/* Pixman image soft-ok lamp for continuum 9071-9080. */
#define B9078_PIXMAN_IMAGE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9078_image_ok(void)
{
	return B9078_PIXMAN_IMAGE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pixman_image_ok_u_9078 - soft pixman image-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not allocate image
 * buffers or call libc. No parent wires.
 */
uint32_t
gj_pixman_image_ok_u_9078(void)
{
	(void)NULL;
	return b9078_image_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pixman_image_ok_u_9078(void)
    __attribute__((alias("gj_pixman_image_ok_u_9078")));
