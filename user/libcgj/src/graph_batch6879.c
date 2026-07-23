/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6879: fsconfig cmd catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_ok_6879(uint32_t cmd);
 *     - Returns 1 when cmd is in soft fsconfig cmd catalog range
 *       [0, 8) (SET_FLAG..CMD_RECONFIGURE), else 0.
 *       Pure-data range stub; not a live fsconfig validation.
 *   uint32_t __gj_fsconfig_cmd_ok_6879  (alias)
 *   __libcgj_batch6879_marker = "libcgj-batch6879"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs — set_flag_6871, set_string_6872, set_binary_6873,
 * set_path_6874, set_path_empty_6875, set_fd_6876, cmd_create_6877,
 * cmd_reconfigure_6878, cmd_ok_6879, continuum + batch_id_6880).
 * Unique surface only; no multi-def. Soft bound covers the eight
 * classic fsconfig(2) commands stubbed in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6879_marker[] = "libcgj-batch6879";

/* Soft exclusive ceiling (FSCONFIG_SET_FLAG..FSCONFIG_CMD_RECONFIGURE). */
#define B6879_FSCONFIG_CMD_MAX  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6879_cmd_ok(uint32_t uCmd)
{
	return (uCmd < B6879_FSCONFIG_CMD_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_ok_6879 - soft fsconfig cmd catalog membership.
 *
 * cmd: candidate fsconfig(2) command id.
 * Returns 1 if cmd is in [0, 8), else 0. Does not query the kernel.
 * No parent wires.
 */
uint32_t
gj_fsconfig_cmd_ok_6879(uint32_t cmd)
{
	(void)NULL;
	return b6879_cmd_ok(cmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_ok_6879(uint32_t cmd)
    __attribute__((alias("gj_fsconfig_cmd_ok_6879")));
