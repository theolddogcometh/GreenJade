/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny in-kernel ramdisk for cold personality bring-up (G-PERS cold FS).
 * Product: move to userspace vfsd; this is interim only.
 */
#pragma once

#include <gj/types.h>

void vfs_ram_init(void);

/**
 * Mount virtio-blk (if ready) as /dev/vda — open/read/write/lseek over sectors.
 * Call after virtio_blk_probe. No-op if block device not ready.
 */
void vfs_ram_mount_blk(void);

/**
 * Mount scsi_mid LUN0 as /dev/sda when virtio-scsi ready (READ10 path).
 */
void vfs_ram_mount_scsi(void);

/**
 * Open path (absolute). Returns fd >= 3 or negative Linux errno.
 * fCreate: create empty file if missing (O_CREAT-like).
 */
i64 vfs_ram_open(const char *szPath, int fCreate);

/** Read from fd into kernel buffer. Returns bytes or -errno. */
i64 vfs_ram_read(i64 i64Fd, void *pBuf, size_t cb);

/** Write to fd from kernel buffer. */
i64 vfs_ram_write(i64 i64Fd, const void *pBuf, size_t cb);

/** Close fd. */
i64 vfs_ram_close(i64 i64Fd);

/** Lseek: whence 0=SET 1=CUR 2=END. */
i64 vfs_ram_lseek(i64 i64Fd, i64 i64Off, int nWhence);

/** Non-zero if fd is a live ramdisk fd. */
int vfs_ram_fd_ok(i64 i64Fd);

/**
 * Create a connected pipe pair (socketpair-shaped / pipe2).
 * Writes aFds[0] and aFds[1] (kernel buffer or caller array).
 * Returns 0 or -errno.
 */
i64 vfs_ram_pipe2(i32 *pFds, int nFlags);

/**
 * AF_UNIX socketpair shape: same as pipe2 for bring-up (bidirectional ring).
 * domain/type/protocol ignored except type must be SOCK_STREAM (1).
 */
i64 vfs_ram_socketpair(int nDomain, int nType, int nProtocol, i32 *pFds);

/** eventfd2-shaped counter fd (flags ignored for bring-up). Returns fd or -errno. */
i64 vfs_ram_eventfd2(u32 u32Init, int nFlags);

/**
 * getdents64-shaped directory listing of seeded paths.
 * pBuf receives packed linux_dirent64-like records.
 * Returns bytes written, 0 at end, or -errno.
 * Cursor stored in fd offset (entry index).
 */
i64 vfs_ram_getdents64(i64 i64Fd, void *pBuf, size_t cb);

/** pread/pwrite: positioned R/W without permanently changing offset. */
i64 vfs_ram_pread(i64 i64Fd, void *pBuf, size_t cb, u64 u64Off);
i64 vfs_ram_pwrite(i64 i64Fd, const void *pBuf, size_t cb, u64 u64Off);

/** dup / dup2 — clone fd table entry. */
i64 vfs_ram_dup(i64 i64Fd);
i64 vfs_ram_dup2(i64 i64Old, i64 i64New);

/**
 * readlink: limited known symlinks (/proc/self/exe → /bin/greenjade).
 * Returns bytes written (no NUL) or -errno.
 */
i64 vfs_ram_readlink(const char *szPath, char *pBuf, size_t cb);

/**
 * access-shaped: 0 if path exists (F_OK) or is openable; -ENOENT otherwise.
 * mode bits ignored for bring-up (all files R/W).
 */
i64 vfs_ram_access(const char *szPath, int nMode);

/**
 * Fill a Linux x86_64 struct stat (144-byte public layout) for an open fd.
 * Returns 0 or -errno.
 */
i64 vfs_ram_fstat(i64 i64Fd, void *pStat, size_t cbStat);

/** Path-based stat (open-lookup without permanently consuming an fd slot). */
i64 vfs_ram_stat(const char *szPath, void *pStat, size_t cbStat);

/** rename: move path (ram files only). Returns 0 or -errno. */
i64 vfs_ram_rename(const char *szOld, const char *szNew);

/** ftruncate: set RAM file size (zero-fill or clip). */
i64 vfs_ram_ftruncate(i64 i64Fd, i64 i64Len);

/**
 * F_DUPFD-shaped: clone fd to lowest free >= i64Min.
 * Returns new fd or -errno.
 */
i64 vfs_ram_dup_from(i64 i64Fd, i64 i64Min);

/** Copy path of open RAM fd into buffer (NUL-terminated). Returns 0 or -errno. */
i64 vfs_ram_fd_path(i64 i64Fd, char *pBuf, size_t cb);

