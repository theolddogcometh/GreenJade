/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9074: soft pixman a8 format catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pixman_a8_id_9074(void);
 *     - Returns 0x41380008 (fourCC-style "A8\0\x08") as the soft a8
 *       alpha-only format catalog id for the pixman soft-id continuum.
 *       Integer product tag only; does not mask alpha planes.
 *   uint32_t __gj_pixman_a8_id_9074  (alias)
 *   __libcgj_batch9074_marker = "libcgj-batch9074"
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

const char __libcgj_batch9074_marker[] = "libcgj-batch9074";

/* Soft a8 alpha-plane format catalog id. */
#define B9074_PIXMAN_A8_ID  0x41380008u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9074_fmt_id(void)
{
	return B9074_PIXMAN_A8_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pixman_a8_id_9074 - soft a8 format catalog id.
 *
 * Always returns 0x41380008. Soft pure-data only; does not sample
 * 8-bit alpha masks or call libc. No parent wires.
 */
uint32_t
gj_pixman_a8_id_9074(void)
{
	(void)NULL;
	return b9074_fmt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pixman_a8_id_9074(void)
    __attribute__((alias("gj_pixman_a8_id_9074")));
