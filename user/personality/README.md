# personality (G-PERS) — userspace cold personality door server

Freestanding ring-3 **cold Linux personality** door server scaffold for
GreenJade (G-PERS product shape). This tree will replace the bring-up
kernel thread `cold_personality_server` as the default cold-path server.

License: **MIT OR Apache-2.0**. Clean-room; no GPL source.

## Why this exists

Option C (hybrid Linux ABI) routes cold syscalls over **doors**:

```text
  Linux task  SYSCALL
       │
       ▼
  hot path  → kernel linux_hot
  cold path → door_call(cold_personality)
                    │
                    ▼
         bring-up: kernel kthread cold_personality_server
                   door_recv → protonrt_cold_linux → door_reply

         product:  userspace G-PERS process (this directory)
                   gj_ipc_recv → serve → gj_ipc_reply
```

| Phase | Who serves cold | Status |
|-------|-----------------|--------|
| **Bring-up** | Kernel `cold_personality_server` + doors | Temporary (allowed) |
| **Interim** | `user/protonrt-server/server.S` blob + `GJ_SYS_PERSONALITY_SERVE` | Soft product shape; policy still kernel |
| **Product (G-PERS)** | This ELF + libprotonrt cold policy in userspace | Required (G-PERS-*) |

Normative freezes: [docs/DESIGN_SPEC_COMPLETE.md](../../docs/DESIGN_SPEC_COMPLETE.md)
§3 (G-PERS-1…3, G-COLD-1…3); hybrid map: [docs/LINUX_ABI_HYBRID.md](../../docs/LINUX_ABI_HYBRID.md).

**ABI-first:** cold personality is part of the product Linux-shaped path
([docs/ABI_FIRST_PIVOT.md](../../docs/ABI_FIRST_PIVOT.md)). Soft module load of
host `.ko` is a separate kernel lab surface — not this door server.

### Linux-shaped residual (Option C + UDX hosts + apps)

This tree’s **soft residual lean** documents the product *direction* for
Linux-shaped userspace — it does **not** attach the door or ship drivers.
**Residual deepen** adds G-PERS/G-COLD honesty, Dual DoD A/B UDX direction,
and concrete host/app tokens — still Soft!=product.

```text
  Linux-shaped apps (sshd / netstackd / shell / vfsd / storaged / Proton …)
       │  hot SYSCALL + cold door (Option C; G-PERS-1..3 / G-COLD-1..3)
       ▼
  personality (this ELF) — cold serve  [scaffold today; product OPEN]
       │
       │  (same host world; separate processes; personality TCB: no IOMMU/IRQ)
       ▼
  UDX driver hosts (rtl8168_udx / xhci_udx / ddi_host_gj)  [userspace; not .ko]
       │  Linux-shaped probe/remove/quiesce/ioremap/dma/irq
       │  freestanding bind walk: SCAN → GET → OPEN → MAP_BAR (GJ_SYS_DDI)
       ▼
  DDI / devmgr caps (MMIO / IRQ Notification / DMA windows)
```

| Surface | Soft residual (this tree) | Product |
|---------|---------------------------|---------|
| **Option C hybrid** | `personality: soft option_c` (deepen: G-PERS/G-COLD) | Hot + cold door as default cold owner |
| **Apps** | `personality: soft apps` (deepen: sshd/netstackd/…/virtio T0) | Linux-shaped apps over hot+cold ABI |
| **UDX hosts** | `personality: soft udx_host` (deepen: rtl/xhci/ddi + Dual DoD A/B) | Clean-room / dual-license userspace drivers |
| **Dual DoD A/B** | residual says **OPEN** (`dual_dod_a` / `dual_dod_b`) | UDX USB + UDX NIC datapath evidence |
| **Freestanding class** | residual says **not product** / **SKIP** | SKIP; product = UDX/DDI |
| **Linux `.ko` in-kernel** | **Forbidden (G-AC-1)** | Never product AC |

