/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux socket-family cold handlers (Option C ABI-first).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 — no GPL source.
 *
 * Product routing (matches net_door SOCKET..ACCEPT policy):
 *   AF_INET + SOCK_STREAM  → net_tcp_*  (virtio + loopback multi-seg)
 *   DGRAM / AF_UNIX / other → net_lo_*
 *
 * Per-op ownership: net_tcp_fd_ok / net_lo_fd_ok select the table.
 * User memory: user_range_ok + copy_{from,to}_user; kernel smoke buffers
 * via direct memcpy (same edge as net_door / linux_hot sock path).
 *
 * v2 deepenings (soft≠product; never hard-gate beyond -LINUX_E*):
 *   getpeername, setsockopt/getsockopt (SOL_SOCKET: REUSEADDR/ERROR/TYPE),
 *   send/recv thin aliases of sendto/recvfrom (null dest/src),
 *   sendmsg/recvmsg single-iov only (no SCM/control).
 *
 * Soft inventory never hard-gates. Greppable markers:
 *   linux_cold_net: soft …
 *   linux_cold_net v2 getpeername sockopt send recv msg
 *
 * Coordinator wires these into cold dispatch later — do not require
 * Makefile / protonrt_cold_link / net_door edits from this unit.
 */
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_cold_net.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/user_access.h>

/* Public Linux socket constants (man pages / ABI docs only). */
#define LCN_AF_INET     2
#define LCN_AF_UNIX     1
#define LCN_SOCK_STREAM 1
#define LCN_SOCK_DGRAM  2

/* SOL_SOCKET + subset (x86_64 Linux ABI numbers; man pages only). */
#define LCN_SOL_SOCKET  1
#define LCN_SO_REUSEADDR 2
#define LCN_SO_TYPE      3
#define LCN_SO_ERROR     4

/* Align bounce with net_door NET_XFER_MAX / net_tcp multi-seg bulk. */
#define LCN_XFER_MAX 4096u

/* Soft TCP SO_REUSEADDR bits for FD 96..111 (matches net_tcp layout). */
#define LCN_TCP_FD_BASE 96u
#define LCN_TCP_MAX     16u

/* Soft log throttle: emit inventory every N successful ops (wrap OK). */
#define LCN_SOFT_LOG_EVERY 64u

/*
 * Soft product counters (diagnostics only; never hard-gate i64Ret).
 * greppable: linux_cold_net: soft
 */
static struct {
	u64 u64Socket;
	u64 u64SocketTcp;
	u64 u64SocketLo;
	u64 u64Bind;
	u64 u64Listen;
	u64 u64Accept;
	u64 u64Connect;
	u64 u64Sendto;
	u64 u64Recvfrom;
	u64 u64Send;
	u64 u64Recv;
	u64 u64Sendmsg;
	u64 u64Recvmsg;
	u64 u64Shutdown;
	u64 u64Getsockname;
	u64 u64Getpeername;
	u64 u64Setsockopt;
	u64 u64Getsockopt;
	u64 u64Fail;
	u64 u64UserCopy;
	u64 u64KernelCopy;
	u64 u64Ops;
	u64 u64LogN;
} g_lcnSoft;

/* Soft TCP SO_REUSEADDR store (slot = fd - 96). Soft≠product. */
static u8 g_lcnTcpReuse[LCN_TCP_MAX];

static void
lcn_soft_bump(u64 *p)
{
	if (p != NULL) {
		(*p)++;
	}
}

