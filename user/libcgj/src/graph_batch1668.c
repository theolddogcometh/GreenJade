/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1668: root UID predicate.
 *
 * Surface (unique symbols):
 *   int gj_uid_is_root_p(uint32_t uid);
 *     — Return 1 if uid is 0 (traditional superuser), else 0.
 *       Pure compare; no getuid/geteuid syscalls.
 *   int __gj_uid_is_root_p  (alias)
 *   __libcgj_batch1668_marker = "libcgj-batch1668"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1668_marker[] = "libcgj-batch1668";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uid_is_root_p — 1 if uid denotes the root user (0).
 *
 * Predicate form (trailing _p): 1/0. Suitable for policy gates over
 * parsed credential fields without touching the kernel.
 */
int
gj_uid_is_root_p(uint32_t u32Uid)
{
	return (u32Uid == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uid_is_root_p(uint32_t u32Uid)
    __attribute__((alias("gj_uid_is_root_p")));
