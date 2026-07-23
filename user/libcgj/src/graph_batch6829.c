/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6829: userfaultfd UFFDIO subcmd catalog membership.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uffdio_cmd_ok_6829(uint32_t subcmd);
 *     - Returns 1 when subcmd is a known soft UFFDIO_* index from this
 *       exclusive wave catalog (API|REGISTER|UNREGISTER|WAKE|COPY|
 *       ZEROPAGE|WRITEPROTECT|CONTINUE), else 0. Pure-data mask stub;
 *       not a live userfaultfd validation.
 *   uint32_t __gj_uffdio_cmd_ok_6829  (alias)
 *   __libcgj_batch6829_marker = "libcgj-batch6829"
 *
 * Exclusive continuum CREATE-ONLY (6821-6830: userfaultfd ioctl stubs —
 * uffdio_api_6821, uffdio_register_6822, uffdio_unregister_6823,
 * uffdio_wake_6824, uffdio_copy_6825, uffdio_zeropage_6826,
 * uffdio_writeprotect_6827, uffdio_continue_6828, uffdio_cmd_ok_6829,
 * continuum + batch_id_6830). Unique surface only; no multi-def.
 * Soft known set covers classic UFFDIO sub-commands. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6829_marker[] = "libcgj-batch6829";

/*
 * Soft known UFFDIO sub-command indices from this exclusive wave:
 *   0x00 REGISTER, 0x01 UNREGISTER, 0x02 WAKE, 0x03 COPY,
 *   0x04 ZEROPAGE, 0x06 WRITEPROTECT, 0x07 CONTINUE, 0x3F API.
 * Membership is exact-match (not a bitfield) against the catalog.
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6829_cmd_ok(uint32_t uSub)
{
	if (uSub == 0x00u || uSub == 0x01u || uSub == 0x02u ||
	    uSub == 0x03u || uSub == 0x04u || uSub == 0x06u ||
	    uSub == 0x07u || uSub == 0x3Fu) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uffdio_cmd_ok_6829 - soft UFFDIO sub-command catalog membership.
 *
 * subcmd: candidate UFFDIO ioctl sub-command index.
 * Returns 1 if subcmd is in the known soft catalog
 * {0x00,0x01,0x02,0x03,0x04,0x06,0x07,0x3F}, else 0.
 * Does not query the kernel. No parent wires.
 */
uint32_t
gj_uffdio_cmd_ok_6829(uint32_t subcmd)
{
	(void)NULL;
	return b6829_cmd_ok(subcmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uffdio_cmd_ok_6829(uint32_t subcmd)
    __attribute__((alias("gj_uffdio_cmd_ok_6829")));
