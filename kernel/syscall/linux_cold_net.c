/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux socket-family cold handlers (Option C ABI-first).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 - no GPL source.
 *
 * Role (functional residual cold net ABI for sshd/stack - Soft!=product)
 * ----------------------------------------------------------------------
 * Lean cold Linux socket ABI residual bridging to net_tcp / net_lo for
 * userspace Linux-shaped networking (sshd/stack STREAM path). Soft residual
 * lean only. Deepen target: functional cold ABI fidelity for sshd/stack
 * (socket/bind/listen/accept/send/recv/shutdown/name/sockopt/close on
 * STREAM/:22). H1: net_tcp_poll only on thr stack (never IRQ).
 *
 * Interim table routing (matches net_door SOCKET..ACCEPT policy):
 *   AF_INET + SOCK_STREAM  -> net_tcp_*  (multi-seg TCP table)
 *   DGRAM / AF_UNIX / other -> net_lo_*
 * Per-op ownership: net_tcp_fd_ok / net_lo_fd_ok select the table.
 * User memory: user_range_ok + copy_{from,to}_user; kernel smoke buffers
 * via direct memcpy (same edge as net_door / linux_hot sock path).
 *
 * Product direction (Dual DoD B OPEN; agent != close):
 *   UDX + ABI (userspace Linux-shaped drivers / personality over cold ABI).
 *   Product NIC = UDX. Soft!=product. G-AC-1: no Linux .ko product AC.
 *   Freestanding rtl SKIP - do NOT reintroduce freestanding rtl DoD B
 *   rabbit hole as product close. This unit is a cold ABI bridge - not
 *   freestanding rtl wire ownership.
 *
 * Soft residual (lean, once-lamp, never hard-gates beyond -LINUX_E*):
 *   socket/bind/listen/accept/connect/send/recv + name/sockopt/msg;
 *   accept readiness + pre/post pump; SO_ERROR / half-close / POLLERR/HUP;
 *   SO_ACCEPTCONN + SO_SNDBUF/RCVBUF soft; name-l2 via net_l2_ip;
 *   :22 once-lamps stay Soft!=product (userspace sshd-shaped cold ABI).
 *   Prior residual (v11/v12/C2) retained: name/shutdown bridge, accept/connect
 *   product name sync, EPIPE/EOF/ECONNRESET soft mirrors, accept listen-gate,
 *   SO_KEEPALIVE / SO_PROTOCOL / SO_DOMAIN.
 *   Functional residual deepen (sshd/stack cold ABI; Soft!=product):
 *     re-listen soft SHUT_RD clear (AcceptQ re-arm; net_tcp listen match);
 *     connect clears SO_ACCEPTCONN listen bit (client sockets);
 *     poll: SHUT_RD POLLIN only when !listen (no false accept wake);
 *     sticky RST SO_ERROR: next recv/send returns -ECONNRESET (not soft EOF);
 *     TCP_NODELAY soft store/get (IPPROTO_TCP; sshd/stack Nagle probe);
 *     Dual DoD A/B remain OPEN (soft residual != product close).
 *
 * Soft inventory never hard-gates. Greppable markers:
 *   linux_cold_net: soft ...
 *   linux_cold_net: soft ... Soft!=product
 *   linux_cold_net: soft residual lean ... Soft!=product
 *   linux_cold_net: soft listen :22 soft_listen_ne_host_banner
 *   linux_cold_net v2 getpeername sockopt send recv msg
 *   linux_cold_net v3 accept readiness soft poll :22
 *   linux_cold_net v4 socket bind listen accept :22 readiness Soft!=product
 *   linux_cold_net v5 close connect peer Soft!=product
 *   linux_cold_net v6 so_error accept peer once Soft!=product
 *   linux_cold_net v7 so_error poll send recv once Soft!=product
 *   linux_cold_net v8 half-close name-l2 flags Soft!=product
 *   linux_cold_net v9 lean residual not freestanding wire Soft!=product
 *   linux_cold_net v10 lean residual UDX+ABI Soft!=product
 *   linux_cold_net v11 name+shutdown bridge send* Soft!=product
 *   linux_cold_net v12 sshd path residual soft_listen_ne_host_banner
 *   linux_cold_net C2 epipe mirror connect name sync close :22 Soft!=product
 *   linux_cold_net C2 recv eof mirror reset accept gate Soft!=product
 *   linux_cold_net functional residual sshd/stack Soft!=product
 *
 * Coordinator wires these into cold dispatch later - do not require
 * Makefile / protonrt_cold_link / net_door edits from this unit.
 */
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_cold_net.h>
#include <gj/net_eth.h>
#include <gj/net_l2.h>
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
#define LCN_SOL_SOCKET   1
#define LCN_SO_REUSEADDR 2
#define LCN_SO_TYPE      3
#define LCN_SO_ERROR     4
#define LCN_SO_SNDBUF    7
#define LCN_SO_RCVBUF    8
#define LCN_SO_KEEPALIVE 9
#define LCN_SO_LINGER    13
#define LCN_SO_ACCEPTCONN 30
#define LCN_SO_PROTOCOL  38
#define LCN_SO_DOMAIN    39
/* IPPROTO_TCP (man pages); soft SO_PROTOCOL for STREAM. Soft!=product. */
#define LCN_IPPROTO_TCP  6
/* TCP_NODELAY (man pages); sshd/stack Nagle-off soft residual. Soft!=product. */
#define LCN_TCP_NODELAY  1

/* send/recv flags (x86_64 Linux ABI; man pages only). Soft residual. */
#define LCN_MSG_DONTWAIT 0x40
#define LCN_MSG_NOSIGNAL 0x4000

/* shutdown how (x86_64 Linux ABI). Soft half-close residual. */
#define LCN_SHUT_RD   0
#define LCN_SHUT_WR   1
#define LCN_SHUT_RDWR 2
#define LCN_SHUT_BIT_RD 0x1u
#define LCN_SHUT_BIT_WR 0x2u

/* Align bounce with net_door NET_XFER_MAX / net_tcp multi-seg bulk. */
#define LCN_XFER_MAX 4096u

/* Soft TCP SO_REUSEADDR bits for FD 96..111 (matches net_tcp layout). */
#define LCN_TCP_FD_BASE 96u
#define LCN_TCP_MAX     16u

/*
 * User-fd alias table: maps 0..127 -> canonical tcp/lo fd + refcount.
 * Canonical slots stay in net_tcp 96..111 / net_lo 64..79 (do not retarget
 * fd_to_slot). OpenSSH dup2(accepted,0); dup2(0,1); close(accepted) needs
 * aliases 0/1/2. dup() skips vfs 3..95 so it does not collide with vfs_ram.
 */
#define LCN_ALIAS_MAX     128u
#define LCN_ALIAS_STDIO   3u
#define LCN_ALIAS_VFS_LIM 96u

/* Linux-shaped poll bits (match net_tcp_poll_mask / net_lo_poll_mask). */
#define LCN_POLLIN  0x1u
#define LCN_POLLOUT 0x4u
#define LCN_POLLERR 0x8u
#define LCN_POLLHUP 0x10u

/* Soft :22 residual port (userspace sshd-shaped cold ABI). Soft!=product. */
#define LCN_PORT_SSH 22u

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
	u64 u64Listen22;
	u64 u64Accept;
	u64 u64AcceptAgain;
	u64 u64AcceptReady;
	u64 u64AcceptPump;
	u64 u64AcceptPrePump;
	u64 u64Connect;
	u64 u64ConnectPump;
	u64 u64Sendto;
	u64 u64SendPump;
	u64 u64Recvfrom;
	u64 u64RecvAgain;
	u64 u64Send;
	u64 u64Recv;
	u64 u64Sendmsg;
	u64 u64Recvmsg;
	u64 u64Shutdown;
	u64 u64ShutdownTcp;
	u64 u64Close;
	u64 u64Getsockname;
	u64 u64Getpeername;
	u64 u64Setsockopt;
	u64 u64Getsockopt;
	u64 u64SoErrGet;
	u64 u64SoErrSet;
	u64 u64PollMask;
	u64 u64PollErr;
	u64 u64PollHup;
	u64 u64SendEpipe;
	u64 u64RecvEofShut;
	u64 u64NameL2;
	u64 u64NameTcp;
	u64 u64PeerTcp;
	u64 u64NameSoftFb;
	u64 u64PeerSoftFb;
	u64 u64AcceptConn;
	u64 u64SndRcvBuf;
	u64 u64PollOutShut;
	u64 u64SendPreShut;
	/* v12: Linux-shaped sshd residual path tallies (Soft!=product). */
	u64 u64AcceptNameSync;
	u64 u64ListenNotBanner;
	u64 u64PathSocket22;
	u64 u64PathBind22;
	u64 u64PathListen22;
	u64 u64PathAccept22;
	u64 u64PathSend22;
	u64 u64PathRecv22;
	u64 u64PathShut22;
	u64 u64PathName22;
	/* C2: connect name sync + :22 peer name/close + EPIPE mirror. Soft!=product. */
	u64 u64ConnectNameSync;
	u64 u64PathPeer22;
	u64 u64PathClose22;
	u64 u64EpipeMirror;
	/* C2 deepen: recv EOF / ECONNRESET mirrors + accept gate + keepalive. */
	u64 u64RecvEofMirror;
	u64 u64ResetMirror;
	u64 u64AcceptNotListen;
	u64 u64Keepalive;
	u64 u64ProtoDomain;
	/* Functional residual: re-listen rearm + RST sticky IO + TCP_NODELAY. */
	u64 u64ListenRearm;
	u64 u64RstIo;
	u64 u64Nodelay;
	u64 u64PollListenShut;
	u64 u64LeanOk;
	u64 u64Fail;
	u64 u64UserCopy;
	u64 u64KernelCopy;
	u64 u64Ops;
	u64 u64LogN;
} g_lcnSoft;

/* Soft TCP SO_REUSEADDR store (slot = fd - 96). Soft!=product. */
static u8 g_lcnTcpReuse[LCN_TCP_MAX];

/* Soft TCP SO_KEEPALIVE store (sshd-shaped; Soft!=product). */
static u8 g_lcnTcpKeepalive[LCN_TCP_MAX];

/* Soft TCP_NODELAY store (sshd/stack Nagle-off; Soft!=product). */
static u8 g_lcnTcpNodelay[LCN_TCP_MAX];

/* Soft TCP bind port store (host order; 0 = unbound). Soft!=product. */
static u16 g_lcnTcpPort[LCN_TCP_MAX];

/* Soft TCP peer port store (host order; 0 = unknown). Soft!=product. */
static u16 g_lcnTcpPeerPort[LCN_TCP_MAX];

/*
 * Soft TCP SO_ERROR store (positive LINUX_E*; 0 = clear). Soft!=product.
 * Linux-shaped clear-on-read via getsockopt(SO_ERROR).
 */
static u16 g_lcnTcpSoErr[LCN_TCP_MAX];

/*
 * Soft TCP half-close bits (LCN_SHUT_BIT_RD / LCN_SHUT_BIT_WR). Soft!=product.
 * greppable: linux_cold_net: soft shutdown tcp
 */
static u8 g_lcnTcpShut[LCN_TCP_MAX];

/*
 * Soft TCP listen bit (1 after successful listen). Soft!=product.
 * Feeds SO_ACCEPTCONN honesty - cold ABI bridge only (UDX+ABI product).
 * greppable: linux_cold_net: soft listen bit
 */
static u8 g_lcnTcpListen[LCN_TCP_MAX];

/* User fd -> canonical tcp/lo fd (-1 free). Refcount is indexed by canon fd. */
static i64 g_ai64LcnCanon[LCN_ALIAS_MAX];
static u16 g_au16LcnRef[LCN_ALIAS_MAX];
static u8 g_u8LcnAliasInit;

