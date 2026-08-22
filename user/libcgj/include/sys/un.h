/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/un.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * AF_UNIX sockaddr_un (Linux sun_path[108]) for OpenSSH sshd (privsep,
 * agent, mux, fd-pass). Layout matches Linux LP64.
 *
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

struct sockaddr_un {
    sa_family_t sun_family;
    char        sun_path[UNIX_PATH_MAX];
};

#ifndef SUN_LEN
#define SUN_LEN(pUn) \
    ((size_t)(((struct sockaddr_un *)0)->sun_path) + strlen((pUn)->sun_path))
#endif

#ifdef __cplusplus
}
#endif
