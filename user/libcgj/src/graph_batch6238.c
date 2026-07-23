/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6238: HDR transfer-characteristic id check.
 *
 * Surface (unique symbols):
 *   int gj_hdr_transfer_id_ok_6238(uint32_t transfer_id);
 *     - Return 1 if transfer_id is a known HDR transfer in this continuum:
 *         16 = PQ (SMPTE ST 2084) — ITU-T H.273 / CICP style
 *         18 = HLG (ARIB STD-B67)
 *       else 0. Soft enum gate for pipeline wiring; not a codec parser.
 *   int __gj_hdr_transfer_id_ok_6238  (alias)
 *   __libcgj_batch6238_marker = "libcgj-batch6238"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_transfer_id_ok_6238 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6238_marker[] = "libcgj-batch6238";

/* H.273 / CICP-style transfer characteristic codes used here. */
#define B6238_XFER_PQ   16u
#define B6238_XFER_HLG  18u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6238_xfer_ok(uint32_t u32XferId)
{
	if (u32XferId == B6238_XFER_PQ) {
		return 1;
	}
	if (u32XferId == B6238_XFER_HLG) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_transfer_id_ok_6238 - 1 if transfer_id is PQ (16) or HLG (18).
 *
 * transfer_id: soft CICP-style transfer characteristic code.
 * Pure predicate for HDR PQ/HLG pipeline selection. No parent wires.
 */
int
gj_hdr_transfer_id_ok_6238(uint32_t u32TransferId)
{
	(void)NULL;
	return b6238_xfer_ok(u32TransferId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hdr_transfer_id_ok_6238(uint32_t u32TransferId)
    __attribute__((alias("gj_hdr_transfer_id_ok_6238")));
