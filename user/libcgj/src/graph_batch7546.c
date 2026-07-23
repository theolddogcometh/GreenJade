/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7546: sigaction flag code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigaction_flag_code_7546(void);
 *     - Returns the soft sigaction-flag code tag for this continuum
 *       (always 0x53463735, fourCC "SF75"). Not a live signal install.
 *   uint32_t __gj_sigaction_flag_code_7546  (alias)
 *   __libcgj_batch7546_marker = "libcgj-batch7546"
 *
 * Exclusive continuum CREATE-ONLY (7541-7550: sigaction flag stubs).
 * Unique gj_sigaction_flag_code_7546 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7546_marker[] = "libcgj-batch7546";

/* Sigaction flag code fourCC "SF75". */
#define B7546_SAF_CODE  0x53463735u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7546_code(void)
{
	return B7546_SAF_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigaction_flag_code_7546 - sigaction flag code tag.
 *
 * Always returns 0x53463735 ("SF75"). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_sigaction_flag_code_7546(void)
{
	(void)NULL;
	return b7546_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sigaction_flag_code_7546(void)
    __attribute__((alias("gj_sigaction_flag_code_7546")));