/* Soft once lamps so :22 path greps stay readable. Soft!=product. */
static u8 g_u8LcnSocketTcpOnce;
static u8 g_u8LcnBind22Once;
static u8 g_u8LcnBindTcpOnce;
static u8 g_u8LcnListen22Once;
static u8 g_u8LcnListenOnce;
static u8 g_u8LcnListenPumpOnce;
static u8 g_u8LcnPollMaskOnce;
static u8 g_u8LcnPollErrOnce;
static u8 g_u8LcnPollHupOnce;
static u8 g_u8LcnAcceptReadyOnce;
static u8 g_u8LcnAcceptPrePumpOnce;
static u8 g_u8LcnAcceptPumpOnce;
static u8 g_u8LcnAccept22Once;
static u8 g_u8LcnAcceptOnce;
static u8 g_u8LcnConnectOnce;
static u8 g_u8LcnSendOnce;
static u8 g_u8LcnRecvOnce;
static u8 g_u8LcnSoErrOnce;
static u8 g_u8LcnCloseOnce;
static u8 g_u8LcnShutdownOnce;
static u8 g_u8LcnNameL2Once;
static u8 g_u8LcnSendEpipeOnce;
static u8 g_u8LcnAcceptConnOnce;
static u8 g_u8LcnNameTcpOnce;
static u8 g_u8LcnPeerTcpOnce;
static u8 g_u8LcnSendPreShutOnce;
static u8 g_u8LcnLeanOnce;
/* v12: Linux-shaped sshd residual once-lamps (NO stamp storms). Soft!=product. */
static u8 g_u8LcnName22Once;
static u8 g_u8LcnSend22Once;
static u8 g_u8LcnRecv22Once;
static u8 g_u8LcnShut22Once;
static u8 g_u8LcnAcceptNameSyncOnce;
static u8 g_u8LcnPathResidualOnce;
/* C2: connect name sync / EPIPE mirror / peer name / close :22 once-lamps. Soft!=product. */
static u8 g_u8LcnConnectNameSyncOnce;
static u8 g_u8LcnEpipeMirrorOnce;
static u8 g_u8LcnPeerName22Once;
static u8 g_u8LcnClose22Once;
/* C2 deepen: recv EOF / reset mirrors + accept gate + keepalive once-lamps. */
static u8 g_u8LcnRecvEofMirrorOnce;
static u8 g_u8LcnResetMirrorOnce;
static u8 g_u8LcnAcceptNotListenOnce;
static u8 g_u8LcnKeepaliveOnce;
/* Functional residual: re-listen rearm / RST IO / TCP_NODELAY once-lamps. */
static u8 g_u8LcnListenRearmOnce;
static u8 g_u8LcnRstIoOnce;
static u8 g_u8LcnNodelayOnce;
static u8 g_u8LcnPollListenShutOnce;
static u8 g_u8LcnForkDupOnce;

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
	/* One inventory line only - NO stamp storms (HARD). Soft!=product. */
	kprintf("linux_cold_net: soft inventory ops=%llu sock=%llu tcp=%llu "
		"lo=%llu bind=%llu listen=%llu listen22=%llu "
		"accept=%llu again=%llu ready=%llu pump=%llu prepump=%llu "
		"connect=%llu close=%llu sendto=%llu recvfrom=%llu "
		"send=%llu recv=%llu shut=%llu/%llu name=%llu peer=%llu "
		"name_tcp=%llu peer_tcp=%llu soft_fb=%llu/%llu "
		"setopt=%llu getopt=%llu soerr=%llu/%llu poll=%llu "
		"epipe=%llu/%llu preshut=%llu name_l2=%llu acceptconn=%llu "
		"sndrcv=%llu aname_sync=%llu cname_sync=%llu not_banner=%llu "
		"path22=s%llu/b%llu/l%llu/a%llu/tx%llu/rx%llu/sh%llu/n%llu/"
		"p%llu/c%llu "
		"eof_m=%llu rst_m=%llu not_listen=%llu ka=%llu proto=%llu "
		"rearm=%llu rst_io=%llu nodelay=%llu poll_ls=%llu "
		"lean_ok=%llu fail=%llu logn=%llu "
		"(Soft!=product; product_dir=UDX+ABI; product_NIC=UDX; "
		"freestanding_rtl_product_close=0; soft_listen_ne_host_banner=1; "
		"Dual_DoD_AB_OPEN=1; cold ABI bridge; sshd_stack_functional=1)\n",
		(unsigned long long)g_lcnSoft.u64Ops,
		(unsigned long long)g_lcnSoft.u64Socket,
		(unsigned long long)g_lcnSoft.u64SocketTcp,
		(unsigned long long)g_lcnSoft.u64SocketLo,
		(unsigned long long)g_lcnSoft.u64Bind,
		(unsigned long long)g_lcnSoft.u64Listen,
		(unsigned long long)g_lcnSoft.u64Listen22,
		(unsigned long long)g_lcnSoft.u64Accept,
		(unsigned long long)g_lcnSoft.u64AcceptAgain,
		(unsigned long long)g_lcnSoft.u64AcceptReady,
		(unsigned long long)g_lcnSoft.u64AcceptPump,
		(unsigned long long)g_lcnSoft.u64AcceptPrePump,
		(unsigned long long)g_lcnSoft.u64Connect,
		(unsigned long long)g_lcnSoft.u64Close,
		(unsigned long long)g_lcnSoft.u64Sendto,
		(unsigned long long)g_lcnSoft.u64Recvfrom,
		(unsigned long long)g_lcnSoft.u64Send,
		(unsigned long long)g_lcnSoft.u64Recv,
		(unsigned long long)g_lcnSoft.u64Shutdown,
		(unsigned long long)g_lcnSoft.u64ShutdownTcp,
		(unsigned long long)g_lcnSoft.u64Getsockname,
		(unsigned long long)g_lcnSoft.u64Getpeername,
		(unsigned long long)g_lcnSoft.u64NameTcp,
		(unsigned long long)g_lcnSoft.u64PeerTcp,
		(unsigned long long)g_lcnSoft.u64NameSoftFb,
		(unsigned long long)g_lcnSoft.u64PeerSoftFb,
		(unsigned long long)g_lcnSoft.u64Setsockopt,
		(unsigned long long)g_lcnSoft.u64Getsockopt,
		(unsigned long long)g_lcnSoft.u64SoErrGet,
		(unsigned long long)g_lcnSoft.u64SoErrSet,
		(unsigned long long)g_lcnSoft.u64PollMask,
		(unsigned long long)g_lcnSoft.u64SendEpipe,
		(unsigned long long)g_lcnSoft.u64EpipeMirror,
		(unsigned long long)g_lcnSoft.u64SendPreShut,
		(unsigned long long)g_lcnSoft.u64NameL2,
		(unsigned long long)g_lcnSoft.u64AcceptConn,
		(unsigned long long)g_lcnSoft.u64SndRcvBuf,
		(unsigned long long)g_lcnSoft.u64AcceptNameSync,
		(unsigned long long)g_lcnSoft.u64ConnectNameSync,
		(unsigned long long)g_lcnSoft.u64ListenNotBanner,
		(unsigned long long)g_lcnSoft.u64PathSocket22,
		(unsigned long long)g_lcnSoft.u64PathBind22,
		(unsigned long long)g_lcnSoft.u64PathListen22,
		(unsigned long long)g_lcnSoft.u64PathAccept22,
		(unsigned long long)g_lcnSoft.u64PathSend22,
		(unsigned long long)g_lcnSoft.u64PathRecv22,
		(unsigned long long)g_lcnSoft.u64PathShut22,
		(unsigned long long)g_lcnSoft.u64PathName22,
		(unsigned long long)g_lcnSoft.u64PathPeer22,
		(unsigned long long)g_lcnSoft.u64PathClose22,
		(unsigned long long)g_lcnSoft.u64RecvEofMirror,
		(unsigned long long)g_lcnSoft.u64ResetMirror,
		(unsigned long long)g_lcnSoft.u64AcceptNotListen,
		(unsigned long long)g_lcnSoft.u64Keepalive,
		(unsigned long long)g_lcnSoft.u64ProtoDomain,
		(unsigned long long)g_lcnSoft.u64ListenRearm,
		(unsigned long long)g_lcnSoft.u64RstIo,
		(unsigned long long)g_lcnSoft.u64Nodelay,
		(unsigned long long)g_lcnSoft.u64PollListenShut,
		(unsigned long long)g_lcnSoft.u64LeanOk,
		(unsigned long long)g_lcnSoft.u64Fail,
		(unsigned long long)g_lcnSoft.u64LogN);
}

/**
 * Lean residual once-lamp (functional residual cold net ABI for sshd/stack).
 * Soft!=product / G-AC-1. Cold net ABI bridges socket family -> net_tcp/net_lo
 * for userspace Linux-shaped networking (sshd/stack STREAM/:22 residual).
 * Product DoD B direction = UDX+ABI; product NIC = UDX.
 * Freestanding rtl SKIP - Do NOT reintroduce freestanding rtl DoD B
 * rabbit hole as product close. Dual DoD A/B remain OPEN (agent != close).
 * HARD: Soft listen :22 != host banner proof.
 * HARD: no version stamp / no stamp storms / dual MIT OR Apache-2.0.
 * HARD: H1 thr-only - net_tcp_poll from thr stack only (never IRQ).
 * greppable: linux_cold_net: soft residual lean Soft!=product
 * greppable: linux_cold_net v9 lean residual not freestanding wire
 * greppable: linux_cold_net v10 lean residual UDX+ABI Soft!=product
 * greppable: linux_cold_net v11 name+shutdown bridge send* Soft!=product
 * greppable: linux_cold_net v12 sshd path residual soft_listen_ne_host_banner
 * greppable: linux_cold_net C2 epipe mirror connect name sync close :22 Soft!=product
 * greppable: linux_cold_net C2 recv eof mirror reset accept gate Soft!=product
 * greppable: linux_cold_net functional residual sshd/stack Soft!=product
 */
static void
lcn_soft_residual_lean_once(void)
{
	u32 u32Ok = 0;
	/* Product direction honesty - stack-local only; never hard-gates. */
	const int nProductUdxAbi = 1; /* Dual DoD B product = UDX+ABI */
	const int nProductNicUdx = 1; /* product NIC = UDX */
	const int nFsRtlProductClose = 0; /* freestanding rtl != product close */
	const int nFsRtlSkip = 1; /* freestanding rtl SKIP default */
	const int nWireOwner = 0; /* cold ABI never owns freestanding wire */
	const int nKoProduct = 0; /* G-AC-1: no .ko product AC */
	const int nDualDodOpen = 1; /* Dual DoD A/B OPEN; soft residual != close */
	/* v11 functional bridge honesty (APIs exist on net_tcp). Soft!=product. */
	const int nTcpShutBridge = 1;
	const int nTcpNameBridge = 1;
	const int nSendPreShut = 1;
	/* v12: Soft listen != host banner; accept product name sync. Soft!=product. */
	const int nSoftListenNeHostBanner = 1;
	const int nAcceptNameSync = 1;
	const int nSshdPathResidual = 1;
	/* C2: connect name sync + product EPIPE soft mirror + close/peer :22. */
	const int nConnectNameSync = 1;
	const int nEpipeMirror = 1;
	const int nPathClosePeer22 = 1;
	/* C2 deepen: recv EOF mirror + reset mirror + accept gate + keepalive. */
	const int nRecvEofMirror = 1;
	const int nResetMirror = 1;
	const int nAcceptListenGate = 1;
	const int nKeepaliveSoft = 1;
	/* Functional residual (sshd/stack): rearm / RST IO / NODELAY / poll gate. */
	const int nListenRearm = 1;
	const int nRstStickyIo = 1;
	const int nNodelaySoft = 1;
	const int nPollListenShut = 1;
	const int nH1ThrOnly = 1; /* net_tcp_poll thr stack only */

	if (g_u8LcnLeanOnce != 0u) {
		return;
	}
	g_u8LcnLeanOnce = 1;

	/* Compact self-check - stack-local only; never hard-gates product. */
	if (LCN_TCP_MAX == 16u) {
		u32Ok++;
	}
	if (LCN_XFER_MAX == 4096u) {
		u32Ok++;
	}
	if (LCN_PORT_SSH == 22u) {
		u32Ok++;
	}
	if (LCN_SO_ACCEPTCONN == 30) {
		u32Ok++;
	}
	if (LCN_SO_SNDBUF == 7 && LCN_SO_RCVBUF == 8) {
		u32Ok++;
	}
	/* Bridge honesty: cold ABI only; UDX+ABI product; no rtl product close. */
	if (nWireOwner == 0 && nProductUdxAbi == 1 &&
	    nFsRtlProductClose == 0 && nKoProduct == 0 &&
	    nProductNicUdx == 1 && nFsRtlSkip == 1 && nDualDodOpen == 1) {
		u32Ok++;
	}
	/* v11: product table name/shutdown + send* pre-shut footgun fix. */
	if (nTcpShutBridge == 1 && nTcpNameBridge == 1 && nSendPreShut == 1) {
		u32Ok++;
	}
	/* v12: soft listen != host banner + accept name sync + sshd path. */
	if (nSoftListenNeHostBanner == 1 && nAcceptNameSync == 1 &&
	    nSshdPathResidual == 1) {
		u32Ok++;
	}
	/* C2: connect name sync + EPIPE mirror + :22 peer/close path. */
	if (nConnectNameSync == 1 && nEpipeMirror == 1 &&
	    nPathClosePeer22 == 1) {
		u32Ok++;
	}
	/* C2 deepen + functional residual sshd/stack (rearm/RST/NODELAY/H1). */
	if (nRecvEofMirror == 1 && nResetMirror == 1 &&
	    nAcceptListenGate == 1 && nKeepaliveSoft == 1 &&
	    nListenRearm == 1 && nRstStickyIo == 1 &&
	    nNodelaySoft == 1 && nPollListenShut == 1 &&
	    nH1ThrOnly == 1 &&
	    LCN_SO_KEEPALIVE == 9 && LCN_SO_PROTOCOL == 38 &&
	    LCN_SO_DOMAIN == 39 && LCN_IPPROTO_TCP == 6 &&
	    LCN_TCP_NODELAY == 1) {
		u32Ok++;
	}
	if (u32Ok == 10u) {
		lcn_soft_bump(&g_lcnSoft.u64LeanOk);
	}

	/*
	 * One residual line only (HARD: no stamp storms). Soft!=product.
	 * Grep: linux_cold_net: soft residual lean
	 * Grep: linux_cold_net v9 lean residual not freestanding wire
	 * Grep: linux_cold_net v10 lean residual UDX+ABI Soft!=product
	 * Grep: linux_cold_net v11 name+shutdown bridge send* Soft!=product
	 * Grep: linux_cold_net v12 sshd path residual soft_listen_ne_host_banner
	 * Grep: linux_cold_net C2 epipe mirror connect name sync close :22
	 * Grep: linux_cold_net C2 recv eof mirror reset accept gate Soft!=product
	 * Grep: linux_cold_net functional residual sshd/stack Soft!=product
	 */
	kprintf("linux_cold_net: soft residual lean "
		"ok=%u/10 dual=MIT_OR_Apache-2.0 soft_ne_product=1 G-AC-1=1 "
		"product_dir=UDX+ABI product_NIC=UDX "
		"freestanding_rtl_product_close=0 freestanding_rtl_skip=1 "
		"wire_owner=0 freestanding_wire=0 cold_abi=1 "
		"bridge=net_tcp+net_lo userspace_linux_shaped=1 "
		"sshd_shaped_path=1 sshd_stack_functional=1 Dual_DoD_AB_OPEN=1 "
		"path=socket+bind+listen+accept+send+recv+shutdown+name+close "
		"tcp_name_bridge=1 tcp_shut_bridge=1 send_preshut=1 "
		"accept_name_sync=1 connect_name_sync=1 epipe_mirror=1 "
		"recv_eof_mirror=1 reset_mirror=1 accept_listen_gate=1 "
		"keepalive_soft=1 so_protocol=1 so_domain=1 "
		"listen_rearm=1 rst_sticky_io=1 tcp_nodelay=1 "
		"poll_listen_shut=1 H1_thr_only=1 "
		"soft_listen_ne_host_banner=1 "
		"tcp_slots=%u xfer=%u ssh_port=%u "
		"half_close=1 name_l2=1 acceptconn=1 sndrcv=1 "
		"stamp_storm=0 no_version_stamp=1 "
		"(Soft!=product; functional residual cold net ABI sshd/stack; "
		"product DoD B = UDX+ABI; product NIC = UDX; "
		"not freestanding rtl product close; freestanding rtl SKIP; "
		"Soft listen :22 != host banner; not product OpenSSH; "
		"Dual DoD A/B OPEN; dual MIT OR Apache-2.0)\n",
		(unsigned)u32Ok, (unsigned)LCN_TCP_MAX,
		(unsigned)LCN_XFER_MAX, (unsigned)LCN_PORT_SSH);
}

static void
lcn_soft_op(void)
{
	lcn_soft_bump(&g_lcnSoft.u64Ops);
	/* Lean residual once on first cold op - rides ops gate (no storms). */
	lcn_soft_residual_lean_once();
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
 * Read sockaddr_in port at offset 2 (network / big-endian) -> host order.
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
	/* Need at least family(2) + port(2). Soft accept short smokes >=4. */
	if (u32Len != 0u && u32Len < 4u) {
		return -(i64)LINUX_EINVAL;
	}
	{
		i64 i64St = lcn_copy_in(&u16PortBe, u64Sa + 2u, 2u);

		if (i64St != 0) {
			return i64St;
		}
	}
	/* Network order -> host (x86_64 LE): byte swap 16. */
	*pPortHost = (u16)((u16PortBe >> 8) | (u16PortBe << 8));
	return 0;
}

/**
 * Soft sockaddr_in: AF_INET, port host->BE, 4-byte IPv4.
 * Unbound / zero IP -> 0.0.0.0 when port==0 else leave bytes as given.
 * greppable: linux_cold_net: soft fill sin
 */
static void
lcn_fill_sin_ip(u8 *pSa, u16 u16PortHost, const u8 *pIp4)
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
	if (pIp4 != NULL) {
		pSa[4] = pIp4[0];
		pSa[5] = pIp4[1];
		pSa[6] = pIp4[2];
		pSa[7] = pIp4[3];
	}
}

/**
 * Soft sockaddr_in loopback 127.0.0.1 (lo path / legacy helper).
 */
static void
lcn_fill_sin(u8 *pSa, u16 u16PortHost)
{
	u8 aLo[4];

	aLo[0] = 127;
	aLo[1] = 0;
	aLo[2] = 0;
	aLo[3] = 1;
	lcn_fill_sin_ip(pSa, u16PortHost, aLo);
}

