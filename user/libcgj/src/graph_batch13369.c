/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13369: input soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_13369(void);
 *     - Returns 1 (soft compile-time product tag: input soft continuum
 *       wave 13361-13370 is present/ready; not a runtime hard probe of
 *       any input stack).
 *   uint32_t __gj_input_soft_ready_u_13369  (alias)
 *   __libcgj_batch13369_marker = "libcgj-batch13369"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_input_soft_ready_u_13369 surface only; no
 * multi-def. Distinct from per-slot ok_u soft stubs (13361-13368,
 * all→0), gj_input_soft_ready_u_13169 (batch13169), and
 * gj_batch_id_13370. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13369_marker[] = "libcgj-batch13369";

/* Soft input continuum ready lamp (wave present). */
#define B13369_INPUT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13369_input_soft_ready(void)
{
	return B13369_INPUT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_13369 - soft input continuum ready tag.
 *
 * Always returns 1 (input soft wave ready/present). Does not call libc
 * or hard-probe any input slot. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_13369(void)
{
	(void)NULL;
	return b13369_input_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_13369(void)
    __attribute__((alias("gj_input_soft_ready_u_13369")));