static void
lcn_soft_maybe_log(int fForce)
{
	u64 u64Ops;

	u64Ops = g_lcnSoft.u64Ops;
	if (fForce == 0) {
		if (u64Ops == 0u || (u64Ops % (u64)LCN_SOFT_LOG_EVERY) != 0u) {
			return;
		}
	}
	g_lcnSoft.u64LogN++;
	/* greppable: linux_cold_net: soft inventory */
	kprintf("linux_cold_net: soft inventory ops=%llu sock=%llu tcp=%llu "
		"lo=%llu bind=%llu listen=%llu accept=%llu connect=%llu "
		"sendto=%llu recvfrom=%llu send=%llu recv=%llu "
		"smsg=%llu rmsg=%llu shut=%llu name=%llu peer=%llu "
		"setopt=%llu getopt=%llu fail=%llu "
		"ucopy=%llu kcopy=%llu logn=%llu\n",
		(unsigned long long)g_lcnSoft.u64Ops,
		(unsigned long long)g_lcnSoft.u64Socket,
		(unsigned long long)g_lcnSoft.u64SocketTcp,
		(unsigned long long)g_lcnSoft.u64SocketLo,
		(unsigned long long)g_lcnSoft.u64Bind,
		(unsigned long long)g_lcnSoft.u64Listen,
		(unsigned long long)g_lcnSoft.u64Accept,
		(unsigned long long)g_lcnSoft.u64Connect,
		(unsigned long long)g_lcnSoft.u64Sendto,
		(unsigned long long)g_lcnSoft.u64Recvfrom,
		(unsigned long long)g_lcnSoft.u64Send,
		(unsigned long long)g_lcnSoft.u64Recv,
		(unsigned long long)g_lcnSoft.u64Sendmsg,
		(unsigned long long)g_lcnSoft.u64Recvmsg,
		(unsigned long long)g_lcnSoft.u64Shutdown,
		(unsigned long long)g_lcnSoft.u64Getsockname,
		(unsigned long long)g_lcnSoft.u64Getpeername,
		(unsigned long long)g_lcnSoft.u64Setsockopt,
		(unsigned long long)g_lcnSoft.u64Getsockopt,
		(unsigned long long)g_lcnSoft.u64Fail,
		(unsigned long long)g_lcnSoft.u64UserCopy,
		(unsigned long long)g_lcnSoft.u64KernelCopy,
		(unsigned long long)g_lcnSoft.u64LogN);
}

static void
lcn_soft_op(void)
{
	lcn_soft_bump(&g_lcnSoft.u64Ops);
	lcn_soft_maybe_log(0);
}

/**
 * Copy cb bytes from user/kernel src into kernel bounce.
 * Returns 0 or -LINUX_EFAULT / -LINUX_EINVAL.
 */
static i64
lcn_copy_in(void *pKdst, u64 u64Src, size_t cb)
{
	if (cb == 0u) {
		return 0;
	}
	if (pKdst == NULL || u64Src == 0u) {
		return -(i64)LINUX_EFAULT;
	}
	if (user_range_ok(u64Src, cb)) {
		lcn_soft_bump(&g_lcnSoft.u64UserCopy);
		if (copy_from_user(pKdst, u64Src, cb) != GJ_OK) {
			return -(i64)LINUX_EFAULT;
		}
	} else {
		lcn_soft_bump(&g_lcnSoft.u64KernelCopy);
		memcpy(pKdst, (const void *)(gj_vaddr_t)u64Src, cb);
	}
	return 0;
}

/**
 * Copy cb bytes from kernel bounce to user/kernel dst.
 */
static i64
lcn_copy_out(u64 u64Dst, const void *pKsrc, size_t cb)
{
	if (cb == 0u) {
		return 0;
	}
	if (pKsrc == NULL || u64Dst == 0u) {
		return -(i64)LINUX_EFAULT;
	}
	if (user_range_ok(u64Dst, cb)) {
		lcn_soft_bump(&g_lcnSoft.u64UserCopy);
		if (copy_to_user(u64Dst, pKsrc, cb) != GJ_OK) {
			return -(i64)LINUX_EFAULT;
		}
	} else {
		lcn_soft_bump(&g_lcnSoft.u64KernelCopy);
		memcpy((void *)(gj_vaddr_t)u64Dst, pKsrc, cb);
	}
	return 0;
}

/**
 * Read sockaddr_in port at offset 2 (network / big-endian) → host order.
 * Returns 0 and *pPortHost, or -LINUX_EFAULT / -LINUX_EINVAL.
 */
