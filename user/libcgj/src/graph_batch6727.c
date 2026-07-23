/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6727: clone CLONE_SETTLS flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_settls_u_6727(uint32_t flags);
 *     - Return 1 if CLONE_SETTLS (set TLS descriptor) is set, else 0.
 *   uint32_t __gj_clone_settls_u_6727  (alias)
 *   __libcgj_batch6727_marker = "libcgj-batch6727"
 *
 * Exclusive continuum CREATE-ONLY (6721-6730: clone flags stubs —
 * vm, fs, files, sighand, thread, sysvsem, settls, parent_settid,
 * threadish, batch_id_6730). Unique gj_clone_settls_u_6727 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6727_marker[] = "libcgj-batch6727";

/* CLONE_SETTLS: create a new TLS for the child (0x00080000). */
#define B6727_CLONE_SETTLS ((uint32_t)0x00080000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6727_clone_settls(uint32_t u32Flags)
{
	return ((u32Flags & B6727_CLONE_SETTLS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_settls_u_6727 - 1 if CLONE_SETTLS is set in clone flags mask.
 *
 * flags: raw clone / clone3 flags bitmask
 *
 * Returns 1 when the set-TLS bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_clone_settls_u_6727(uint32_t u32Flags)
{
	(void)NULL;
	return b6727_clone_settls(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_settls_u_6727(uint32_t u32Flags)
    __attribute__((alias("gj_clone_settls_u_6727")));
