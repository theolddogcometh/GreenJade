/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2091: freestanding nonzero id predicate.
 *
 * Surface (unique symbols):
 *   int gj_id_is_nonzero_p(uint64_t id);
 *     - Return 1 if id is nonzero, else 0. Pure integer predicate.
 *   int __gj_id_is_nonzero_p  (alias)
 *   __libcgj_batch2091_marker = "libcgj-batch2091"
 *
 * Post-2000 id exclusive wave (2091-2100). Unique gj_id_is_nonzero_p
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2091_marker[] = "libcgj-batch2091";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2091_nonzero_p(uint64_t u64Id)
{
	return u64Id != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_id_is_nonzero_p - true if id is a nonzero identifier value.
 *
 * id: opaque 64-bit id word (0 is treated as unset or nil)
 *
 * Returns 1 when id != 0, else 0. Does not call libc.
 */
int
gj_id_is_nonzero_p(uint64_t u64Id)
{
	(void)NULL;
	return b2091_nonzero_p(u64Id);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_id_is_nonzero_p(uint64_t u64Id)
    __attribute__((alias("gj_id_is_nonzero_p")));
