/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6249: H.273 / AVC matrix_coefficients ok stub.
 *
 * Surface (unique symbols):
 *   int gj_cs_matrix_id_ok_6249(uint32_t matrix_id);
 *     - Return 1 if matrix_id is a recognized BT.709 / BT.2020 NCL id
 *       used by this continuum: 1 (BT.709) or 9 (BT.2020 non-constant
 *       luminance); else 0.
 *   int __gj_cs_matrix_id_ok_6249  (alias)
 *   __libcgj_batch6249_marker = "libcgj-batch6249"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_matrix_id_ok_6249 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6249_marker[] = "libcgj-batch6249";

/* ITU-T H.273 / ISO/IEC 23091-2 matrix_coefficients (subset). */
#define B6249_MATRIX_BT709    1u
#define B6249_MATRIX_BT2020NCL 9u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6249_matrix_ok(uint32_t uMatrixId)
{
	if (uMatrixId == B6249_MATRIX_BT709) {
		return 1;
	}
	if (uMatrixId == B6249_MATRIX_BT2020NCL) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_matrix_id_ok_6249 - 1 if matrix_id is BT.709 (1) or BT.2020 NCL (9).
 *
 * Soft gate for color-space matrix tags accepted by this exclusive
 * continuum. Other H.273 ids (BT.601, FCC, identity, …) intentionally
 * rejected here. Pure predicate; no bitstream parse. Does not call libc.
 * No parent wires.
 */
int
gj_cs_matrix_id_ok_6249(uint32_t uMatrixId)
{
	(void)NULL;
	return b6249_matrix_ok(uMatrixId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cs_matrix_id_ok_6249(uint32_t uMatrixId)
    __attribute__((alias("gj_cs_matrix_id_ok_6249")));
