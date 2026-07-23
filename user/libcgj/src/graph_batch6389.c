/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6389: family share aggregate gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_gate_6389(uint32_t mask);
 *     - Return 1 if bits 0..5 of mask are all set (enabled, member,
 *       lender, borrower, title, device soft stages), else 0.
 *       Soft pure-data aggregate family share gate only.
 *   int __gj_fshare_gate_6389  (alias)
 *   __libcgj_batch6389_marker = "libcgj-batch6389"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_gate_6389 surface only; no multi-def.
 * Self-contained; does not call sibling fshare helpers. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6389_marker[] = "libcgj-batch6389";

/*
 * Required soft stage bits for family share aggregate pass:
 *   bit0 enabled, bit1 member, bit2 lender, bit3 borrower,
 *   bit4 title,   bit5 device.
 */
#define B6389_GATE_MASK  0x3fu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6389_gate(uint32_t uMask)
{
	if ((uMask & B6389_GATE_MASK) != B6389_GATE_MASK) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_gate_6389 - soft aggregate family share gate.
 *
 * mask: bitfield of family share soft stages (bits 0..5 required).
 * Returns 1 when (mask & 0x3f) == 0x3f, else 0.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
int
gj_fshare_gate_6389(uint32_t uMask)
{
	(void)NULL;
	return b6389_gate(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_gate_6389(uint32_t uMask)
    __attribute__((alias("gj_fshare_gate_6389")));
