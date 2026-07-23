/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10560: controller soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10560(void);
 *     - Returns the compile-time graph batch number for this TU (10560).
 *   uint32_t __gj_batch_id_10560  (alias)
 *   __libcgj_batch10560_marker = "libcgj-batch10560"
 *
 * Exclusive continuum CREATE-ONLY (10551-10560: controller soft all→0 —
 * controller_ok_u_10551, gamepad_ok_u_10552,
 * steamdeck_controls_ok_u_10553, hid_ok_u_10554, xinput_ok_u_10555,
 * evdev_ok_u_10556, haptics_ok_u_10557, gyro_ok_u_10558,
 * controller_soft_ready_u_10559, batch_id_10560). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10060 (batch10060). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10560_marker[] = "libcgj-batch10560";

#define B10560_BATCH_ID  10560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10560_id(void)
{
	return B10560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10560 - report this TU's graph batch number.
 *
 * Always returns 10560.
 */
uint32_t
gj_batch_id_10560(void)
{
	(void)NULL;
	return b10560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10560(void)
    __attribute__((alias("gj_batch_id_10560")));
