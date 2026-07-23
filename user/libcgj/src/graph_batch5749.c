/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5749: EDID block checksum stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_edid_checksum_stub_5749(void);
 *     - Returns stub EDID 128-byte block checksum byte (0 = valid).
 *   uint32_t __gj_edid_checksum_stub_5749  (alias)
 *   __libcgj_batch5749_marker = "libcgj-batch5749"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_edid_checksum_stub_5749
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5749_marker[] = "libcgj-batch5749";

/*
 * Stub EDID checksum: sum of all 128 block bytes mod 256 is 0 when
 * valid. Soft tag returns 0 (checksum OK) without reading DDC/EDID.
 */
#define B5749_EDID_CHECKSUM_STUB  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5749_edid_checksum_stub(void)
{
	return B5749_EDID_CHECKSUM_STUB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_edid_checksum_stub_5749 - report stub EDID block checksum status.
 *
 * Always returns 0 (valid). Soft pure-data EDID checksum tag; does not
 * probe I2C/DDC or sum a real block. Does not call libc. No parent wires.
 */
uint32_t
gj_edid_checksum_stub_5749(void)
{
	(void)NULL;
	return b5749_edid_checksum_stub();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_edid_checksum_stub_5749(void)
    __attribute__((alias("gj_edid_checksum_stub_5749")));