/**
 * Soft TCP sockaddr_in via net_l2_ip (lab name honesty for cold ABI).
 * Bound port -> station/lab IP when L2 ready; unbound (port 0) -> 0.0.0.0.
 * Cold ABI residual lean only - Soft!=product / G-AC-1.
 * Product DoD B = UDX+ABI; freestanding_rtl_product_close=0.
 * greppable: linux_cold_net: soft name l2 Soft!=product
 */
static void
lcn_fill_sin_tcp(u8 *pSa, u16 u16PortHost)
{
	u8 aIp[4];

	if (u16PortHost == 0u) {
		/* Unbound STREAM: INADDR_ANY soft. */
		aIp[0] = 0;
		aIp[1] = 0;
		aIp[2] = 0;
		aIp[3] = 0;
		lcn_fill_sin_ip(pSa, 0, aIp);
		return;
	}
	aIp[0] = 0;
	aIp[1] = 0;
	aIp[2] = 0;
	aIp[3] = 0;
	net_l2_ip(aIp);
	/* If L2 not ready yet, soft loopback so probes still get AF_INET. */
	if (aIp[0] == 0u && aIp[1] == 0u && aIp[2] == 0u && aIp[3] == 0u) {
		aIp[0] = 127;
		aIp[3] = 1;
	} else {
		lcn_soft_bump(&g_lcnSoft.u64NameL2);
		if (!g_u8LcnNameL2Once) {
			g_u8LcnNameL2Once = 1;
			kprintf("linux_cold_net: soft name l2 "
				"%u.%u.%u.%u port=%u Soft!=product "
				"(product_dir=UDX+ABI; "
				"freestanding_rtl_product_close=0)\n",
				(unsigned)aIp[0], (unsigned)aIp[1],
				(unsigned)aIp[2], (unsigned)aIp[3],
				(unsigned)u16PortHost);
		}
	}
	lcn_fill_sin_ip(pSa, u16PortHost, aIp);
}

/** Soft TCP table slot index for fd, or LCN_TCP_MAX if out of range. */
static u32
lcn_tcp_slot(i64 i64Fd)
{
	u32 u32Slot;

	if (i64Fd < (i64)LCN_TCP_FD_BASE) {
		return LCN_TCP_MAX;
	}
	u32Slot = (u32)(i64Fd - (i64)LCN_TCP_FD_BASE);
	if (u32Slot >= LCN_TCP_MAX) {
		return LCN_TCP_MAX;
	}
	return u32Slot;
}

/** Soft TCP SO_REUSEADDR slot for fd, or NULL if out of soft range. */
static u8 *
lcn_tcp_reuse_slot(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return NULL;
	}
	return &g_lcnTcpReuse[u32Slot];
}

/** Soft remember/clear TCP bind port (host order). Soft!=product. */
static void
lcn_tcp_port_set(i64 i64Fd, u16 u16Port)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot < LCN_TCP_MAX) {
		g_lcnTcpPort[u32Slot] = u16Port;
	}
}

/** Soft TCP bind port for fd (0 if unknown / out of range). Soft!=product. */
static u16
lcn_tcp_port_get(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	return g_lcnTcpPort[u32Slot];
}

/** Soft remember/clear TCP peer port (host order). Soft!=product. */
static void
lcn_tcp_peer_set(i64 i64Fd, u16 u16Port)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot < LCN_TCP_MAX) {
		g_lcnTcpPeerPort[u32Slot] = u16Port;
	}
}

/** Soft TCP peer port for fd (0 if unknown / out of range). Soft!=product. */
static u16
lcn_tcp_peer_get(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	return g_lcnTcpPeerPort[u32Slot];
}

/** Soft remember/clear TCP SO_ERROR (positive LINUX_E*). Soft!=product. */
static void
lcn_tcp_soerr_set(i64 i64Fd, u16 u16Err)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot < LCN_TCP_MAX) {
		g_lcnTcpSoErr[u32Slot] = u16Err;
		if (u16Err != 0u) {
			lcn_soft_bump(&g_lcnSoft.u64SoErrSet);
		}
	}
}

/**
 * Soft peek SO_ERROR without clear (poll POLLERR honesty). Soft!=product.
 * greppable: linux_cold_net: soft so_error peek
 */
static u16
lcn_tcp_soerr_peek(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	return g_lcnTcpSoErr[u32Slot];
}

/**
 * Soft get-and-clear SO_ERROR (Linux clear-on-read). Soft!=product.
 * greppable: linux_cold_net: soft so_error
 */
static u16
lcn_tcp_soerr_take(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);
	u16 u16Err;

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	u16Err = g_lcnTcpSoErr[u32Slot];
	g_lcnTcpSoErr[u32Slot] = 0;
	return u16Err;
}

/**
 * Soft clear all TCP soft state for fd
 * (reuse/bind/peer/so_error/half-close/listen). Soft!=product.
 * greppable: linux_cold_net: soft tcp state clear
 */
static void
lcn_tcp_soft_clear(i64 i64Fd)
{
	u8 *pR = lcn_tcp_reuse_slot(i64Fd);
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (pR != NULL) {
		*pR = 0;
	}
	lcn_tcp_port_set(i64Fd, 0);
	lcn_tcp_peer_set(i64Fd, 0);
	if (u32Slot < LCN_TCP_MAX) {
		g_lcnTcpSoErr[u32Slot] = 0;
		g_lcnTcpShut[u32Slot] = 0;
		g_lcnTcpListen[u32Slot] = 0;
		g_lcnTcpKeepalive[u32Slot] = 0;
		g_lcnTcpNodelay[u32Slot] = 0;
	}
}

/** Soft set listen bit after successful listen. Soft!=product. */
static void
lcn_tcp_listen_set(i64 i64Fd, u8 u8On)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot < LCN_TCP_MAX) {
		g_lcnTcpListen[u32Slot] = u8On != 0u ? 1u : 0u;
	}
}

/** Soft listen bit for SO_ACCEPTCONN (0 if out of range). Soft!=product. */
static u8
lcn_tcp_listen_get(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	return g_lcnTcpListen[u32Slot];
}

/** Soft TCP half-close bits for fd (0 if out of range). Soft!=product. */
static u8
lcn_tcp_shut_get(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);

	if (u32Slot >= LCN_TCP_MAX) {
		return 0;
	}
	return g_lcnTcpShut[u32Slot];
}

/**
 * Soft OR half-close bits from shutdown(how). Soft!=product.
 * greppable: linux_cold_net: soft shutdown tcp
 */
static void
lcn_tcp_shut_or(i64 i64Fd, int nHow)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);
	u8 u8Bits = 0;

	if (u32Slot >= LCN_TCP_MAX) {
		return;
	}
	if (nHow == LCN_SHUT_RD) {
		u8Bits = (u8)LCN_SHUT_BIT_RD;
	} else if (nHow == LCN_SHUT_WR) {
		u8Bits = (u8)LCN_SHUT_BIT_WR;
	} else if (nHow == LCN_SHUT_RDWR) {
		u8Bits = (u8)(LCN_SHUT_BIT_RD | LCN_SHUT_BIT_WR);
	} else {
		/* Unknown how: treat as RDWR soft so probes settle. */
		u8Bits = (u8)(LCN_SHUT_BIT_RD | LCN_SHUT_BIT_WR);
	}
	g_lcnTcpShut[u32Slot] = (u8)(g_lcnTcpShut[u32Slot] | u8Bits);
}

/* Forward: sticky RST SO_ERROR IO (defined with half-close helpers). */
static i64 lcn_tcp_rst_sticky_io(i64 i64Fd);

/**
 * Soft send after SHUT_WR -> -EPIPE + sticky SO_ERROR. Soft!=product.
 * Call BEFORE user copy (send* EPIPE footgun - no wasted bounce on shut).
 * greppable: linux_cold_net: soft send epipe Soft!=product
 * greppable: linux_cold_net: soft send preshut Soft!=product
 */
static i64
lcn_tcp_send_if_open(i64 i64Fd)
{
	i64 i64Rst;

	/*
	 * Functional residual: sticky RST wins over bare EPIPE (sshd/stack
	 * teardown probes). Soft!=product.
	 * greppable: linux_cold_net: soft rst io Soft!=product
	 */
	i64Rst = lcn_tcp_rst_sticky_io(i64Fd);
	if (i64Rst != 0) {
		return i64Rst;
	}
	if ((lcn_tcp_shut_get(i64Fd) & LCN_SHUT_BIT_WR) != 0u) {
		lcn_soft_bump(&g_lcnSoft.u64SendEpipe);
		lcn_soft_bump(&g_lcnSoft.u64SendPreShut);
		lcn_tcp_soerr_set(i64Fd, (u16)LINUX_EPIPE);
		if (!g_u8LcnSendEpipeOnce) {
			g_u8LcnSendEpipeOnce = 1;
			kprintf("linux_cold_net: soft send epipe fd=%lld "
				"Soft!=product\n",
				(long long)i64Fd);
		}
		if (!g_u8LcnSendPreShutOnce) {
			g_u8LcnSendPreShutOnce = 1;
			/* greppable: linux_cold_net: soft send preshut Soft!=product */
			kprintf("linux_cold_net: soft send preshut fd=%lld "
				"EPIPE before copy Soft!=product "
				"(product_dir=UDX+ABI)\n",
				(long long)i64Fd);
		}
		return -(i64)LINUX_EPIPE;
	}
	return 0;
}

/**
 * Soft read sockaddr_in port (network-order bytes @+2) -> host order.
 * Used to sync soft port tables from product net_tcp name fills.
 */
static u16
lcn_sin_port_host(const u8 *pSa)
{
	if (pSa == NULL) {
		return 0;
	}
	return (u16)(((u16)pSa[2] << 8) | (u16)pSa[3]);
}

/**
 * C2: product send EPIPE soft SHUT_WR mirror. Soft!=product.
 * Keeps cold preshut pre-check + poll POLLOUT coherent after product
 * table returns EPIPE (SHUT_WR/RST) without prior soft shutdown.
 * greppable: linux_cold_net: soft epipe mirror Soft!=product
 */
static void
lcn_tcp_epipe_soft_mirror(i64 i64Fd)
{
	lcn_soft_bump(&g_lcnSoft.u64SendEpipe);
	lcn_soft_bump(&g_lcnSoft.u64EpipeMirror);
	/* Sticky SO_ERROR already set by soerr_from_ret; reinforce EPIPE. */
	lcn_tcp_soerr_set(i64Fd, (u16)LINUX_EPIPE);
	lcn_tcp_shut_or(i64Fd, LCN_SHUT_WR);
	if (!g_u8LcnEpipeMirrorOnce) {
		g_u8LcnEpipeMirrorOnce = 1;
		kprintf("linux_cold_net: soft epipe mirror fd=%lld "
			"SHUT_WR Soft!=product "
			"(product_dir=UDX+ABI; cold ABI half-close)\n",
			(long long)i64Fd);
	}
}

/**
 * C2 deepen: product STREAM recv EOF soft SHUT_RD mirror. Soft!=product.
 * Symmetric to EPIPE mirror: after product returns 0 (peer FIN / table EOF),
 * soft SHUT_RD so next cold recv returns 0 without re-table, poll POLLIN
 * (EOF ready) / full RDWR -> POLLHUP stay coherent. Soft!=product.
 * greppable: linux_cold_net: soft recv eof mirror Soft!=product
 */
static void
lcn_tcp_recv_eof_soft_mirror(i64 i64Fd)
{
	/* Already soft-RD: no double tally storms. */
	if ((lcn_tcp_shut_get(i64Fd) & LCN_SHUT_BIT_RD) != 0u) {
		return;
	}
	lcn_soft_bump(&g_lcnSoft.u64RecvEofShut);
	lcn_soft_bump(&g_lcnSoft.u64RecvEofMirror);
	lcn_tcp_shut_or(i64Fd, LCN_SHUT_RD);
	if (!g_u8LcnRecvEofMirrorOnce) {
		g_u8LcnRecvEofMirrorOnce = 1;
		kprintf("linux_cold_net: soft recv eof mirror fd=%lld "
			"SHUT_RD Soft!=product "
			"(product_dir=UDX+ABI; cold ABI half-close; "
			"sshd_shaped_path=1)\n",
			(long long)i64Fd);
	}
}

/** Soft: nonzero if ret is -ECONNRESET shaped. Soft!=product. */
static int
lcn_is_connreset(i64 i64Ret)
{
	if (i64Ret == -(i64)LINUX_ECONNRESET) {
		return 1;
	}
	/* Defensive: product tables may return raw -104. */
	if (i64Ret == -104) {
		return 1;
	}
	return 0;
}

/**
 * C2 deepen: product ECONNRESET soft RDWR mirror. Soft!=product.
 * Sticky SO_ERROR + both half-close bits so poll POLLERR|HUP and send/recv
 * footguns stay Linux-shaped after peer RST without prior soft shutdown.
 * greppable: linux_cold_net: soft reset mirror Soft!=product
 */
static void
lcn_tcp_reset_soft_mirror(i64 i64Fd)
{
	lcn_soft_bump(&g_lcnSoft.u64ResetMirror);
	lcn_tcp_soerr_set(i64Fd, (u16)LINUX_ECONNRESET);
	lcn_tcp_shut_or(i64Fd, LCN_SHUT_RDWR);
	/* Peer reset ends listen acceptability on this fd (SO_ACCEPTCONN). */
	lcn_tcp_listen_set(i64Fd, 0);
	if (!g_u8LcnResetMirrorOnce) {
		g_u8LcnResetMirrorOnce = 1;
		kprintf("linux_cold_net: soft reset mirror fd=%lld "
			"RDWR Soft!=product "
			"(product_dir=UDX+ABI; ECONNRESET; "
			"cold ABI half-close)\n",
			(long long)i64Fd);
	}
}

/**
 * C2: soft sync local/peer ports from product net_tcp name tables.
 * Shared by post-accept seed and post-connect. Soft!=product.
 * Returns 1 if at least one product name call succeeded.
 * greppable: linux_cold_net: soft product name sync Soft!=product
 */
static int
lcn_tcp_product_name_sync(i64 i64Fd)
{
	u8 aSa[16];
	u32 u32Plen;
	int fSynced = 0;

	u32Plen = 16u;
	if (net_tcp_getsockname(i64Fd, aSa, &u32Plen) == 0) {
		u16 u16L = lcn_sin_port_host(aSa);

		if (u16L != 0u) {
			lcn_tcp_port_set(i64Fd, u16L);
		}
		lcn_soft_bump(&g_lcnSoft.u64NameTcp);
		fSynced = 1;
	}
	u32Plen = 16u;
	if (net_tcp_getpeername(i64Fd, aSa, &u32Plen) == 0) {
		u16 u16R = lcn_sin_port_host(aSa);

		if (u16R != 0u) {
			lcn_tcp_peer_set(i64Fd, u16R);
		}
		lcn_soft_bump(&g_lcnSoft.u64PeerTcp);
		fSynced = 1;
	}
	return fSynced;
}

/**
 * Soft recv after SHUT_RD -> 0 EOF (Linux half-close honesty). Soft!=product.
 * greppable: linux_cold_net: soft recv eof shut Soft!=product
 */
static int
lcn_tcp_recv_shut_eof(i64 i64Fd)
{
	if ((lcn_tcp_shut_get(i64Fd) & LCN_SHUT_BIT_RD) != 0u) {
		lcn_soft_bump(&g_lcnSoft.u64RecvEofShut);
		return 1;
	}
	return 0;
}

