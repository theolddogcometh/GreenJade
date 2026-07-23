/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9620: docker soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9620(void);
 *     - Returns the compile-time graph batch number for this TU (9620).
 *   uint32_t __gj_batch_id_9620  (alias)
 *   __libcgj_batch9620_marker = "libcgj-batch9620"
 *
 * Exclusive continuum CREATE-ONLY (9611-9620: docker soft id
 * stubs — docker_ok_u_9611, docker_image_ok_u_9612,
 * docker_container_ok_u_9613, docker_network_ok_u_9614,
 * docker_volume_ok_u_9615, docker_build_ok_u_9616,
 * docker_run_ok_u_9617, docker_compose_ok_u_9618, docker_ready_u_9619,
 * batch_id_9620). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9620_marker[] = "libcgj-batch9620";

#define B9620_BATCH_ID  9620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9620_id(void)
{
	return B9620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9620 - report this TU's graph batch number.
 *
 * Always returns 9620.
 */
uint32_t
gj_batch_id_9620(void)
{
	(void)NULL;
	return b9620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9620(void)
    __attribute__((alias("gj_batch_id_9620")));