static i64
lcn_parse_sin_port(u64 u64Sa, u32 u32Len, u16 *pPortHost)
{
	u16 u16PortBe = 0;

	if (pPortHost == NULL) {
		return -(i64)LINUX_EINVAL;
	}
	if (u64Sa == 0u) {
		return -(i64)LINUX_EFAULT;
	}
	/* Need at least family(2) + port(2). Soft accept short smokes ≥4. */
	if (u32Len != 0u && u32Len < 4u) {
		return -(i64)LINUX_EINVAL;
	}
	{
		i64 i64St = lcn_copy_in(&u16PortBe, u64Sa + 2u, 2u);

		if (i64St != 0) {
			return i64St;
		}
	}
	/* Network order → host (x86_64 LE): byte swap 16. */
	*pPortHost = (u16)((u16PortBe >> 8) | (u16PortBe << 8));
	return 0;
}

/** Soft sockaddr_in: AF_INET, port host→BE, 127.0.0.1. */
static void
lcn_fill_sin(u8 *pSa, u16 u16PortHost)
{
	u32 i;

	if (pSa == NULL) {
		return;
	}
	for (i = 0; i < 16u; i++) {
		pSa[i] = 0;
	}
	pSa[0] = (u8)LCN_AF_INET;
	pSa[2] = (u8)(u16PortHost >> 8);
	pSa[3] = (u8)(u16PortHost & 0xffu);
	pSa[4] = 127;
	pSa[5] = 0;
	pSa[6] = 0;
	pSa[7] = 1;
}

/** Soft TCP SO_REUSEADDR slot for fd, or NULL if out of soft range. */
static u8 *
lcn_tcp_reuse_slot(i64 i64Fd)
{
	u32 u32Slot;

	if (i64Fd < (i64)LCN_TCP_FD_BASE) {
		return NULL;
	}
	u32Slot = (u32)(i64Fd - (i64)LCN_TCP_FD_BASE);
	if (u32Slot >= LCN_TCP_MAX) {
		return NULL;
	}
	return &g_lcnTcpReuse[u32Slot];
}

/**
 * Read first iovec from x86_64 msghdr (msg_iov@+16, msg_iovlen@+24).
 * Soft: only first vector; iovlen==0 → base/len zeroed, return 0.
 */
static i64
lcn_msg_first_iov(u64 u64Msg, u64 *pBase, u64 *pLen)
{
	u64 u64Iov = 0;
	u64 u64IovLen = 0;

	if (pBase == NULL || pLen == NULL) {
		return -(i64)LINUX_EINVAL;
	}
	*pBase = 0;
	*pLen = 0;
	if (u64Msg == 0u) {
		return -(i64)LINUX_EFAULT;
	}
	{
		i64 i64St = lcn_copy_in(&u64Iov, u64Msg + 16u, 8u);

		if (i64St != 0) {
			return i64St;
		}
		i64St = lcn_copy_in(&u64IovLen, u64Msg + 24u, 8u);
		if (i64St != 0) {
			return i64St;
		}
	}
	if (u64Iov == 0u || u64IovLen == 0u) {
		return 0;
	}
	/* Soft single-iov: ignore remaining vectors. */
	{
		i64 i64St = lcn_copy_in(pBase, u64Iov, 8u);

		if (i64St != 0) {
			return i64St;
		}
		i64St = lcn_copy_in(pLen, u64Iov + 8u, 8u);
		if (i64St != 0) {
			return i64St;
		}
	}
	return 0;
}

/* ---- exported cold handlers ------------------------------------------- */

i64
gj_linux_cold_socket(struct gj_linux_regs *pRegs)
{
	int nDomain;
	int nType;
	int nProto;
	i64 i64Fd;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Socket);
	lcn_soft_op();

	nDomain = (int)pRegs->u64Arg0;
	nType = (int)pRegs->u64Arg1;
	nProto = (int)pRegs->u64Arg2;

	/*
	 * Product STREAM path: AF_INET SOCK_STREAM → net_tcp (matches
	 * GJ_NET_OP_SOCKET). Type may include SOCK_CLOEXEC/NONBLOCK bits
	 * in low glibc; mask to base type for bring-up (0xFF).
	 */
	if (nDomain == LCN_AF_INET &&
	    (nType & 0xff) == LCN_SOCK_STREAM) {
		i64Fd = net_tcp_socket();
		lcn_soft_bump(&g_lcnSoft.u64SocketTcp);
		/* greppable: linux_cold_net: soft socket tcp */
		if (i64Fd < 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			kprintf("linux_cold_net: soft socket tcp fail=%ld\n",
				(long)i64Fd);
		} else {
			/* Soft clear REUSEADDR for new TCP fd slot. */
			u8 *pR = lcn_tcp_reuse_slot(i64Fd);

			if (pR != NULL) {
				*pR = 0;
			}
		}
		return i64Fd;
	}

	i64Fd = net_lo_socket(nDomain, nType & 0xff, nProto);
	lcn_soft_bump(&g_lcnSoft.u64SocketLo);
	/* greppable: linux_cold_net: soft socket lo */
	if (i64Fd < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
	}
	return i64Fd;
}

