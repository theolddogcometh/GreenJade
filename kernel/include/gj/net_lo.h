/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Loopback socket stubs for cold Linux net path (pre-netstackd).
 * Pure C, dual-licensed. Product STREAM path uses net_tcp via the door.
 *
 * Soft: listen backlog clamp, SOL_SOCKET get/set subset (REUSEADDR/PORT,
 * BROADCAST, KEEPALIVE, SNDBUF/RCVBUF, LINGER, ACCEPTCONN), bind EADDRINUSE.
 */
#pragma once

#include <gj/types.h>

void net_lo_init(void);

/** socket(AF_INET|AF_UNIX, SOCK_STREAM/DGRAM, 0) → fd or -errno */
i64 net_lo_socket(int nDomain, int nType, int nProto);

i64 net_lo_bind(i64 i64Fd, u16 u16Port);
/** listen: store/clamp backlog (soft accept-queue depth). */
i64 net_lo_listen(i64 i64Fd, int nBacklog);
i64 net_lo_connect(i64 i64Fd, u16 u16Port);
/** accept: return connected peer fd or -EAGAIN if none pending. */
i64 net_lo_accept(i64 i64Fd);
i64 net_lo_send(i64 i64Fd, const void *pBuf, size_t cb);
i64 net_lo_recv(i64 i64Fd, void *pBuf, size_t cb);
i64 net_lo_close(i64 i64Fd);
int net_lo_fd_ok(i64 i64Fd);
/** shutdown: how 0=RD 1=WR 2=RDWR — mark half-closed. */
i64 net_lo_shutdown(i64 i64Fd, int nHow);
/**
 * getsockopt/setsockopt: SOL_SOCKET subset
 * (TYPE, ERROR, REUSEADDR/PORT, BROADCAST, KEEPALIVE, SND/RCVBUF,
 *  LINGER soft, ACCEPTCONN).
 */
i64 net_lo_getsockopt(i64 i64Fd, int nLevel, int nOpt, void *pVal, u32 *pLen);
i64 net_lo_setsockopt(i64 i64Fd, int nLevel, int nOpt, const void *pVal, u32 u32Len);
/** getsockname/getpeername: fill sockaddr_in-shaped 16-byte buffer. */
i64 net_lo_getsockname(i64 i64Fd, void *pAddr, u32 *pLen);
i64 net_lo_getpeername(i64 i64Fd, void *pAddr, u32 *pLen);
