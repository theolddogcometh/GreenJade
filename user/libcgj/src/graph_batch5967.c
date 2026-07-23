/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5967: Steam Deck onboard controller PID match.
 *
 * Surface (unique symbols):
 *   int gj_deck_pid_is_controller_5967(uint32_t pid);
 *     - Return 1 if pid equals the Steam Deck onboard controller USB
 *       product id 0x1205, else 0. Narrower than the multi-PID known set.
 *   int __gj_deck_pid_is_controller_5967  (alias)
 *   __libcgj_batch5967_marker = "libcgj-batch5967"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_ctrl_pid_is_known_5963 (multi-PID soft set).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5967_marker[] = "libcgj-batch5967";

/* Steam Deck (Jupiter) onboard controller product id. */
#define B5967_DECK_CTRL_PID  0x1205u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5967_pid_is_controller(uint32_t uPid)
{
	if (uPid == B5967_DECK_CTRL_PID) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_pid_is_controller_5967 - soft check for Deck controller PID.
 *
 * pid: USB product id (16-bit value zero-extended to uint32_t)
 *
 * Returns 1 iff pid == 0x1205, else 0. No parent wires.
 */
int
gj_deck_pid_is_controller_5967(uint32_t uPid)
{
	(void)NULL;
	return b5967_pid_is_controller(uPid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_pid_is_controller_5967(uint32_t uPid)
    __attribute__((alias("gj_deck_pid_is_controller_5967")));