i64
gj_linux_cold_bind(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u16 u16Port = 0;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Bind);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	i64St = lcn_parse_sin_port(pRegs->u64Arg1, (u32)pRegs->u64Arg2,
				   &u16Port);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64St = net_tcp_bind(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft bind tcp */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_bind(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft bind lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	lcn_soft_bump(&g_lcnSoft.u64Fail);
	return -(i64)LINUX_EBADF;
}

i64
gj_linux_cold_listen(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	int nBacklog;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Listen);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	nBacklog = (int)pRegs->u64Arg1;

	if (net_tcp_fd_ok(i64Fd)) {
		i64St = net_tcp_listen(i64Fd, nBacklog);
		/* greppable: linux_cold_net: soft listen tcp */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_listen(i64Fd, nBacklog);
		/* greppable: linux_cold_net: soft listen lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	lcn_soft_bump(&g_lcnSoft.u64Fail);
	return -(i64)LINUX_EBADF;
}

i64
gj_linux_cold_accept(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	i64 i64Peer;
	u8 aSa[16];
	u32 u32Len = 16;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Accept);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	/* arg1=addr arg2=addrlen* soft-optional; arg3 flags ignored. */

	if (net_tcp_fd_ok(i64Fd)) {
		i64Peer = net_tcp_accept(i64Fd);
		/* greppable: linux_cold_net: soft accept tcp */
	} else if (net_lo_fd_ok(i64Fd)) {
		i64Peer = net_lo_accept(i64Fd);
		/* greppable: linux_cold_net: soft accept lo */
	} else {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	if (i64Peer < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64Peer;
	}

	/* Soft: fill peer sockaddr when user supplied addr. */
	if (pRegs->u64Arg1 != 0u) {
		if (pRegs->u64Arg2 != 0u) {
			if (user_range_ok(pRegs->u64Arg2, 4u)) {
				(void)copy_from_user(&u32Len, pRegs->u64Arg2,
						     4u);
			} else {
				u32Len =
				    *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
			}
		}
		if (u32Len > 16u) {
			u32Len = 16u;
		}
		if (u32Len >= 16u) {
			lcn_fill_sin(aSa, 0);
			if (lcn_copy_out(pRegs->u64Arg1, aSa, 16u) != 0) {
				/* Peer already accepted — soft ignore copy. */
			}
			u32Len = 16u;
			if (pRegs->u64Arg2 != 0u) {
				if (user_range_ok(pRegs->u64Arg2, 4u)) {
					(void)copy_to_user(pRegs->u64Arg2,
							   &u32Len, 4u);
				} else {
					*(u32 *)(gj_vaddr_t)pRegs->u64Arg2 =
					    u32Len;
				}
			}
		}
	}
	return i64Peer;
}

i64
gj_linux_cold_connect(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u16 u16Port = 0;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Connect);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	i64St = lcn_parse_sin_port(pRegs->u64Arg1, (u32)pRegs->u64Arg2,
				   &u16Port);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64St = net_tcp_connect(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft connect tcp */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_connect(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft connect lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	lcn_soft_bump(&g_lcnSoft.u64Fail);
	return -(i64)LINUX_EBADF;
}

i64
gj_linux_cold_sendto(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u8 aBuf[LCN_XFER_MAX];
	size_t cb;
	i64 i64N;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Sendto);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	cb = (size_t)pRegs->u64Arg2;
	/* flags / dest / destlen ignored for connected STREAM bring-up. */

	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}
	if (cb == 0u) {
		return 0;
	}
	if (pRegs->u64Arg1 == 0u) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EFAULT;
	}
	if (cb > (size_t)LCN_XFER_MAX) {
		cb = (size_t)LCN_XFER_MAX;
	}

	i64St = lcn_copy_in(aBuf, pRegs->u64Arg1, cb);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendto tcp */
	} else {
		i64N = net_lo_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendto lo */
	}
	if (i64N < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
	}
	return i64N;
}

