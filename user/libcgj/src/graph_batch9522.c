/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9522: soft Python embed module-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_py_module_ok_u_9522(void);
 *     - Returns 1 (Python embed soft-id continuum module ok). Soft
 *       pure-data product tag; does not load or register modules.
 *   uint32_t __gj_py_module_ok_u_9522  (alias)
 *   __libcgj_batch9522_marker = "libcgj-batch9522"
 *
 * Exclusive continuum CREATE-ONLY (9521-9530: python embed soft id stubs —
 * py_interp_ok_u_9521, py_module_ok_u_9522, py_object_ok_u_9523,
 * py_dict_ok_u_9524, py_list_ok_u_9525, py_tuple_ok_u_9526,
 * py_run_ok_u_9527, py_import_ok_u_9528, python_ready_u_9529,
 * batch_id_9530).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No Python / CPython implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9522_marker[] = "libcgj-batch9522";

/* Soft Python embed module-ok flag. */
#define B9522_MODULE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9522_module_ok(void)
{
	return B9522_MODULE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_py_module_ok_u_9522 - Python embed soft-id continuum module-ok.
 *
 * Always returns 1. Soft pure-data product tag; does not load modules
 * or call libc. No parent wires.
 */
uint32_t
gj_py_module_ok_u_9522(void)
{
	(void)NULL;
	return b9522_module_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_py_module_ok_u_9522(void)
    __attribute__((alias("gj_py_module_ok_u_9522")));
