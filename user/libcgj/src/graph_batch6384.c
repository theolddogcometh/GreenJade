/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6384: family share borrower-id gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_borrower_ok_6384(uint32_t borrower_id, uint32_t lender_id);
 *     - Return 1 if borrower_id is non-zero and distinct from lender_id,
 *       else 0. Soft pure-data gate only.
 *   int __gj_fshare_borrower_ok_6384  (alias)
 *   __libcgj_batch6384_marker = "libcgj-batch6384"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_borrower_ok_6384 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6384_marker[] = "libcgj-batch6384";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6384_borrower_ok(uint32_t uBorrowerId, uint32_t uLenderId)
{
	if (uBorrowerId == 0u) {
		return 0;
	}
	if (uBorrowerId == uLenderId) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_borrower_ok_6384 - soft family share borrower-id gate.
 *
 * borrower_id: opaque id for the borrowing account.
 * lender_id:   opaque id for the library lender (must differ).
 * Returns 1 when borrower_id != 0 and borrower_id != lender_id, else 0.
 * Does not resolve accounts. Does not call libc. No parent wires.
 */
int
gj_fshare_borrower_ok_6384(uint32_t uBorrowerId, uint32_t uLenderId)
{
	(void)NULL;
	return b6384_borrower_ok(uBorrowerId, uLenderId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_borrower_ok_6384(uint32_t uBorrowerId, uint32_t uLenderId)
    __attribute__((alias("gj_fshare_borrower_ok_6384")));
