/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6489: friend presence aggregate gate.
 *
 * Surface (unique symbols):
 *   int gj_fpres_gate_6489(uint32_t mask);
 *     - Return 1 if bits 0..5 of mask are all set (uid, state, present,
 *       last_seen, appid, enabled soft stages), else 0.
 *       Soft pure-data aggregate friend presence gate only.
 *   int __gj_fpres_gate_6489  (alias)
 *   __libcgj_batch6489_marker = "libcgj-batch6489"
 *
 * Exclusive continuum CREATE-ONLY (6481-6490: friend presence state stubs
 * — fpres_uid_ok_6481, fpres_state_ok_6482, fpres_online_p_6483,
 * fpres_away_p_6484, fpres_busy_p_6485, fpres_ingame_p_6486,
 * fpres_last_seen_ok_6487, fpres_appid_ok_6488, fpres_gate_6489,
 * batch_id_6490). Unique gj_fpres_gate_6489 surface only; no multi-def.
 * Self-contained; does not call sibling fpres helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6489_marker[] = "libcgj-batch6489";

/*
 * Required soft stage bits for friend presence aggregate pass:
 *   bit0 uid_ok, bit1 state_ok, bit2 present (online|busy|away|snooze),
 *   bit3 last_seen_ok, bit4 appid_stage, bit5 enabled.
 */
#define B6489_GATE_MASK  0x3fu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6489_gate(uint32_t uMask)
{
	if ((uMask & B6489_GATE_MASK) != B6489_GATE_MASK) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fpres_gate_6489 - soft aggregate friend presence gate.
 *
 * mask: bitfield of friend presence soft stages (bits 0..5 required).
 * Returns 1 when (mask & 0x3f) == 0x3f, else 0.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
int
gj_fpres_gate_6489(uint32_t mask)
{
	(void)NULL;
	return b6489_gate(mask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fpres_gate_6489(uint32_t mask)
    __attribute__((alias("gj_fpres_gate_6489")));
