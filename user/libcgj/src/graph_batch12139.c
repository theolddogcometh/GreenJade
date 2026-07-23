/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12139: IM soft-ready aggregate lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_soft_ready_u_12139(void);
 *     - Returns 1 (soft compile-time IM soft-ready lamp for the
 *       ibus/fcitx soft-id wave; stub surfaces present).
 *   uint32_t __gj_im_soft_ready_u_12139  (alias)
 *   __libcgj_batch12139_marker = "libcgj-batch12139"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_im_soft_ready_u_12139 surface
 * only; no multi-def. Distinct from gj_im_soft_ready_u_11839
 * (batch11839), gj_im_soft_ready_u_11639 (batch11639), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12139_marker[] = "libcgj-batch12139";

/* Soft IM ready lamp: compile-time soft surfaces present. */
#define B12139_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12139_ready(void)
{
	return B12139_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_soft_ready_u_12139 - soft input-method soft-ready lamp.
 *
 * Always returns 1 (soft IM id stubs present for this wave). Does not
 * probe runtime IM daemons or call libc. No parent wires.
 */
uint32_t
gj_im_soft_ready_u_12139(void)
{
	(void)NULL;
	return b12139_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_soft_ready_u_12139(void)
    __attribute__((alias("gj_im_soft_ready_u_12139")));
