/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6923: copy_file_range flags acceptance.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_flags_ok_6923(uint32_t flags);
 *     - Return 1 if flags == 0 (currently the only accepted mask);
 *       else 0.
 *   uint32_t __gj_cfr_flags_ok_6923  (alias)
 *   __libcgj_batch6923_marker = "libcgj-batch6923"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_flags_ok_6923 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6923_marker[] = "libcgj-batch6923";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6923_flags_ok(uint32_t u32Flags)
{
	return (u32Flags == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_flags_ok_6923 - soft copy_file_range flags acceptance test.
 *
 * flags: soft copy_file_range flags bitmask
 *
 * Returns 1 when flags is zero (accepted), else 0. Pure-data gate;
 * does not call copy_file_range. No parent wires.
 */
uint32_t
gj_cfr_flags_ok_6923(uint32_t u32Flags)
{
	(void)NULL;
	return b6923_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_flags_ok_6923(uint32_t u32Flags)
    __attribute__((alias("gj_cfr_flags_ok_6923")));
