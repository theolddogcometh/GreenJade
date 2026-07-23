/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6387: family share authorized-device gate.
 *
 * Surface (unique symbols):
 *   int gj_fshare_device_ok_6387(uint32_t device_id, uint32_t authorized);
 *     - Return 1 if device_id is non-zero and authorized is non-zero,
 *       else 0. Soft pure-data device auth gate only.
 *   int __gj_fshare_device_ok_6387  (alias)
 *   __libcgj_batch6387_marker = "libcgj-batch6387"
 *
 * Exclusive continuum CREATE-ONLY (6381-6390: family share gate stubs
 * — fshare_enabled_p_6381, fshare_member_ok_6382, fshare_lender_ok_6383,
 * fshare_borrower_ok_6384, fshare_slot_ok_6385, fshare_title_ok_6386,
 * fshare_device_ok_6387, fshare_cooldown_clear_6388, fshare_gate_6389,
 * batch_id_6390). Unique gj_fshare_device_ok_6387 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6387_marker[] = "libcgj-batch6387";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6387_device_ok(uint32_t uDeviceId, uint32_t uAuthorized)
{
	if (uDeviceId == 0u) {
		return 0;
	}
	if (uAuthorized == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fshare_device_ok_6387 - soft family share authorized-device gate.
 *
 * device_id:  opaque non-zero device identity under probe.
 * authorized: non-zero means device is family-share authorized.
 * Returns 1 when device_id != 0 and authorized != 0, else 0.
 * Does not probe hardware. Does not call libc. No parent wires.
 */
int
gj_fshare_device_ok_6387(uint32_t uDeviceId, uint32_t uAuthorized)
{
	(void)NULL;
	return b6387_device_ok(uDeviceId, uAuthorized);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fshare_device_ok_6387(uint32_t uDeviceId, uint32_t uAuthorized)
    __attribute__((alias("gj_fshare_device_ok_6387")));
