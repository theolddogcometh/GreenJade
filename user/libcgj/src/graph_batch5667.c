/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5667: steamui ready integer / handshake codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ready_int_5667(void);
 *     - Primary steamui ready integer (always 0x53554931, fourCC SUI1).
 *   uint32_t gj_steamui_handshake_5667(uint32_t slot);
 *     - Map handshake slot (0..3) to a packed ready integer. Unknown → 0.
 *   uint32_t gj_batch_id_5667(void);
 *   uint32_t __gj_steamui_ready_int_5667 / __gj_steamui_handshake_5667
 *   uint32_t __gj_batch_id_5667  (aliases)
 *   __libcgj_batch5667_marker = "libcgj-batch5667"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers). Unique *_5667
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5667_marker[] = "libcgj-batch5667";

#define B5667_BATCH_ID     5667u
#define B5667_READY_INT    0x53554931u /* "SUI1" */
#define B5667_HS_MAX       4u

/*
 * steamui handshake ready integers (soft product tags):
 *   0 host, 1 ui load, 2 library, 3 friends chrome.
 */
static const uint32_t s_b5667_hs[B5667_HS_MAX] = {
	0x484f5354u, /* HOST */
	0x55494c44u, /* UILD (ui load) */
	0x4c494200u, /* LIB. */
	0x46524e44u  /* FRND */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5667_hs(uint32_t u32Slot)
{
	if (u32Slot >= B5667_HS_MAX) {
		return 0u;
	}
	return s_b5667_hs[u32Slot];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ready_int_5667 - primary steamui ready integer.
 *
 * Always returns 0x53554931 ("SUI1"). Soft pure-data; does not load
 * steamui.so. Does not call libc. No parent wires.
 */
uint32_t
gj_steamui_ready_int_5667(void)
{
	(void)NULL;
	return B5667_READY_INT;
}

/*
 * gj_steamui_handshake_5667 - handshake-slot ready integer.
 *
 * slot: 0..3 known; else 0.
 */
uint32_t
gj_steamui_handshake_5667(uint32_t slot)
{
	return b5667_hs(slot);
}

uint32_t
gj_batch_id_5667(void)
{
	return B5667_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steamui_ready_int_5667(void)
    __attribute__((alias("gj_steamui_ready_int_5667")));

uint32_t __gj_steamui_handshake_5667(uint32_t slot)
    __attribute__((alias("gj_steamui_handshake_5667")));

uint32_t __gj_batch_id_5667(void)
    __attribute__((alias("gj_batch_id_5667")));
