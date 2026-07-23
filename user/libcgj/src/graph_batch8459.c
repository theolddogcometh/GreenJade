/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8459: lowercase hex alphabet identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hex_lower_id_8459(void);
 *     - Returns the ASCII base for lowercase hex letters ('a' = 97):
 *       soft continuum tag for a-f alphabet identity (default encode
 *       case for this wave).
 *   uint32_t __gj_hex_lower_id_8459  (alias)
 *   __libcgj_batch8459_marker = "libcgj-batch8459"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs).
 * Unique gj_hex_lower_id_8459 surface only; no multi-def. Companion to
 * gj_hex_upper_id_8458. Digit/hi/lo stubs in this wave use lowercase.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8459_marker[] = "libcgj-batch8459";

/* ASCII base for lowercase hex letters a-f (default encode case). */
#define B8459_HEX_LOWER_ID  ((uint32_t)'a')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8459_lower_id(void)
{
	return B8459_HEX_LOWER_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_lower_id_8459 - lowercase hex letter alphabet identity.
 *
 * Always returns (uint32_t)'a' (97). Soft pure data product tag for the
 * a-f continuum (default encode case). No parent wires.
 */
uint32_t
gj_hex_lower_id_8459(void)
{
	(void)NULL;
	return b8459_lower_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hex_lower_id_8459(void)
    __attribute__((alias("gj_hex_lower_id_8459")));