Companions (outside this tree): [docs/UDX_LINUX_PORTER.md](../../docs/UDX_LINUX_PORTER.md) ·
[docs/DDI_SOFT.md](../../docs/DDI_SOFT.md) ·
[docs/LAPTOP_LINUX_DRIVER_HOST.md](../../docs/LAPTOP_LINUX_DRIVER_HOST.md) ·
[docs/ABI_FIRST_PIVOT.md](../../docs/ABI_FIRST_PIVOT.md).

### How it replaces `cold_personality_server`

1. **Boot** maps/spawns this NATIVE process (or embeds ELF like vfsd/storaged)
   and parks it on the cold personality door.
2. Cold clients keep using `door_call` / `GJ_SYS_IPC_CALL` — wire ABI unchanged.
3. Server loop: `gj_ipc_recv` → cold serve (libprotonrt / door clients) →
   `gj_ipc_reply` — same rendezvous the kthread uses today.
4. Kernel stops creating `cold_personality_server` as the default; kthread
   remains only as optional fallback if map/spawn fails (as
   `pers: user map/create failed — kernel fallback` does today).
5. Cap surface (G-PERS-2): COLD door, vfs client, optional net client —
   **no** IOMMU/IRQ in the personality TCB.

Companion interim path (not this tree): `user/protonrt-server/` (asm door
loop + `PERSONALITY_SERVE` kernel policy). This C scaffold is the freestanding
ELF product line (vfsd/storaged-style), separate from the in-kernel blob map.

## Door attach contract

Who may park on the cold personality door, how badge + `cold_ipc` attach
interact, and when it is safe to enable `PERSONALITY_DOOR_LOOP`.

| Role | Owner today | Notes |
|------|-------------|--------|
| **Cold door object** | Kernel `door_cold_personality()` | Global ENDPOINT; installed into boot / server CNode as `GJ_CAP_ENDPOINT` |
| **Bring-up server** | `user/protonrt-server` blob **or** kthread `cold_personality_server` | Mapped/scheduled by kernel (`user_personality_map` / fallback) |
| **Product server (G-PERS)** | This ELF (`personality.elf`) | Only after coordinator wires embed + spawn **and** transfers door ownership |
| **Clients** | Linux hybrid tasks via `door_call` / `GJ_SYS_IPC_CALL` | Wire ABI unchanged; do not set server badge |
| **UDX hosts** | Separate userspace processes (`user/udx` + drivers) | Not this door server; share Linux-shaped host *world*, not TCB |

### Badge (server-authoritative)

- Badges follow SECURITY_CORE §1 rule 3: **server sets**, clients cannot forge
  via the public door API.
- Bring-up smoke may stamp e.g. `door_set_badge(door_cold_personality(), 0xc0ffee)`.
- On a completed flight the kernel snapshots badge into `u32LastBadge` for the
  client (`door_get_last_badge`).
- This scaffold does **not** call `door_set_badge` yet — badge policy stays
  kernel / coordinator until product attach lands.

### `cold_ipc` attach flag

- `cold_ipc_set_personality_attached(1)` / `cold_ipc_personality_attached()`
  (kernel `gj/cold_ipc.h`) gate cold-path soft modes and hybrid smokes.
- Attach means “a personality consumer is live” (service pfn, queue consumer,
  or door server). Default mode includes `GJ_COLD_MODE_REQUIRE_SERVER` so
  `door_call` does not hang on a stale endpoint with no server.
- Product G-PERS attach (outside this tree): spawn this NATIVE process, bind
  it as door `pServer`, set attached, then cold clients may use doors-first.

### When to enable the door loop

| Flag | Default | Meaning |
|------|---------|---------|
| `PERSONALITY_DOOR_LOOP` | **0** | Soft scaffold only — no `gj_ipc_recv`; smoke-safe |
| `PERSONALITY_WOULD_RECV_CAP` | **0** | With loop=1: `0` = infinite product park; `N>0` = soft-miss cap then return |

**Do not** set `PERSONALITY_DOOR_LOOP=1` until:

1. Boot maps/spawns this process as NATIVE door owner (or transfers ownership
   from protonrt-server / kthread fallback).
