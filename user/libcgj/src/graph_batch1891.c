/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1891: x86_64 syscall number soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_x86_64_syscall_ok_nr(long nr);
 *     — Soft validity hint: return 1 if nr is a non-negative syscall
 *       number strictly less than 512 (soft table bound used by many
 *       Linux/x86_64 dispatch tables), else 0. Not a guarantee the
 *       number is implemented; pure range data check only.
 *   int __gj_x86_64_syscall_ok_nr  (alias)
 *   __libcgj_batch1891_marker = "libcgj-batch1891"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1891_marker[] = "libcgj-batch1891";

/* Soft upper bound for x86_64 Linux-style syscall number tables. */
#define B1891_NR_SOFT  512u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nr is in [0, B1891_NR_SOFT). */
static int
b1891_ok_nr(long nNr)
{
	if (nNr < 0L) {
		return 0;
	}
	return ((unsigned long)nNr < (unsigned long)B1891_NR_SOFT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_x86_64_syscall_ok_nr — soft ok if 0 <= nr < 512.
 *
 * nr: candidate syscall number (signed; negatives rejected)
 * Returns 1 when in soft range, else 0.
 */
int
gj_x86_64_syscall_ok_nr(long nNr)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1891_ok_nr(nNr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_x86_64_syscall_ok_nr(long nNr)
    __attribute__((alias("gj_x86_64_syscall_ok_nr")));
