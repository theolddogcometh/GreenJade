/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5963: Steam Deck / Steam controller PID known
 * soft predicate.
 *
 * Surface (unique symbols):
 *   int gj_deck_ctrl_pid_is_known_5963(uint32_t pid);
 *     - Return 1 if pid is a soft-known Valve controller product id
 *       used with Deck / Steam Input style pads (0x1205 Deck, 0x1102 /
 *       0x1142 Steam Controller family), else 0.
 *   int __gj_deck_ctrl_pid_is_known_5963  (alias)
 *   __libcgj_batch5963_marker = "libcgj-batch5963"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_ctrl_usb_pid_5962. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5963_marker[] = "libcgj-batch5963";

/* Soft-known Valve controller product ids (public idProduct values). */
#define B5963_PID_DECK_CTRL   0x1205u
#define B5963_PID_STEAM_CTRL  0x1102u
#define B5963_PID_STEAM_WIRE  0x1142u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5963_pid_is_known(uint32_t uPid)
{
	if (uPid == B5963_PID_DECK_CTRL) {
		return 1;
	}
	if (uPid == B5963_PID_STEAM_CTRL) {
		return 1;
	}
	if (uPid == B5963_PID_STEAM_WIRE) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_ctrl_pid_is_known_5963 - soft check for known controller PIDs.
 *
 * pid: USB product id (16-bit value zero-extended to uint32_t)
 *
 * Returns 1 for 0x1205 / 0x1102 / 0x1142, else 0. Catalog-free stub.
 * No parent wires.
 */
int
gj_deck_ctrl_pid_is_known_5963(uint32_t uPid)
{
	(void)NULL;
	return b5963_pid_is_known(uPid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_ctrl_pid_is_known_5963(uint32_t uPid)
    __attribute__((alias("gj_deck_ctrl_pid_is_known_5963")));
