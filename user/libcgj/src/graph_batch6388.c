/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6388: family share cooldown-clear gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_cooldown_clear_6388(uint32_t remaining_sec);
 *     - Return 1 if remaining_sec is 0 (family share cooldown elapsed),
 *       else 0. Soft pure-data gate only.
 *   int __gj_fshare_cooldown_clear_6388  (alias)
 *   __libcgj_batch6388_marker = "libcgj-batch6388"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_cooldown_clear_6388 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6388_marker[] = "libcgj-batch6388";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6388_cooldown_clear(uint32_t uRemainingSec)
{
	if (uRemainingSec != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_cooldown_clear_6388 - soft family share cooldown-clear gate.
 *
 * remaining_sec: seconds left on family share library cooldown.
 * Returns 1 when remaining_sec == 0, else 0.
 * Does not read wall clocks. Does not call libc. No parent wires.
 */
int
gj_fshare_cooldown_clear_6388(uint32_t uRemainingSec)
{
	(void)NULL;
	return b6388_cooldown_clear(uRemainingSec);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_cooldown_clear_6388(uint32_t uRemainingSec)
    __attribute__((alias("gj_fshare_cooldown_clear_6388")));
