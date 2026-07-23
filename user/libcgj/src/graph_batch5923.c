/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5923: shell command surface name max length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_surf_name_max_5923(void);
 *     - Returns the soft max length of a shell command surface name
 *       string in bytes excluding NUL (48). Catalog bound only; does
 *       not parse argv.
 *   uint32_t __gj_shell_cmd_surf_name_max_5923  (alias)
 *   __libcgj_batch5923_marker = "libcgj-batch5923"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_surf_name_max_5923 surface only;
 * no multi-def. Distinct from gj_shell_cmd_reg_cap_5921 and
 * gj_shell_cmd_reg_stride_5922. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5923_marker[] = "libcgj-batch5923";

/* Soft max surface name length (bytes, excluding NUL). */
#define B5923_SHELL_CMD_SURF_NAME_MAX  48u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5923_name_max(void)
{
	return B5923_SHELL_CMD_SURF_NAME_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_surf_name_max_5923 - shell command surface name max.
 *
 * Always returns 48. Soft pure-data bound; does not walk strings or
 * call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_surf_name_max_5923(void)
{
	(void)NULL;
	return b5923_name_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_surf_name_max_5923(void)
    __attribute__((alias("gj_shell_cmd_surf_name_max_5923")));
