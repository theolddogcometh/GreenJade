/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13939: IM soft-ready aggregate lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_soft_ready_u_13939(void);
 *     - Returns 1 (soft compile-time IM soft-ready lamp for the
 *       ibus/fcitx soft-id wave; stub surfaces present).
 *   uint32_t __gj_im_soft_ready_u_13939  (alias)
 *   __libcgj_batch13939_marker = "libcgj-batch13939"
 *
 * Exclusive CREATE-ONLY wave 13931-13940 (ibus/fcitx soft id stubs).
 * Unique gj_im_soft_ready_u_13939 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13939_marker[] = "libcgj-batch13939";

/* Soft IM ready lamp: compile-time soft surfaces present. */
#define B13939_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13939_ready(void)
{
	return B13939_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_soft_ready_u_13939 - soft input-method soft-ready lamp.
 *
 * Always returns 1 (soft IM id stubs present for this wave). Does not
 * probe runtime IM daemons or call libc. No parent wires.
 */
uint32_t
gj_im_soft_ready_u_13939(void)
{
	(void)NULL;
	return b13939_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_soft_ready_u_13939(void)
    __attribute__((alias("gj_im_soft_ready_u_13939")));
