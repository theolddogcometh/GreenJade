/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9613: docker container ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_docker_container_ok_u_9613(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       docker container status / ps probe.
 *   uint32_t __gj_docker_container_ok_u_9613  (alias)
 *   __libcgj_batch9613_marker = "libcgj-batch9613"
 *
 * Exclusive continuum CREATE-ONLY (9611-9620: docker soft id
 * stubs — docker_ok_u_9611, docker_image_ok_u_9612,
 * docker_container_ok_u_9613, docker_network_ok_u_9614,
 * docker_volume_ok_u_9615, docker_build_ok_u_9616,
 * docker_run_ok_u_9617, docker_compose_ok_u_9618, docker_ready_u_9619,
 * batch_id_9620). Unique surface only; no multi-def. No parent wires.
 * No __int128. No docker/compose implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9613_marker[] = "libcgj-batch9613";

/* Soft docker-container-ok lamp: always off (not a real container probe). */
#define B9613_DOCKER_CONTAINER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9613_docker_container_ok(void)
{
	return B9613_DOCKER_CONTAINER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_docker_container_ok_u_9613 - docker container ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect containers
 * or call libc. No parent wires.
 */
uint32_t
gj_docker_container_ok_u_9613(void)
{
	(void)NULL;
	return b9613_docker_container_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_docker_container_ok_u_9613(void)
    __attribute__((alias("gj_docker_container_ok_u_9613")));
