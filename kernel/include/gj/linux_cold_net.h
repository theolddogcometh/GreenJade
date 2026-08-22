/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux socket-family cold handlers (Option C ABI-first pivot).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 - no GPL source.
 *
 * Role (v12 exclusive residual deepen - Soft!=product / G-AC-1)
 * ------------------------------------------------------------
 * Lean cold Linux socket ABI residual bridging to net_tcp / net_lo for
 * userspace Linux-shaped networking (glibc socket/bind/listen/accept/
 * connect/send/recv + name/sockopt/shutdown/msg single-iov).
 * Target residual: Linux-shaped sshd cold path on STREAM/:22.
 *   AF_INET + SOCK_STREAM -> net_tcp_* (interim multi-seg TCP table)
 *   DGRAM / AF_UNIX / other -> net_lo_* (loopback + SOL_SOCKET soft)
 *
 * Product direction (Dual DoD B):
 *   UDX + ABI (userspace Linux-shaped drivers / personality over cold ABI).
 *   Product NIC = UDX. Soft residual lean only - Soft!=product.
 *   Do NOT reintroduce freestanding rtl DoD B rabbit hole as product close
 *   (freestanding rtl SKIP). G-AC-1: no Linux .ko product AC / no in-kernel
 *   .ko exec.
 *
 * Soft residual honesty (not product close):
 *   accept readiness via net_tcp_poll_mask / net_lo_poll_mask before take;
 *   soft pump net_tcp_poll on EAGAIN + one re-accept (handshake progress);
 *   gj_linux_cold_poll_mask bridges STREAM readiness for cold poll honesty;
 *   soft SO_ERROR / half-close / SO_ACCEPTCONN / name-l2 / :22 once-lamps;
 *   v11: bridge net_tcp_shutdown / getsockname / getpeername + send* preshut;
 *   v12 deepen for Linux-shaped sshd residual:
 *     post-accept product name sync (getsockname+getpeername seed);
 *     soft path tallies socket/bind/listen/accept/send/recv/shutdown/name
 *     on :22; listen SHUT_RD clears SO_ACCEPTCONN; bind clears listen bit;
 *     Soft listen :22 != host banner proof (soft_listen_ne_host_banner=1);
 *   never claim product OpenSSH / host nc banner - Soft!=product only.
 *
 * These are free functions for the cold personality / coordinator to
 * wire later (Makefile / protonrt_cold_link / net_door not edited here).
 * Dispatch contract: return i64 >=0 success or -LINUX_E* (linux_abi.h).
 *
 * User pointers: copy_{from,to}_user when user_range_ok; kernel smoke
 * buffers accepted via direct memcpy (same pattern as net_door / hot).
 *
 * greppable: linux_cold_net gj_linux_cold_socket net_tcp_socket
 * greppable: linux_cold_net: soft
 * greppable: linux_cold_net: soft ... Soft!=product
 * greppable: linux_cold_net: soft residual lean Soft!=product
 * greppable: linux_cold_net: soft listen :22 soft_listen_ne_host_banner
 * greppable: linux_cold_net v2 getpeername sockopt send recv msg
 * greppable: linux_cold_net v3 accept readiness soft poll :22
 * greppable: linux_cold_net v5 close connect peer Soft!=product
 * greppable: linux_cold_net v6 so_error accept peer once Soft!=product
 * greppable: linux_cold_net v7 so_error poll send recv once Soft!=product
 * greppable: linux_cold_net v8 half-close name-l2 flags Soft!=product
 * greppable: linux_cold_net v9 lean residual not freestanding wire Soft!=product
 * greppable: linux_cold_net v10 lean residual UDX+ABI Soft!=product
 * greppable: linux_cold_net v11 name+shutdown bridge send* Soft!=product
 * greppable: linux_cold_net v12 sshd path residual soft_listen_ne_host_banner
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

/**
 * socket(domain, type, protocol) - arg0/1/2.
 * AF_INET(2) SOCK_STREAM(1) -> net_tcp_socket; else net_lo_socket.
 */
i64 gj_linux_cold_socket(struct gj_linux_regs *pRegs);

/**
 * bind(fd, sockaddr *, addrlen) - parse sockaddr_in port (network order)
 * -> host order for net_tcp_bind / net_lo_bind.
 * Soft: remembers TCP bind port for getsockname / :22 honesty.
 * Once-lamp on :22 / TCP success (NO stamp storms). Soft!=product.
 */
