/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12144: journal soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_journal_ok_u_12144(void);
 *     - Returns 0 (soft stub: systemd-journal presence is soft/unprobed;
 *       not a runtime journald or sd-journal probe).
 *   uint32_t __gj_journal_ok_u_12144  (alias)
 *   __libcgj_batch12144_marker = "libcgj-batch12144"
 *
 * Exclusive continuum CREATE-ONLY (12141-12150: dbus/systemd soft id
 * stubs). Unique gj_journal_ok_u_12144 surface only; no multi-def.
 * Distinct from sibling soft ok stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12144_marker[] = "libcgj-batch12144";

/* Soft-stub journal ok value (unprobed / not present). */
#define B12144_JOURNAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12144_journal_ok(void)
{
	return B12144_JOURNAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_journal_ok_u_12144 - soft journal presence stub.
 *
 * Always returns 0 (soft/unprobed journal slot). Does not call libc or
 * probe systemd-journald. No parent wires.
 */
uint32_t
gj_journal_ok_u_12144(void)
{
	(void)NULL;
	return b12144_journal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_journal_ok_u_12144(void)
    __attribute__((alias("gj_journal_ok_u_12144")));
