/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5927: shell command surface flag mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_surf_flag_mask_5927(void);
 *     - Returns the soft valid-bits mask for shell command surface
 *       flags (0x0000000f: bits 0..3). Catalog mask only; does not
 *       interpret live flags.
 *   uint32_t __gj_shell_cmd_surf_flag_mask_5927  (alias)
 *   __libcgj_batch5927_marker = "libcgj-batch5927"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_surf_flag_mask_5927 surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5927_marker[] = "libcgj-batch5927";

/* Soft valid-bits mask for shell command surface flags (bits 0..3). */
#define B5927_SHELL_CMD_SURF_FLAG_MASK  0x0000000fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5927_flag_mask(void)
{
	return B5927_SHELL_CMD_SURF_FLAG_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_surf_flag_mask_5927 - shell command surface flag mask.
 *
 * Always returns 0x0000000f. Soft pure-data mask; does not call libc.
 * No parent wires.
 */
uint32_t
gj_shell_cmd_surf_flag_mask_5927(void)
{
	(void)NULL;
	return b5927_flag_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_surf_flag_mask_5927(void)
    __attribute__((alias("gj_shell_cmd_surf_flag_mask_5927")));
