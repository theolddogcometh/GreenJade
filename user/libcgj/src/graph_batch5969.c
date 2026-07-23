/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5969: Steam Deck serial hex-nibble pack stub.
 *
 * Surface (unique symbols):
 *   int gj_deck_serial_nibble_pack_5969(int hi, int lo, uint8_t *out);
 *     - Pack two hex digit characters (hi, lo) into one byte at *out.
 *       Accepts '0'-'9', 'A'-'F', 'a'-'f'. Returns 0 on success, -1 on
 *       failure (bad digit or NULL out). Soft serial-decode stub.
 *   int __gj_deck_serial_nibble_pack_5969  (alias)
 *   __libcgj_batch5969_marker = "libcgj-batch5969"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_serial_hex_digit_ok_5965. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5969_marker[] = "libcgj-batch5969";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one hex digit char to 0..15, or -1 if invalid. */
static int
b5969_nibble(int nCh)
{
	if (nCh >= '0' && nCh <= '9') {
		return nCh - '0';
	}
	if (nCh >= 'A' && nCh <= 'F') {
		return nCh - 'A' + 10;
	}
	if (nCh >= 'a' && nCh <= 'f') {
		return nCh - 'a' + 10;
	}
	return -1;
}

static int
b5969_nibble_pack(int nHi, int nLo, uint8_t *pOut)
{
	int nH;
	int nL;

	if (pOut == NULL) {
		return -1;
	}
	nH = b5969_nibble(nHi);
	if (nH < 0) {
		return -1;
	}
	nL = b5969_nibble(nLo);
	if (nL < 0) {
		return -1;
	}
	*pOut = (uint8_t)(((unsigned)nH << 4) | (unsigned)nL);
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_serial_nibble_pack_5969 - pack two hex serial digits to a byte.
 *
 * hi, lo: hex digit characters
 * out:    destination byte (must be non-NULL)
 *
 * Returns 0 on success, -1 on bad digit or NULL out. No parent wires.
 */
int
gj_deck_serial_nibble_pack_5969(int nHi, int nLo, uint8_t *pOut)
{
	(void)NULL;
	return b5969_nibble_pack(nHi, nLo, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_serial_nibble_pack_5969(int nHi, int nLo, uint8_t *pOut)
    __attribute__((alias("gj_deck_serial_nibble_pack_5969")));
