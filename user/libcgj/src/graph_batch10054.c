/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10054: Switch Pro soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_switch_pro_ok_u_10054(void);
 *     - Soft HID id lamp for Nintendo Switch Pro controller presence
 *       (always 0 — not detected / soft-unavailable). Compile-time
 *       pure-data tag; not a live USB/HID device probe.
 *   uint32_t __gj_switch_pro_ok_u_10054  (alias)
 *   __libcgj_batch10054_marker = "libcgj-batch10054"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique
 * gj_switch_pro_ok_u_10054 surface only; no multi-def. Soft stub — does
 * not open HID nodes or enumerate Switch Pro pads. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10054_marker[] = "libcgj-batch10054";

/* Soft unavailable / not-detected Switch Pro lamp. */
#define B10054_SWITCH_PRO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10054_switch_pro_ok(void)
{
	return B10054_SWITCH_PRO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_switch_pro_ok_u_10054 - Switch Pro soft-id lamp.
 *
 * Always returns 0. Soft pure-data HID presence tag. Does not probe
 * USB/HID or call libc. No parent wires.
 */
uint32_t
gj_switch_pro_ok_u_10054(void)
{
	(void)NULL;
	return b10054_switch_pro_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_switch_pro_ok_u_10054(void)
    __attribute__((alias("gj_switch_pro_ok_u_10054")));
