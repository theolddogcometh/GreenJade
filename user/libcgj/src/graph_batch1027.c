/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1027: C0/DEL control-character predicate.
 *
 * Surface (unique symbols):
 *   int gj_control_char_p(int ch);
 *     — Return 1 if ch is an ASCII control character (0x00..0x1F
 *       or 0x7F), else 0. Negative → 0.
 *   int __gj_control_char_p  (alias)
 *   __libcgj_batch1027_marker = "libcgj-batch1027"
 *
 * Does NOT redefine iscntrl.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1027_marker[] = "libcgj-batch1027";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_control_char_p — true for ASCII C0 controls and DEL.
 *
 * nCh: character value
 *
 * Matches classic iscntrl for the C locale on 0..127: 0x00..0x1F, 0x7F.
 * Values outside that set return 0.
 */
int
gj_control_char_p(int nCh)
{
	if (nCh < 0) {
		return 0;
	}
	if (nCh <= 0x1F) {
		return 1;
	}
	if (nCh == 0x7F) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_control_char_p(int nCh)
    __attribute__((alias("gj_control_char_p")));
