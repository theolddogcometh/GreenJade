/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7741: getdents dirent stub root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_getdents_dirent_root_7741(void);
 *     - Returns the soft getdents/dirent stub root layout tag for this
 *       continuum (always 0x47444E54, fourCC "GDNT"). Not a live
 *       getdents(2) call.
 *   uint32_t __gj_getdents_dirent_root_7741  (alias)
 *   __libcgj_batch7741_marker = "libcgj-batch7741"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_getdents_dirent_root_7741 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7741_marker[] = "libcgj-batch7741";

/* Getdents/dirent stub root fourCC "GDNT". */
#define B7741_GDNT_ROOT  0x47444E54u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7741_root(void)
{
	return B7741_GDNT_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_getdents_dirent_root_7741 - getdents dirent stub root.
 *
 * Always returns 0x47444E54 ("GDNT"). Soft pure-data layout tag for the
 * 7741-7750 continuum. Does not call getdents or libc.
 * No parent wires.
 */
uint32_t
gj_getdents_dirent_root_7741(void)
{
	(void)NULL;
	return b7741_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_getdents_dirent_root_7741(void)
    __attribute__((alias("gj_getdents_dirent_root_7741")));
