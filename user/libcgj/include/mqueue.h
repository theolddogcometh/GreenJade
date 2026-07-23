/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped mqueue.h (subset). Not GNU glibc.
 * Bring-up: mq_* return ENOSYS (kernel also returns ENOSYS).
 */
#pragma once

#include <fcntl.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int mqd_t;

struct mq_attr {
    long mq_flags;
    long mq_maxmsg;
    long mq_msgsize;
    long mq_curmsgs;
};

mqd_t mq_open(const char *szName, int nFlags, ...);
int   mq_close(mqd_t mq);
int   mq_unlink(const char *szName);
int   mq_send(mqd_t mq, const char *pMsg, size_t cb, unsigned uPrio);
int   mq_timedsend(mqd_t mq, const char *pMsg, size_t cb, unsigned uPrio,
                   const struct timespec *pAbs);
ssize_t mq_receive(mqd_t mq, char *pMsg, size_t cb, unsigned *pPrio);
ssize_t mq_timedreceive(mqd_t mq, char *pMsg, size_t cb, unsigned *pPrio,
                        const struct timespec *pAbs);
int mq_notify(mqd_t mq, const struct sigevent *pEv);
int mq_getattr(mqd_t mq, struct mq_attr *pAttr);
int mq_setattr(mqd_t mq, const struct mq_attr *pNew, struct mq_attr *pOld);

#ifdef __cplusplus
}
#endif
