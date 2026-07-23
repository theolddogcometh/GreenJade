/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX AIO — synchronous freestanding fill-in (immediate pread/pwrite).
 * Completes in the calling thread; no worker pool. Desktop link surface.
 */
#include <aio.h>
#include <errno.h>
#include <unistd.h>

/* Store completion in aiocb.__pad: int err; ssize_t ret; int state. */
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
aio_complete(struct aiocb *pCb, int nErr, ssize_t nRet)
{
	struct aio_slot *pS = slot_of(pCb);

	pS->err = nErr;
	pS->ret = nRet;
	pS->state = AIO_ST_DONE;
}

int
aio_read(struct aiocb *pCb)
{
	ssize_t n;

	if (pCb == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (pCb->aio_fildes < 0 || pCb->aio_buf == NULL) {
		errno = EBADF;
		aio_complete(pCb, EBADF, -1);
		return -1;
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

	if (pCb == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (pCb->aio_fildes < 0 || pCb->aio_buf == NULL) {
		errno = EBADF;
		aio_complete(pCb, EBADF, -1);
		return -1;
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
	return pS->err;
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
	if (pS->err != 0) {
		errno = pS->err;
		return -1;
	}
	return pS->ret;
}

int
aio_cancel(int nFd, struct aiocb *pCb)
{
	(void)nFd;
	if (pCb == NULL) {
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

	(void)pTimeout;
	if (pList == NULL || nEnt <= 0) {
		errno = EINVAL;
		return -1;
	}
	/* Sync AIO: all ops already complete or idle. */
	for (i = 0; i < nEnt; i++) {
		if (pList[i] != NULL &&
		    slot_of_c(pList[i])->state == AIO_ST_DONE) {
			return 0;
		}
	}
	errno = EAGAIN;
	return -1;
}

int
aio_fsync(int nOp, struct aiocb *pCb)
{
	int n;

	(void)nOp;
	if (pCb == NULL || pCb->aio_fildes < 0) {
		errno = EBADF;
		if (pCb != NULL) {
			aio_complete(pCb, EBADF, -1);
		}
		return -1;
	}
	n = fsync(pCb->aio_fildes);
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
