/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6386: family share title shareable gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_title_ok_6386(uint32_t appid, uint32_t shareable);
 *     - Return 1 if appid is non-zero and shareable is non-zero, else 0.
 *       Soft pure-data title shareability gate only.
 *   int __gj_fshare_title_ok_6386  (alias)
 *   __libcgj_batch6386_marker = "libcgj-batch6386"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_title_ok_6386 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6386_marker[] = "libcgj-batch6386";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6386_title_ok(uint32_t uAppid, uint32_t uShareable)
{
	if (uAppid == 0u) {
		return 0;
	}
	if (uShareable == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_title_ok_6386 - soft family share title shareability gate.
 *
 * appid:     non-zero application id for the title under probe.
 * shareable: non-zero means title is marked family-share eligible.
 * Returns 1 when appid != 0 and shareable != 0, else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_fshare_title_ok_6386(uint32_t uAppid, uint32_t uShareable)
{
	(void)NULL;
	return b6386_title_ok(uAppid, uShareable);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_title_ok_6386(uint32_t uAppid, uint32_t uShareable)
    __attribute__((alias("gj_fshare_title_ok_6386")));
