/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6383: family share lender-id gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_lender_ok_6383(uint32_t lender_id);
 *     - Return 1 if lender_id is non-zero (soft library lender present),
 *       else 0. Soft pure-data gate only.
 *   int __gj_fshare_lender_ok_6383  (alias)
 *   __libcgj_batch6383_marker = "libcgj-batch6383"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_lender_ok_6383 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6383_marker[] = "libcgj-batch6383";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6383_lender_ok(uint32_t uLenderId)
{
	if (uLenderId == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_lender_ok_6383 - soft family share lender-id gate.
 *
 * lender_id: opaque non-zero id for the library lender account.
 * Returns 1 when lender_id != 0, else 0.
 * Does not resolve accounts. Does not call libc. No parent wires.
 */
int
gj_fshare_lender_ok_6383(uint32_t uLenderId)
{
	(void)NULL;
	return b6383_lender_ok(uLenderId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_lender_ok_6383(uint32_t uLenderId)
    __attribute__((alias("gj_fshare_lender_ok_6383")));