i64 gj_linux_cold_bind(struct gj_linux_regs *pRegs);

/**
 * listen(fd, backlog) - soft greppable when bound port is :22
 * (soft residual lean; Soft!=product; product DoD B = UDX+ABI).
 * Once-lamp + one post-listen pump. Soft: clears SO_ERROR on success;
 * sets SO_ACCEPTCONN listen bit; once-lamp generic listen.
 *
 * HARD honesty (v12): Soft listen :22 != host banner proof.
 * greppable: soft_listen_ne_host_banner=1 Soft!=product
 * Not product OpenSSH; freestanding rtl SKIP; product NIC = UDX.
 */
i64 gj_linux_cold_listen(struct gj_linux_regs *pRegs);

/**
 * accept(fd, addr, addrlen) / accept4-shaped: peer addr soft-optional
 * (NULL addr OK). Flags (arg3 on accept4) ignored for bring-up.
 *
 * Soft residual lean (Soft!=product; product DoD B = UDX+ABI):
 *   - readiness via net_*_poll_mask (POLLIN) before take;
 *   - -EAGAIN is soft again (not hard fail inventory);
 *   - TCP: one net_tcp_poll + re-accept so handshake can progress;
 *   - peer soft seed + v12 product name sync (getsockname/getpeername);
 *   - once-lamp success (NO stamp storms in accept-yield loops).
 * Linux-shaped sshd residual path; Soft listen != host banner.
 * Not freestanding rtl product close.
 */
i64 gj_linux_cold_accept(struct gj_linux_regs *pRegs);

/**
 * connect(fd, sockaddr *, addrlen) - port from sockaddr_in network order.
 * Soft pump + peer-port store; SO_ERROR on hard fail. Soft!=product.
 */
i64 gj_linux_cold_connect(struct gj_linux_regs *pRegs);

/**
 * sendto(fd, buf, len, flags, dest, destlen) - bounce <=4 KiB; dest ignored
 * for connected STREAM bring-up (product path = net_tcp_send multi-seg).
 * Soft: SHUT_WR pre-check before copy (EPIPE footgun); one poll+retry on
 * EAGAIN; SO_ERROR on hard fail / clear on success; MSG_NOSIGNAL soft no-op;
 * once-lamp first TCP send (NO stamp storms). Soft!=product.
 */
i64 gj_linux_cold_sendto(struct gj_linux_regs *pRegs);

/**
 * recvfrom(fd, buf, len, flags, src, srclen) - bounce <=4 KiB; src soft
 * optional (left untouched when NULL). Soft: SHUT_RD -> 0 EOF; -EAGAIN not
 * hard fail; SO_ERROR on hard fail / clear on success; once-lamp first TCP
 * recv. Soft!=product.
 */
i64 gj_linux_cold_recvfrom(struct gj_linux_regs *pRegs);

/**
 * send(fd, buf, len, flags) - thin alias of sendto with null dest.
 * arg0..arg3 only; dest/destlen forced null.
 */
i64 gj_linux_cold_send(struct gj_linux_regs *pRegs);

/**
 * recv(fd, buf, len, flags) - thin alias of recvfrom with null src.
 * arg0..arg3 only; src/srclen forced null.
 */
i64 gj_linux_cold_recv(struct gj_linux_regs *pRegs);

/**
 * sendmsg(fd, msghdr *, flags) - soft single iov only (first vector);
 * no SCM/control. Bounce <=4 KiB -> net_tcp_send / net_lo_send.
 * SHUT_WR pre-check before iov copy (send* EPIPE footgun). Soft!=product.
 * x86_64 msghdr: msg_iov@+16, msg_iovlen@+24; iovec base@0 len@+8.
 */
i64 gj_linux_cold_sendmsg(struct gj_linux_regs *pRegs);

/**
 * recvmsg(fd, msghdr *, flags) - soft single iov only; no SCM.
 * First iovec filled; msg_name/control left untouched.
 */
i64 gj_linux_cold_recvmsg(struct gj_linux_regs *pRegs);

/**
 * shutdown(fd, how) - lo -> net_lo_shutdown; TCP -> net_tcp_shutdown
 * (FIN on SHUT_WR) + soft half-close mirror for send EPIPE / recv EOF /
 * POLLHUP. SHUT_RD/RDWR clears soft SO_ACCEPTCONN listen bit (v12).
 * Soft residual lean; Soft!=product; not freestanding rtl close.
 */
