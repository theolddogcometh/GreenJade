/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6821: userfaultfd UFFDIO_API ioctl subcmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uffdio_api_6821(void);
 *     - Returns 0x0000003F (soft UFFDIO_API ioctl sub-command index).
 *       Pure-data catalog stub; not a live userfaultfd ioctl.
 *   uint32_t __gj_uffdio_api_6821  (alias)
 *   __libcgj_batch6821_marker = "libcgj-batch6821"
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

const char __libcgj_batch6821_marker[] = "libcgj-batch6821";

/* Soft UFFDIO_API (negotiate userfaultfd API / features) subcmd. */
#define B6821_UFFDIO_API  0x0000003Fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6821_api(void)
{
	return B6821_UFFDIO_API;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uffdio_api_6821 - soft UFFDIO_API ioctl sub-command catalog index.
 *
 * Always returns 0x0000003F. Pure-data stub for product userfaultfd
 * ioctl catalogs; does not open a userfaultfd or issue ioctl.
 * No parent wires.
 */
uint32_t
gj_uffdio_api_6821(void)
{
	(void)NULL;
	return b6821_api();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uffdio_api_6821(void)
    __attribute__((alias("gj_uffdio_api_6821")));
