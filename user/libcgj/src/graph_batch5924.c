/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5924: shell command registry slot soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_shell_cmd_reg_slot_ok_5924(uint32_t slot);
 *     - Return 1 if slot is a plausible shell command surface registry
 *       index: strictly less than 256 (matches soft capacity from
 *       batch5921). Else 0. Soft range check only; does not mutate a
 *       registry.
 *   int __gj_shell_cmd_reg_slot_ok_5924  (alias)
 *   __libcgj_batch5924_marker = "libcgj-batch5924"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_slot_ok_5924 surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5924_marker[] = "libcgj-batch5924";

/* Soft exclusive upper bound for shell command registry slots. */
#define B5924_SHELL_CMD_REG_CAP  256u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if slot is in [0, B5924_SHELL_CMD_REG_CAP). */
static int
b5924_slot_ok(uint32_t uSlot)
{
	if (uSlot >= B5924_SHELL_CMD_REG_CAP) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_slot_ok_5924 - soft shell command registry slot check.
 *
 * slot: candidate shell command surface registry index
 *
 * Returns 1 when slot < 256, else 0. Does not call libc or touch a
 * live registry. No parent wires.
 */
int
gj_shell_cmd_reg_slot_ok_5924(uint32_t uSlot)
{
	(void)NULL;
	return b5924_slot_ok(uSlot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_cmd_reg_slot_ok_5924(uint32_t uSlot)
    __attribute__((alias("gj_shell_cmd_reg_slot_ok_5924")));
