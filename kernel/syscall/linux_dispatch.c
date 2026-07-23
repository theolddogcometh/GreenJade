/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Hybrid Option C: hot table / cold personality (doors) / ENOSYS.
 * Pure C11 clean-room Linux x86_64 surface — no GPL source.
 */
#include <gj/cold_ipc.h>
#include <gj/linux_dispatch.h>
#include <gj/syscall.h>
#include <gj/wow64.h>

typedef i64 (*gj_linux_hot_fn_t)(struct gj_linux_regs *pRegs);

static gj_linux_hot_fn_t g_apfnHot[GJ_LINUX_NR_TABLE];
static u8 g_aPath[GJ_LINUX_NR_TABLE]; /* enum gj_linux_path */

static gj_linux_cold_fn_t g_pfnCold;
static void *g_pColdCtx;

static struct gj_linux_dispatch_stats g_stats;

static void
set_hot(u32 u32Nr, gj_linux_hot_fn_t pfn)
{
    if (u32Nr < GJ_LINUX_NR_TABLE && pfn != NULL) {
        g_apfnHot[u32Nr] = pfn;
        g_aPath[u32Nr] = (u8)GJ_LINUX_PATH_HOT;
    }
}

static void
set_cold(u32 u32Nr)
{
    if (u32Nr < GJ_LINUX_NR_TABLE) {
        g_apfnHot[u32Nr] = NULL;
        g_aPath[u32Nr] = (u8)GJ_LINUX_PATH_COLD;
    }
}

