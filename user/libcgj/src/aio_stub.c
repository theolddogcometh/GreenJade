/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX AIO — synchronous freestanding fill-in (immediate pread/pwrite).
 * Completes in the calling thread; no worker pool. Desktop link surface.
 * Soft deepen: sigevent notify (NONE/THREAD), O_DSYNC→fdatasync, cancel-by-fd,
 * zero-length ops, stricter validation, idle/inflight state machine.
 */
#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <unistd.h>

/* Store completion in aiocb.__pad: int err; int state; ssize_t ret; */
enum { AIO_ST_IDLE = 0, AIO_ST_DONE = 1, AIO_ST_INFLIGHT = 2 };

struct aio_slot {
	int err;
	int state;
	ssize_t ret;
};

static struct aio_slot *
slot_of(struct aiocb *pCb)
{
	return (struct aio_slot *)(void *)pCb->__pad;
}

static const struct aio_slot *
slot_of_c(const struct aiocb *pCb)
{
	return (const struct aio_slot *)(const void *)pCb->__pad;
}

static void
aio_notify(struct aiocb *pCb)
{
	struct sigevent *pEv;

	if (pCb == NULL) {
		return;
	}
	pEv = &pCb->aio_sigevent;
	if (pEv->sigev_notify == SIGEV_NONE) {
		return;
	}
	if (pEv->sigev_notify == SIGEV_THREAD &&
	    pEv->sigev_notify_function != NULL) {
		/* Soft sync: run notify in-thread after completion. */
		pEv->sigev_notify_function(pEv->sigev_value);
		return;
	}
	/*
	 * SIGEV_SIGNAL / THREAD_ID: freestanding soft path does not raise
	 * signals (avoids killing smokes). Completions remain queryable.
	 */
}

static void
aio_complete(struct aiocb *pCb, int nErr, ssize_t nRet)
{
	struct aio_slot *pS = slot_of(pCb);

	pS->err = nErr;
	pS->ret = nRet;
	pS->state = AIO_ST_DONE;
	aio_notify(pCb);
}

