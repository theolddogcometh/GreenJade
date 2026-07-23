/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Loopback socket table for cold Linux net path (pre-full-netstackd).
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * Product STREAM multi-seg path uses net_tcp via the door; this module
 * owns DGRAM + non-TCP STREAM fallback, AF_UNIX-shaped sockets, and the
 * SOL_SOCKET subset exercised by cold personality / sshd smokes.
 *
 * FD layout (avoid vfs_ram 3..31 collision):
 *   FDs 64..79 → 16 slots (NET_LO_MAX). net_lo_fd_ok is the ownership test.
 *
 * Soft product deepenings (v1):
 *   - listen backlog stored/clamped (NET_LO_BACKLOG_MAX=8); connect rejects
 *     when accept queue full
 *   - SOL_SOCKET subset: REUSEADDR, REUSEPORT, TYPE, ERROR, BROADCAST,
 *     KEEPALIVE, SNDBUF/RCVBUF (stored), LINGER soft, ACCEPTCONN
 *   - bind EADDRINUSE when port taken without reuse
 *   - shutdown half-close marks (RD/WR/RDWR)
 *   - peer ring buffer NET_LO_BUF (512) for send/recv
 *
 * Door integration: GJ_NET_OP_SOCKET..ACCEPT may route STREAM AF_INET
 * to net_tcp; DGRAM/UNIX stay here. Callers must use net_*_fd_ok for the
 * matching table.
 *
 * Greppable: net_lo: init (loopback + peer ring + sockopt/backlog soft)
 */
#pragma once

#include <gj/types.h>

/** Zero socket table. Safe to re-call (drops live sockets soft). */
void net_lo_init(void);

/**
 * socket(AF_INET|AF_UNIX, SOCK_STREAM/DGRAM, 0) → fd or -errno.
 * proto ignored for bring-up. EMFILE when table full.
 */
i64 net_lo_socket(int nDomain, int nType, int nProto);

/**
 * bind: associate local port (host order). EADDRINUSE without reuse flags
 * when another live socket holds the port.
 */
i64 net_lo_bind(i64 i64Fd, u16 u16Port);

/** listen: store/clamp backlog (soft accept-queue depth 1..BACKLOG_MAX). */
i64 net_lo_listen(i64 i64Fd, int nBacklog);

/**
 * connect: pair to local listener on port (STREAM) or set peer (DGRAM soft).
 * -EAGAIN / -ECONNREFUSED-shaped when no listener or backlog full.
 */
i64 net_lo_connect(i64 i64Fd, u16 u16Port);

/** accept: return connected peer fd or -EAGAIN if none pending. */
i64 net_lo_accept(i64 i64Fd);

/**
 * send/recv on peer ring. Short OK. -EPIPE / 0 on half-close / EOF soft.
 * cb clamped to ring capacity.
 */
i64 net_lo_send(i64 i64Fd, const void *pBuf, size_t cb);
i64 net_lo_recv(i64 i64Fd, void *pBuf, size_t cb);

/** close: free slot; peer sees half-close / EOF soft. */
i64 net_lo_close(i64 i64Fd);

/** Non-zero if fd is a live net_lo table entry (not vfs_ram / net_tcp). */
int net_lo_fd_ok(i64 i64Fd);

/** shutdown: how 0=RD 1=WR 2=RDWR — mark half-closed; does not free slot. */
i64 net_lo_shutdown(i64 i64Fd, int nHow);

/**
 * getsockopt/setsockopt: SOL_SOCKET subset
 * (TYPE, ERROR, REUSEADDR/PORT, BROADCAST, KEEPALIVE, SND/RCVBUF,
 *  LINGER soft, ACCEPTCONN). Other levels → -ENOPROTOOPT soft.
 */
i64 net_lo_getsockopt(i64 i64Fd, int nLevel, int nOpt, void *pVal, u32 *pLen);
i64 net_lo_setsockopt(i64 i64Fd, int nLevel, int nOpt, const void *pVal, u32 u32Len);

/**
 * getsockname/getpeername: fill sockaddr_in-shaped 16-byte buffer
 * (family AF_INET, port, 127.0.0.1 soft). Updates *pLen.
 */
i64 net_lo_getsockname(i64 i64Fd, void *pAddr, u32 *pLen);
i64 net_lo_getpeername(i64 i64Fd, void *pAddr, u32 *pLen);