/**
 * Create a symlink entry (path → target). Limited table for bring-up.
 * Returns 0 or -errno.
 */
i64 vfs_ram_symlink(const char *szTarget, const char *szLink);

/** utimensat-shaped: touch path (create empty if missing when flags allow). */
i64 vfs_ram_utimens(const char *szPath);

/** hard link: new path shares RAM file data. */
i64 vfs_ram_link(const char *szOld, const char *szNew);

/** unlink RAM file or symlink by path. */
i64 vfs_ram_unlink(const char *szPath);

/** rmdir: only empty marker dirs under /tmp for bring-up. */
i64 vfs_ram_rmdir(const char *szPath);

/** fchmod: set mode bits on RAM file (stored for fstat). */
i64 vfs_ram_fchmod(i64 i64Fd, u32 u32Mode);

/** Mark open RAM fd as directory (mkdir path). */
i64 vfs_ram_mark_dir(i64 i64Fd);

/**
 * fallocate-shaped: ensure file size >= offset+len (mode ignored for bring-up).
 */
i64 vfs_ram_fallocate(i64 i64Fd, i64 i64Off, i64 i64Len);

/**
 * sendfile-shaped: copy up to cb bytes from in_fd to out_fd.
 * *pOff advanced on success when non-NULL.
 */
i64 vfs_ram_sendfile(i64 i64Out, i64 i64In, u64 *pOff, size_t cb);

/** epoll_create1 — returns epoll fd or -errno. */
i64 vfs_ram_epoll_create1(int nFlags);

/**
 * epoll_ctl: nOp 1=ADD 2=DEL 3=MOD.
 * u32Events: EPOLLIN=1 EPOLLOUT=4 (public values).
 */
i64 vfs_ram_epoll_ctl(i64 i64Ep, int nOp, i64 i64Fd, u32 u32Events, u64 u64Data);

/**
 * epoll_wait: fill packed {u32 events; u64 data} records (12 bytes each).
 * nTimeout ignored (non-blocking probe). Returns ready count or -errno.
 */
i64 vfs_ram_epoll_wait(i64 i64Ep, void *pEvents, int nMax, int nTimeout);

/** timerfd_create / settime / gettime (software ticks for bring-up). */
i64 vfs_ram_timerfd_create(int nClockid, int nFlags);
i64 vfs_ram_timerfd_settime(i64 i64Fd, int nFlags, u64 u64ValueNsec,
                            u64 u64IntervalNsec);
i64 vfs_ram_timerfd_gettime(i64 i64Fd, u64 *pValueNsec, u64 *pIntervalNsec);

/**
 * signalfd4: i64Fd < 0 creates new; else updates mask.
 * Inject pending via vfs_ram_signalfd_inject.
 */
i64 vfs_ram_signalfd4(i64 i64Fd, u64 u64Mask, int nFlags);
void vfs_ram_signalfd_inject(u32 u32Signo);

/** pidfd_open-shaped: fd carries target pid (query via vfs_ram_pidfd_pid). */
i64 vfs_ram_pidfd_open(u32 u32Pid, int nFlags);
u32 vfs_ram_pidfd_pid(i64 i64Fd);

/** io_uring minimal ring fd (slot index stored in u32File). */
i64 vfs_ram_io_uring_open(u32 u32RingSlot);
/** Returns 1 if fd is io_uring; fills *pSlot with ring slot when non-NULL. */
int vfs_ram_io_uring_fd(i64 i64Fd, u32 *pSlot);

/** inotify_init1 / add_watch / rm_watch (fixed table + event queue). */
i64 vfs_ram_inotify_init1(int nFlags);
i64 vfs_ram_inotify_add_watch(i64 i64Fd, const char *szPath, u32 u32Mask);
i64 vfs_ram_inotify_rm_watch(i64 i64Fd, i32 i32Wd);

/**
 * copy_file_range-shaped: copy cb bytes from in@offIn to out@offOut.
 * Advances *pOffIn / *pOffOut when non-NULL.
 */
i64 vfs_ram_copy_file_range(i64 i64In, u64 *pOffIn, i64 i64Out, u64 *pOffOut,
                            size_t cb);

/**
 * poll readiness mask for an fd (EPOLLIN=1 EPOLLOUT=4 EPOLLERR=8 EPOLLHUP=0x10).
 * Returns ready bits among u32Want, or 0 if not ready / unknown.
 */
u32 vfs_ram_poll_mask(i64 i64Fd, u32 u32Want);
