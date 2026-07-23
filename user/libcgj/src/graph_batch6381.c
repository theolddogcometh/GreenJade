/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6381: family share feature-enabled gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_enabled_p_6381(uint32_t flag);
 *     - Return 1 if flag is non-zero (family share feature soft-on),
 *       else 0. Soft pure-data gate only.
 *   int __gj_fshare_enabled_p_6381  (alias)
 *   __libcgj_batch6381_marker = "libcgj-batch6381"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_enabled_p_6381 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6381_marker[] = "libcgj-batch6381";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6381_enabled_p(uint32_t uFlag)
{
	if (uFlag == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_enabled_p_6381 - soft family share feature-enabled gate.
 *
 * flag: non-zero means family share soft-on for this probe.
 * Returns 1 when flag != 0, else 0.
 * Does not consult Steam/Family APIs. Does not call libc. No parent wires.
 */
int
gj_fshare_enabled_p_6381(uint32_t uFlag)
{
	(void)NULL;
	return b6381_enabled_p(uFlag);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_enabled_p_6381(uint32_t uFlag)
    __attribute__((alias("gj_fshare_enabled_p_6381")));
