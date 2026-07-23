/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5929: shell command surface registry ABI version.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_surf_abi_ver_5929(void);
 *     - Returns the soft ABI version tag for the shell command surface
 *       registry stubs continuum (1). Version table constant only;
 *       does not negotiate a protocol.
 *   uint32_t __gj_shell_cmd_surf_abi_ver_5929  (alias)
 *   __libcgj_batch5929_marker = "libcgj-batch5929"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_surf_abi_ver_5929 surface only;
 * no multi-def. Distinct from gj_shell_cmd_reg_ready_5928. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5929_marker[] = "libcgj-batch5929";

/* Soft ABI version for shell command surface registry stubs. */
#define B5929_SHELL_CMD_SURF_ABI_VER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5929_abi_ver(void)
{
	return B5929_SHELL_CMD_SURF_ABI_VER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_surf_abi_ver_5929 - shell command surface registry ABI ver.
 *
 * Always returns 1. Soft compile-time version tag; does not negotiate
 * and does not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_surf_abi_ver_5929(void)
{
	(void)NULL;
	return b5929_abi_ver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_surf_abi_ver_5929(void)
    __attribute__((alias("gj_shell_cmd_surf_abi_ver_5929")));
