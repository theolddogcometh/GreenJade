/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13569: input soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_13569(void);
 *     - Returns 1 (soft compile-time product tag: input soft continuum
 *       wave 13561-13570 is present/ready; not a runtime hard probe of
 *       any input stack).
 *   uint32_t __gj_input_soft_ready_u_13569  (alias)
 *   __libcgj_batch13569_marker = "libcgj-batch13569"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_input_soft_ready_u_13569 surface only; no
 * multi-def. Distinct from per-slot ok_u soft stubs (13561-13568,
 * all→0), gj_input_soft_ready_u_13369 (batch13369), and
 * gj_batch_id_13570. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13569_marker[] = "libcgj-batch13569";

/* Soft input continuum ready lamp (wave present). */
#define B13569_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13569_input_soft_ready(void)
{
	return B13569_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_13569 - soft input continuum ready tag.
 *
 * Always returns 1 (input soft wave ready/present). Does not call libc
 * or hard-probe any input slot. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_13569(void)
{
	(void)NULL;
	return b13569_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_13569(void)
    __attribute__((alias("gj_input_soft_ready_u_13569")));