2. Door `pServer` points at this task’s thread (single server waiter).
3. `cold_ipc` personality-attached is set (or equivalent product path).
4. Coordinator accepts that smoke may park forever unless
   `PERSONALITY_WOULD_RECV_CAP` is also set for a soft-miss escape.

```text
# product park (after attach) — coordinator Makefile USER_CFLAGS fragment:
-DPERSONALITY_DOOR_LOOP=1

# optional smoke-safe soft-miss cap (would_recv iterations then return):
-DPERSONALITY_DOOR_LOOP=1 -DPERSONALITY_WOULD_RECV_CAP=64
```

Soft!= product: enabling the loop without attach will soft-miss-yield (or
block in `IPC_RECV` if the kernel parks the waiter) and is **not** a product
claim.

## Layout

| Path | Role |
|------|------|
| `personality_gj.c` | Freestanding scaffold (`_start` → `main`); soft PASS + soft serve ready + residual lean |
| `README.md` | This file — product story, attach contract, residual deepen, Makefile lines for coordinator |
| *(no local `.ld`)* | **Reuses** [`user/init/user.ld`](../init/user.ld) like other `*-gj` daemons |

Source of truth under this tree: `personality_gj.c` only. Parent Makefile /
kernel embed / boot spawn live **outside** this tree (coordinator merges).

## Freestanding path (`personality_gj.c`) — scaffold only

Current binary is a **soft userspace scaffold**, not a live door server:

1. Enter `_start` -> call `main`
2. Soft phase 1: greppable scaffold PASS via native `GJ_SYS_DEBUG_LOG`
3. Soft phase 2: soft once `personality-gj: soft serve ready` (scaffold complete)
4. Soft phase 3: lean residual (honesty / path / inventory / option_c /
   udx_host / apps / residual lean PASS)
5. Exit 0

Default: `PERSONALITY_DOOR_LOOP=0` -- **never** enters blocking `gj_ipc_recv`.
Phase 3 residual lamps emit only on the scaffold path (`DOOR_LOOP=0`).

### Planned door loop (compiled in only when `PERSONALITY_DOOR_LOOP=1`)

```text
DEBUG_LOG "personality-gj: door server up"
fixed gj_linux_regs-shaped frame (wire 64 B; headroom 128 B)
loop:
  zero wire frame
  gj_ipc_recv(frame)          /* door_recv / would_recv iteration */
  on soft miss → one-shot soft recv-miss; count would_recv;
                 if WOULD_RECV_CAP>0 and count>=cap → soft would_recv cap, return
                 else yield, retry (never EXIT)
  serve cold NR (libprotonrt / PERSONALITY_SERVE interim)
  on soft miss → one-shot soft serve-miss (still reply)
  gj_ipc_reply(ret)           /* door_reply */
  on soft miss → one-shot soft reply-miss; yield, retry
  first full flight → one-shot soft serve ready
```

`gj_ipc_recv` / `gj_ipc_reply` / `gj_personality_serve` already exist in
`user/libgj/include/gj/syscalls.h`. Scaffold does **not** enter the blocking
recv loop until `PERSONALITY_DOOR_LOOP=1` after attach (else smoke would hang).

## Smoke markers

Soft (scaffold; default build -- greppable, optional for smoke-all):

```text
personality: soft userspace scaffold PASS
personality-gj: soft serve ready
personality: soft honesty
personality: soft path
personality: soft inventory
personality: soft option_c
personality: soft udx_host
personality: soft apps
personality: soft residual lean
personality: soft residual lean PASS
```

