/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <netinet/tcp.h> for libcgj (GreenJade
 * freestanding libc). Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Linux TCP_* setsockopt numbers. OpenSSH sshd (misc.c) needs TCP_NODELAY.
 * Values match linux/tcp.h. Socket calls go through existing setsockopt.
 *
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_NODELAY      1
#define TCP_MAXSEG       2
#define TCP_CORK         3
#define TCP_KEEPIDLE     4
#define TCP_KEEPINTVL    5
#define TCP_KEEPCNT      6
#define TCP_SYNCNT       7
#define TCP_LINGER2      8
#define TCP_DEFER_ACCEPT 9
#define TCP_WINDOW_CLAMP 10
#define TCP_INFO         11
#define TCP_QUICKACK     12
#define TCP_CONGESTION   13
#define TCP_MD5SIG       14
#define TCP_FASTOPEN     23

#ifndef SOL_TCP
#define SOL_TCP 6
#endif

#ifndef TH_FIN
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#endif

#ifdef __cplusplus
}
#endif