i64
gj_linux_cold_recvfrom(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u8 aBuf[LCN_XFER_MAX];
	size_t cb;
	i64 i64N;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Recvfrom);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	cb = (size_t)pRegs->u64Arg2;
	/* flags / src / srclen soft-optional (src left untouched). */

	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}
	if (cb == 0u) {
		return 0;
	}
	if (pRegs->u64Arg1 == 0u) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EFAULT;
	}
	if (cb > (size_t)LCN_XFER_MAX) {
		cb = (size_t)LCN_XFER_MAX;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvfrom tcp */
	} else {
		i64N = net_lo_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvfrom lo */
	}
	if (i64N < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64N;
	}
	if (i64N == 0) {
		return 0; /* EOF soft */
	}

	i64St = lcn_copy_out(pRegs->u64Arg1, aBuf, (size_t)i64N);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	return i64N;
}

i64
gj_linux_cold_send(struct gj_linux_regs *pRegs)
{
	struct gj_linux_regs r;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Send);
	/* greppable: linux_cold_net: soft send alias */
	r = *pRegs;
	r.u64Arg4 = 0; /* dest = NULL */
	r.u64Arg5 = 0; /* destlen = 0 */
	return gj_linux_cold_sendto(&r);
}

i64
gj_linux_cold_recv(struct gj_linux_regs *pRegs)
{
	struct gj_linux_regs r;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Recv);
	/* greppable: linux_cold_net: soft recv alias */
	r = *pRegs;
	r.u64Arg4 = 0; /* src = NULL */
	r.u64Arg5 = 0; /* srclen = 0 */
	return gj_linux_cold_recvfrom(&r);
}

i64
gj_linux_cold_sendmsg(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u64 u64Base = 0;
	u64 u64Len = 0;
	u8 aBuf[LCN_XFER_MAX];
	size_t cb;
	i64 i64St;
	i64 i64N;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Sendmsg);
	lcn_soft_op();
	/* greppable: linux_cold_net: soft sendmsg single-iov */

	i64Fd = (i64)pRegs->u64Arg0;
	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	i64St = lcn_msg_first_iov(pRegs->u64Arg1, &u64Base, &u64Len);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	if (u64Base == 0u || u64Len == 0u) {
		return 0;
	}

	cb = (size_t)u64Len;
	if (cb > (size_t)LCN_XFER_MAX) {
		cb = (size_t)LCN_XFER_MAX;
	}

	i64St = lcn_copy_in(aBuf, u64Base, cb);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendmsg tcp */
	} else {
		i64N = net_lo_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendmsg lo */
	}
	if (i64N < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
	}
	return i64N;
}

i64
gj_linux_cold_recvmsg(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u64 u64Base = 0;
	u64 u64Len = 0;
	u8 aBuf[LCN_XFER_MAX];
	size_t cb;
	i64 i64St;
	i64 i64N;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Recvmsg);
	lcn_soft_op();
	/* greppable: linux_cold_net: soft recvmsg single-iov */

	i64Fd = (i64)pRegs->u64Arg0;
	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	i64St = lcn_msg_first_iov(pRegs->u64Arg1, &u64Base, &u64Len);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	if (u64Base == 0u || u64Len == 0u) {
		return 0;
	}

	cb = (size_t)u64Len;
	if (cb > (size_t)LCN_XFER_MAX) {
		cb = (size_t)LCN_XFER_MAX;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvmsg tcp */
	} else {
		i64N = net_lo_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvmsg lo */
	}
	if (i64N < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64N;
	}
	if (i64N == 0) {
		return 0; /* EOF soft */
	}

	i64St = lcn_copy_out(u64Base, aBuf, (size_t)i64N);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	/* Soft: msg_name / msg_control / msg_flags left untouched (no SCM). */
	return i64N;
}

