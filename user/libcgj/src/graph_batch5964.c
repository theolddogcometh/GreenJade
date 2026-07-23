/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5964: Steam Deck controller serial length soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_deck_serial_len_ok_5964(uint32_t len);
 *     - Return 1 if len is a soft-valid USB iSerial / controller serial
 *       string length in [8, 32] (covers typical short hex serials with
 *       headroom), else 0.
 *   int __gj_deck_serial_len_ok_5964  (alias)
 *   __libcgj_batch5964_marker = "libcgj-batch5964"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5964_marker[] = "libcgj-batch5964";

/* Inclusive soft serial length band (UTF-16 unit count or byte count). */
#define B5964_SERIAL_LEN_MIN  8u
#define B5964_SERIAL_LEN_MAX  32u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5964_serial_len_ok(uint32_t uLen)
{
	if (uLen < B5964_SERIAL_LEN_MIN) {
		return 0;
	}
	if (uLen > B5964_SERIAL_LEN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_serial_len_ok_5964 - soft check for controller serial length.
 *
 * len: serial string length (units as used by the caller)
 *
 * Returns 1 when len ∈ [8, 32], else 0. Does not inspect string bytes.
 * No parent wires.
 */
int
gj_deck_serial_len_ok_5964(uint32_t uLen)
{
	(void)NULL;
	return b5964_serial_len_ok(uLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_serial_len_ok_5964(uint32_t uLen)
    __attribute__((alias("gj_deck_serial_len_ok_5964")));
