/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6823: userfaultfd UFFDIO_UNREGISTER ioctl subcmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uffdio_unregister_6823(void);
 *     - Returns 0x00000001 (soft UFFDIO_UNREGISTER ioctl sub-command index).
 *       Pure-data catalog stub; not a live userfaultfd ioctl.
 *   uint32_t __gj_uffdio_unregister_6823  (alias)
 *   __libcgj_batch6823_marker = "libcgj-batch6823"
 *
 * Exclusive continuum CREATE-ONLY (6821-6830: userfaultfd ioctl stubs —
 * uffdio_api_6821, uffdio_register_6822, uffdio_unregister_6823,
 * uffdio_wake_6824, uffdio_copy_6825, uffdio_zeropage_6826,
 * uffdio_writeprotect_6827, uffdio_continue_6828, uffdio_cmd_ok_6829,
 * continuum + batch_id_6830). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6823_marker[] = "libcgj-batch6823";

/* Soft UFFDIO_UNREGISTER (drop a registered memory range) subcmd. */
#define B6823_UFFDIO_UNREGISTER  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6823_unregister(void)
{
	return B6823_UFFDIO_UNREGISTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uffdio_unregister_6823 - soft UFFDIO_UNREGISTER ioctl sub-command index.
 *
 * Always returns 0x00000001. Pure-data stub for product userfaultfd
 * ioctl catalogs; does not open a userfaultfd or issue ioctl.
 * No parent wires.
 */
uint32_t
gj_uffdio_unregister_6823(void)
{
	(void)NULL;
	return b6823_unregister();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uffdio_unregister_6823(void)
    __attribute__((alias("gj_uffdio_unregister_6823")));
