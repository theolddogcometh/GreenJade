/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6147: RFCOMM channel open stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bt_rfcomm_channel_ok_6147(uint32_t channel);
 *     - Soft RFCOMM channel-open accept lamp. Returns 1 when channel
 *       is in the classic RFCOMM soft range [1, 30], else 0. Pure data;
 *       does not bind RFCOMM sockets or talk to BlueZ profile agents.
 *   uint32_t __gj_bt_rfcomm_channel_ok_6147  (alias)
 *   __libcgj_batch6147_marker = "libcgj-batch6147"
 *
 * Exclusive continuum CREATE-ONLY (6141-6150: bluez/bt audio stubs).
 * Unique gj_bt_rfcomm_channel_ok_6147 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6147_marker[] = "libcgj-batch6147";

#define B6147_CH_MIN  1u
#define B6147_CH_MAX  30u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6147_rfcomm_ok(uint32_t u32Ch)
{
	return (u32Ch >= B6147_CH_MIN && u32Ch <= B6147_CH_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_rfcomm_channel_ok_6147 - RFCOMM channel open stub.
 *
 * channel: soft RFCOMM channel number (1..30 classic range)
 *
 * Returns 1 if channel is in [1,30], else 0. Soft pure-data BT serial
 * channel lamp. No parent wires.
 */
uint32_t
gj_bt_rfcomm_channel_ok_6147(uint32_t channel)
{
	(void)NULL;
	return b6147_rfcomm_ok(channel);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bt_rfcomm_channel_ok_6147(uint32_t channel)
    __attribute__((alias("gj_bt_rfcomm_channel_ok_6147")));
