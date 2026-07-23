/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6878: fsconfig FSCONFIG_CMD_RECONFIGURE cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_reconfigure_6878(void);
 *     - Returns 7 (soft FSCONFIG_CMD_RECONFIGURE fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_reconfigure_6878  (alias)
 *   __libcgj_batch6878_marker = "libcgj-batch6878"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_reconfigure_6878 surface only; no
 * multi-def. Distinct from cmd_create (batch6877) and cmd_ok
 * (batch6879). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6878_marker[] = "libcgj-batch6878";

/* Soft FSCONFIG_CMD_RECONFIGURE (invoke superblock reconfiguration). */
#define B6878_FSCONFIG_CMD_RECONFIGURE  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6878_cmd_reconfigure(void)
{
	return B6878_FSCONFIG_CMD_RECONFIGURE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_reconfigure_6878 - soft FSCONFIG_CMD_RECONFIGURE.
 *
 * Always returns 7. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or reconfigure a superblock. No
 * parent wires.
 */
uint32_t
gj_fsconfig_cmd_reconfigure_6878(void)
{
	(void)NULL;
	return b6878_cmd_reconfigure();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_reconfigure_6878(void)
    __attribute__((alias("gj_fsconfig_cmd_reconfigure_6878")));
