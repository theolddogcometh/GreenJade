/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10144: journal soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_journal_ok_u_10144(void);
 *     - Returns 0 (soft stub: systemd-journal presence is soft/unprobed;
 *       not a runtime journald or sd-journal probe).
 *   uint32_t __gj_journal_ok_u_10144  (alias)
 *   __libcgj_batch10144_marker = "libcgj-batch10144"
 *
 * Exclusive continuum CREATE-ONLY (10141-10150: dbus/systemd soft id
 * stubs). Unique gj_journal_ok_u_10144 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10144_marker[] = "libcgj-batch10144";

/* Soft-stub journal ok value (unprobed / not present). */
#define B10144_JOURNAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10144_journal_ok(void)
{
	return B10144_JOURNAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_journal_ok_u_10144 - soft journal presence stub.
 *
 * Always returns 0 (soft/unprobed journal slot). Does not call libc or
 * probe systemd-journald. No parent wires.
 */
uint32_t
gj_journal_ok_u_10144(void)
{
	(void)NULL;
	return b10144_journal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_journal_ok_u_10144(void)
    __attribute__((alias("gj_journal_ok_u_10144")));