| Marker | When | Meaning |
|--------|------|---------|
| `personality: soft userspace scaffold PASS` | Always (phase 1) | ELF ran; `gj_debug_log` works |
| `personality-gj: soft serve ready` | Soft once (phase 2) | Scaffold complete **or** first full door flight when loop=1 |
| `personality: soft honesty` | Soft once (phase 3; DOOR_LOOP=0) | Door/personality honesty; Soft!=product; dual DoD A/B + bar3 OPEN |
| `personality: soft path` | Soft once (phase 3; DOOR_LOOP=0) | ABI-first host path; Option C + UDX + apps; G-PERS/G-COLD; G-AC-1 |
| `personality: soft inventory` | Soft once (phase 3; DOOR_LOOP=0) | Lean residual inventory + deepen tokens (hosts/apps OPEN) |
| `personality: soft option_c` | Soft once (phase 3; DOOR_LOOP=0) | Hybrid hot+cold residual deepen; G-PERS/G-COLD honesty; attach OPEN |
| `personality: soft udx_host` | Soft once (phase 3; DOOR_LOOP=0) | UDX host residual deepen (rtl/xhci/ddi + Dual DoD A/B); not `.ko` |
| `personality: soft apps` | Soft once (phase 3; DOOR_LOOP=0) | Apps residual deepen (sshd/netstackd/…/virtio T0); no bar3 claim |
| `personality: soft residual lean` | Soft once (phase 3; DOOR_LOOP=0) | Residual lean honesty (not live G-PERS); ok=10/10 self-check |
| `personality: soft residual lean PASS` | Soft once (phase 3; DOOR_LOOP=0) | Residual lean + deepen complete; Soft!=product |

Do **not** rename the PASS / soft serve ready / residual lean substrings once
smoke-all greps depend on them. Deepen tokens on `option_c` / `udx_host` /
`apps` lines are additive fixed strings only (no new prefix-stable markers).

### Lean soft residual (exclusive; this tree only)

Soft residual lean is **scaffold honesty** only -- never hard-gates product
G-PERS attach, never claims freestanding class drivers, never waives **G-AC-1**,
never closes dual DoD / bar3 / Deck Top 50.

| Claim | Soft residual | Product |
|-------|---------------|---------|
| Userspace cold door ELF scaffold | **Yes** (this tree) | G-PERS attach OPEN (coordinator) |
| Door loop park | Default **0** (soft) | `PERSONALITY_DOOR_LOOP=1` after attach |
| Live path PASS | **Not claimed** | Future hard marker |
| Option C hybrid (hot + cold) | `soft option_c` deepen (G-PERS/G-COLD) | Userspace cold owner default |
| ABI-first Linux-shaped host path | Residual lean + path lamps | Apps/drivers over cold door + UDX |
| UDX driver hosts (userspace) | `soft udx_host` deepen (rtl/xhci/ddi) | Clean-room / dual-license class hosts |
| Linux-shaped apps | `soft apps` deepen (named apps + virtio T0) | Apps on hot+cold ABI |
| Dual DoD A/B (USB/NIC UDX) | `dual_dod_a=OPEN` / `dual_dod_b=OPEN` | DUT evidence only |
| Freestanding class drivers (rtl/xhci) | Residual says **not product** / SKIP | Product = userspace UDX/DDI |
| Linux `.ko` in-kernel product AC | **Forbidden (G-AC-1)** | Never |
| Soft != product | Explicit on residual lines | Dual MIT OR Apache-2.0 |

#### Residual deepen tokens (fixed strings; freestanding)

