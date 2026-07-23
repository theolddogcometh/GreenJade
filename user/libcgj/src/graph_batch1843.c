/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1843: ASCII key lowercase fold.
 *
 * Surface (unique symbols):
 *   unsigned gj_key_to_lower_ascii(unsigned key);
 *     — If key is 'A'..'Z', return the corresponding 'a'..'z';
 *       otherwise return key unchanged. Pure ASCII fold; no locale.
 *   unsigned __gj_key_to_lower_ascii  (alias)
 *   __libcgj_batch1843_marker = "libcgj-batch1843"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1843_marker[] = "libcgj-batch1843";

/* ASCII uppercase inclusive band; fold delta is 0x20. */
#define B1843_UP_LO 0x41u /* 'A' */
#define B1843_UP_HI 0x5au /* 'Z' */
#define B1843_FOLD  0x20u

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold A..Z to a..z; identity elsewhere. */
static unsigned
b1843_to_lower(unsigned uKey)
{
	if (uKey >= B1843_UP_LO && uKey <= B1843_UP_HI) {
		return uKey + B1843_FOLD;
	}
	return uKey;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_key_to_lower_ascii — fold uppercase ASCII key to lowercase.
 *
 * uKey: key value as unsigned code point
 * Returns uKey with A..Z mapped to a..z; all other values unchanged.
 */
unsigned
gj_key_to_lower_ascii(unsigned uKey)
{
	return b1843_to_lower(uKey);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_key_to_lower_ascii(unsigned uKey)
    __attribute__((alias("gj_key_to_lower_ascii")));