i64
gj_linux_cold_shutdown(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	int nHow;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Shutdown);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	nHow = (int)pRegs->u64Arg1;

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_shutdown(i64Fd, nHow);
		/* greppable: linux_cold_net: soft shutdown lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * Soft: no net_tcp_shutdown yet — accept as success so
		 * glibc shutdown probes do not hard-fail product STREAM.
		 * greppable: linux_cold_net: soft shutdown tcp
		 */
		(void)nHow;
		return 0;
	}
	lcn_soft_bump(&g_lcnSoft.u64Fail);
	return -(i64)LINUX_EBADF;
}

i64
gj_linux_cold_getsockname(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u8 aSa[16];
	u32 u32Len = 16u;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Getsockname);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;

	if (pRegs->u64Arg2 != 0u) {
		if (user_range_ok(pRegs->u64Arg2, 4u)) {
			(void)copy_from_user(&u32Len, pRegs->u64Arg2, 4u);
		} else {
			u32Len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
		}
	}

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_getsockname(i64Fd, aSa, &u32Len);
		/* greppable: linux_cold_net: soft getsockname lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	} else if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * Soft: net_tcp has no getsockname — fabricate loopback
		 * sockaddr_in (port 0) for glibc probes.
		 * greppable: linux_cold_net: soft getsockname tcp
		 */
		if (u32Len < 16u) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_EINVAL;
		}
		lcn_fill_sin(aSa, 0);
		u32Len = 16u;
	} else {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	if (pRegs->u64Arg1 != 0u) {
		i64St = lcn_copy_out(pRegs->u64Arg1, aSa, (size_t)u32Len);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	}
	if (pRegs->u64Arg2 != 0u) {
		if (user_range_ok(pRegs->u64Arg2, 4u)) {
			if (copy_to_user(pRegs->u64Arg2, &u32Len, 4u) !=
			    GJ_OK) {
				lcn_soft_bump(&g_lcnSoft.u64Fail);
				return -(i64)LINUX_EFAULT;
			}
		} else {
			*(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u32Len;
		}
	}
	return 0;
}

i64
gj_linux_cold_getpeername(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	u8 aSa[16];
	u32 u32Len = 16u;
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Getpeername);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;

	if (pRegs->u64Arg2 != 0u) {
		if (user_range_ok(pRegs->u64Arg2, 4u)) {
			(void)copy_from_user(&u32Len, pRegs->u64Arg2, 4u);
		} else {
			u32Len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
		}
	}

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_getpeername(i64Fd, aSa, &u32Len);
		/* greppable: linux_cold_net: soft getpeername lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	} else if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * Soft: net_tcp has no getpeername — fabricate loopback
		 * sockaddr_in (port 0) so glibc peer probes succeed.
		 * greppable: linux_cold_net: soft getpeername tcp
		 */
		if (u32Len < 16u) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_EINVAL;
		}
		lcn_fill_sin(aSa, 0);
		u32Len = 16u;
	} else {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	if (pRegs->u64Arg1 != 0u) {
		i64St = lcn_copy_out(pRegs->u64Arg1, aSa, (size_t)u32Len);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	}
	if (pRegs->u64Arg2 != 0u) {
		if (user_range_ok(pRegs->u64Arg2, 4u)) {
			if (copy_to_user(pRegs->u64Arg2, &u32Len, 4u) !=
			    GJ_OK) {
				lcn_soft_bump(&g_lcnSoft.u64Fail);
				return -(i64)LINUX_EFAULT;
			}
		} else {
			*(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u32Len;
		}
	}
	return 0;
}

i64
gj_linux_cold_setsockopt(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	int nLevel;
	int nOpt;
	u8 aVal[16];
	u32 u32Len;
	u32 u32Copy;
	i64 i64St;
	u32 i;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Setsockopt);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	nLevel = (int)pRegs->u64Arg1;
	nOpt = (int)pRegs->u64Arg2;
	u32Len = (u32)pRegs->u64Arg4;

	for (i = 0; i < sizeof(aVal); i++) {
		aVal[i] = 0;
	}
	u32Copy = u32Len;
	if (u32Copy > (u32)sizeof(aVal)) {
		u32Copy = (u32)sizeof(aVal);
	}
	if (pRegs->u64Arg3 != 0u && u32Copy != 0u) {
		i64St = lcn_copy_in(aVal, pRegs->u64Arg3, (size_t)u32Copy);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	}

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_setsockopt(i64Fd, nLevel, nOpt, aVal, u32Len);
		/* greppable: linux_cold_net: soft setsockopt lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * Soft TCP SOL_SOCKET subset — no full stack.
		 * greppable: linux_cold_net: soft setsockopt tcp
		 */
		if (nLevel == LCN_SOL_SOCKET && nOpt == LCN_SO_REUSEADDR) {
			u8 *pR = lcn_tcp_reuse_slot(i64Fd);
			int nVal = 0;

			if (u32Copy >= 4u) {
				nVal = *(const int *)(const void *)aVal;
			}
			if (pR != NULL) {
				*pR = nVal ? 1u : 0u;
			}
			return 0;
		}
		/* Other levels / opts: soft accept (glibc probes). */
		return 0;
	}
	lcn_soft_bump(&g_lcnSoft.u64Fail);
	return -(i64)LINUX_EBADF;
}