void
gj_linux_dispatch_init(void)
{
    u32 iNr;

    for (iNr = 0; iNr < GJ_LINUX_NR_TABLE; iNr++) {
        g_apfnHot[iNr] = NULL;
        g_aPath[iNr] = (u8)GJ_LINUX_PATH_NONE;
    }
    g_pfnCold = NULL;
    g_pColdCtx = NULL;
    g_stats.u64HotHits = 0;
    g_stats.u64ColdHits = 0;
    g_stats.u64Enosys = 0;

    /* ---- Hot paths (kernel) ------------------------------------------- */
    set_hot(LINUX_NR_write, gj_linux_hot_write);
    set_hot(LINUX_NR_writev, gj_linux_hot_writev);
    set_hot(LINUX_NR_readv, gj_linux_hot_readv);
    set_hot(LINUX_NR_preadv, gj_linux_hot_preadv);
    set_hot(LINUX_NR_pwritev, gj_linux_hot_pwritev);
    set_hot(LINUX_NR_preadv2, gj_linux_hot_preadv);
    set_hot(LINUX_NR_pwritev2, gj_linux_hot_pwritev);
    set_hot(LINUX_NR_pread64, gj_linux_hot_pread64);
    set_hot(LINUX_NR_pwrite64, gj_linux_hot_pwrite64);
    set_hot(LINUX_NR_tkill, gj_linux_hot_tkill);
    set_hot(LINUX_NR_tgkill, gj_linux_hot_tgkill);
    set_hot(LINUX_NR_sigaltstack, gj_linux_hot_sigaltstack);
    set_hot(LINUX_NR_sched_yield, gj_linux_hot_sched_yield);
    set_hot(LINUX_NR_sched_getscheduler, gj_linux_hot_sched_getscheduler);
    set_hot(LINUX_NR_sched_setscheduler, gj_linux_hot_sched_setscheduler);
    set_hot(LINUX_NR_sched_getparam, gj_linux_hot_sched_getparam);
    set_hot(LINUX_NR_sched_setparam, gj_linux_hot_sched_setparam);
    set_hot(LINUX_NR_sched_get_priority_max, gj_linux_hot_sched_get_priority_max);
    set_hot(LINUX_NR_sched_get_priority_min, gj_linux_hot_sched_get_priority_min);
    set_hot(LINUX_NR_sched_setaffinity, gj_linux_hot_sched_setaffinity);
    set_hot(LINUX_NR_sched_getaffinity, gj_linux_hot_sched_getaffinity);
    set_hot(LINUX_NR_sched_setattr, gj_linux_hot_sched_setattr);
    set_hot(LINUX_NR_sched_getattr, gj_linux_hot_sched_getattr);
    set_hot(LINUX_NR_msync, gj_linux_hot_msync);
    set_hot(LINUX_NR_mincore, gj_linux_hot_mincore);
    set_hot(LINUX_NR_getpid, gj_linux_hot_getpid);
    set_hot(LINUX_NR_gettid, gj_linux_hot_gettid);
    set_hot(LINUX_NR_getuid, gj_linux_hot_getuid);
    set_hot(LINUX_NR_getgid, gj_linux_hot_getgid);
    set_hot(LINUX_NR_geteuid, gj_linux_hot_geteuid);
    set_hot(LINUX_NR_getegid, gj_linux_hot_getegid);
    set_hot(LINUX_NR_setuid, gj_linux_hot_setuid);
    set_hot(LINUX_NR_setgid, gj_linux_hot_setgid);
    set_hot(LINUX_NR_setreuid, gj_linux_hot_setreuid);
    set_hot(LINUX_NR_setregid, gj_linux_hot_setregid);
    set_hot(LINUX_NR_setresuid, gj_linux_hot_setresuid);
    set_hot(LINUX_NR_setresgid, gj_linux_hot_setresgid);
    set_hot(LINUX_NR_getresuid, gj_linux_hot_getresuid);
    set_hot(LINUX_NR_getresgid, gj_linux_hot_getresgid);
    set_hot(LINUX_NR_setpgid, gj_linux_hot_setpgid);
    set_hot(LINUX_NR_setsid, gj_linux_hot_setsid);
    set_hot(LINUX_NR_setgroups, gj_linux_hot_setgroups);
    set_hot(LINUX_NR_getgroups, gj_linux_hot_getgroups);
    set_hot(LINUX_NR_getpgrp, gj_linux_hot_getpgrp);
    set_hot(LINUX_NR_getppid, gj_linux_hot_getppid);
    set_hot(LINUX_NR_getsid, gj_linux_hot_getsid);
    set_hot(LINUX_NR_mlock, gj_linux_hot_mlock);
    set_hot(LINUX_NR_munlock, gj_linux_hot_munlock);
    set_hot(LINUX_NR_mlockall, gj_linux_hot_mlockall);
    set_hot(LINUX_NR_munlockall, gj_linux_hot_munlockall);
    set_hot(LINUX_NR_mlock2, gj_linux_hot_mlock);
    set_hot(LINUX_NR_mbind, gj_linux_hot_mbind);
    set_hot(LINUX_NR_set_mempolicy, gj_linux_hot_set_mempolicy);
    set_hot(LINUX_NR_get_mempolicy, gj_linux_hot_get_mempolicy);
    set_hot(LINUX_NR_pkey_mprotect, gj_linux_hot_mprotect);
    set_hot(LINUX_NR_pkey_alloc, gj_linux_hot_pkey_alloc);
    set_hot(LINUX_NR_pkey_free, gj_linux_hot_pkey_free);
    set_hot(LINUX_NR_personality, gj_linux_hot_personality);
    set_hot(LINUX_NR_exit, gj_linux_hot_exit);
    set_hot(LINUX_NR_exit_group, gj_linux_hot_exit_group);
    set_hot(LINUX_NR_brk, gj_linux_hot_brk);
    set_hot(LINUX_NR_mmap, gj_linux_hot_mmap);
    set_hot(LINUX_NR_munmap, gj_linux_hot_munmap);
    set_hot(LINUX_NR_mremap, gj_linux_hot_mremap);
    set_hot(LINUX_NR_mprotect, gj_linux_hot_mprotect);
    set_hot(LINUX_NR_clock_gettime, gj_linux_hot_clock_gettime);
    set_hot(LINUX_NR_clock_getres, gj_linux_hot_clock_getres);
    set_hot(LINUX_NR_clock_nanosleep, gj_linux_hot_clock_nanosleep);
    set_hot(LINUX_NR_clock_settime, gj_linux_hot_clock_settime);
    set_hot(LINUX_NR_clock_adjtime, gj_linux_hot_clock_adjtime);
    set_hot(LINUX_NR_nanosleep, gj_linux_hot_nanosleep);
    set_hot(LINUX_NR_gettimeofday, gj_linux_hot_gettimeofday);
    set_hot(LINUX_NR_settimeofday, gj_linux_hot_settimeofday);
    set_hot(LINUX_NR_time, gj_linux_hot_time);
    set_hot(LINUX_NR_futex, gj_linux_hot_futex);
    set_hot(LINUX_NR_futex_wait, gj_linux_hot_futex_wait2);
    set_hot(LINUX_NR_futex_wake, gj_linux_hot_futex_wake2);
    set_hot(LINUX_NR_futex_requeue, gj_linux_hot_futex);
    set_hot(LINUX_NR_arch_prctl, gj_linux_hot_arch_prctl);
    set_hot(LINUX_NR_set_tid_address, gj_linux_hot_set_tid_address);
    set_hot(LINUX_NR_sysinfo, gj_linux_hot_sysinfo);
    set_hot(LINUX_NR_times, gj_linux_hot_times);
    set_hot(LINUX_NR_getrusage, gj_linux_hot_getrusage);
    set_hot(LINUX_NR_getpriority, gj_linux_hot_getpriority);
    set_hot(LINUX_NR_setpriority, gj_linux_hot_setpriority);
    set_hot(LINUX_NR_uname, gj_linux_hot_uname);
    set_hot(LINUX_NR_getrandom, gj_linux_hot_getrandom);
    set_hot(LINUX_NR_getsockopt, gj_linux_hot_getsockopt);
    set_hot(LINUX_NR_setsockopt, gj_linux_hot_setsockopt);
    set_hot(LINUX_NR_getsockname, gj_linux_hot_getsockname);
    set_hot(LINUX_NR_getpeername, gj_linux_hot_getpeername);
    set_hot(LINUX_NR_shutdown, gj_linux_hot_shutdown);
    set_hot(LINUX_NR_rt_sigaction, gj_linux_hot_rt_sigaction);
    set_hot(LINUX_NR_rt_sigprocmask, gj_linux_hot_rt_sigprocmask);
    set_hot(LINUX_NR_rt_sigreturn, gj_linux_hot_rt_sigreturn);
    set_hot(LINUX_NR_getitimer, gj_linux_hot_getitimer);
    set_hot(LINUX_NR_setitimer, gj_linux_hot_setitimer);
    set_hot(LINUX_NR_alarm, gj_linux_hot_alarm);
    set_hot(LINUX_NR_pause, gj_linux_hot_pause);
    set_hot(LINUX_NR_prctl, gj_linux_hot_prctl);
    set_hot(LINUX_NR_set_robust_list, gj_linux_hot_set_robust_list);
    set_hot(LINUX_NR_get_robust_list, gj_linux_hot_get_robust_list);
    set_hot(LINUX_NR_waitid, gj_linux_hot_waitid);
    set_hot(LINUX_NR_process_vm_readv, gj_linux_hot_process_vm_readv);
    set_hot(LINUX_NR_process_vm_writev, gj_linux_hot_process_vm_writev);
    set_hot(LINUX_NR_membarrier, gj_linux_hot_membarrier);
    set_hot(LINUX_NR_rseq, gj_linux_hot_rseq);
    set_hot(LINUX_NR_capget, gj_linux_hot_capget);
    set_hot(LINUX_NR_capset, gj_linux_hot_capset);
    set_hot(LINUX_NR_getcpu, gj_linux_hot_getcpu);

    /* ---- Cold paths (doors / libprotonrt personality) ------------------ */
    set_cold(LINUX_NR_read);
    set_cold(LINUX_NR_open);
    set_cold(LINUX_NR_close);
    set_cold(LINUX_NR_openat);
    set_cold(LINUX_NR_stat);
    set_cold(LINUX_NR_fstat);
    set_cold(LINUX_NR_newfstatat);
    set_cold(LINUX_NR_statx);
    set_cold(LINUX_NR_statfs);
    set_cold(LINUX_NR_fstatfs);
    set_cold(LINUX_NR_openat2);
    set_cold(LINUX_NR_faccessat2);
    set_cold(LINUX_NR_ioctl);
    set_cold(LINUX_NR_poll);
    set_cold(LINUX_NR_ppoll);
    set_cold(LINUX_NR_epoll_create);
    set_cold(LINUX_NR_epoll_create1);
    set_cold(LINUX_NR_io_setup);
    set_cold(LINUX_NR_io_destroy);
    set_cold(LINUX_NR_io_submit);
    set_cold(LINUX_NR_io_getevents);
    set_cold(LINUX_NR_io_cancel);
    set_cold(LINUX_NR_epoll_ctl);
    set_cold(LINUX_NR_epoll_wait);
    set_cold(LINUX_NR_epoll_pwait);
    set_cold(LINUX_NR_getdents64);
    set_cold(LINUX_NR_memfd_create);
    set_cold(LINUX_NR_socket);
    set_cold(LINUX_NR_connect);
    set_cold(LINUX_NR_bind);
    set_cold(LINUX_NR_listen);
    set_cold(LINUX_NR_accept);
    set_cold(LINUX_NR_accept4);
    set_cold(LINUX_NR_sendfile);
    set_cold(LINUX_NR_fallocate);
    set_cold(LINUX_NR_sendto);
    set_cold(LINUX_NR_recvfrom);
    set_cold(LINUX_NR_sendmsg);
    set_cold(LINUX_NR_recvmsg);
    set_cold(LINUX_NR_sendmmsg);
    set_cold(LINUX_NR_recvmmsg);
    set_cold(LINUX_NR_dup3);
    set_cold(LINUX_NR_clone);
    set_cold(LINUX_NR_clone3);
    set_cold(LINUX_NR_fork);
    set_cold(LINUX_NR_vfork);
    set_cold(LINUX_NR_execve);
    set_cold(LINUX_NR_execveat);
    set_cold(LINUX_NR_pidfd_open);
    set_cold(LINUX_NR_pidfd_send_signal);
    set_cold(LINUX_NR_close_range);
    set_cold(LINUX_NR_fcntl);
    set_cold(LINUX_NR_fsync);
    set_cold(LINUX_NR_fdatasync);
    set_cold(LINUX_NR_sync);
    set_cold(LINUX_NR_syncfs);
    set_cold(LINUX_NR_inotify_init1);
    set_cold(LINUX_NR_inotify_add_watch);
    set_cold(LINUX_NR_inotify_rm_watch);
    set_cold(LINUX_NR_renameat);
    set_cold(LINUX_NR_renameat2);
    set_cold(LINUX_NR_linkat);
    set_cold(LINUX_NR_symlinkat);
    set_cold(LINUX_NR_copy_file_range);
    set_cold(LINUX_NR_io_pgetevents);
    set_cold(LINUX_NR_userfaultfd);
    set_cold(LINUX_NR_seccomp);
    set_cold(LINUX_NR_bpf);
    set_cold(LINUX_NR_keyctl);
    set_cold(LINUX_NR_add_key);
    set_cold(LINUX_NR_request_key);
    set_cold(LINUX_NR_name_to_handle_at);
    set_cold(LINUX_NR_open_by_handle_at);
    /*
     * io_uring stays cold: min ring impl lives in protonrt cold path
     * (io_uring_min.c via protonrt_cold_link). Do not promote to hot.
     */
    set_cold(LINUX_NR_io_uring_setup);
    set_cold(LINUX_NR_io_uring_enter);
    set_cold(LINUX_NR_io_uring_register);
    set_cold(LINUX_NR_open_tree);
    set_cold(LINUX_NR_move_mount);
    set_cold(LINUX_NR_fsopen);
    set_cold(LINUX_NR_fsconfig);
    set_cold(LINUX_NR_fsmount);
    set_cold(LINUX_NR_fspick);
    set_cold(LINUX_NR_mount_setattr);
    set_cold(LINUX_NR_quotactl_fd);
    set_cold(LINUX_NR_pidfd_getfd);
    set_cold(LINUX_NR_process_madvise);
    set_cold(LINUX_NR_epoll_pwait2);
    set_cold(LINUX_NR_fanotify_init);
    set_cold(LINUX_NR_fanotify_mark);
    set_cold(LINUX_NR_kcmp);
    set_cold(LINUX_NR_quotactl);
    set_cold(LINUX_NR_remap_file_pages);
    set_cold(LINUX_NR_restart_syscall);
    set_cold(LINUX_NR_migrate_pages);
    set_cold(LINUX_NR_move_pages);
    set_cold(LINUX_NR_perf_event_open);
    set_cold(LINUX_NR_timer_create);
    set_cold(LINUX_NR_timer_settime);
    set_cold(LINUX_NR_timer_gettime);
    set_cold(LINUX_NR_timer_getoverrun);
    set_cold(LINUX_NR_timer_delete);
    set_cold(LINUX_NR_mq_open);
    set_cold(LINUX_NR_mq_unlink);
    set_cold(LINUX_NR_mq_timedsend);
    set_cold(LINUX_NR_mq_timedreceive);
    set_cold(LINUX_NR_mq_notify);
    set_cold(LINUX_NR_mq_getsetattr);
    set_cold(LINUX_NR_inotify_init);
    set_cold(LINUX_NR_splice);
    set_cold(LINUX_NR_tee);
    set_cold(LINUX_NR_pipe);
    set_cold(LINUX_NR_pipe2);
    set_cold(LINUX_NR_socketpair);
    set_cold(LINUX_NR_eventfd2);
    set_cold(LINUX_NR_eventfd);
    set_cold(LINUX_NR_timerfd_create);
    set_cold(LINUX_NR_timerfd_settime);
    set_cold(LINUX_NR_timerfd_gettime);
    set_cold(LINUX_NR_signalfd4);
    set_cold(LINUX_NR_prlimit64);
    set_cold(LINUX_NR_getrlimit);
    set_cold(LINUX_NR_setrlimit);
    set_cold(LINUX_NR_unshare);
    set_cold(LINUX_NR_setns);
    set_cold(LINUX_NR_chroot);
    set_cold(LINUX_NR_mount);
    set_cold(LINUX_NR_umount2);
    set_cold(LINUX_NR_pivot_root);
    set_cold(LINUX_NR_swapon);
    set_cold(LINUX_NR_swapoff);
    set_cold(LINUX_NR_reboot);
    set_cold(LINUX_NR_sethostname);
    set_cold(LINUX_NR_setdomainname);
    set_cold(LINUX_NR_syslog);
    set_cold(LINUX_NR_ustat);
    set_cold(LINUX_NR_sysfs);
    set_cold(LINUX_NR_bdflush);
    set_cold(LINUX_NR_readahead);
    set_cold(LINUX_NR_sync_file_range);
    set_cold(LINUX_NR_vmsplice);
    set_cold(LINUX_NR_getdents);
    set_cold(LINUX_NR_rt_sigsuspend);
    set_cold(LINUX_NR_rt_sigtimedwait);
    set_cold(LINUX_NR_rt_sigpending);
    set_cold(LINUX_NR_rt_sigqueueinfo);
    set_cold(LINUX_NR_sched_rr_get_interval);
    set_cold(LINUX_NR_landlock_create_ruleset);
    set_cold(LINUX_NR_landlock_add_rule);
    set_cold(LINUX_NR_landlock_restrict_self);
    set_cold(LINUX_NR_memfd_secret);
    set_cold(LINUX_NR_process_mrelease);
    set_cold(LINUX_NR_cachestat);
    set_cold(LINUX_NR_fchmodat2);
    set_cold(LINUX_NR_futex_waitv);
    set_cold(LINUX_NR_set_mempolicy_home_node);
    set_cold(LINUX_NR_map_shadow_stack);
    set_cold(LINUX_NR_madvise);
    set_cold(LINUX_NR_lseek);
    set_cold(LINUX_NR_getcwd);
    set_cold(LINUX_NR_chdir);
    set_cold(LINUX_NR_fchdir);
    set_cold(LINUX_NR_select);
    set_cold(LINUX_NR_pselect6);
    set_cold(LINUX_NR_symlink);
    set_cold(LINUX_NR_readlinkat);
    set_cold(LINUX_NR_fchmodat);
    set_cold(LINUX_NR_getxattr);
    set_cold(LINUX_NR_lgetxattr);
    set_cold(LINUX_NR_fgetxattr);
    set_cold(LINUX_NR_setxattr);
    set_cold(LINUX_NR_lsetxattr);
    set_cold(LINUX_NR_fsetxattr);
    set_cold(LINUX_NR_listxattr);
    set_cold(LINUX_NR_llistxattr);
    set_cold(LINUX_NR_flistxattr);
    set_cold(LINUX_NR_removexattr);
    set_cold(LINUX_NR_lremovexattr);
    set_cold(LINUX_NR_fremovexattr);
    set_cold(LINUX_NR_utime);
    set_cold(LINUX_NR_utimensat);
    set_cold(LINUX_NR_mkdir);
    set_cold(LINUX_NR_mkdirat);
    set_cold(LINUX_NR_rmdir);
    set_cold(LINUX_NR_link);
    set_cold(LINUX_NR_unlink);
    set_cold(LINUX_NR_unlinkat);
    set_cold(LINUX_NR_creat);
    set_cold(LINUX_NR_flock);
    set_cold(LINUX_NR_fchmod);
    set_cold(LINUX_NR_fchown);
    set_cold(LINUX_NR_access);
    set_cold(LINUX_NR_faccessat);
    set_cold(LINUX_NR_readlink);
    set_cold(LINUX_NR_chmod);
    set_cold(LINUX_NR_rename);
    set_cold(LINUX_NR_ftruncate);
    set_cold(LINUX_NR_dup);
    set_cold(LINUX_NR_dup2);
    set_cold(LINUX_NR_wait4);
    set_cold(LINUX_NR_kill);
    set_cold(LINUX_NR_lstat);
}

