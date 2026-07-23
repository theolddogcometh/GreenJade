/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6877: fsconfig FSCONFIG_CMD_CREATE cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_create_6877(void);
 *     - Returns 6 (soft FSCONFIG_CMD_CREATE fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_create_6877  (alias)
 *   __libcgj_batch6877_marker = "libcgj-batch6877"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_create_6877 surface only; no
 * multi-def. Distinct from set_fd (batch6876) and cmd_reconfigure
 * (batch6878). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6877_marker[] = "libcgj-batch6877";

/* Soft FSCONFIG_CMD_CREATE (invoke superblock creation). */
#define B6877_FSCONFIG_CMD_CREATE  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6877_cmd_create(void)
{
	return B6877_FSCONFIG_CMD_CREATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_create_6877 - soft FSCONFIG_CMD_CREATE cmd catalog id.
 *
 * Always returns 6. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or create a superblock. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_create_6877(void)
{
	(void)NULL;
	return b6877_cmd_create();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_create_6877(void)
    __attribute__((alias("gj_fsconfig_cmd_create_6877")));