i64
gj_linux_cold_getsockopt(struct gj_linux_regs *pRegs)
{
	i64 i64Fd;
	int nLevel;
	int nOpt;
	/* Bounce ≥ linger (8) so lo full subset cannot stack-overflow soft. */
	u8 aVal[16];
	u32 u32Len = 4u;
	u32 u32Out;
	i64 i64St;
	u32 i;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Getsockopt);
	lcn_soft_op();

	i64Fd = (i64)pRegs->u64Arg0;
	nLevel = (int)pRegs->u64Arg1;
	nOpt = (int)pRegs->u64Arg2;

	for (i = 0; i < sizeof(aVal); i++) {
		aVal[i] = 0;
	}

	if (pRegs->u64Arg4 != 0u) {
		if (user_range_ok(pRegs->u64Arg4, 4u)) {
			(void)copy_from_user(&u32Len, pRegs->u64Arg4, 4u);
		} else {
			u32Len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg4;
		}
	}
	/* Cap kernel bounce; lo LINGER needs 8, int opts need 4. */
	if (u32Len == 0u) {
		u32Len = 4u;
	}
	if (u32Len > (u32)sizeof(aVal)) {
		u32Len = (u32)sizeof(aVal);
	}

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_getsockopt(i64Fd, nLevel, nOpt, aVal, &u32Len);
		/* greppable: linux_cold_net: soft getsockopt lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	} else if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * Soft TCP SOL_SOCKET: TYPE / ERROR / REUSEADDR.
		 * greppable: linux_cold_net: soft getsockopt tcp
		 */
		int nVal = 0;

		if (nLevel != LCN_SOL_SOCKET) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_ENOPROTOOPT;
		}
		if (u32Len < 4u) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_EINVAL;
		}
		if (nOpt == LCN_SO_TYPE) {
			nVal = LCN_SOCK_STREAM;
		} else if (nOpt == LCN_SO_ERROR) {
			nVal = 0;
		} else if (nOpt == LCN_SO_REUSEADDR) {
			u8 *pR = lcn_tcp_reuse_slot(i64Fd);

			nVal = (pR != NULL && *pR != 0u) ? 1 : 0;
		} else {
			/* Soft: unknown SOL_SOCKET opt → 0 value. */
			nVal = 0;
		}
		*(int *)(void *)aVal = nVal;
		u32Len = 4u;
	} else {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	u32Out = u32Len;
	if (u32Out > (u32)sizeof(aVal)) {
		u32Out = (u32)sizeof(aVal);
	}
	if (pRegs->u64Arg3 != 0u && u32Out != 0u) {
		i64St = lcn_copy_out(pRegs->u64Arg3, aVal, (size_t)u32Out);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return i64St;
		}
	}
	if (pRegs->u64Arg4 != 0u) {
		if (user_range_ok(pRegs->u64Arg4, 4u)) {
			if (copy_to_user(pRegs->u64Arg4, &u32Len, 4u) !=
			    GJ_OK) {
				lcn_soft_bump(&g_lcnSoft.u64Fail);
				return -(i64)LINUX_EFAULT;
			}
		} else {
			*(u32 *)(gj_vaddr_t)pRegs->u64Arg4 = u32Len;
		}
	}
	return 0;
}
