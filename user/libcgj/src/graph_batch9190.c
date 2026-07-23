/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9190: install/image soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9190(void);
 *     - Returns the compile-time graph batch number for this TU (9190).
 *   uint32_t __gj_batch_id_9190  (alias)
 *   __libcgj_batch9190_marker = "libcgj-batch9190"
 *
 * Exclusive continuum CREATE-ONLY (9181-9190: install/image soft stubs —
 * install_root_tag_9181, install_efi_tag_9182, install_boot_tag_9183,
 * install_rootfs_tag_9184, install_partition_ok_u_9185,
 * install_write_ready_u_9186, install_verify_ready_u_9187,
 * install_reboot_ready_u_9188, install_soft_ready_u_9189,
 * batch_id_9190). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9190_marker[] = "libcgj-batch9190";

#define B9190_BATCH_ID  9190u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9190_id(void)
{
	return B9190_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9190 - report this TU's graph batch number.
 *
 * Always returns 9190.
 */
uint32_t
gj_batch_id_9190(void)
{
	(void)NULL;
	return b9190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9190(void)
    __attribute__((alias("gj_batch_id_9190")));
