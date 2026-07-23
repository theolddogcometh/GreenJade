/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13969: input soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_13969(void);
 *     - Returns 1 (soft compile-time product tag: input soft continuum
 *       wave 13961-13970 is present/ready; not a runtime hard probe of
 *       any input stack).
 *   uint32_t __gj_input_soft_ready_u_13969  (alias)
 *   __libcgj_batch13969_marker = "libcgj-batch13969"
 *
 * Exclusive continuum CREATE-ONLY (13961-13970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13970 —
 * udev_ok_u_13961, libinput_ok_u_13962, evdev_ok_u_13963,
 * uinput_ok_u_13964, hidraw_ok_u_13965, kbd_ok_u_13966,
 * mouse_ok_u_13967, touch_ok_u_13968, input_soft_ready_u_13969,
 * batch_id_13970). Unique gj_input_soft_ready_u_13969 surface only; no
 * multi-def. Distinct from per-slot ok_u soft stubs (13961-13968,
 * all→0), gj_input_soft_ready_u_13769 (batch13769),
 * gj_input_soft_ready_u_13569 (batch13569), gj_input_soft_ready_u_13369
 * (batch13369), and gj_batch_id_13970. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13969_marker[] = "libcgj-batch13969";

/* Soft input continuum ready lamp (wave present). */
#define B13969_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13969_input_soft_ready(void)
{
	return B13969_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_13969 - soft input continuum ready tag.
 *
 * Always returns 1 (input soft wave ready/present). Does not call libc
 * or hard-probe any input slot. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_13969(void)
{
	(void)NULL;
	return b13969_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_13969(void)
    __attribute__((alias("gj_input_soft_ready_u_13969")));