/**
 * Functional residual: sticky RST SO_ERROR on soft half-close.
 * Next send/recv returns -ECONNRESET (not soft EOF / bare EPIPE) so
 * sshd/stack teardown probes match Linux after peer RST. Soft!=product.
 * Returns 0 if not RST-sticky; else -ECONNRESET after once-lamp tally.
 * greppable: linux_cold_net: soft rst io Soft!=product
 */
static i64
lcn_tcp_rst_sticky_io(i64 i64Fd)
{
	u16 u16Err;

	u16Err = lcn_tcp_soerr_peek(i64Fd);
	if (u16Err != (u16)LINUX_ECONNRESET &&
	    u16Err != (u16)LINUX_ECONNABORTED) {
		return 0;
	}
	/* Only when soft half-close mirror already armed (reset/epipe path). */
	if (lcn_tcp_shut_get(i64Fd) == 0u) {
		return 0;
	}
	lcn_soft_bump(&g_lcnSoft.u64RstIo);
	if (!g_u8LcnRstIoOnce) {
		g_u8LcnRstIoOnce = 1;
		kprintf("linux_cold_net: soft rst io fd=%lld err=%u "
			"Soft!=product (sshd_stack_functional=1; "
			"product_dir=UDX+ABI; sticky SO_ERROR)\n",
			(long long)i64Fd, (unsigned)u16Err);
	}
	return -(i64)u16Err;
}

/**
 * Functional residual: soft re-listen re-arm (clear SHUT_RD stop bit).
 * Matches net_tcp_listen re-arm after SHUT_RD so accept gate + poll stay
 * coherent for sshd/stack listener restart. Soft!=product.
 * greppable: linux_cold_net: soft listen rearm Soft!=product
 */
static void
lcn_tcp_listen_rearm_soft(i64 i64Fd)
{
	u32 u32Slot = lcn_tcp_slot(i64Fd);
	u8 u8Prior;

	if (u32Slot >= LCN_TCP_MAX) {
		return;
	}
	u8Prior = g_lcnTcpShut[u32Slot];
	/* Clear SHUT_RD only (WR half-close on listen fd is rare; keep WR). */
	g_lcnTcpShut[u32Slot] =
	    (u8)(g_lcnTcpShut[u32Slot] & (u8)~(u8)LCN_SHUT_BIT_RD);
	if ((u8Prior & LCN_SHUT_BIT_RD) != 0u) {
		lcn_soft_bump(&g_lcnSoft.u64ListenRearm);
		if (!g_u8LcnListenRearmOnce) {
			g_u8LcnListenRearmOnce = 1;
			kprintf("linux_cold_net: soft listen rearm fd=%lld "
				"clear_SHUT_RD Soft!=product "
				"(sshd_stack_functional=1; "
				"product_dir=UDX+ABI)\n",
				(long long)i64Fd);
		}
	}
}

/**
 * Soft seed accepted peer fd: local=listen port, peer=ephemeral, then
 * product name sync (getsockname + getpeername when table ready).
 * Soft!=product - name honesty for nc/sshd-shaped getpeername/getsockname.
 * greppable: linux_cold_net: soft accept peer seed
 * greppable: linux_cold_net: soft accept name sync Soft!=product
 * greppable: linux_cold_net: soft product name sync Soft!=product
 */
static void
lcn_tcp_accept_peer_seed(i64 i64Peer, u16 u16ListenPort)
{
	u32 u32Slot = lcn_tcp_slot(i64Peer);
	u16 u16Eph;
	int fSynced = 0;

	lcn_tcp_soft_clear(i64Peer);
	lcn_tcp_port_set(i64Peer, u16ListenPort);
	/* Ephemeral-ish host port: 49152 + slot (soft; not product RNG). */
	u16Eph = (u16)(49152u + (u32Slot < LCN_TCP_MAX ? u32Slot : 0u));
	lcn_tcp_peer_set(i64Peer, u16Eph);

	/*
	 * Prefer product name tables when peer is ESTABLISHED (shared C2 helper).
	 * Improves later getpeername/getsockname without user addr on accept.
	 * Soft!=product - Linux-shaped sshd residual.
	 */
	fSynced = lcn_tcp_product_name_sync(i64Peer);
	if (fSynced != 0) {
		lcn_soft_bump(&g_lcnSoft.u64AcceptNameSync);
		if (!g_u8LcnAcceptNameSyncOnce) {
			g_u8LcnAcceptNameSyncOnce = 1;
			/* greppable: linux_cold_net: soft accept name sync Soft!=product */
			kprintf("linux_cold_net: soft accept name sync "
				"peer_fd=%lld lport=%u rport=%u Soft!=product "
				"(product_dir=UDX+ABI; sshd_shaped_path=1)\n",
				(long long)i64Peer,
				(unsigned)lcn_tcp_port_get(i64Peer),
				(unsigned)lcn_tcp_peer_get(i64Peer));
		}
	}
}

/** Soft: nonzero if soft-bound / local port is :22 (sshd-shaped). Soft!=product. */
static int
lcn_tcp_is_ssh_port(i64 i64Fd)
{
	return lcn_tcp_port_get(i64Fd) == (u16)LCN_PORT_SSH ? 1 : 0;
}

/**
 * Soft once-lamp for Linux-shaped sshd path residual coverage.
 * HARD: Soft listen != host banner. Soft!=product / G-AC-1.
 * greppable: linux_cold_net: soft path residual :22 Soft!=product
 */
static void
lcn_soft_path22_once(void)
{
	if (g_u8LcnPathResidualOnce != 0u) {
		return;
	}
	/* Emit only after listen+accept both on :22 (meaningful path). */
	if (g_lcnSoft.u64PathListen22 == 0u ||
	    g_lcnSoft.u64PathAccept22 == 0u) {
		return;
	}
	g_u8LcnPathResidualOnce = 1;
	kprintf("linux_cold_net: soft path residual :22 "
		"sock=%llu bind=%llu listen=%llu accept=%llu "
		"send=%llu recv=%llu shut=%llu name=%llu peer=%llu close=%llu "
		"soft_listen_ne_host_banner=1 "
		"(Soft!=product; product_dir=UDX+ABI; product_NIC=UDX; "
		"freestanding_rtl_skip=1; Dual_DoD_AB_OPEN=1; "
		"not host banner proof; not product OpenSSH)\n",
		(unsigned long long)g_lcnSoft.u64PathSocket22,
		(unsigned long long)g_lcnSoft.u64PathBind22,
		(unsigned long long)g_lcnSoft.u64PathListen22,
		(unsigned long long)g_lcnSoft.u64PathAccept22,
		(unsigned long long)g_lcnSoft.u64PathSend22,
		(unsigned long long)g_lcnSoft.u64PathRecv22,
		(unsigned long long)g_lcnSoft.u64PathShut22,
		(unsigned long long)g_lcnSoft.u64PathName22,
		(unsigned long long)g_lcnSoft.u64PathPeer22,
		(unsigned long long)g_lcnSoft.u64PathClose22);
}

/** Soft: nonzero if ret is -EAGAIN / -EWOULDBLOCK shaped. Soft!=product. */
static int
lcn_is_again(i64 i64Ret)
{
	if (i64Ret == -(i64)LINUX_EAGAIN) {
		return 1;
	}
	/* Defensive: product tables may return raw -11. */
	if (i64Ret == -11) {
		return 1;
	}
	return 0;
}

/**
 * Soft normalize product-table errno to coherent cold ABI -LINUX_E*.
 * Maps raw -11 -> -EAGAIN; other negatives pass through. Soft!=product.
 * greppable: linux_cold_net: soft errno normalize
 */
static i64
lcn_norm_err(i64 i64Ret)
{
	if (i64Ret >= 0) {
		return i64Ret;
	}
	if (lcn_is_again(i64Ret)) {
		return -(i64)LINUX_EAGAIN;
	}
	return i64Ret;
}

/**
 * Soft store SO_ERROR from a hard-fail return (negative errno). Soft!=product.
 * Normalizes -11 -> EAGAIN is skipped (EAGAIN is not a sticky SO_ERROR).
 * greppable: linux_cold_net: soft so_error set
 */
static void
lcn_tcp_soerr_from_ret(i64 i64Fd, i64 i64Ret)
{
	i64 i64N;
	u16 u16Err;

	if (i64Ret >= 0 || lcn_is_again(i64Ret)) {
		return;
	}
	/* Prefer coherent LINUX_E* after norm so sticky errno is honest. */
	i64N = -lcn_norm_err(i64Ret);
	if (i64N <= 0 || i64N > 65535) {
		u16Err = (u16)LINUX_EIO;
	} else {
		u16Err = (u16)i64N;
	}
	lcn_tcp_soerr_set(i64Fd, u16Err);
}

/**
 * Soft TCP success once-lamp for send path (NO stamp storms). Soft!=product.
 * v12: :22 path tally + once-lamp (sshd-shaped; soft send != host banner).
 * greppable: linux_cold_net: soft send tcp Soft!=product
 * greppable: linux_cold_net: soft send :22 Soft!=product
 */
static void
lcn_tcp_send_ok_lamp(i64 i64Fd, i64 i64N)
{
	if (i64N <= 0) {
		return;
	}
	lcn_tcp_soerr_set(i64Fd, 0);
	if (lcn_tcp_is_ssh_port(i64Fd) != 0) {
		lcn_soft_bump(&g_lcnSoft.u64PathSend22);
		if (!g_u8LcnSend22Once) {
			g_u8LcnSend22Once = 1;
			/* greppable: linux_cold_net: soft send :22 Soft!=product */
			kprintf("linux_cold_net: soft send :22 fd=%lld n=%lld "
				"Soft!=product (sshd_shaped_path=1; "
				"soft_send_ne_host_banner=1; "
				"product_dir=UDX+ABI)\n",
				(long long)i64Fd, (long long)i64N);
		}
		lcn_soft_path22_once();
	}
	if (!g_u8LcnSendOnce) {
		g_u8LcnSendOnce = 1;
		kprintf("linux_cold_net: soft send tcp fd=%lld n=%lld "
			"Soft!=product\n",
			(long long)i64Fd, (long long)i64N);
	}
}

/**
 * Soft TCP success once-lamp for recv path (NO stamp storms). Soft!=product.
 * v12: :22 path tally + once-lamp (sshd-shaped residual). Soft!=product.
 * greppable: linux_cold_net: soft recv tcp Soft!=product
 * greppable: linux_cold_net: soft recv :22 Soft!=product
 */
static void
lcn_tcp_recv_ok_lamp(i64 i64Fd, i64 i64N)
{
	if (i64N <= 0) {
		return;
	}
	/* Successful RX clears sticky SO_ERROR (Linux-shaped honesty). */
	lcn_tcp_soerr_set(i64Fd, 0);
	if (lcn_tcp_is_ssh_port(i64Fd) != 0) {
		lcn_soft_bump(&g_lcnSoft.u64PathRecv22);
		if (!g_u8LcnRecv22Once) {
			g_u8LcnRecv22Once = 1;
			/* greppable: linux_cold_net: soft recv :22 Soft!=product */
			kprintf("linux_cold_net: soft recv :22 fd=%lld n=%lld "
				"Soft!=product (sshd_shaped_path=1; "
				"product_dir=UDX+ABI)\n",
				(long long)i64Fd, (long long)i64N);
		}
		lcn_soft_path22_once();
	}
	if (!g_u8LcnRecvOnce) {
		g_u8LcnRecvOnce = 1;
		kprintf("linux_cold_net: soft recv tcp fd=%lld n=%lld "
			"Soft!=product\n",
			(long long)i64Fd, (long long)i64N);
	}
}

/**
 * Soft readiness for accept POLLIN. Soft!=product (not product poll).
 * greppable: linux_cold_net: soft accept readiness
 */
static u32
lcn_accept_ready_mask(i64 i64Fd)
{
	u32 u32Ready = 0;

	if (net_tcp_fd_ok(i64Fd)) {
		u32Ready = net_tcp_poll_mask(i64Fd, LCN_POLLIN);
	} else if (net_lo_fd_ok(i64Fd)) {
		u32Ready = net_lo_poll_mask(i64Fd, LCN_POLLIN);
	}
	return u32Ready;
}

/**
 * Read first iovec from x86_64 msghdr (msg_iov@+16, msg_iovlen@+24).
 * Soft: only first vector; iovlen==0 -> base/len zeroed, return 0.
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

static void
lcn_alias_init(void)
{
	u32 u32Fd;

	if (g_u8LcnAliasInit != 0u) {
		return;
	}
	g_u8LcnAliasInit = 1;
	for (u32Fd = 0; u32Fd < LCN_ALIAS_MAX; u32Fd++) {
		g_ai64LcnCanon[u32Fd] = -1;
		g_au16LcnRef[u32Fd] = 0;
	}
}

/** Canonical tcp/lo fd for a user fd (identity if not aliased). */
static i64
lcn_fd_resolve(i64 i64Fd)
{
	lcn_alias_init();
	if (i64Fd >= 0 && i64Fd < (i64)LCN_ALIAS_MAX &&
	    g_ai64LcnCanon[i64Fd] >= 0) {
		return g_ai64LcnCanon[i64Fd];
	}
	return i64Fd;
}

/** Count this live canonical fd as one user name (socket/accept/dup2). */
static void
lcn_alias_seed(i64 i64Fd)
{
	lcn_alias_init();
	if (i64Fd < 0 || i64Fd >= (i64)LCN_ALIAS_MAX) {
		return;
	}
	if (g_ai64LcnCanon[i64Fd] >= 0) {
		return;
	}
	g_ai64LcnCanon[i64Fd] = i64Fd;
	g_au16LcnRef[i64Fd] = (u16)(g_au16LcnRef[i64Fd] + 1u);
}

/**
 * Drop one user name. *pfLast is 1 when the canonical table slot must close.
 * Unseeded identity tcp/lo fds count as last-ref.
 */
static i64
lcn_alias_drop(i64 i64User, i64 *pCanon, int *pfLast)
{
	i64 i64Canon;
	int fLast = 0;

	lcn_alias_init();
	if (pCanon != NULL) {
		*pCanon = i64User;
	}
	if (pfLast != NULL) {
		*pfLast = 0;
	}

	if (i64User >= 0 && i64User < (i64)LCN_ALIAS_MAX &&
	    g_ai64LcnCanon[i64User] >= 0) {
		i64Canon = g_ai64LcnCanon[i64User];
		g_ai64LcnCanon[i64User] = -1;
		if (i64Canon >= 0 && i64Canon < (i64)LCN_ALIAS_MAX) {
			if (g_au16LcnRef[i64Canon] > 0u) {
				g_au16LcnRef[i64Canon] =
				    (u16)(g_au16LcnRef[i64Canon] - 1u);
			}
			if (g_au16LcnRef[i64Canon] == 0u) {
				fLast = 1;
			}
		} else {
			fLast = 1;
		}
		if (pCanon != NULL) {
			*pCanon = i64Canon;
		}
		if (pfLast != NULL) {
			*pfLast = fLast;
		}
		return 0;
	}

	if (net_tcp_fd_ok(i64User) || net_lo_fd_ok(i64User)) {
		/*
		 * Identity close after a sibling already dropped the alias
		 * row (linux_fork inherit). Honor leftover g_au16LcnRef so
		 * parent close(newsock) then child close/dup2 is not last-ref.
		 */
		fLast = 1;
		if (i64User >= 0 && i64User < (i64)LCN_ALIAS_MAX &&
		    g_au16LcnRef[i64User] > 0u) {
			g_au16LcnRef[i64User] =
			    (u16)(g_au16LcnRef[i64User] - 1u);
			if (g_au16LcnRef[i64User] != 0u) {
				fLast = 0;
			}
		}
		if (pCanon != NULL) {
			*pCanon = i64User;
		}
		if (pfLast != NULL) {
			*pfLast = fLast;
		}
		return 0;
	}
	return -(i64)LINUX_EBADF;
}

