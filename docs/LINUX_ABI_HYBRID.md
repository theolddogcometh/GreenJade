# GreenJade — Hybrid Linux ABI (Option C)

| Field | Value |
|-------|--------|
| **Document** | Linux ABI hybrid **v1.5** |
| **Status** | **Accepted** — per-CPU swapgs, scheduler sleep, doors cold path |
| **Production freezes** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) 1–4 |
| **Model** | **Option C:** kernel hot paths + **doors** cold personality thread |
| **License** | Clean-room dual **MIT OR Apache-2.0** only — **no GPL / no Linux source** |
| **Honesty (Wave 73)** | Soft continuum toward 21500 ≠ product / bar3; product lamps **0** (5) |
| **Companion** | [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) · [CAP_ADDRESSING.md](CAP_ADDRESSING.md) · [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) |

---

## 1. Architecture

```text
  ring-3 Linux task
       │ SYSCALL
       ▼
  swapgs → per-CPU kernel stack (struct gj_cpu @ GS)
       │
       ▼
  gj_linux_syscall_dispatch
       ├─ HOT  → linux_hot (+ user_copy, CapJit, PA futex)
       ├─ COLD → door_call(cold_personality)
       │              │
       │              ▼
       │         personality kernel thread
       │              door_recv → protonrt_cold_linux → door_reply
       └─ NONE → -ENOSYS
```

### Per-CPU / multi-process

| Piece | Status |
|-------|--------|
| `struct gj_cpu` + GS / KERNEL_GS | **Done** (`cpu_init_bsp`) |
| SYSCALL `swapgs` + `u64KernelRsp` | **Done** (`syscall_entry.S`) |
| `process.u64Cr3` + `cpu_load_cr3` on schedule | **Done** (shared boot CR3 until per-AS clone) |
| Multi-CPU AP online | Later (MADT) |

### Scheduler

| Piece | Status |
|-------|--------|
| Kernel threads, `switch_context` | **Done** |
| `thread_block` / `thread_wake` / `schedule` | **Done** |
| Futex sleep via block (not spin) | **Done** when `thread_current()` exists |
| Cold doors sleep | **Done** (`door_call` / `door_recv`) |
| Preemption / timer tick | Later |

### Cold path = doors process (kernel thread stand-in)

Until userland personality process exists, **`cold_personality_server`** is a **kernel thread** using the same doors API a userspace server will later use over IPC.

---

## 2. Hot vs cold (summary)

Unchanged hot set (write, mmap/vmm, mprotect+CapJit, futex PA-share, …).  
Cold: openat/read/… via **door_call** → personality thread → libprotonrt.

---

## 3. Code map (new)

| Path | Role |
|------|------|
| `kernel/cpu/cpu.c` | per-CPU, GS, CR3 |
| `kernel/sched/thread.c` | threads + schedule |
| `kernel/arch/x86_64/switch.S` | context switch |
| `kernel/ipc/door.c` | doors rendezvous |
| `kernel/syscall/cold_ipc.c` | submit → door_call |

---

## 4. Production requirements (from design complete)

| Item | Spec |
|------|------|
| Untrusted ELF | **After** IDT + supervisor-only kernel maps |
| AS isolation | Per-process CR3 clone (kernel half shared) |
| Personality | Migrate kernel thread → **userspace** door server |
| Cold API | **Doors only** (dequeue syscalls deprecated) |
| CapJit | Cap/right, not permanent boolean authority |
| Pointers | `copy_*_user` only; no kernel bypass in product |
| Idle | **HLT** when idle; timer for preemption + futex timeout |

Implementation remaining: code those freezes — design is closed.

---

## 5. Honesty bounds (soft surface ≠ product bar)

This section is additive honesty only. It does **not** change Option C architecture or production freezes.

| Claim surface | What shipped means | What it does **not** mean |
|---------------|--------------------|---------------------------|
| **Hybrid ABI (this doc)** | Option C design + kernel hot/cold dispatch skeleton (swapgs, doors cold path, hot subset) | Steam client runs; full Linux userspace; **bar3 closed** |
| **Hot/cold SYSCALL map** | Documented dispatch; some hot paths + cold door stand-in | Every glibc/Steam Runtime NR implemented end-to-end |
| **io_uring min rings** | Kernel soft surface: setup / enter / register (+ related smokes when greppable) | Full game / title I/O path; Steam Runtime I/O complete |
| **Continuum graph** | Clean-room CREATE-ONLY soft gates; high-water **advancing toward 21500** soft only (parent wires; tree may lag; growth ≠ runtime ABI) | Client launch; Deck Top 50 matrix fill; bar3 done; product complete |
| **Product lamps** | Soft score / bar3-ready stubs remain **0** by design | Any product-score claim |
| **Deck Top 50** | Product **targeting** only ([PROTON_PERSONALITY.md](PROTON_PERSONALITY.md)) | Any title **PASS** or off **NOT-TRIED** |

**Hard rules for this surface:**

1. Hybrid ABI progress is **soft surface** — kernel smoke / design acceptance — not a Steam product claim.
2. **Bar3 remains OPEN** until real-DUT Steam **client** launch + matrix work; see [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md).
3. **io_uring min rings ≠ full game I/O.** Do not promote ring setup smokes to title or client I/O readiness.
4. Continuum high-water **advancing toward 21500** is **soft graph growth only** (parent wires / CREATE-ONLY). Soft deepen on non-w13 surfaces. Targeting or advancing toward 21500 does **not** close bar3, light product lamps, or imply Deck Top 50 progress. **Soft ≠ product complete.**
5. **No Deck Top 50 title claims** from this document, hybrid ABI smokes, continuum gates, or io_uring soft PASS alone. Matrix claim level stays **targeting only** until titles are actually tried. Product lamps remain **0**.

**License / project tone (unchanged):** clean-room dual **MIT OR Apache-2.0**; no GPL import; no Linux kernel source; depth driven by product need, not by pasting foreign trees.

---

*v1.5 — hybrid + production freezes + Wave 73 soft-surface honesty (bar3 open; continuum toward 21500 soft; product lamps 0; soft ≠ product complete; io_uring min ≠ game I/O; no Deck Top 50 claims).*
