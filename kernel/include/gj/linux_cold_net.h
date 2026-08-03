/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux socket-family cold handlers (Option C ABI-first pivot).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 — no GPL source.
 *
 * Role
 * ----
 * glibc-shaped socket() / bind / listen / accept / connect / sendto /
 * recvfrom (+ soft shutdown / getsockname / getpeername / sockopt /
 * send / recv / single-iov sendmsg/recvmsg) map onto product interim
 * tables:
 *   AF_INET + SOCK_STREAM → net_tcp_* (product multi-seg TCP path)
 *   DGRAM / AF_UNIX / other → net_lo_* (loopback + SOL_SOCKET soft)
 *
 * These are free functions for the cold personality / coordinator to
 * wire later (Makefile / protonrt_cold_link / net_door not edited here).
 * Dispatch contract: return i64 ≥0 success or -LINUX_E* (linux_abi.h).
 *
 * User pointers: copy_{from,to}_user when user_range_ok; kernel smoke
 * buffers accepted via direct memcpy (same pattern as net_door / hot).
 *
 * greppable: linux_cold_net gj_linux_cold_socket net_tcp_socket
 * greppable: linux_cold_net: soft
 * greppable: linux_cold_net v2 getpeername sockopt send recv msg
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

/**
 * socket(domain, type, protocol) — arg0/1/2.
 * AF_INET(2) SOCK_STREAM(1) → net_tcp_socket; else net_lo_socket.
 */
i64 gj_linux_cold_socket(struct gj_linux_regs *pRegs);

/**
 * bind(fd, sockaddr *, addrlen) — parse sockaddr_in port (network order)
 * → host order for net_tcp_bind / net_lo_bind.
 */
i64 gj_linux_cold_bind(struct gj_linux_regs *pRegs);

/** listen(fd, backlog) */
i64 gj_linux_cold_listen(struct gj_linux_regs *pRegs);

/**
 * accept(fd, addr, addrlen) / accept4-shaped: peer addr soft-optional
 * (NULL addr OK). Flags (arg3 on accept4) ignored for bring-up.
 */
i64 gj_linux_cold_accept(struct gj_linux_regs *pRegs);

/**
 * connect(fd, sockaddr *, addrlen) — port from sockaddr_in network order.
 */
i64 gj_linux_cold_connect(struct gj_linux_regs *pRegs);

/**
 * sendto(fd, buf, len, flags, dest, destlen) — bounce ≤4 KiB; dest ignored
 * for connected STREAM bring-up (product path = net_tcp_send multi-seg).
 */
i64 gj_linux_cold_sendto(struct gj_linux_regs *pRegs);

/**
 * recvfrom(fd, buf, len, flags, src, srclen) — bounce ≤4 KiB; src soft
 * optional (left untouched when NULL).
 */
i64 gj_linux_cold_recvfrom(struct gj_linux_regs *pRegs);

/**
 * send(fd, buf, len, flags) — thin alias of sendto with null dest.
 * arg0..arg3 only; dest/destlen forced null.
 */
i64 gj_linux_cold_send(struct gj_linux_regs *pRegs);

/**
 * recv(fd, buf, len, flags) — thin alias of recvfrom with null src.
 * arg0..arg3 only; src/srclen forced null.
 */
i64 gj_linux_cold_recv(struct gj_linux_regs *pRegs);

/**
 * sendmsg(fd, msghdr *, flags) — soft single iov only (first vector);
 * no SCM/control. Bounce ≤4 KiB → net_tcp_send / net_lo_send.
 * x86_64 msghdr: msg_iov@+16, msg_iovlen@+24; iovec base@0 len@+8.
 */
i64 gj_linux_cold_sendmsg(struct gj_linux_regs *pRegs);

/**
 * recvmsg(fd, msghdr *, flags) — soft single iov only; no SCM.
 * First iovec filled; msg_name/control left untouched.
 */
i64 gj_linux_cold_recvmsg(struct gj_linux_regs *pRegs);

/**
 * shutdown(fd, how) — soft: net_lo_shutdown when lo fd; TCP soft 0
 * (no half-close API yet on net_tcp).
 */
i64 gj_linux_cold_shutdown(struct gj_linux_regs *pRegs);

/**
 * getsockname(fd, addr, *addrlen) — soft: net_lo_getsockname when lo;
 * TCP fabricates AF_INET 127.0.0.1 port 0 sockaddr_in when no name API.
 */
i64 gj_linux_cold_getsockname(struct gj_linux_regs *pRegs);

/**
 * getpeername(fd, addr, *addrlen) — soft: net_lo_getpeername when lo;
 * TCP fabricates AF_INET 127.0.0.1 port 0 (no peer-name API on net_tcp).
 */
i64 gj_linux_cold_getpeername(struct gj_linux_regs *pRegs);

/**
 * setsockopt(fd, level, optname, optval, optlen) — SOL_SOCKET soft:
 * SO_REUSEADDR (2) stored; other SOL_SOCKET / levels soft-accept 0.
 * lo → net_lo_setsockopt; TCP soft table (REUSEADDR only meaningful).
 */
i64 gj_linux_cold_setsockopt(struct gj_linux_regs *pRegs);

/**
 * getsockopt(fd, level, optname, optval, *optlen) — SOL_SOCKET soft:
 * SO_TYPE (3), SO_ERROR (4), SO_REUSEADDR (2). Non-SOL_SOCKET →
 * -ENOPROTOOPT soft on get. lo → net_lo_getsockopt.
 */
i64 gj_linux_cold_getsockopt(struct gj_linux_regs *pRegs);