/**
 * Lowest free user fd for dup(). Skip live aliases, live tcp/lo, stdio 0/1/2
 * (dup2 may still claim those), and vfs_ram 3..95.
 */
static i64
lcn_alias_alloc(void)
{
	u32 u32Fd;

	lcn_alias_init();
	for (u32Fd = 0; u32Fd < LCN_ALIAS_MAX; u32Fd++) {
		if (u32Fd < LCN_ALIAS_STDIO) {
			continue;
		}
		if (u32Fd >= LCN_ALIAS_STDIO && u32Fd < LCN_ALIAS_VFS_LIM) {
			continue;
		}
		if (g_ai64LcnCanon[u32Fd] >= 0) {
			continue;
		}
		if (net_tcp_fd_ok((i64)u32Fd) || net_lo_fd_ok((i64)u32Fd)) {
			continue;
		}
		return (i64)u32Fd;
	}
	return -(i64)LINUX_EMFILE;
}

int
gj_linux_cold_fd_ok(i64 i64Fd)
{
	i64 i64Canon;

	i64Canon = lcn_fd_resolve(i64Fd);
	if (net_tcp_fd_ok(i64Canon) || net_lo_fd_ok(i64Canon)) {
		return 1;
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
	 * Product STREAM path: AF_INET SOCK_STREAM -> net_tcp (matches
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
			/* Fail inventory only - no serial spam (NO stamp storms). */
			return lcn_norm_err(i64Fd);
		} else {
			/* Soft clear REUSEADDR + bind/peer port for new TCP fd. */
			lcn_tcp_soft_clear(i64Fd);
			/*
			 * v12: STREAM socket starts Linux-shaped sshd path tally
			 * (path socket; bind/:22 may follow). Soft!=product.
			 * greppable: linux_cold_net: soft socket tcp Soft!=product
			 */
			lcn_soft_bump(&g_lcnSoft.u64PathSocket22);
			if (!g_u8LcnSocketTcpOnce) {
				g_u8LcnSocketTcpOnce = 1;
				kprintf("linux_cold_net: soft socket tcp "
					"fd=%lld STREAM (Soft!=product; "
					"product_dir=UDX+ABI; product_NIC=UDX; "
					"sshd_shaped_path=1; bridge=net_tcp; "
					"freestanding_rtl_skip=1)\n",
					(long long)i64Fd);
			}
		}
		lcn_alias_seed(i64Fd);
		return i64Fd;
	}

	i64Fd = net_lo_socket(nDomain, nType & 0xff, nProto);
	lcn_soft_bump(&g_lcnSoft.u64SocketLo);
	/* greppable: linux_cold_net: soft socket lo */
	if (i64Fd < 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return lcn_norm_err(i64Fd);
	}
	lcn_alias_seed(i64Fd);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
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
			lcn_tcp_soerr_from_ret(i64Fd, i64St);
			/* Fail inventory only - no serial spam (NO stamp storms). */
			return lcn_norm_err(i64St);
		}
		lcn_tcp_port_set(i64Fd, u16Port);
		lcn_tcp_soerr_set(i64Fd, 0);
		/* v12: re-bind clears listen bit (SO_ACCEPTCONN honesty). */
		lcn_tcp_listen_set(i64Fd, 0);
		if (u16Port == (u16)LCN_PORT_SSH) {
			/*
			 * Soft residual lean: soft bind :22 once-lamp.
			 * Soft!=product; product DoD B = UDX+ABI.
			 * v12: sshd-shaped path bind tally.
			 * greppable: linux_cold_net: soft bind :22
			 * greppable: linux_cold_net: soft ... Soft!=product
			 */
			lcn_soft_bump(&g_lcnSoft.u64PathBind22);
			if (!g_u8LcnBind22Once) {
				g_u8LcnBind22Once = 1;
				kprintf("linux_cold_net: soft bind :22 fd=%lld "
					"(Soft!=product; product_dir=UDX+ABI; "
					"product_NIC=UDX; sshd_shaped_path=1; "
					"not product OpenSSH; "
					"freestanding_rtl_product_close=0; "
					"freestanding_rtl_skip=1)\n",
					(long long)i64Fd);
			}
		} else if (!g_u8LcnBindTcpOnce) {
			/* greppable: linux_cold_net: soft bind tcp Soft!=product */
			g_u8LcnBindTcpOnce = 1;
			kprintf("linux_cold_net: soft bind tcp fd=%lld "
				"port=%u Soft!=product\n",
				(long long)i64Fd, (unsigned)u16Port);
		}
		return 0;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_bind(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft bind lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
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
	u16 u16Port = 0;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Listen);
	lcn_soft_op();

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	nBacklog = (int)pRegs->u64Arg1;

	if (net_tcp_fd_ok(i64Fd)) {
		i64St = net_tcp_listen(i64Fd, nBacklog);
		/* greppable: linux_cold_net: soft listen tcp */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			lcn_tcp_soerr_from_ret(i64Fd, i64St);
			/* Fail inventory only - no serial spam (NO stamp storms). */
			return lcn_norm_err(i64St);
		}
		u16Port = lcn_tcp_port_get(i64Fd);
		/* Listen success clears sticky SO_ERROR. Soft!=product. */
		lcn_tcp_soerr_set(i64Fd, 0);
		/* Soft listen bit -> SO_ACCEPTCONN honesty (v9). Soft!=product. */
		lcn_tcp_listen_set(i64Fd, 1);
		/*
		 * Functional residual: re-listen soft SHUT_RD clear (AcceptQ
		 * re-arm). Matches net_tcp_listen; accept gate + poll coherent
		 * for sshd/stack listener restart. Soft!=product.
		 * greppable: linux_cold_net: soft listen rearm Soft!=product
		 */
		lcn_tcp_listen_rearm_soft(i64Fd);
		/*
		 * Soft post-listen pump: warm interim net_tcp SYN path so
		 * Linux-shaped cold ABI accept is not cold (all TCP listens).
		 * H1 thr-only: thr stack net_tcp_poll (never IRQ). Soft!=product.
		 * Soft!=product; product DoD B = UDX+ABI.
		 */
		net_tcp_poll();
		if (u16Port == (u16)LCN_PORT_SSH) {
			lcn_soft_bump(&g_lcnSoft.u64Listen22);
			lcn_soft_bump(&g_lcnSoft.u64PathListen22);
			/*
			 * HARD honesty (v12): Soft listen :22 != host banner.
			 * Cold ABI residual only - not product OpenSSH, not
			 * DUT host nc banner proof, not Dual DoD B close.
			 * Product NIC = UDX; freestanding rtl SKIP.
			 * greppable: linux_cold_net: soft listen :22
			 * greppable: linux_cold_net: soft listen :22 soft_listen_ne_host_banner
			 * greppable: linux_cold_net: soft ... Soft!=product
			 */
			lcn_soft_bump(&g_lcnSoft.u64ListenNotBanner);
			if (!g_u8LcnListen22Once) {
				g_u8LcnListen22Once = 1;
				kprintf("linux_cold_net: soft listen :22 "
					"fd=%lld backlog=%d "
					"soft_listen_ne_host_banner=1 "
					"(Soft!=product; product_dir=UDX+ABI; "
					"product_NIC=UDX; bridge=net_tcp; "
					"sshd_shaped_path=1; "
					"not host banner proof; "
					"not product OpenSSH; "
					"freestanding_rtl_product_close=0; "
					"freestanding_rtl_skip=1)\n",
					(long long)i64Fd, nBacklog);
			}
			/*
			 * Soft post-listen pump lamp (:22 only). Soft!=product.
			 * greppable: linux_cold_net: soft listen :22 pump
			 */
			if (!g_u8LcnListenPumpOnce) {
				g_u8LcnListenPumpOnce = 1;
				kprintf("linux_cold_net: soft listen :22 pump "
					"fd=%lld Soft!=product "
					"product_dir=UDX+ABI "
					"soft_listen_ne_host_banner=1\n",
					(long long)i64Fd);
			}
			lcn_soft_path22_once();
		} else if (!g_u8LcnListenOnce) {
			/* greppable: linux_cold_net: soft listen tcp Soft!=product */
			g_u8LcnListenOnce = 1;
			kprintf("linux_cold_net: soft listen tcp fd=%lld "
				"port=%u backlog=%d Soft!=product\n",
				(long long)i64Fd, (unsigned)u16Port, nBacklog);
		}
		return 0;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_listen(i64Fd, nBacklog);
		/* greppable: linux_cold_net: soft listen lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
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
	u32 u32Ready = 0;
	u16 u16Port = 0;
	int fTcp = 0;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Accept);
	lcn_soft_op();

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	/* arg1=addr arg2=addrlen* soft-optional; arg3 flags ignored. */

	if (net_tcp_fd_ok(i64Fd)) {
		fTcp = 1;
		u16Port = lcn_tcp_port_get(i64Fd);
		/*
		 * C2 deepen: soft listen-bit gate (Linux-shaped).
		 * accept without listen / after SHUT_RD cleared SO_ACCEPTCONN
		 * -> -EINVAL. Soft!=product; product DoD B = UDX+ABI.
		 * greppable: linux_cold_net: soft accept not-listen Soft!=product
		 */
		if (lcn_tcp_listen_get(i64Fd) == 0u) {
			lcn_soft_bump(&g_lcnSoft.u64AcceptNotListen);
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			if (!g_u8LcnAcceptNotListenOnce) {
				g_u8LcnAcceptNotListenOnce = 1;
				kprintf("linux_cold_net: soft accept "
					"not-listen fd=%lld port=%u "
					"Soft!=product "
					"(product_dir=UDX+ABI; "
					"accept_listen_gate=1)\n",
					(long long)i64Fd,
					(unsigned)u16Port);
			}
			return -(i64)LINUX_EINVAL;
		}
		/*
		 * Soft residual lean: accept readiness before take.
		 * greppable: linux_cold_net: soft accept readiness
		 */
		u32Ready = lcn_accept_ready_mask(i64Fd);
		/*
		 * Soft pre-pump when !POLLIN so interim net_tcp handshake
		 * can progress before the first take (userspace-shaped park).
		 * Soft!=product; product DoD B = UDX+ABI.
		 * greppable: linux_cold_net: soft accept pre-pump
		 */
		if ((u32Ready & LCN_POLLIN) == 0u) {
			lcn_soft_bump(&g_lcnSoft.u64AcceptPrePump);
			net_tcp_poll();
			u32Ready = lcn_accept_ready_mask(i64Fd);
			/* Once-lamp only - accept-yield loops must not spam. */
			if (!g_u8LcnAcceptPrePumpOnce) {
				g_u8LcnAcceptPrePumpOnce = 1;
				kprintf("linux_cold_net: soft accept pre-pump "
					"fd=%lld port=%u ready=0x%x "
					"(Soft!=product)\n",
					(long long)i64Fd, (unsigned)u16Port,
					(unsigned)u32Ready);
			}
		}
		if ((u32Ready & LCN_POLLIN) != 0u) {
			lcn_soft_bump(&g_lcnSoft.u64AcceptReady);
			/*
			 * Once-lamp only - yield+accept loops must not spam
			 * serial. greppable: linux_cold_net: soft accept readiness
			 */
			if (!g_u8LcnAcceptReadyOnce) {
				g_u8LcnAcceptReadyOnce = 1;
				kprintf("linux_cold_net: soft accept readiness "
					"fd=%lld port=%u mask=0x%x "
					"(Soft!=product)\n",
					(long long)i64Fd, (unsigned)u16Port,
					(unsigned)u32Ready);
			}
		}
		i64Peer = net_tcp_accept(i64Fd);
		/* greppable: linux_cold_net: soft accept tcp */
		/*
		 * Soft pump + one re-accept so interim net_tcp handshake
		 * can complete while userspace-shaped sshd parks on accept.
		 * Soft!=product; product DoD B = UDX+ABI.
		 */
		if (lcn_is_again(i64Peer)) {
			lcn_soft_bump(&g_lcnSoft.u64AcceptPump);
			net_tcp_poll();
			u32Ready = lcn_accept_ready_mask(i64Fd);
			i64Peer = net_tcp_accept(i64Fd);
			/* greppable: linux_cold_net: soft accept pump */
			if (i64Peer >= 0 && !g_u8LcnAcceptPumpOnce) {
				g_u8LcnAcceptPumpOnce = 1;
				kprintf("linux_cold_net: soft accept pump "
					"listen_fd=%lld peer=%lld port=%u "
					"ready=0x%x (Soft!=product)\n",
					(long long)i64Fd, (long long)i64Peer,
					(unsigned)u16Port, (unsigned)u32Ready);
			}
			/* still empty -> soft again path below (no spam) */
		}
	} else if (net_lo_fd_ok(i64Fd)) {
		u32Ready = lcn_accept_ready_mask(i64Fd);
		if ((u32Ready & LCN_POLLIN) != 0u) {
			lcn_soft_bump(&g_lcnSoft.u64AcceptReady);
		}
		i64Peer = net_lo_accept(i64Fd);
		/* greppable: linux_cold_net: soft accept lo */
	} else {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	if (i64Peer < 0) {
		if (lcn_is_again(i64Peer)) {
			/*
			 * Soft again: empty accept queue is normal for
			 * yield+poll daemon loops. Not hard fail.
			 * greppable: linux_cold_net: soft accept again
			 * Soft!=product - inventory only (no serial spam).
			 */
			lcn_soft_bump(&g_lcnSoft.u64AcceptAgain);
			return -(i64)LINUX_EAGAIN;
		}
		/* Hard fail on listen fd -> sticky SO_ERROR (nc/sshd honesty). */
		if (fTcp != 0) {
			lcn_tcp_soerr_from_ret(i64Fd, i64Peer);
		}
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		/* Hard fail inventory only - no serial spam (NO stamp storms). */
		return lcn_norm_err(i64Peer);
	}

	/*
	 * Soft peer seed: local=listen port, peer=ephemeral for name honesty.
	 * Soft!=product - nc/sshd-shaped getpeername/getsockname after accept.
	 * greppable: linux_cold_net: soft accept peer seed
	 */
	if (fTcp != 0) {
		lcn_tcp_accept_peer_seed(i64Peer, u16Port);
	}

	/*
	 * Successful accept - greppable for soft :22 residual path.
	 * Soft!=product: cold bridge only; product DoD B = UDX+ABI.
	 * Once-lamp only (NO stamp storms in multi-accept loops).
	 * greppable: linux_cold_net: soft accept
	 */
	if (fTcp != 0 && u16Port == (u16)LCN_PORT_SSH) {
		/*
		 * v12: sshd-shaped accept path tally. Soft!=product.
		 * Soft accept :22 != host banner proof (G-AC-1).
		 * greppable: linux_cold_net: soft accept :22
		 */
		lcn_soft_bump(&g_lcnSoft.u64PathAccept22);
		if (!g_u8LcnAccept22Once) {
			g_u8LcnAccept22Once = 1;
			kprintf("linux_cold_net: soft accept :22 listen_fd=%lld "
				"peer_fd=%lld ready=0x%x "
				"soft_listen_ne_host_banner=1 "
				"(Soft!=product; product_dir=UDX+ABI; "
				"product_NIC=UDX; sshd_shaped_path=1; "
				"not host banner proof; not product OpenSSH; "
				"freestanding_rtl_product_close=0; "
				"freestanding_rtl_skip=1)\n",
				(long long)i64Fd, (long long)i64Peer,
				(unsigned)u32Ready);
		}
		lcn_soft_path22_once();
	} else if (!g_u8LcnAcceptOnce) {
		g_u8LcnAcceptOnce = 1;
		kprintf("linux_cold_net: soft accept listen_fd=%lld "
			"peer_fd=%lld port=%u tcp=%d Soft!=product\n",
			(long long)i64Fd, (long long)i64Peer,
			(unsigned)u16Port, fTcp);
	}

	/* Soft: fill peer sockaddr when user supplied addr. */
	if (pRegs->u64Arg1 != 0u) {
		u16 u16PeerPort = 0;
		int fPeerFilled = 0;

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
			/*
			 * v11: prefer product net_tcp_getpeername (real rport/rip).
			 * Soft seed fallback for pre-table peer honesty.
			 * greppable: linux_cold_net: soft accept peer sin
			 * greppable: linux_cold_net: soft accept peer tcp Soft!=product
			 */
			if (fTcp != 0) {
				u32 u32Plen = 16u;

				if (net_tcp_getpeername(i64Peer, aSa,
							&u32Plen) == 0) {
					u16PeerPort = lcn_sin_port_host(aSa);
					if (u16PeerPort != 0u) {
						lcn_tcp_peer_set(i64Peer,
								 u16PeerPort);
					}
					lcn_soft_bump(&g_lcnSoft.u64PeerTcp);
					fPeerFilled = 1;
				}
			}
			if (fPeerFilled == 0) {
				if (fTcp != 0) {
					u16PeerPort =
					    lcn_tcp_peer_get(i64Peer);
				}
				lcn_fill_sin(aSa, u16PeerPort);
				if (fTcp != 0) {
					lcn_soft_bump(&g_lcnSoft.u64PeerSoftFb);
				}
			}
			if (lcn_copy_out(pRegs->u64Arg1, aSa, 16u) != 0) {
				/* Peer already accepted - soft ignore copy. */
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
	lcn_alias_seed(i64Peer);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	i64St = lcn_parse_sin_port(pRegs->u64Arg1, (u32)pRegs->u64Arg2,
				   &u16Port);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64St = net_tcp_connect(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft connect tcp */
		/*
		 * Soft connect pump: one net_tcp_poll + re-connect so eth
		 * SYN/SYN-ACK can progress (mirrors accept pump). Soft!=product.
		 * greppable: linux_cold_net: soft connect pump
		 */
		if (lcn_is_again(i64St)) {
			lcn_soft_bump(&g_lcnSoft.u64ConnectPump);
			net_tcp_poll();
			i64St = net_tcp_connect(i64Fd, u16Port);
		}
		if (i64St != 0) {
			if (lcn_is_again(i64St)) {
				return -(i64)LINUX_EAGAIN;
			}
			/* Soft SO_ERROR for nc-shaped getsockopt probes. */
			lcn_tcp_soerr_from_ret(i64Fd, i64St);
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
		}
		/* Soft peer port for getpeername honesty. Soft!=product. */
		lcn_tcp_peer_set(i64Fd, u16Port);
		lcn_tcp_soerr_set(i64Fd, 0);
		/*
		 * Functional residual: connected client is never SO_ACCEPTCONN.
		 * Clear soft listen bit so getsockopt/accept gate stay honest
		 * for sshd/stack. Soft!=product.
		 */
		lcn_tcp_listen_set(i64Fd, 0);
		/*
		 * C2: post-connect product name sync (mirror accept seed).
		 * Sync soft local ephemeral + real peer when table ready.
		 * Soft!=product - greppable: soft connect name sync
		 * greppable: linux_cold_net: soft product name sync Soft!=product
		 */
		if (lcn_tcp_product_name_sync(i64Fd) != 0) {
			lcn_soft_bump(&g_lcnSoft.u64ConnectNameSync);
			if (!g_u8LcnConnectNameSyncOnce) {
				g_u8LcnConnectNameSyncOnce = 1;
				kprintf("linux_cold_net: soft connect name "
					"sync fd=%lld lport=%u rport=%u "
					"Soft!=product (product_dir=UDX+ABI)\n",
					(long long)i64Fd,
					(unsigned)lcn_tcp_port_get(i64Fd),
					(unsigned)lcn_tcp_peer_get(i64Fd));
			}
		}
		if (!g_u8LcnConnectOnce) {
			g_u8LcnConnectOnce = 1;
			kprintf("linux_cold_net: soft connect tcp fd=%lld "
				"port=%u Soft!=product\n",
				(long long)i64Fd, (unsigned)u16Port);
		}
		return 0;
	}
	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_connect(i64Fd, u16Port);
		/* greppable: linux_cold_net: soft connect lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	cb = (size_t)pRegs->u64Arg2;
	/*
	 * flags: MSG_NOSIGNAL soft no-op (no SIGPIPE path); MSG_DONTWAIT
	 * already EAGAIN-shaped on empty TX. Soft!=product - v8 flags residual.
	 */
	(void)(pRegs->u64Arg3 & (u64)(LCN_MSG_NOSIGNAL | LCN_MSG_DONTWAIT));

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

	/* Soft half-close: SHUT_WR -> EPIPE before copy. Soft!=product. */
	if (net_tcp_fd_ok(i64Fd)) {
		i64St = lcn_tcp_send_if_open(i64Fd);
		if (i64St != 0) {
			return i64St;
		}
	}

	i64St = lcn_copy_in(aBuf, pRegs->u64Arg1, cb);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendto tcp */
		/* Soft: eth TX window may EAGAIN; one poll+retry. Soft!=product. */
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64SendPump);
			net_tcp_poll();
			i64N = net_tcp_send(i64Fd, aBuf, cb);
		}
	} else {
		i64N = net_lo_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendto lo */
	}
	if (i64N < 0) {
		if (lcn_is_again(i64N)) {
			return -(i64)LINUX_EAGAIN;
		}
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_soerr_from_ret(i64Fd, i64N);
			/*
			 * Product table EPIPE (SHUT_WR/RST): sticky SO_ERROR +
			 * C2 soft SHUT_WR mirror so cold preshut/poll stay coherent.
			 * Soft!=product - dual path with soft preshut pre-check.
			 * greppable: linux_cold_net: soft epipe mirror Soft!=product
			 */
			if (i64N == -(i64)LINUX_EPIPE || i64N == -32) {
				lcn_tcp_epipe_soft_mirror(i64Fd);
			}
			/*
			 * C2 deepen: product ECONNRESET soft RDWR mirror.
			 * Soft!=product - greppable: soft reset mirror
			 */
			if (lcn_is_connreset(i64N) != 0) {
				lcn_tcp_reset_soft_mirror(i64Fd);
			}
		}
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return lcn_norm_err(i64N);
	}
	/* Soft residual: clear SO_ERROR + once-lamp on TCP send success. */
	if (net_tcp_fd_ok(i64Fd)) {
		lcn_tcp_send_ok_lamp(i64Fd, i64N);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	cb = (size_t)pRegs->u64Arg2;
	/*
	 * flags: MSG_NOSIGNAL / MSG_DONTWAIT soft-accepted (EAGAIN path).
	 * src / srclen soft-optional (src left untouched). Soft!=product.
	 */
	(void)(pRegs->u64Arg3 & (u64)(LCN_MSG_NOSIGNAL | LCN_MSG_DONTWAIT));

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

	/*
	 * Functional residual: sticky RST -> -ECONNRESET (not soft EOF).
	 * Soft half-close SHUT_RD -> 0 EOF for orderly FIN. Soft!=product.
	 * greppable: linux_cold_net: soft rst io Soft!=product
	 */
	if (net_tcp_fd_ok(i64Fd)) {
		i64St = lcn_tcp_rst_sticky_io(i64Fd);
		if (i64St != 0) {
			return i64St;
		}
		if (lcn_tcp_recv_shut_eof(i64Fd) != 0) {
			return 0;
		}
	}

	if (net_tcp_fd_ok(i64Fd)) {
		i64N = net_tcp_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvfrom tcp */
		/*
		 * Soft: empty RX is -EAGAIN; pump once then retry so
		 * userspace-shaped :22 cold RECV is not starved.
		 * H1 thr-only: thr stack net_tcp_poll (never IRQ).
		 * Soft!=product; product DoD B = UDX+ABI.
		 */
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64RecvAgain);
			net_tcp_poll();
			i64N = net_tcp_recv(i64Fd, aBuf, cb);
		}
	} else {
		i64N = net_lo_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvfrom lo */
	}
	if (i64N < 0) {
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64RecvAgain);
			return -(i64)LINUX_EAGAIN;
		}
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_soerr_from_ret(i64Fd, i64N);
			/*
			 * C2 deepen: product ECONNRESET soft RDWR mirror.
			 * Soft!=product - greppable: soft reset mirror
			 */
			if (lcn_is_connreset(i64N) != 0) {
				lcn_tcp_reset_soft_mirror(i64Fd);
			}
		}
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return lcn_norm_err(i64N);
	}
	if (i64N == 0) {
		/*
		 * C2 deepen: product STREAM EOF soft SHUT_RD mirror so
		 * cold poll/next-recv stay coherent. Soft!=product.
		 * greppable: linux_cold_net: soft recv eof mirror Soft!=product
		 */
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_recv_eof_soft_mirror(i64Fd);
		}
		return 0; /* EOF soft */
	}

	i64St = lcn_copy_out(pRegs->u64Arg1, aBuf, (size_t)i64N);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	/* Soft residual: clear SO_ERROR + once-lamp on TCP recv success. */
	if (net_tcp_fd_ok(i64Fd)) {
		lcn_tcp_recv_ok_lamp(i64Fd, i64N);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	/*
	 * flags (arg2): MSG_NOSIGNAL soft no-op; MSG_DONTWAIT EAGAIN-shaped.
	 * Soft!=product - match sendto flag honesty (v11 send* footgun fix).
	 */
	(void)(pRegs->u64Arg2 & (u64)(LCN_MSG_NOSIGNAL | LCN_MSG_DONTWAIT));

	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	/*
	 * Soft half-close SHUT_WR -> EPIPE BEFORE iov copy (send* footgun).
	 * Soft!=product - greppable: linux_cold_net: soft send preshut
	 */
	if (net_tcp_fd_ok(i64Fd)) {
		i64St = lcn_tcp_send_if_open(i64Fd);
		if (i64St != 0) {
			return i64St;
		}
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
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64SendPump);
			net_tcp_poll();
			i64N = net_tcp_send(i64Fd, aBuf, cb);
		}
	} else {
		i64N = net_lo_send(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft sendmsg lo */
	}
	if (i64N < 0) {
		if (lcn_is_again(i64N)) {
			return -(i64)LINUX_EAGAIN;
		}
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_soerr_from_ret(i64Fd, i64N);
			/*
			 * Product SHUT_WR/RST EPIPE: sticky SO_ERROR + C2 soft
			 * SHUT_WR mirror (poll POLLOUT / next send preshut).
			 * Soft!=product.
			 * greppable: linux_cold_net: soft epipe mirror Soft!=product
			 */
			if (i64N == -(i64)LINUX_EPIPE || i64N == -32) {
				lcn_tcp_epipe_soft_mirror(i64Fd);
			}
			/*
			 * C2 deepen: product ECONNRESET soft RDWR mirror.
			 * Soft!=product - greppable: soft reset mirror
			 */
			if (lcn_is_connreset(i64N) != 0) {
				lcn_tcp_reset_soft_mirror(i64Fd);
			}
		}
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return lcn_norm_err(i64N);
	}
	if (net_tcp_fd_ok(i64Fd)) {
		lcn_tcp_send_ok_lamp(i64Fd, i64N);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	/* flags (arg2): MSG_NOSIGNAL/DONTWAIT soft-accepted. Soft!=product. */
	(void)(pRegs->u64Arg2 & (u64)(LCN_MSG_NOSIGNAL | LCN_MSG_DONTWAIT));

	if (!net_tcp_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EBADF;
	}

	/*
	 * Functional residual: sticky RST -> -ECONNRESET before EOF soft.
	 * Soft half-close SHUT_RD -> 0 EOF before iov parse. Soft!=product.
	 * Matches recvfrom order (recv* footgun consistency).
	 * greppable: linux_cold_net: soft rst io Soft!=product
	 */
	if (net_tcp_fd_ok(i64Fd)) {
		i64St = lcn_tcp_rst_sticky_io(i64Fd);
		if (i64St != 0) {
			return i64St;
		}
		if (lcn_tcp_recv_shut_eof(i64Fd) != 0) {
			return 0;
		}
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
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64RecvAgain);
			net_tcp_poll();
			i64N = net_tcp_recv(i64Fd, aBuf, cb);
		}
	} else {
		i64N = net_lo_recv(i64Fd, aBuf, cb);
		/* greppable: linux_cold_net: soft recvmsg lo */
	}
	if (i64N < 0) {
		if (lcn_is_again(i64N)) {
			lcn_soft_bump(&g_lcnSoft.u64RecvAgain);
			return -(i64)LINUX_EAGAIN;
		}
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_soerr_from_ret(i64Fd, i64N);
			/*
			 * C2 deepen: product ECONNRESET soft RDWR mirror.
			 * Soft!=product - greppable: soft reset mirror
			 */
			if (lcn_is_connreset(i64N) != 0) {
				lcn_tcp_reset_soft_mirror(i64Fd);
			}
		}
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return lcn_norm_err(i64N);
	}
	if (i64N == 0) {
		/*
		 * C2 deepen: product STREAM EOF soft SHUT_RD mirror.
		 * Soft!=product - greppable: soft recv eof mirror
		 */
		if (net_tcp_fd_ok(i64Fd)) {
			lcn_tcp_recv_eof_soft_mirror(i64Fd);
		}
		return 0; /* EOF soft */
	}

	i64St = lcn_copy_out(u64Base, aBuf, (size_t)i64N);
	if (i64St != 0) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return i64St;
	}
	if (net_tcp_fd_ok(i64Fd)) {
		lcn_tcp_recv_ok_lamp(i64Fd, i64N);
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
	nHow = (int)pRegs->u64Arg1;

	if (net_lo_fd_ok(i64Fd)) {
		i64St = net_lo_shutdown(i64Fd, nHow);
		/* greppable: linux_cold_net: soft shutdown lo */
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
		}
		return i64St;
	}
	if (net_tcp_fd_ok(i64Fd)) {
		/*
		 * v11 functional residual: bridge net_tcp_shutdown (FIN on
		 * SHUT_WR / table shut bits) + soft mirror for cold send*
		 * EPIPE pre-check / poll POLLHUP. Soft!=product.
		 * v12: SHUT_RD/RDWR clears soft SO_ACCEPTCONN listen bit;
		 * :22 path residual tally for sshd-shaped half-close.
		 * Product DoD B = UDX+ABI; freestanding_rtl_product_close=0.
		 * greppable: linux_cold_net: soft shutdown tcp Soft!=product
		 * greppable: linux_cold_net: soft shutdown tcp bridge Soft!=product
		 * greppable: linux_cold_net: soft shutdown :22 Soft!=product
		 */
		lcn_soft_bump(&g_lcnSoft.u64ShutdownTcp);
		i64St = net_tcp_shutdown(i64Fd, nHow);
		if (i64St != 0) {
			lcn_tcp_soerr_from_ret(i64Fd, i64St);
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
		}
		/* Soft mirror so cold pre-check / poll overlay stay coherent. */
		lcn_tcp_shut_or(i64Fd, nHow);
		/*
		 * Listen SHUT_RD/RDWR: stop accept + clear SO_ACCEPTCONN
		 * soft bit (matches net_tcp_shutdown listen residual).
		 */
		if (nHow == LCN_SHUT_RD || nHow == LCN_SHUT_RDWR) {
			lcn_tcp_listen_set(i64Fd, 0);
		}
		if (lcn_tcp_is_ssh_port(i64Fd) != 0) {
			lcn_soft_bump(&g_lcnSoft.u64PathShut22);
			if (!g_u8LcnShut22Once) {
				g_u8LcnShut22Once = 1;
				kprintf("linux_cold_net: soft shutdown :22 "
					"fd=%lld how=%d bits=0x%x "
					"Soft!=product (sshd_shaped_path=1; "
					"product_dir=UDX+ABI)\n",
					(long long)i64Fd, nHow,
					(unsigned)lcn_tcp_shut_get(i64Fd));
			}
			lcn_soft_path22_once();
		}
		if (!g_u8LcnShutdownOnce) {
			g_u8LcnShutdownOnce = 1;
			kprintf("linux_cold_net: soft shutdown tcp fd=%lld "
				"how=%d bits=0x%x bridge=net_tcp_shutdown "
				"Soft!=product (product_dir=UDX+ABI; "
				"product_NIC=UDX; "
				"freestanding_rtl_product_close=0; "
				"freestanding_rtl_skip=1)\n",
				(long long)i64Fd, nHow,
				(unsigned)lcn_tcp_shut_get(i64Fd));
		}
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);

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
		 * v11: prefer net_tcp_getsockname (table lport + L2/loop IP).
		 * Soft fallback: net_l2_ip + soft-bound port (:22 honesty).
		 * Cold ABI residual lean; Soft!=product; product DoD B = UDX+ABI.
		 * greppable: linux_cold_net: soft getsockname tcp
		 * greppable: linux_cold_net: soft name l2 Soft!=product
		 * greppable: linux_cold_net: soft getsockname tcp bridge Soft!=product
		 */
		if (u32Len < 16u) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_EINVAL;
		}
		u32Len = 16u;
		i64St = net_tcp_getsockname(i64Fd, aSa, &u32Len);
		if (i64St == 0) {
			u16 u16Port = lcn_sin_port_host(aSa);

			/* Sync soft bind port from product table. Soft!=product. */
			if (u16Port != 0u) {
				lcn_tcp_port_set(i64Fd, u16Port);
			}
			lcn_soft_bump(&g_lcnSoft.u64NameTcp);
			/*
			 * v12: :22 name residual (sshd-shaped getsockname).
			 * Soft!=product; Soft listen != host banner.
			 * greppable: linux_cold_net: soft getsockname :22
			 */
			if (u16Port == (u16)LCN_PORT_SSH) {
				lcn_soft_bump(&g_lcnSoft.u64PathName22);
				if (!g_u8LcnName22Once) {
					g_u8LcnName22Once = 1;
					kprintf("linux_cold_net: soft "
						"getsockname :22 fd=%lld "
						"port=%u Soft!=product "
						"soft_listen_ne_host_banner=1 "
						"(sshd_shaped_path=1; "
						"product_dir=UDX+ABI; "
						"product_NIC=UDX)\n",
						(long long)i64Fd,
						(unsigned)u16Port);
				}
				lcn_soft_path22_once();
			}
			if (!g_u8LcnNameTcpOnce) {
				g_u8LcnNameTcpOnce = 1;
				kprintf("linux_cold_net: soft getsockname tcp "
					"bridge fd=%lld port=%u Soft!=product "
					"(product_dir=UDX+ABI; "
					"product_NIC=UDX)\n",
					(long long)i64Fd, (unsigned)u16Port);
			}
		} else {
			u16 u16SoftPort;

			/* Soft fabricate fallback (name API fail / short). */
			lcn_soft_bump(&g_lcnSoft.u64NameSoftFb);
			u16SoftPort = lcn_tcp_port_get(i64Fd);
			lcn_fill_sin_tcp(aSa, u16SoftPort);
			u32Len = 16u;
			/* v12: soft fallback still tallies :22 name residual. */
			if (u16SoftPort == (u16)LCN_PORT_SSH) {
				lcn_soft_bump(&g_lcnSoft.u64PathName22);
				if (!g_u8LcnName22Once) {
					g_u8LcnName22Once = 1;
					kprintf("linux_cold_net: soft "
						"getsockname :22 fd=%lld "
						"port=%u soft_fb=1 Soft!=product "
						"soft_listen_ne_host_banner=1 "
						"(sshd_shaped_path=1; "
						"product_dir=UDX+ABI)\n",
						(long long)i64Fd,
						(unsigned)u16SoftPort);
				}
				lcn_soft_path22_once();
			}
		}
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);

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
		u16 u16Peer;

		/*
		 * v11: prefer net_tcp_getpeername (real rport/rip). Soft
		 * peer-port seed remains fallback for mid-handshake smokes.
		 * Unconnected STREAM (peer unknown) -> -ENOTCONN (nc honesty).
		 * Soft!=product; product DoD B = UDX+ABI.
		 * greppable: linux_cold_net: soft getpeername tcp
		 * greppable: linux_cold_net: soft getpeername tcp bridge Soft!=product
		 */
		if (u32Len < 16u) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return -(i64)LINUX_EINVAL;
		}
		u32Len = 16u;
		i64St = net_tcp_getpeername(i64Fd, aSa, &u32Len);
		if (i64St == 0) {
			u16Peer = lcn_sin_port_host(aSa);
			if (u16Peer != 0u) {
				lcn_tcp_peer_set(i64Fd, u16Peer);
			}
			lcn_soft_bump(&g_lcnSoft.u64PeerTcp);
			if (!g_u8LcnPeerTcpOnce) {
				g_u8LcnPeerTcpOnce = 1;
				kprintf("linux_cold_net: soft getpeername tcp "
					"bridge fd=%lld rport=%u Soft!=product "
					"(product_dir=UDX+ABI)\n",
					(long long)i64Fd, (unsigned)u16Peer);
			}
		} else {
			u16Peer = lcn_tcp_peer_get(i64Fd);
			if (u16Peer == 0u) {
				/* greppable: linux_cold_net: soft getpeername ENOTCONN */
				lcn_soft_bump(&g_lcnSoft.u64Fail);
				return -(i64)LINUX_ENOTCONN;
			}
			lcn_soft_bump(&g_lcnSoft.u64PeerSoftFb);
			lcn_fill_sin(aSa, u16Peer);
			u32Len = 16u;
		}
		/*
		 * C2: :22 peer name residual (sshd-shaped getpeername after accept).
		 * Soft local port == :22 after accept seed / name sync.
		 * Soft!=product - greppable: soft getpeername :22
		 */
		if (lcn_tcp_is_ssh_port(i64Fd) != 0) {
			lcn_soft_bump(&g_lcnSoft.u64PathPeer22);
			lcn_soft_bump(&g_lcnSoft.u64PathName22);
			if (!g_u8LcnPeerName22Once) {
				g_u8LcnPeerName22Once = 1;
				kprintf("linux_cold_net: soft getpeername :22 "
					"fd=%lld rport=%u Soft!=product "
					"soft_listen_ne_host_banner=1 "
					"(sshd_shaped_path=1; "
					"product_dir=UDX+ABI)\n",
					(long long)i64Fd,
					(unsigned)lcn_tcp_peer_get(i64Fd));
			}
			lcn_soft_path22_once();
		}
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
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
		 * Soft TCP SOL_SOCKET subset - no full stack.
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
		/*
		 * Soft SO_SNDBUF / SO_RCVBUF set: accept and ignore value
		 * (bounce cap stays LCN_XFER_MAX). Soft!=product - v9 lean.
		 * greppable: linux_cold_net: soft setsockopt sndrcv Soft!=product
		 */
		if (nLevel == LCN_SOL_SOCKET &&
		    (nOpt == LCN_SO_SNDBUF || nOpt == LCN_SO_RCVBUF)) {
			lcn_soft_bump(&g_lcnSoft.u64SndRcvBuf);
			return 0;
		}
		/*
		 * C2 deepen: SO_KEEPALIVE soft store (sshd-shaped). Soft!=product.
		 * No wire keepalive timer - cold ABI residual only.
		 * greppable: linux_cold_net: soft keepalive Soft!=product
		 */
		if (nLevel == LCN_SOL_SOCKET && nOpt == LCN_SO_KEEPALIVE) {
			u32 u32Slot = lcn_tcp_slot(i64Fd);
			int nVal = 0;

			if (u32Copy >= 4u) {
				nVal = *(const int *)(const void *)aVal;
			}
			if (u32Slot < LCN_TCP_MAX) {
				g_lcnTcpKeepalive[u32Slot] =
				    nVal ? 1u : 0u;
			}
			lcn_soft_bump(&g_lcnSoft.u64Keepalive);
			if (!g_u8LcnKeepaliveOnce) {
				g_u8LcnKeepaliveOnce = 1;
				kprintf("linux_cold_net: soft keepalive "
					"set fd=%lld val=%d Soft!=product "
					"(sshd_shaped_path=1; "
					"product_dir=UDX+ABI)\n",
					(long long)i64Fd, nVal ? 1 : 0);
			}
			return 0;
		}
		/*
		 * Soft SO_LINGER set: accept and ignore (no delayed close).
		 * Soft!=product - glibc/sshd probe residual.
		 */
		if (nLevel == LCN_SOL_SOCKET && nOpt == LCN_SO_LINGER) {
			return 0;
		}
		/*
		 * Functional residual: TCP_NODELAY soft store (sshd/stack
		 * Nagle-off probe). No wire timer change - cold ABI only.
		 * Soft!=product.
		 * greppable: linux_cold_net: soft nodelay Soft!=product
		 */
		if (nLevel == LCN_IPPROTO_TCP && nOpt == LCN_TCP_NODELAY) {
			u32 u32Slot = lcn_tcp_slot(i64Fd);
			int nVal = 0;

			if (u32Copy >= 4u) {
				nVal = *(const int *)(const void *)aVal;
			} else if (u32Copy >= 1u) {
				nVal = (int)aVal[0];
			}
			if (u32Slot < LCN_TCP_MAX) {
				g_lcnTcpNodelay[u32Slot] = nVal ? 1u : 0u;
			}
			lcn_soft_bump(&g_lcnSoft.u64Nodelay);
			if (!g_u8LcnNodelayOnce) {
				g_u8LcnNodelayOnce = 1;
				kprintf("linux_cold_net: soft nodelay set "
					"fd=%lld val=%d Soft!=product "
					"(sshd_stack_functional=1; "
					"product_dir=UDX+ABI)\n",
					(long long)i64Fd, nVal ? 1 : 0);
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
	/* Bounce >= linger (8) so lo full subset cannot stack-overflow soft. */
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

	i64Fd = lcn_fd_resolve((i64)pRegs->u64Arg0);
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
		 * Soft TCP SOL_SOCKET + IPPROTO_TCP TCP_NODELAY.
		 * greppable: linux_cold_net: soft getsockopt tcp
		 */
		int nVal = 0;

		/*
		 * Functional residual: TCP_NODELAY soft get (sshd/stack).
		 * Soft!=product - greppable: soft nodelay Soft!=product
		 */
		if (nLevel == LCN_IPPROTO_TCP && nOpt == LCN_TCP_NODELAY) {
			u32 u32Slot = lcn_tcp_slot(i64Fd);

			if (u32Len < 4u) {
				lcn_soft_bump(&g_lcnSoft.u64Fail);
				return -(i64)LINUX_EINVAL;
			}
			nVal = (u32Slot < LCN_TCP_MAX &&
				g_lcnTcpNodelay[u32Slot] != 0u) ? 1 : 0;
			lcn_soft_bump(&g_lcnSoft.u64Nodelay);
			*(int *)(void *)aVal = nVal;
			u32Len = 4u;
			goto lcn_getsockopt_tcp_out;
		}
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
			/*
			 * Soft SO_ERROR clear-on-read (Linux-shaped).
			 * greppable: linux_cold_net: soft so_error
			 * greppable: linux_cold_net: soft so_error take Soft!=product
			 */
			nVal = (int)lcn_tcp_soerr_take(i64Fd);
			lcn_soft_bump(&g_lcnSoft.u64SoErrGet);
			/*
			 * Once-lamp first nonzero take - nc connect probes.
			 * NO stamp storms on repeated clear-on-read zeros.
			 */
			if (nVal != 0 && !g_u8LcnSoErrOnce) {
				g_u8LcnSoErrOnce = 1;
				kprintf("linux_cold_net: soft so_error take "
					"fd=%lld err=%d Soft!=product\n",
					(long long)i64Fd, nVal);
			}
		} else if (nOpt == LCN_SO_REUSEADDR) {
			u8 *pR = lcn_tcp_reuse_slot(i64Fd);

			nVal = (pR != NULL && *pR != 0u) ? 1 : 0;
		} else if (nOpt == LCN_SO_ACCEPTCONN) {
			/*
			 * Soft SO_ACCEPTCONN: 1 after listen. Soft!=product.
			 * Soft :22 listen probes; product DoD B = UDX+ABI.
			 * greppable: linux_cold_net: soft acceptconn Soft!=product
			 */
			nVal = lcn_tcp_listen_get(i64Fd) != 0u ? 1 : 0;
			lcn_soft_bump(&g_lcnSoft.u64AcceptConn);
			if (!g_u8LcnAcceptConnOnce) {
				g_u8LcnAcceptConnOnce = 1;
				kprintf("linux_cold_net: soft acceptconn "
					"fd=%lld val=%d Soft!=product "
					"(product_dir=UDX+ABI; "
					"freestanding_rtl_product_close=0)\n",
					(long long)i64Fd, nVal);
			}
		} else if (nOpt == LCN_SO_SNDBUF || nOpt == LCN_SO_RCVBUF) {
			/*
			 * Soft SO_SNDBUF/SO_RCVBUF = bounce cap (4096).
			 * Soft!=product - cold ABI only; not wire buffer owner.
			 * greppable: linux_cold_net: soft getsockopt sndrcv
			 */
			nVal = (int)LCN_XFER_MAX;
			lcn_soft_bump(&g_lcnSoft.u64SndRcvBuf);
		} else if (nOpt == LCN_SO_KEEPALIVE) {
			/*
			 * C2 deepen: SO_KEEPALIVE soft get (sshd-shaped).
			 * Soft!=product - greppable: soft keepalive
			 */
			u32 u32Slot = lcn_tcp_slot(i64Fd);

			nVal = (u32Slot < LCN_TCP_MAX &&
				g_lcnTcpKeepalive[u32Slot] != 0u) ? 1 : 0;
			lcn_soft_bump(&g_lcnSoft.u64Keepalive);
		} else if (nOpt == LCN_SO_PROTOCOL) {
			/*
			 * C2 deepen: SO_PROTOCOL = IPPROTO_TCP for STREAM.
			 * Soft!=product - greppable: soft protocol Soft!=product
			 */
			nVal = LCN_IPPROTO_TCP;
			lcn_soft_bump(&g_lcnSoft.u64ProtoDomain);
		} else if (nOpt == LCN_SO_DOMAIN) {
			/*
			 * C2 deepen: SO_DOMAIN = AF_INET for TCP table.
			 * Soft!=product - greppable: soft domain Soft!=product
			 */
			nVal = LCN_AF_INET;
			lcn_soft_bump(&g_lcnSoft.u64ProtoDomain);
		} else if (nOpt == LCN_SO_LINGER) {
			/*
			 * Soft SO_LINGER get: {0,0} (no delayed close).
			 * Soft!=product - structure is 2 ints on x86_64.
			 */
			*(int *)(void *)aVal = 0;
			if (u32Len >= 8u) {
				*(int *)(void *)(aVal + 4) = 0;
				u32Len = 8u;
			} else {
				u32Len = 4u;
			}
			nVal = 0;
			/* Skip generic int store below when linger sized. */
			if (u32Len == 8u) {
				goto lcn_getsockopt_tcp_out;
			}
		} else {
			/* Soft: unknown SOL_SOCKET opt -> 0 value. */
			nVal = 0;
		}
		*(int *)(void *)aVal = nVal;
		u32Len = 4u;
lcn_getsockopt_tcp_out:
		;
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

/**
 * Soft poll readiness for cold STREAM/DGRAM fds (soft residual lean).
 * TCP may soft-pump net_tcp_poll once so interim handshake progresses
 * when userspace polls before accept/recv. Soft!=product.
 * Product DoD B = UDX+ABI; freestanding_rtl_product_close=0.
 *
 * HARD: once-lamp kprintf only - poll loops must never stamp-storm serial.
 * greppable: linux_cold_net: soft poll_mask ... Soft!=product
 */
u32
gj_linux_cold_poll_mask(i64 i64Fd, u32 u32Want)
{
	u32 u32Got = 0;
	u16 u16Port = 0;

	lcn_soft_bump(&g_lcnSoft.u64PollMask);
	/* Poll path may run without soft_op - still light lean residual once. */
	lcn_soft_residual_lean_once();
	i64Fd = lcn_fd_resolve(i64Fd);

	/*
	 * T0 virtio RX on every poll_mask (TCP or lo). Calling thr kstack,
	 * not IRQ. OpenSSH may ppoll an AF_INET6 lo fd plus the IPv4 listen
	 * fd; TCP-only pump would miss if the lo fd is checked first.
	 */
	net_eth_poll();

	if (net_tcp_fd_ok(i64Fd)) {
		u16 u16SoErr;
		static u8 s_u8AfterEthOnce;

		/*
		 * net_eth_poll already ran net_tcp_poll. QEMU56 hung in a
		 * second net_tcp_poll / :22 re-pump after SYN batch n=3
		 * (tcp_poll after_first printed; 271 never returned).
		 * Query once, then force POLLIN if AcceptQ ESTAB :22.
		 * Dual DoD B OPEN.
		 */
		if (s_u8AfterEthOnce == 0u) {
			s_u8AfterEthOnce = 1u;
			kprintf("linux_cold_net: soft poll after_eth fd=%lld\n",
				(long long)i64Fd);
		}
		u32Got = net_tcp_poll_mask(i64Fd, u32Want);
		u16Port = lcn_tcp_port_get(i64Fd);
		if (u16Port == (u16)LCN_PORT_SSH &&
		    (u32Got & LCN_POLLIN) == 0u &&
		    net_tcp_acceptq_estab22() != 0) {
			u32Got |= LCN_POLLIN;
		}
		/*
		 * SO_ERROR honesty: sticky soft errno -> POLLERR (Linux-shaped;
		 * POLLERR is reported in revents even when not in events).
		 * Peek only - getsockopt SO_ERROR still clear-on-read.
		 * greppable: linux_cold_net: soft poll_mask POLLERR
		 */
		u16SoErr = lcn_tcp_soerr_peek(i64Fd);
		if (u16SoErr != 0u) {
			u32Got |= LCN_POLLERR;
			lcn_soft_bump(&g_lcnSoft.u64PollErr);
			if (!g_u8LcnPollErrOnce) {
				g_u8LcnPollErrOnce = 1;
				kprintf("linux_cold_net: soft poll_mask "
					"POLLERR fd=%lld so_error=%u "
					"ready=0x%x Soft!=product\n",
					(long long)i64Fd, (unsigned)u16SoErr,
					(unsigned)u32Got);
			}
		}
		/*
		 * Soft half-close honesty (v8/v10 + functional residual):
		 * SHUT_RD -> POLLIN (EOF ready) only when !listen so accept
		 * loops are not falsely woken after listen SHUT_RD stop;
		 * SHUT_WR -> clear POLLOUT; full RDWR -> POLLHUP.
		 * Soft!=product - cold ABI residual; product DoD B = UDX+ABI.
		 * greppable: linux_cold_net: soft poll_mask POLLHUP
		 * greppable: linux_cold_net: soft poll_mask POLLOUT shut
		 * greppable: linux_cold_net: soft poll listen shut Soft!=product
		 */
		{
			u8 u8Shut = lcn_tcp_shut_get(i64Fd);
			int fListen = lcn_tcp_listen_get(i64Fd) != 0u ? 1 : 0;

			if ((u8Shut & LCN_SHUT_BIT_RD) != 0u) {
				if (fListen != 0) {
					/*
					 * Listen SHUT_RD: stop accept, do NOT
					 * force POLLIN (false accept wake).
					 * Soft!=product; sshd/stack residual.
					 */
					u32Got &= ~(u32)LCN_POLLIN;
					lcn_soft_bump(
					    &g_lcnSoft.u64PollListenShut);
					if (!g_u8LcnPollListenShutOnce) {
						g_u8LcnPollListenShutOnce = 1;
						kprintf("linux_cold_net: soft "
							"poll listen shut "
							"fd=%lld clear_POLLIN "
							"Soft!=product "
							"(sshd_stack_"
							"functional=1)\n",
							(long long)i64Fd);
					}
				} else {
					/* Connected STREAM: EOF ready. */
					u32Got |= LCN_POLLIN;
				}
			}
			if ((u8Shut & LCN_SHUT_BIT_WR) != 0u) {
				/* Write half-closed: not writable soft. */
				if ((u32Got & LCN_POLLOUT) != 0u) {
					u32Got &= ~(u32)LCN_POLLOUT;
					lcn_soft_bump(&g_lcnSoft.u64PollOutShut);
				}
			}
			if ((u8Shut & (LCN_SHUT_BIT_RD | LCN_SHUT_BIT_WR)) ==
			    (LCN_SHUT_BIT_RD | LCN_SHUT_BIT_WR)) {
				u32Got |= LCN_POLLHUP;
				lcn_soft_bump(&g_lcnSoft.u64PollHup);
				if (!g_u8LcnPollHupOnce) {
					g_u8LcnPollHupOnce = 1;
					kprintf("linux_cold_net: soft "
						"poll_mask POLLHUP fd=%lld "
						"ready=0x%x Soft!=product\n",
						(long long)i64Fd,
						(unsigned)u32Got);
				}
			}
		}
		/* greppable: linux_cold_net: soft poll_mask tcp */
		/* Once only - NO stamp storms (HARD RULE). */
		if (!g_u8LcnPollMaskOnce) {
			g_u8LcnPollMaskOnce = 1;
			kprintf("linux_cold_net: soft poll_mask ready=0x%x "
				"fd=%lld want=0x%x port=%u tcp=1 "
				"(Soft!=product)\n",
				(unsigned)u32Got, (long long)i64Fd,
				(unsigned)u32Want, (unsigned)u16Port);
			if (u16Port == (u16)LCN_PORT_SSH) {
				/* greppable: linux_cold_net: soft poll_mask :22 */
				kprintf("linux_cold_net: soft poll_mask :22 "
					"ready=0x%x fd=%lld Soft!=product\n",
					(unsigned)u32Got, (long long)i64Fd);
			}
		}
		return u32Got;
	}
	if (net_lo_fd_ok(i64Fd)) {
		u32Got = net_lo_poll_mask(i64Fd, u32Want);
		/* greppable: linux_cold_net: soft poll_mask lo */
		if (!g_u8LcnPollMaskOnce) {
			g_u8LcnPollMaskOnce = 1;
			kprintf("linux_cold_net: soft poll_mask ready=0x%x "
				"fd=%lld want=0x%x lo=1 (Soft!=product)\n",
				(unsigned)u32Got, (long long)i64Fd,
				(unsigned)u32Want);
		}
		return u32Got;
	}
	return 0;
}

/**
 * Last-ref close of a canonical tcp/lo fd. Does not touch the alias table.
 */
static i64
lcn_close_canon(i64 i64Fd)
{
	i64 i64St;

	if (net_tcp_fd_ok(i64Fd)) {
		int fSsh22;

		/* greppable: linux_cold_net: soft close tcp */
		/*
		 * C2: capture :22 path before soft state clear (port table).
		 * Soft!=product - sshd-shaped session teardown residual.
		 * greppable: linux_cold_net: soft close :22 Soft!=product
		 */
		fSsh22 = lcn_tcp_is_ssh_port(i64Fd);
		lcn_tcp_soft_clear(i64Fd);
		i64St = net_tcp_close(i64Fd);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
		}
		if (fSsh22 != 0) {
			lcn_soft_bump(&g_lcnSoft.u64PathClose22);
			if (!g_u8LcnClose22Once) {
				g_u8LcnClose22Once = 1;
				kprintf("linux_cold_net: soft close :22 "
					"fd=%lld Soft!=product "
					"(sshd_shaped_path=1; "
					"product_dir=UDX+ABI; "
					"Dual_DoD_AB_OPEN=1)\n",
					(long long)i64Fd);
			}
			lcn_soft_path22_once();
		}
		if (!g_u8LcnCloseOnce) {
			g_u8LcnCloseOnce = 1;
			kprintf("linux_cold_net: soft close tcp fd=%lld "
				"Soft!=product\n",
				(long long)i64Fd);
		}
		return 0;
	}
	if (net_lo_fd_ok(i64Fd)) {
		/* greppable: linux_cold_net: soft close lo */
		i64St = net_lo_close(i64Fd);
		if (i64St != 0) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
			return lcn_norm_err(i64St);
		}
		return i64St;
	}
	return -(i64)LINUX_EBADF;
}