| Token | On markers | Meaning |
|-------|------------|---------|
| `product_dir=UDX+ABI` | honesty / path / inventory / lean / PASS | Product direction (not freestanding wire) |
| `option_c=1` | honesty / path / inventory / lean / PASS / option_c | Hybrid hot+cold residual documented |
| `udx_host=1` | honesty / path / inventory / lean / PASS / udx_host | Userspace UDX hosts residual |
| `apps=1` / `apps_linux_shaped=1` | honesty / path / inventory / lean / PASS / apps | Linux-shaped apps residual |
| `residual_deepen=1` | inventory / option_c / udx_host / apps / lean / PASS | Option C + UDX host/apps deepen wave |
| `g_pers1=1` / `g_pers2=1` / `g_pers3=OPEN` | path / option_c / lean | G-PERS freezes honesty (attach still OPEN) |
| `g_cold1=1` / `g_cold2=deprecated_scaffold` / `g_cold3=1` | option_c / lean | G-COLD freezes honesty |
| `hot_dispatch=1` / `cold_door_call=1` | option_c | Hybrid dispatch residual (not live path) |
| `rtl8168_udx=1` / `xhci_udx=1` / `ddi_host_gj=1` | udx_host / inventory / lean | Host trees residual (outside this TU) |
| `g752_nic=10ec:8168` / `g752_xhci=8086:a12f` | udx_host | G752 target IDs residual only |
| `bind_walk=SCAN_GET_OPEN_MAP_BAR` | udx_host | Soft DDI bind walk residual |
| `dual_dod_a=OPEN` / `dual_dod_b=OPEN` | honesty / path / inventory / udx_host / lean / PASS | Dual DoD A=USB UDX · B=NIC UDX |
| `app_sshd=soft` / `app_netstackd=soft` / … | apps | Named app residual surface (not product PASS) |
| `virtio_t0=1` / `udx_owns_wire=OPEN` | apps | T0 net until UDX owns wire |
| `freestanding_class_product=0` / `freestanding_class_skip=1` | honesty / inventory / lean / PASS / udx_host | No freestanding class product claim |
| `ko_product=0` / `g_ac1=1` | all residual | G-AC-1 held |
| `g_pers_attach=OPEN` | honesty / inventory / option_c / PASS | Product door attach still coordinator |
| `dual_dod=OPEN` | honesty / inventory / udx_host | Dual laptop DoD not closed by residual |
| `bar3=OPEN` / `deck_top50=NOT_CLAIMED` | honesty / apps | No Steam product claim from residual |
| `ok=10/10` | residual lean | Stack-local self-check (compile-time 10 under default) |
| `Soft!=product` | all residual | Soft residual != product complete |

When `PERSONALITY_DOOR_LOOP=1` (product attach / capped smoke):

```text
personality-gj: door server up
personality-gj: soft recv-miss
personality-gj: soft serve-miss
personality-gj: soft reply-miss
personality-gj: soft serve ready
personality-gj: soft would_recv cap
```

Future hard markers (not claimed yet):

```text
personality-gj: live path PASS
```

Kernel companions (outside this tree, today):

```text
pers: userspace door server scheduled (G-PERS)
pers: user map/create failed — kernel fallback
pers: kernel door server parked (fallback)
door: badge=0x…
```

## Linker script

**Reuse** `user/init/user.ld` (shared freestanding script):

- `ENTRY(_start)`
- load base `0x1000000` (above kernel BSS / `GJ_USER_CODE_VA`)
- discard `.note` / `.eh_frame`

No `user/personality/personality.ld` — same pattern as `vfsd`, `storaged`,
`hda_client`, `scsi_mid`. Only `ld-gj` ships a private script.

Syscalls used by this scaffold: `gj_debug_log` / `gj_exit` always;
`gj_ipc_recv` / `gj_personality_serve` / `gj_ipc_reply` / `gj_yield` only when
`PERSONALITY_DOOR_LOOP=1`. All via libgj inlines (`gj/syscalls.h`).

## Makefile lines for the coordinator

**Do not** edit the root Makefile from this tree. Coordinator should add
(alongside other `*-gj` freestanding targets, near `VFSD_GJ_ELF` /
`HDA_CLIENT_GJ_ELF`):

### 1. Variable (with other `*_GJ_ELF` defs)

```make
PERSONALITY_GJ_ELF := $(BUILD)/user/personality.elf
```

### 2. Phony target list

Add `personality-gj` to the root `.PHONY:` line (with `vfsd-gj`,
`hda_client-gj`, …).

### 3. Build rules

```make
.PHONY: personality-gj
personality-gj: $(PERSONALITY_GJ_ELF)
	@echo "personality-gj: $(PERSONALITY_GJ_ELF)"

$(PERSONALITY_GJ_ELF): user/personality/personality_gj.c $(LIBGJ) user/init/user.ld
	@mkdir -p $(dir $@) $(BUILD)/user/personality
	$(CC) $(USER_CFLAGS) -c -o $(BUILD)/user/personality/personality_gj.o \
		user/personality/personality_gj.c
	$(LD) $(USER_LDFLAGS) -o $@ $(BUILD)/user/personality/personality_gj.o $(LIBGJ)
	@echo "built $@"
```

Default build keeps `PERSONALITY_DOOR_LOOP=0` (no extra `-D`). To enable the
door loop after attach, append to the compile line:

```make
# product (after door attach only):
#   $(CC) $(USER_CFLAGS) -DPERSONALITY_DOOR_LOOP=1 -c -o …
# smoke-safe soft-miss cap:
#   $(CC) $(USER_CFLAGS) -DPERSONALITY_DOOR_LOOP=1 -DPERSONALITY_WOULD_RECV_CAP=64 -c -o …
```

### 4. Optional `userland` aggregate

If personality should build with the rest of freestanding userland:

```make
# append to userland: prerequisites and echo list
userland: … $(PERSONALITY_GJ_ELF)
	@echo "userland: … $(PERSONALITY_GJ_ELF)"
```

### 5. Later (not scaffold): embed + boot

When productizing (mirror `vfsd_embed.S` / `storaged_embed.S`):

```make
# example only — coordinator owns kernel/proc/
# $(BUILD)/kernel/proc/personality_embed.o: $(PERSONALITY_GJ_ELF)
# kernel/proc/personality_embed.S: $(PERSONALITY_GJ_ELF)
```

And drop default `thread_create(…, cold_personality_server, …)` once this
ELF is the attached cold door owner.

### Build (after merge)

```text
make personality-gj    → build/user/personality.elf
```

## Product notes

| Topic | Status |
|-------|--------|
| Soft scaffold ELF + greppable PASS + soft serve ready | **This tree** |
| Lean soft residual (honesty / path / inventory / residual lean PASS) | **This tree** (DOOR_LOOP=0 only) |
| Residual deepen: option_c / udx_host / apps once-lamps | **This tree** (DOOR_LOOP=0 only) |
| Option C deepen (G-PERS/G-COLD / hot_dispatch / cold_door_call) | **This tree** (soft residual only) |
| UDX host deepen (rtl/xhci/ddi IDs + Dual DoD A/B OPEN) | **This tree** (soft residual only) |
| Apps deepen (sshd/netstackd/shell/vfsd/storaged/proton + virtio T0) | **This tree** (soft residual only) |
| Door loop in freestanding C | **Scaffold only** (`DOOR_LOOP=0` default; API in libgj) |
| Door attach contract (badge, cold_ipc, owner) | **Documented** above; wire-up is coordinator |
| Optional soft would_recv cap | **Compile-time** when `DOOR_LOOP=1` |
| Replace kthread as default | **Coordinator** (kernel/main + embed) |
| Cold policy fully in userspace | **OPEN** -- interim `PERSONALITY_SERVE` / libprotonrt |
| Cap TCB (G-PERS-2) | Design freeze; enforce at product attach |
| ABI-first Linux-shaped host (apps/drivers) | Residual deepen lamps; product via cold door + UDX |
| UDX driver hosts product datapath | **OPEN** (residual only; hosts outside this tree) |
| Freestanding class drivers as product | **No** (residual honesty; freestanding SKIP) |
| Linux `.ko` in-kernel product AC | **Forbidden (G-AC-1)** |
| Dual DoD A (USB UDX) / B (NIC UDX) | **OPEN** (residual never closes) |
| bar3 / Deck Top 50 | **OPEN / NOT_CLAIMED** (residual never closes) |

Honesty: soft scaffold PASS / soft serve ready / residual lean PASS != product
cold path; bar3 / Deck Top 50 / Dual DoD A/B unchanged. Soft!=product. Dual
MIT OR Apache-2.0. **G-AC-1:** no Linux `.ko` product AC. Product direction =
**UDX + ABI** (userspace hosts + Option C apps), not freestanding class thrash.

See [docs/PROTON_PERSONALITY.md](../../docs/PROTON_PERSONALITY.md) ·
[docs/ABI_FIRST_PIVOT.md](../../docs/ABI_FIRST_PIVOT.md) ·
[docs/LINUX_ABI_HYBRID.md](../../docs/LINUX_ABI_HYBRID.md).

---

**Project:** GreenJade pure-C microkernel · dual **MIT OR Apache-2.0** · Soft≠product · Dual DoD A/B **OPEN**. See [root README](../../README.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