static int
aio_validate_io(struct aiocb *pCb, int fNeedBuf)
{
	if (pCb == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (pCb->aio_fildes < 0) {
		errno = EBADF;
		aio_complete(pCb, EBADF, -1);
		return -1;
	}
	if (fNeedBuf && pCb->aio_buf == NULL && pCb->aio_nbytes != 0) {
		errno = EINVAL;
		aio_complete(pCb, EINVAL, -1);
		return -1;
	}
	if (pCb->aio_offset < 0) {
		errno = EINVAL;
		aio_complete(pCb, EINVAL, -1);
		return -1;
	}
	return 0;
}

int
aio_read(struct aiocb *pCb)
{
	ssize_t n;

	if (aio_validate_io(pCb, 1) != 0) {
		return -1;
	}
	/* Zero-length: complete successfully without I/O. */
	if (pCb->aio_nbytes == 0) {
		aio_complete(pCb, 0, 0);
		return 0;
	}
	slot_of(pCb)->state = AIO_ST_INFLIGHT;
	n = pread(pCb->aio_fildes, (void *)pCb->aio_buf, pCb->aio_nbytes,
		  pCb->aio_offset);
	if (n < 0) {
		aio_complete(pCb, errno ? errno : EIO, -1);
		errno = slot_of(pCb)->err;
		return -1;
	}
	aio_complete(pCb, 0, n);
	return 0;
}

int
aio_write(struct aiocb *pCb)
{
	ssize_t n;

	if (aio_validate_io(pCb, 1) != 0) {
		return -1;
	}
	if (pCb->aio_nbytes == 0) {
		aio_complete(pCb, 0, 0);
		return 0;
	}
	slot_of(pCb)->state = AIO_ST_INFLIGHT;
	n = pwrite(pCb->aio_fildes, (const void *)pCb->aio_buf, pCb->aio_nbytes,
		   pCb->aio_offset);
	if (n < 0) {
		aio_complete(pCb, errno ? errno : EIO, -1);
		errno = slot_of(pCb)->err;
		return -1;
	}
	aio_complete(pCb, 0, n);
	return 0;
}

int
aio_error(const struct aiocb *pCb)
{
	const struct aio_slot *pS;

	if (pCb == NULL) {
		return EINVAL;
	}
	pS = slot_of_c(pCb);
	if (pS->state == AIO_ST_IDLE) {
		return EINVAL;
	}
	if (pS->state == AIO_ST_INFLIGHT) {
		return EINPROGRESS;
	}
	return pS->err; /* 0 on success */
}

ssize_t
aio_return(struct aiocb *pCb)
{
	struct aio_slot *pS;

	if (pCb == NULL) {
		errno = EINVAL;
		return -1;
	}
	pS = slot_of(pCb);
	if (pS->state != AIO_ST_DONE) {
		errno = EINVAL;
		return -1;
	}
	/* After return, mark idle (glibc: subsequent aio_return is undefined). */
	pS->state = AIO_ST_IDLE;
	if (pS->err != 0) {
		errno = pS->err;
		return -1;
	}
	return pS->ret;
}

int
aio_cancel(int nFd, struct aiocb *pCb)
{
	/*
	 * Sync AIO: ops finish before submit returns, so nothing is
	 * cancelable. pCb==NULL means "all on nFd" → ALLDONE.
	 */
	if (pCb == NULL) {
		(void)nFd;
		return AIO_ALLDONE;
	}
	if (nFd >= 0 && pCb->aio_fildes != nFd) {
		/* Not matching this fd — nothing to cancel for this cb. */
		return AIO_ALLDONE;
	}
	if (slot_of(pCb)->state == AIO_ST_INFLIGHT) {
		return AIO_NOTCANCELED;
	}
	return AIO_ALLDONE;
}

int
aio_suspend(const struct aiocb *const pList[], int nEnt,
	    const struct timespec *pTimeout)
{
	int i;
	int fAny = 0;
	int fDone = 0;

	(void)pTimeout; /* Sync path: no wait; timeout unused. */
	if (pList == NULL || nEnt <= 0) {
		errno = EINVAL;
		return -1;
	}
	for (i = 0; i < nEnt; i++) {
		const struct aiocb *pCb = pList[i];
		int nSt;

		if (pCb == NULL) {
			continue;
		}
		fAny = 1;
		nSt = slot_of_c(pCb)->state;
		if (nSt == AIO_ST_DONE) {
			fDone = 1;
		}
		if (nSt == AIO_ST_INFLIGHT) {
			/* Soft sync should not leave inflight; treat as done path. */
			errno = EINPROGRESS;
			return -1;
		}
	}
	if (!fAny) {
		errno = EINVAL;
		return -1;
	}
	if (fDone) {
		return 0;
	}
	/* All idle (never submitted or already reaped): like timeout. */
	errno = EAGAIN;
	return -1;
}

int
aio_fsync(int nOp, struct aiocb *pCb)
{
	int n;

	if (pCb == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (pCb->aio_fildes < 0) {
		errno = EBADF;
		aio_complete(pCb, EBADF, -1);
		return -1;
	}
	slot_of(pCb)->state = AIO_ST_INFLIGHT;
	/*
	 * O_DSYNC → fdatasync when available; anything else → fsync.
	 * (aio.h / fcntl: O_DSYNC may be 0 if header omits it.)
	 */
#if defined(O_DSYNC) && (O_DSYNC != 0)
	if (nOp == O_DSYNC) {
		n = fdatasync(pCb->aio_fildes);
	} else
#endif
	{
		(void)nOp;
		n = fsync(pCb->aio_fildes);
	}
	if (n != 0) {
		aio_complete(pCb, errno ? errno : EIO, -1);
		return -1;
	}
	aio_complete(pCb, 0, 0);
	return 0;
}

void
aio_init(const struct aioinit *pInit)
{
	/*
	 * Soft: no thread pool. Accept and ignore tuning (threads/num/idle).
	 * Documented no-op so apps that call aio_init() still link and run.
	 */
	(void)pInit;
}

int
aio_read64(struct aiocb *pCb)
{
	return aio_read(pCb);
}

int
aio_write64(struct aiocb *pCb)
{
	return aio_write(pCb);
}

int
aio_error64(const struct aiocb *pCb)
{
	return aio_error(pCb);
}

ssize_t
aio_return64(struct aiocb *pCb)
{
	return aio_return(pCb);
}

int
aio_cancel64(int nFd, struct aiocb *pCb)
{
	return aio_cancel(nFd, pCb);
}

int
aio_suspend64(const struct aiocb *const pList[], int nEnt,
	      const struct timespec *pTimeout)
{
	return aio_suspend(pList, nEnt, pTimeout);
}

int
aio_fsync64(int nOp, struct aiocb *pCb)
{
	return aio_fsync(nOp, pCb);
}