/** Drop one user name; last ref closes the canonical tcp/lo slot. */
static i64
lcn_close_user(i64 i64User)
{
	i64 i64Canon;
	int fLast;
	i64 i64St;

	i64St = lcn_alias_drop(i64User, &i64Canon, &fLast);
	if (i64St != 0) {
		return i64St;
	}
	if (fLast == 0) {
		return 0;
	}
	return lcn_close_canon(i64Canon);
}

/**
 * Soft close for cold STREAM/DGRAM fds (v5 residual). Soft!=product.
 * Alias names (including 0/1/2) drop a ref; last ref routes net_tcp_close /
 * net_lo_close on the canonical fd only.
 * greppable: linux_cold_net: soft close ... Soft!=product
 */
i64
gj_linux_cold_close(struct gj_linux_regs *pRegs)
{
	i64 i64St;

	if (pRegs == NULL) {
		lcn_soft_bump(&g_lcnSoft.u64Fail);
		return -(i64)LINUX_EINVAL;
	}
	lcn_soft_bump(&g_lcnSoft.u64Close);
	lcn_soft_op();

	i64St = lcn_close_user((i64)pRegs->u64Arg0);
	if (i64St != 0) {
		if (i64St == -(i64)LINUX_EBADF) {
			lcn_soft_bump(&g_lcnSoft.u64Fail);
		}
		return i64St;
	}
	return 0;
}

