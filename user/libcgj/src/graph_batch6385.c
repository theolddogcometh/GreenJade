/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6385: family share library-slot gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_slot_ok_6385(uint32_t slot);
 *     - Return 1 if slot is in [0, 5] (six soft library share slots),
 *       else 0. Soft pure-data gate only.
 *   int __gj_fshare_slot_ok_6385  (alias)
 *   __libcgj_batch6385_marker = "libcgj-batch6385"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_slot_ok_6385 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6385_marker[] = "libcgj-batch6385";

/* Soft library share slot band (inclusive, zero-based): [0, 5]. */
#define B6385_SLOT_MAX  5u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6385_slot_ok(uint32_t uSlot)
{
	if (uSlot > B6385_SLOT_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_slot_ok_6385 - soft family share library-slot gate.
 *
 * slot: zero-based library share slot index.
 * Returns 1 when slot is in [0, 5], else 0.
 * Does not allocate slots. Does not call libc. No parent wires.
 */
int
gj_fshare_slot_ok_6385(uint32_t uSlot)
{
	(void)NULL;
	return b6385_slot_ok(uSlot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_slot_ok_6385(uint32_t uSlot)
    __attribute__((alias("gj_fshare_slot_ok_6385")));
