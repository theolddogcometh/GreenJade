/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9820: efi boot soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9820(void);
 *     - Returns the compile-time graph batch number for this TU (9820).
 *   uint32_t __gj_batch_id_9820  (alias)
 *   __libcgj_batch9820_marker = "libcgj-batch9820"
 *
 * Exclusive continuum CREATE-ONLY (9811-9820: efi boot soft id
 * stubs — efi_system_table_ok_u_9811, efi_boot_services_ok_u_9812,
 * efi_runtime_ok_u_9813, efi_gop_ok_u_9814, efi_fs_ok_u_9815,
 * efi_load_ok_u_9816, efi_exit_boot_ok_u_9817, efi_var_ok_u_9818,
 * efi_ready_u_9819, batch_id_9820). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9820_marker[] = "libcgj-batch9820";

#define B9820_BATCH_ID  9820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9820_id(void)
{
	return B9820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9820 - report this TU's graph batch number.
 *
 * Always returns 9820.
 */
uint32_t
gj_batch_id_9820(void)
{
	(void)NULL;
	return b9820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9820(void)
    __attribute__((alias("gj_batch_id_9820")));