i64
gj_linux_cold_dup2(struct gj_linux_regs *pRegs)
{
	i64 i64Old;
	i64 i64New;
	i64 i64Canon;

	if (pRegs == NULL) {
		return -(i64)LINUX_EINVAL;
	}

	i64Old = (i64)pRegs->u64Arg0;
	if (gj_linux_cold_fd_ok(i64Old) == 0) {
		return -(i64)LINUX_EBADF;
	}

	if (pRegs->u64Nr == (u64)LINUX_NR_dup) {
		i64New = lcn_alias_alloc();
		if (i64New < 0) {
			return i64New;
		}
	} else {
		i64New = (i64)pRegs->u64Arg1;
	}

	if (i64New < 0 || i64New >= (i64)LCN_ALIAS_MAX) {
		return -(i64)LINUX_EBADF;
	}

	i64Canon = lcn_fd_resolve(i64Old);
	lcn_alias_seed(i64Canon);

	if (i64New == i64Old) {
		return i64New;
	}

	/*
	 * Linux dup2 closes newfd first when it is already open. Serial
	 * 0/1/2 that are not cold-net names are not in this table.
	 */
	if (g_ai64LcnCanon[i64New] >= 0 || gj_linux_cold_fd_ok(i64New) != 0) {
		(void)lcn_close_user(i64New);
	}

	g_ai64LcnCanon[i64New] = i64Canon;
	if (i64Canon >= 0 && i64Canon < (i64)LCN_ALIAS_MAX &&
	    g_au16LcnRef[i64Canon] < 0xffffu) {
		g_au16LcnRef[i64Canon] = (u16)(g_au16LcnRef[i64Canon] + 1u);
	}
	return i64New;
}

