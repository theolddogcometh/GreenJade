/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11160: controller soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11160(void);
 *     - Returns the compile-time graph batch number for this TU (11160).
 *   uint32_t __gj_batch_id_11160  (alias)
 *   __libcgj_batch11160_marker = "libcgj-batch11160"
 *
 * Exclusive continuum CREATE-ONLY (11151-11160: controller soft id
 * stubs — controller_ok_u_11151, gamepad_ok_u_11152,
 * steamdeck_controls_ok_u_11153, hid_ok_u_11154, xinput_ok_u_11155,
 * evdev_ok_u_11156, haptics_ok_u_11157, gyro_ok_u_11158,
 * controller_soft_ready_u_11159, batch_id_11160). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11160_marker[] = "libcgj-batch11160";

#define B11160_BATCH_ID  11160u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11160_id(void)
{
	return B11160_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11160 - report this TU's graph batch number.
 *
 * Always returns 11160.
 */
uint32_t
gj_batch_id_11160(void)
{
	(void)NULL;
	return b11160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11160(void)
    __attribute__((alias("gj_batch_id_11160")));
