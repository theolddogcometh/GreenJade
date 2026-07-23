/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5965: Steam Deck serial hex-digit soft predicate.
 *
 * Surface (unique symbols):
 *   int gj_deck_serial_hex_digit_ok_5965(int ch);
 *     - Return 1 if ch is a soft-valid hexadecimal serial digit
 *       ('0'-'9', 'A'-'F', 'a'-'f'), else 0. Soft serial-string stub.
 *   int __gj_deck_serial_hex_digit_ok_5965  (alias)
 *   __libcgj_batch5965_marker = "libcgj-batch5965"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_serial_len_ok_5964. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5965_marker[] = "libcgj-batch5965";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5965_hex_digit_ok(int nCh)
{
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	if (nCh >= 'A' && nCh <= 'F') {
		return 1;
	}
	if (nCh >= 'a' && nCh <= 'f') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_serial_hex_digit_ok_5965 - soft hex digit check for serials.
 *
 * ch: character code (signed so negatives reject)
 *
 * Returns 1 for [0-9A-Fa-f], else 0. Does not call libc. No parent wires.
 */
int
gj_deck_serial_hex_digit_ok_5965(int nCh)
{
	(void)NULL;
	return b5965_hex_digit_ok(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_serial_hex_digit_ok_5965(int nCh)
    __attribute__((alias("gj_deck_serial_hex_digit_ok_5965")));
