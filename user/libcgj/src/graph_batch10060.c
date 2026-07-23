/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10060: controller/HID soft-id batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10060(void);
 *     - Returns the compile-time graph batch number for this TU (10060).
 *   uint32_t __gj_batch_id_10060  (alias)
 *   __libcgj_batch10060_marker = "libcgj-batch10060"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique gj_batch_id_10060
 * surface only; does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10060_marker[] = "libcgj-batch10060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10060_id(void)
{
	return 10060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10060 - report this TU's graph batch number.
 *
 * Always returns 10060. Soft pure-data batch identity for the
 * 10051-10060 controller/HID soft-id wave. No parent wires.
 */
uint32_t
gj_batch_id_10060(void)
{
	(void)NULL;
	return b10060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10060(void)
    __attribute__((alias("gj_batch_id_10060")));
