/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9075: soft pixman Porter-Duff OVER op catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pixman_op_over_id_9075(void);
 *     - Returns 3 as the soft Porter-Duff OVER operator catalog id for
 *       the pixman soft-id continuum. Integer product tag only; does
 *       not run compositing kernels or link libpixman.
 *   uint32_t __gj_pixman_op_over_id_9075  (alias)
 *   __libcgj_batch9075_marker = "libcgj-batch9075"
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

const char __libcgj_batch9075_marker[] = "libcgj-batch9075";

/* Soft Porter-Duff OVER operator catalog id. */
#define B9075_PIXMAN_OP_OVER_ID  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9075_op_id(void)
{
	return B9075_PIXMAN_OP_OVER_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pixman_op_over_id_9075 - soft Porter-Duff OVER op catalog id.
 *
 * Always returns 3. Soft pure-data only; does not alpha-blend or call
 * libc. No parent wires.
 */
uint32_t
gj_pixman_op_over_id_9075(void)
{
	(void)NULL;
	return b9075_op_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pixman_op_over_id_9075(void)
    __attribute__((alias("gj_pixman_op_over_id_9075")));
