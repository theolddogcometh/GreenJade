/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5781: logind stub present lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_stub_present_5781(void);
 *     - Returns 1 (logind seat/session stub surface is present).
 *   uint32_t __gj_logind_stub_present_5781  (alias)
 *   __libcgj_batch5781_marker = "libcgj-batch5781"
 *
 * Exclusive continuum CREATE-ONLY (5781-5790: user session seats,
 * logind stubs, seat0 active). Unique gj_logind_stub_present_5781
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5781_marker[] = "libcgj-batch5781";

/* Logind stub surface compiled into this wave. */
#define B5781_LOGIND_STUB_PRESENT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5781_logind_stub_present(void)
{
	return B5781_LOGIND_STUB_PRESENT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_stub_present_5781 - report logind stub present lamp.
 *
 * Always returns 1 (present). Integer-only product tag; does not open
 * a D-Bus connection to systemd-logind. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_logind_stub_present_5781(void)
{
	(void)NULL;
	return b5781_logind_stub_present();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_stub_present_5781(void)
    __attribute__((alias("gj_logind_stub_present_5781")));
