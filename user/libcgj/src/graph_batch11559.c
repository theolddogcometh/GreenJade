/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11559: controller soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_11559(void);
 *     - Returns 1 (soft compile-time product tag: controller soft
 *       continuum wave 11551-11560 is present/ready; not a runtime hard
 *       probe of any gamepad/HID stack).
 *   uint32_t __gj_controller_soft_ready_u_11559  (alias)
 *   __libcgj_batch11559_marker = "libcgj-batch11559"
 *
 * Exclusive continuum CREATE-ONLY (11551-11560: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11560). Unique
 * gj_controller_soft_ready_u_11559 surface only; no multi-def. Distinct
 * from per-slot ok_u soft stubs (11551-11558, all→0) and
 * gj_batch_id_11560. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11559_marker[] = "libcgj-batch11559";

/* Soft controller continuum ready lamp (wave present). */
#define B11559_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11559_controller_soft_ready(void)
{
	return B11559_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_11559 - soft controller continuum ready tag.
 *
 * Always returns 1 (controller soft wave ready/present). Does not call
 * libc or hard-probe any controller slot. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_11559(void)
{
	(void)NULL;
	return b11559_controller_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_11559(void)
    __attribute__((alias("gj_controller_soft_ready_u_11559")));
