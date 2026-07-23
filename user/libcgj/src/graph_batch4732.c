/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4732: saturating total frame length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_total_u(uint32_t payload);
 *     - Return payload + 8 with saturation at UINT32_MAX (no wrap).
 *   uint32_t __gj_frame_total_u  (alias)
 *   __libcgj_batch4732_marker = "libcgj-batch4732"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_total_u surface only; no multi-def. Distinct from
 * gj_u32_sat_add (batch923) — framing-specific total. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4732_marker[] = "libcgj-batch4732";

/* Fixed protocol frame header length (bytes). */
#define B4732_HDR_SIZE  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4732_total(uint32_t u32Payload)
{
	if (u32Payload > (UINT32_MAX - B4732_HDR_SIZE)) {
		return UINT32_MAX;
	}
	return u32Payload + B4732_HDR_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_total_u - saturating total frame length from payload size.
 *
 * payload: payload byte length
 *
 * Returns payload + 8, clamped to UINT32_MAX on overflow. Pair with
 * gj_frame_hdr_size_u (batch4731). Self-contained; no parent wires.
 */
uint32_t
gj_frame_total_u(uint32_t u32Payload)
{
	(void)NULL;
	return b4732_total(u32Payload);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_total_u(uint32_t u32Payload)
    __attribute__((alias("gj_frame_total_u")));
