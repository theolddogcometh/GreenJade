/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/msg.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/msg.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stddef.h>
#include <sys/ipc.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_NOERROR 010000
#define MSG_EXCEPT  020000
#define MSG_COPY    040000

#define MSG_STAT 11
#define MSG_INFO 12

typedef unsigned long msgqnum_t;
typedef unsigned long msglen_t;

struct msqid_ds {
    struct ipc_perm msg_perm;
    time_t          msg_stime;
    time_t          msg_rtime;
    time_t          msg_ctime;
    unsigned long   __msg_cbytes;
    msgqnum_t       msg_qnum;
    msglen_t        msg_qbytes;
    pid_t           msg_lspid;
    pid_t           msg_lrpid;
    unsigned long   __unused1;
    unsigned long   __unused2;
};

struct msgbuf {
    long mtype;
    char mtext[1];
};

int msgget(key_t key, int nMsgflg);
int msgsnd(int nMsqid, const void *pMsgp, size_t cbMsgsz, int nMsgflg);
ssize_t msgrcv(int nMsqid, void *pMsgp, size_t cbMsgsz, long nMsgtyp,
               int nMsgflg);
int msgctl(int nMsqid, int nCmd, struct msqid_ds *pBuf);

#ifdef __cplusplus
}
#endif
