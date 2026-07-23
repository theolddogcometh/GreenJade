/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8734: MS_NOEXEC mount flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_noexec_id_8734(void);
 *     - Return soft MS_NOEXEC mount flag bit (8).
 *   uint32_t __gj_ms_noexec_id_8734  (alias)
 *   __libcgj_batch8734_marker = "libcgj-batch8734"
 *
 * Exclusive continuum CREATE-ONLY (8731-8740: mount/umount flag id stubs —
 * ms_rdonly_id, ms_nosuid_id, ms_nodev_id, ms_noexec_id, ms_remount_id,
 * ms_bind_id, ms_rec_id, umount_force_id, mount_ready_u, batch_id_8740).
 * Unique gj_ms_noexec_id_8734 surface only; no multi-def. Catalog id only;
 * no mount/umount syscalls. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8734_marker[] = "libcgj-batch8734";

/* Soft mount flag: MS_NOEXEC (disallow program execution). */
#define B8734_MS_NOEXEC  ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8734_noexec_id(void)
{
	return B8734_MS_NOEXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_noexec_id_8734 - soft MS_NOEXEC mount flag id.
 *
 * Always returns 8. Catalog id only; does not call mount. No parent
 * wires.
 */
uint32_t
gj_ms_noexec_id_8734(void)
{
	(void)NULL;
	return b8734_noexec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_noexec_id_8734(void)
    __attribute__((alias("gj_ms_noexec_id_8734")));