i64 gj_linux_cold_shutdown(struct gj_linux_regs *pRegs);

/**
 * getsockname(fd, addr, *addrlen) - lo -> net_lo_getsockname; TCP prefers
 * net_tcp_getsockname (L2 IP + table port), soft-bound port + name-l2
 * fallback. :22 path once-lamp (sshd-shaped name residual). Soft!=product.
 */
i64 gj_linux_cold_getsockname(struct gj_linux_regs *pRegs);

/**
 * getpeername(fd, addr, *addrlen) - lo -> net_lo_getpeername; TCP prefers
 * net_tcp_getpeername (real peer), soft peer-port fallback; -ENOTCONN.
 * Post-accept product seed (v12) improves peer honesty for sshd-shaped path.
 */
i64 gj_linux_cold_getpeername(struct gj_linux_regs *pRegs);

/**
 * setsockopt(fd, level, optname, optval, optlen) - SOL_SOCKET soft:
 * SO_REUSEADDR (2) stored; other SOL_SOCKET / levels soft-accept 0.
 * lo -> net_lo_setsockopt; TCP soft table (REUSEADDR only meaningful).
 */
i64 gj_linux_cold_setsockopt(struct gj_linux_regs *pRegs);

/**
 * getsockopt(fd, level, optname, optval, *optlen) - SOL_SOCKET soft:
 * SO_TYPE (3), SO_ERROR (4, clear-on-read), SO_REUSEADDR (2).
 * Non-SOL_SOCKET -> -ENOPROTOOPT soft on get. lo -> net_lo_getsockopt.
 */
i64 gj_linux_cold_getsockopt(struct gj_linux_regs *pRegs);

/**
 * Soft poll readiness for a cold STREAM/DGRAM fd (soft residual lean).
 * Routes net_tcp_poll_mask / net_lo_poll_mask; 0 if neither table owns fd.
 * Bits: POLLIN=0x1 POLLOUT=0x4 POLLERR=0x8 POLLHUP=0x10 (Linux-shaped).
 * TCP path may soft-pump net_tcp_poll once for handshake progress.
 * SO_ERROR honesty: sticky soft errno peeks into POLLERR (not clear-on-read).
 * Once-lamp kprintf only (NO stamp storms in poll loops). Soft!=product.
 * Product DoD B direction = UDX+ABI; not freestanding rtl product close.
 *
 * greppable: linux_cold_net: soft poll_mask ... Soft!=product
 * greppable: linux_cold_net: soft poll_mask POLLERR
 */
u32 gj_linux_cold_poll_mask(i64 i64Fd, u32 u32Want);

/**
 * close(fd) - soft ownership route for personality/coordinator wire-later:
 *   alias table first (user fd may be 0/1/2); last ref only
 *   net_tcp_close / net_lo_close the canonical tcp/lo fd
 *   else -EBADF
 * Soft!=product - not product fd-table / multi-server close.
 * G-AC-1: UDX+ABI path (userspace Linux-shaped); no .ko product AC.
 *
 * greppable: linux_cold_net: soft close ... Soft!=product
 */
i64 gj_linux_cold_close(struct gj_linux_regs *pRegs);

/**
 * Non-zero if fd is a live cold STREAM/DGRAM socket or an alias of one.
 * Canonical fds stay in net_tcp 96..111 / net_lo 64..79; aliases may be 0/1/2.
 */
int gj_linux_cold_fd_ok(i64 i64Fd);

/**
 * dup / dup2 / dup3 of a cold net fd (canonical or alias).
 * LINUX_NR_dup allocates a free user fd; dup2/dup3 use arg1 (0/1/2 allowed).
 * Last close of a name calls net_tcp_close / net_lo_close on the canonical
 * fd only. Returns -EBADF when oldfd is not a cold net fd (vfs_ram fallback).
 */
i64 gj_linux_cold_dup2(struct gj_linux_regs *pRegs);

/**
 * linux_fork inherit: one extra LCN holder per live user name so the
 * parent close(newsock) is not last-ref on the accepted ESTAB. Global
 * alias table is not a per-process fd table; leftover refs keep the
 * canonical tcp/lo slot until the child dup2/close path drops them.
 * Soft!=product. Dual DoD B stays OPEN until host interactive SSH login.
 *
 * greppable: linux_cold_net: soft fork dup names
 */
void gj_linux_cold_fork_dup_names(void);
