/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6464: screenshot extension code soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_ext_ok_6464(uint32_t ext_code);
 *     - Soft extension code gate: 1=png, 2=jpg/jpeg, 3=bmp, 4=tga,
 *       5=webp → 1; any other code → 0. Pure table, no string I/O.
 *   uint32_t __gj_sshot_ext_ok_6464  (alias)
 *   __libcgj_batch6464_marker = "libcgj-batch6464"
 *
 * Exclusive continuum CREATE-ONLY (6461-6470: screenshot path/size stubs —
 * path_len_ok_6461, path_cap_ok_6462, path_depth_ok_6463, ext_ok_6464,
 * width_ok_6465, height_ok_6466, bpp_ok_6467, byte_est_6468, size_ok_6469,
 * path_size_ready_6470). Unique surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6464_marker[] = "libcgj-batch6464";

#define B6464_EXT_PNG   1u
#define B6464_EXT_JPG   2u
#define B6464_EXT_BMP   3u
#define B6464_EXT_TGA   4u
#define B6464_EXT_WEBP  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6464_ext_ok(uint32_t u32Code)
{
	if (u32Code == B6464_EXT_PNG || u32Code == B6464_EXT_JPG ||
	    u32Code == B6464_EXT_BMP || u32Code == B6464_EXT_TGA ||
	    u32Code == B6464_EXT_WEBP) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_ext_ok_6464 - soft screenshot file-extension code gate.
 *
 * ext_code: 1=png, 2=jpg, 3=bmp, 4=tga, 5=webp
 *
 * Returns 1 for known codes, else 0. Pure integer table. No parent wires.
 */
uint32_t
gj_sshot_ext_ok_6464(uint32_t ext_code)
{
	(void)NULL;
	return b6464_ext_ok(ext_code);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_ext_ok_6464(uint32_t ext_code)
    __attribute__((alias("gj_sshot_ext_ok_6464")));
