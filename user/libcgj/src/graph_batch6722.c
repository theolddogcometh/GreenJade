/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6722: clone CLONE_FS flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_fs_u_6722(uint32_t flags);
 *     - Return 1 if CLONE_FS (share filesystem info) is set, else 0.
 *   uint32_t __gj_clone_fs_u_6722  (alias)
 *   __libcgj_batch6722_marker = "libcgj-batch6722"
 *
 * Exclusive continuum CREATE-ONLY (6721-6730: clone flags stubs —
 * vm, fs, files, sighand, thread, sysvsem, settls, parent_settid,
 * threadish, batch_id_6730). Unique gj_clone_fs_u_6722 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6722_marker[] = "libcgj-batch6722";

/* CLONE_FS: share filesystem info (0x00000200). */
#define B6722_CLONE_FS ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6722_clone_fs(uint32_t u32Flags)
{
	return ((u32Flags & B6722_CLONE_FS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_fs_u_6722 - 1 if CLONE_FS is set in clone flags mask.
 *
 * flags: raw clone / clone3 flags bitmask
 *
 * Returns 1 when the share-FS bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_clone_fs_u_6722(uint32_t u32Flags)
{
	(void)NULL;
	return b6722_clone_fs(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_fs_u_6722(uint32_t u32Flags)
    __attribute__((alias("gj_clone_fs_u_6722")));
