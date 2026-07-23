/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13169: input soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_13169(void);
 *     - Returns 1 (soft compile-time product tag: input soft continuum
 *       wave 13161-13170 is present/ready; not a runtime hard probe of
 *       any input stack).
 *   uint32_t __gj_input_soft_ready_u_13169  (alias)
 *   __libcgj_batch13169_marker = "libcgj-batch13169"
 *
 * Exclusive continuum CREATE-ONLY (13161-13170: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13170 —
 * udev_ok_u_13161, libinput_ok_u_13162, evdev_ok_u_13163,
 * uinput_ok_u_13164, hidraw_ok_u_13165, kbd_ok_u_13166,
 * mouse_ok_u_13167, touch_ok_u_13168, input_soft_ready_u_13169,
 * batch_id_13170). Unique gj_input_soft_ready_u_13169 surface only; no
 * multi-def. Distinct from per-slot ok_u soft stubs (13161-13168,
 * all→0) and gj_batch_id_13170. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13169_marker[] = "libcgj-batch13169";

/* Soft input continuum ready lamp (wave present). */
#define B13169_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13169_input_soft_ready(void)
{
	return B13169_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_13169 - soft input continuum ready tag.
 *
 * Always returns 1 (input soft wave ready/present). Does not call libc
 * or hard-probe any input slot. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_13169(void)
{
	(void)NULL;
	return b13169_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_13169(void)
    __attribute__((alias("gj_input_soft_ready_u_13169")));
