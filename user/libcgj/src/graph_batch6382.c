/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6382: family share member-count gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_member_ok_6382(uint32_t members);
 *     - Return 1 if members is in [1, 6] (organizer + up to five
 *       family members soft band), else 0. Soft pure-data gate only.
 *   int __gj_fshare_member_ok_6382  (alias)
 *   __libcgj_batch6382_marker = "libcgj-batch6382"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_member_ok_6382 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6382_marker[] = "libcgj-batch6382";

/* Soft family member count band (inclusive). */
#define B6382_MEMBER_MIN  1u
#define B6382_MEMBER_MAX  6u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6382_member_ok(uint32_t uMembers)
{
	if (uMembers < B6382_MEMBER_MIN) {
		return 0;
	}
	if (uMembers > B6382_MEMBER_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_member_ok_6382 - soft family share member-count gate.
 *
 * members: family group size (organizer + invitees).
 * Returns 1 when members is in [1, 6], else 0.
 * Does not enumerate a family group. Does not call libc. No parent wires.
 */
int
gj_fshare_member_ok_6382(uint32_t uMembers)
{
	(void)NULL;
	return b6382_member_ok(uMembers);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_member_ok_6382(uint32_t uMembers)
    __attribute__((alias("gj_fshare_member_ok_6382")));
