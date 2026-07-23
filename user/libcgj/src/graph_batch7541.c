/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7541: sigaction flag stub root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigaction_flag_root_7541(void);
 *     - Returns the soft sigaction-flag stub root layout tag for this
 *       continuum (always 0x53414652, fourCC "SAFR"). Not a live
 *       sigaction(2) install.
 *   uint32_t __gj_sigaction_flag_root_7541  (alias)
 *   __libcgj_batch7541_marker = "libcgj-batch7541"
 *
 * Exclusive continuum CREATE-ONLY (7541-7550: sigaction flag stubs).
 * Unique gj_sigaction_flag_root_7541 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7541_marker[] = "libcgj-batch7541";

/* Sigaction flag stub root fourCC "SAFR". */
#define B7541_SAF_ROOT  0x53414652u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7541_root(void)
{
	return B7541_SAF_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigaction_flag_root_7541 - sigaction flag stub root.
 *
 * Always returns 0x53414652 ("SAFR"). Soft pure-data layout tag for the
 * 7541-7550 continuum. Does not call sigaction or libc.
 * No parent wires.
 */
uint32_t
gj_sigaction_flag_root_7541(void)
{
	(void)NULL;
	return b7541_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigaction_flag_root_7541(void)
    __attribute__((alias("gj_sigaction_flag_root_7541")));