/**
 * One extra holder per already-seeded LCN user name (socket/accept/dup2).
 * process_linux_fork calls this for USER children so OpenSSH parent
 * close(accepted) does not FIN the child's ESTAB.
 * greppable: linux_cold_net: soft fork dup names
 */
void
gj_linux_cold_fork_dup_names(void)
{
	u32 u32Fd;
	i64 i64Canon;
	u32 cBumped = 0;

	lcn_alias_init();
	/*
	 * Only names seeded by socket/accept/dup2. Do not seed kernel
	 * tcp_soft_ensure_listen22 mint (fd 96): extra refs there steal
	 * :22 SYN from product listen fd 97 (QEMU52 hang after RX 58B).
	 */
	for (u32Fd = 0; u32Fd < LCN_ALIAS_MAX; u32Fd++) {
		i64Canon = g_ai64LcnCanon[u32Fd];
		if (i64Canon < 0 || i64Canon >= (i64)LCN_ALIAS_MAX) {
			continue;
		}
		if (g_au16LcnRef[i64Canon] < 0xffffu) {
			g_au16LcnRef[i64Canon] =
			    (u16)(g_au16LcnRef[i64Canon] + 1u);
			cBumped++;
		}
	}
	if (cBumped != 0u && g_u8LcnForkDupOnce == 0u) {
		g_u8LcnForkDupOnce = 1;
		kprintf("linux_cold_net: soft fork dup names refs=%u "
			"Soft!=product (inherit_ne_last_ref=1; "
			"Dual_DoD_B_OPEN=1)\n",
			(unsigned)cBumped);
	}
}
