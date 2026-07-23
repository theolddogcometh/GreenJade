/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8760: acl/mode bit continuum batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8760(void);
 *     - Returns the compile-time graph batch number for this TU (8760).
 *   uint32_t __gj_batch_id_8760  (alias)
 *   __libcgj_batch8760_marker = "libcgj-batch8760"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique surfaces only; no multi-def. Does NOT
 * redefine bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8760_marker[] = "libcgj-batch8760";

#define B8760_BATCH_ID  8760u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8760_id(void)
{
	return B8760_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8760 - report this TU's graph batch number.
 *
 * Always returns 8760.
 */
uint32_t
gj_batch_id_8760(void)
{
	(void)NULL;
	return b8760_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8760(void)
    __attribute__((alias("gj_batch_id_8760")));