void
gj_linux_set_cold_handler(gj_linux_cold_fn_t pfn, void *pCtx)
{
    g_pfnCold = pfn;
    g_pColdCtx = pCtx;
}

enum gj_linux_path
gj_linux_classify(u64 u64Nr)
{
    if (u64Nr >= GJ_LINUX_NR_TABLE) {
        return GJ_LINUX_PATH_NONE;
    }
    return (enum gj_linux_path)g_aPath[(u32)u64Nr];
}

void
gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs)
{
    enum gj_linux_path ePath;
    gj_linux_hot_fn_t pfnHot;
    u32 u32Nr;

    if (pRegs == NULL) {
        return;
    }
    pRegs->i64Ret = -LINUX_ENOSYS;

    /* WoW64: map i386 NR → x86_64 NR; zero-extend lower 32 of args */
    if (wow64_enabled()) {
        u32 u32Nr64;

        if (wow64_translate_nr((u32)pRegs->u64Nr, &u32Nr64) == 0) {
            pRegs->u64Nr = u32Nr64;
        }
        pRegs->u64Arg0 &= 0xffffffffull;
        pRegs->u64Arg1 &= 0xffffffffull;
        pRegs->u64Arg2 &= 0xffffffffull;
        pRegs->u64Arg3 &= 0xffffffffull;
        pRegs->u64Arg4 &= 0xffffffffull;
        pRegs->u64Arg5 &= 0xffffffffull;
    }

    if (pRegs->u64Nr >= GJ_LINUX_NR_TABLE) {
        g_stats.u64Enosys++;
        return;
    }
    u32Nr = (u32)pRegs->u64Nr;
    ePath = (enum gj_linux_path)g_aPath[u32Nr];

    if (ePath == GJ_LINUX_PATH_HOT) {
        pfnHot = g_apfnHot[u32Nr];
        if (pfnHot != NULL) {
            pRegs->i64Ret = pfnHot(pRegs);
            /* Hot may return -ENOSYS to defer (e.g. non-anon mmap). */
            if (pRegs->i64Ret != -LINUX_ENOSYS) {
                g_stats.u64HotHits++;
                return;
            }
        }
        /* Fall through to cold personality. */
    }

    if (ePath == GJ_LINUX_PATH_COLD || ePath == GJ_LINUX_PATH_HOT) {
        /* Doors / sync service via cold_ipc (libprotonrt). */
        if (cold_ipc_personality_attached()) {
            pRegs->i64Ret = cold_ipc_submit(pRegs, 0);
            g_stats.u64ColdHits++;
            return;
        }
        /* Legacy direct cold hook (tests / early bring-up). */
        if (g_pfnCold != NULL) {
            pRegs->i64Ret = g_pfnCold(pRegs, g_pColdCtx);
            g_stats.u64ColdHits++;
            return;
        }
        g_stats.u64Enosys++;
        pRegs->i64Ret = -LINUX_ENOSYS;
        return;
    }

    g_stats.u64Enosys++;
    pRegs->i64Ret = -LINUX_ENOSYS;
}

void
gj_linux_dispatch_stats_get(struct gj_linux_dispatch_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    *pOut = g_stats;
}

void
gj_linux_dispatch_stats_reset(void)
{
    g_stats.u64HotHits = 0;
    g_stats.u64ColdHits = 0;
    g_stats.u64Enosys = 0;
}
