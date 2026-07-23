/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6648: signalfd flags soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_flags_ok_6648(uint32_t flags);
 *     - Returns 1 when flags uses only soft SFD_CLOEXEC|SFD_NONBLOCK
 *       catalog bits (0x80000 | 0x800), else 0. Pure-data flag check.
 *   uint32_t __gj_sfd_flags_ok_6648  (alias)
 *   __libcgj_batch6648_marker = "libcgj-batch6648"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_flags_ok_6648 surface only; no multi-def. Soft flag
 * catalog only (not a live signalfd). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6648_marker[] = "libcgj-batch6648";

/*
 * Soft catalog mirrors common Linux signalfd flags:
 *   SFD_NONBLOCK = 0x800
 *   SFD_CLOEXEC  = 0x80000
 */
#define B6648_SFD_NONBLOCK  0x800u
#define B6648_SFD_CLOEXEC   0x80000u
#define B6648_SFD_ALL       (B6648_SFD_NONBLOCK | B6648_SFD_CLOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6648_flags_ok(uint32_t uFlags)
{
	if ((uFlags & ~B6648_SFD_ALL) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_flags_ok_6648 - soft signalfd flags catalog membership.
 *
 * flags: candidate signalfd create flags.
 * Returns 1 if only known SFD_NONBLOCK|SFD_CLOEXEC bits (or none) are
 * set, else 0. Does not open a signalfd. No parent wires.
 */
uint32_t
gj_sfd_flags_ok_6648(uint32_t flags)
{
	(void)NULL;
	return b6648_flags_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_flags_ok_6648(uint32_t flags)
    __attribute__((alias("gj_sfd_flags_ok_6648")));
