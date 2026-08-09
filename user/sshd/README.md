# sshd (product)

GreenJade product SSH daemon — freestanding ELF + host smoke. Tail of Dual DoD **B**:

```text
rtl8168_udx → netstackd → sshd :22  (lab 10.200.125.50)
```

**Soft!=product.** Soft listen **:22** / live spawn **≠** Dual DoD B close (needs UDX NIC wire + host banner on glass). Dual DoD B **OPEN**.

**Product markers** (greppable): `sshd-gj: … PASS`, `sshd: … PASS`,
`sshd: live spawn PASS` (kernel). See tables below.

## License

| Item | Policy |
|------|--------|
| Product code (`sshd_gj.c`, `ssh_crypto.c`, `sshd.c`) | **MIT OR Apache-2.0** (SPDX dual) |
| Crypto | Clean-room pure C from published RFCs (FIPS 180-4, RFC 7748, 2104, 8439, 4253) |
| **Not** in tree | GPL / LGPL OpenSSH source, or any GPL paste |
| Future optional backends | May vend **BSD** OpenSSH or **MIT** Dropbear under project third-party policy; prefer clean-room adapters first |

See repository root `LICENSE` and `third_party/PROVENANCE.md`.

## Layout

| Path | Role |
|------|------|
| `src/sshd_gj.c` | Freestanding product daemon (TCP :22, full KEX/session smoke) |
| `src/ssh_crypto.c` | SHA-256, X25519, HMAC-SHA256, ChaCha20, Poly1305, host identity |
| `src/sshd.c` | Host POSIX listen + soft banner/KEXINIT smoke |

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/sshd.elf` | `make sshd-gj` | Freestanding product daemon (net door, port **22**, **on by default**) |
| `build/sshd` | `make sshd` | Host POSIX smoke (default `127.0.0.1:2222`) |

### Host path (`sshd.c`)

POSIX listen + soft SSH-2.0 identification + soft `SSH_MSG_KEXINIT` only.
**Not** production crypto (that is `sshd_gj.c` + `ssh_crypto.c`).

```
make sshd && ./build/sshd --once     # self-connect CI smoke (default)
./build/sshd --listen                # accept external clients (nc/ssh)
./build/sshd --port 2223 --once      # or env GJ_SSHD_PORT
printf 'SSH-2.0-GreenJade_test\r\n' | nc 127.0.0.1 2222   # with --listen
```

Host smoke markers (success, greppable):

```
sshd: authorized_keys soft PASS
sshd: listening 127.0.0.1:2222 (product host smoke)
sshd: client saw server banner PASS
sshd: peer banner PASS
sshd: kexinit soft PASS
sshd: client kexinit soft PASS
sshd: host path PASS
```

| Marker | Meaning |
|--------|---------|
| `authorized_keys soft PASS` | Soft parse of ed25519/rsa/ecdsa line shapes |
| `client saw server banner PASS` | Loopback client read `SSH-2.0-GreenJade_sshd` |
| `peer banner PASS` | Server accepted client identification (`SSH-…`) |
| `kexinit soft PASS` | Minimal binary `SSH_MSG_KEXINIT` sent |
| `client kexinit soft PASS` | Client drained soft KEXINIT (msg type 20) |
| `host path PASS` | Full host smoke green (exit 0) |

Failure lines use the same prefix with **FAIL**. Default mode is `--once`
(self-connect); use `--listen` for long-running external probing.

### Freestanding path (`sshd_gj.c` + `ssh_crypto.c`)

1. **TCP** :22 via `net_tcp` (virtio-net IPv4 + loopback; retransmit/window/MSS)
2. Banners (RFC 4253) over TCP — product id `SSH-2.0-GreenJade_sshd`
3. **KEX** `curve25519-sha256` — X25519 + SHA-256 shared secret
4. **Production host identity** — seeded host key (not demo `0x41…` scalars); HMAC-SHA256 signature of exchange hash `H`
5. **SSH_MSG_KEX_ECDH_REPLY** on wire (K_S ‖ Q_S ‖ sig) + dual shared-secret match
6. **NEWKEYS** both directions (peer drain) + soft **SERVICE_REQUEST/ACCEPT** (`ssh-userauth`)
7. **Session channel** + shell MOTD (client drains cleartext)
8. Key derivation (RFC 4253 §7.2) → ChaCha20 + HMAC-SHA256 integrity
9. **Encrypted CHANNEL_DATA** send + recv (constant-time MAC verify + decrypt)
10. **Poly1305** soft AEAD leg (RFC 8439 §2.5.2 vector + post-keys tag)
11. Daemon parks with listen held
12. **Dual DoD B** eth path: yield+poll ACCEPT loop → banner
   `SSH-2.0-GreenJade_sshd` → KEX start → soft session (Soft!=product;
   product path = UDX+ABI over net doors once UDX/L2; freestanding rtl SKIP)

**Residual lean deepen (Gap B, once UDX/L2 + L2/TCP works → L3 host proof):**
product sshd over **net doors** once UDX/L2 exists and **product net owns
wire**. Dual DoD B **product** path = **UDX+ABI** (not freestanding rtl wire
owner; freestanding rtl **SKIP** / lab interim only). Prefer **functional
accept/session residual deepen**: door-first AcceptQ claim
(`eth_accept_door_claim` / `net_door_progress_lean` / `eth_accept_yield_poll`)
+ denser door POLL+ACCEPT under `product_net_owns_wire=1` + functional session
settle (`eth_session_settle_residual`: door head → short pure → door re-arm →
lean yield residual) before banner TX. Post-SEND banner flush uses product-net
door head + short pure POLL + lean **yield-every-outer** residual
(`net_flush_residual` / `net_banner_flush_after_send` /
`net_banner_flush_post_pass`) with door re-arm in progress cadence — anti-stall,
**no** multi-thousand pure-POLL busy-spin storms. Prefer `uInner=16`, yield
every outer, ~<=256 POLL per call site. Banner-once (never re-SEND product id
from offset 0). SEND/RECV wait residual is **door-first** when product owns
wire. Accept residual is **door-first then yield-first** (accept-every-yield,
lean idle drain with post-yield door reclaim) so freestanding `net_eth_poll` /
`net_tcp_poll` advance TCP **:22** only as lab interim. Soft listen != host
banner proof: Dual DoD B stays **OPEN** until DUT host `nc` sees product id on
eth :22 (`soft_listen_ne_host_banner=1`; G-AC-1 Soft!=product). Greppable:
`sshd-gj: soft residual lean … Soft!=product product_path=UDX
product_net_owns_wire=1 functional_accept=1 functional_session=1
not_freestanding_rtl=1 soft_listen_ne_host_banner=1` and
`sshd-gj: soft residual lean deepen …` /
`sshd-gj: soft residual lean deepen accept …` /
`sshd-gj: soft residual lean deepen session …`. No kernel stamp from this tree;
dual **MIT OR Apache-2.0**; no GPL.

QEMU hostfwd `2222→22` targets guest TCP :22. Lab freestanding NIC (e.g.
`10.200.125.50:22`) uses the eth accept/session path after park (interim wire
only). Lab-host SSH remains for serial/bench (operator path, not product
crypto). Soft!=product.

### Freestanding product markers (`sshd_gj.c`)

Success path emits greppable **PASS** lines (order fixed). KEX milestones are
explicit so boot logs distinguish crypto KEX from post-KEX session work:

```
sshd-gj: start TCP :22 full path
sshd-gj: TCP listening :22 (virtio + loopback)
sshd-gj: peer banner PASS
sshd-gj: poly1305 selfcheck PASS
sshd-gj: hostkey sign PASS
sshd-gj: ECDH_REPLY PASS
sshd-gj: shared secret match PASS
sshd-gj: x25519 KEX shared PASS
sshd-gj: KEX PASS (curve25519-sha256 + hostkey)
sshd-gj: NEWKEYS exchange PASS
sshd-gj: service soft PASS
sshd-gj: channel+shell MOTD PASS
sshd-gj: key derivation PASS (ChaCha20+HMAC)
sshd-gj: poly1305 aead soft PASS
sshd-gj: encrypted channel PASS
sshd-gj: encrypted channel recv PASS
sshd-gj: post-KEX session PASS
sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)
sshd-gj: daemon park (TCP :22 listen held)
sshd-gj: eth accept loop ready (yield+poll Dual DoD B)
sshd-gj: soft eth park residual_lean=1 accept-every-yield yield_first=1 functional_accept=1 functional_session=1 product_net_owns_wire=1 door_first=1 … Soft!=product dual_dod_b=OPEN product_path=UDX not_freestanding_rtl=1 …
sshd-gj: soft residual lean residual_lean=1 net_door=1 once_UDX_L2=1 product_path=UDX product_net_owns_wire=1 not_freestanding_rtl=1 functional_accept=1 functional_session=1 soft_listen_ne_host_banner=1 door_first=1 … Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1=1
sshd-gj: soft residual lean deepen residual_lean=1 functional_accept=1 functional_session=1 product_net_owns_wire=1 product_path=UDX not_freestanding_rtl=1 soft_listen_ne_host_banner=1 door_first=1 door_claim=1 door_poll_accept=1 Soft!=product dual_dod_b=OPEN G-AC-1=1 dual=MIT_OR_Apache-2.0
sshd-gj: soft residual lean deepen accept functional_accept=1 door_claim=1 door_poll_accept=1 residual_lean=1 product_net_owns_wire=1 product_path=UDX not_freestanding_rtl=1 soft_listen_ne_host_banner=1 Soft!=product dual_dod_b=OPEN
sshd-gj: soft residual lean deepen session functional_session=1 door_settle=1 banner_once=1 residual_lean=1 product_net_owns_wire=1 product_path=UDX not_freestanding_rtl=1 soft_listen_ne_host_banner=1 Soft!=product dual_dod_b=OPEN
```

External eth accept/session (after park; greppable Dual DoD B residual lean):

```
sshd-gj: eth accept session
sshd-gj: eth session Soft!=product banner_once=1 residual_lean=1 functional_accept=1 functional_session=1 product_net_owns_wire=1 yield_first=1 door_first=1 product_path=UDX not_freestanding_rtl=1 soft_listen_ne_host_banner=1 …
sshd-gj: eth banner PASS
sshd-gj: eth peer early close soft PASS   # host nc: read banner → close
sshd-gj: eth KEX start PASS                 # when peer stays connected
sshd-gj: eth session path PASS
sshd-gj: eth accept session PASS
sshd-gj: soft eth functional_session residual_lean=1 product_net_owns_wire=1 product_path=UDX not_freestanding_rtl=1 door_first=1 Soft!=product dual_dod_b=OPEN
sshd-gj: soft eth accept_n=… banner_n=… sess_n=… residual_lean=1 functional_accept=1 functional_session=1 product_net_owns_wire=1 soft_ne_product=1 Soft!=product dual_dod_b=OPEN product_path=UDX not_freestanding_rtl=1
sshd-gj: eth accept session done
```

Early host `nc` (connect → see `SSH-2.0-GreenJade_sshd` → close) is a
**session PASS** after server banner TX; no client banner/KEX required.
Soft!=product.

| Marker | Meaning |
|--------|---------|
| `peer banner PASS` | RFC 4253 identification string exchange |
| `poly1305 selfcheck PASS` | RFC 8439 §2.5.2 Poly1305 test vector |
| `hostkey sign PASS` | Product hostkey sign + verify of `H` |
| `ECDH_REPLY PASS` | SSH_MSG_KEX_ECDH_REPLY on wire + soft parse |
| `shared secret match PASS` | Server/client X25519 shared secrets equal |
| `x25519 KEX shared PASS` | X25519 shared secret from ECDH |
| `KEX PASS (…)` | Aggregate: hostkey + curve25519-sha256 KEX OK |
| `NEWKEYS exchange PASS` | SSH_MSG_NEWKEYS both directions + peer drain |
| `service soft PASS` | Soft SERVICE_REQUEST/ACCEPT (`ssh-userauth`) |
| `channel+shell MOTD PASS` | Session channel open + shell + MOTD |
| `key derivation PASS` | RFC 4253 §7.2 keys armed (ChaCha20 + HMAC) |
| `poly1305 aead soft PASS` | Poly1305 over session material (soft AEAD leg) |
| `encrypted channel PASS` | Encrypted CHANNEL_DATA after NEWKEYS |
| `encrypted channel recv PASS` | Client MAC verify + ChaCha20 decrypt of channel data |
| `post-KEX session PASS` | Full `do_kex_and_session` success |
| `live path PASS (…)` | Banner + session both green; daemon parks |
| `eth accept loop ready` | Parked; yield-first POLL+ACCEPT (Dual DoD B) |
| `soft eth park residual_lean=1` | Lean residual honesty at park (Soft!=product) |
| `soft residual lean … product_path=UDX` | Exclusive residual lean once; product sshd over net doors once UDX/L2; product net owns wire; not freestanding rtl (Soft!=product · G-AC-1) |
| `soft residual lean deepen …` | Functional accept/session residual lean deepen once; `product_net_owns_wire=1` `door_first=1` |
| `soft residual lean deepen accept …` | Accept residual deepen honesty: door claim + door POLL+ACCEPT |
| `soft residual lean deepen session …` | Session residual deepen honesty: door settle + banner-once |
| `functional_accept=1` | Door-first AcceptQ claim + denser door POLL+ACCEPT residual |
| `functional_session=1` | Door-first session settle (head → pure → re-arm → yield) + banner residual |
| `product_net_owns_wire=1` | Product NIC path = UDX+ABI over net doors (rtl SKIP) |
| `door_first=1` | Prefer door POLL/ACCEPT when product net owns wire |
| `yield_first=1` | Yield so freestanding poll advances :22 (L3 path; lab interim) |
| `eth accept session` | External TCP peer accepted on :22 |
| `residual_lean=1` | Lean POLL budgets (once UDX/L2 + L2/TCP works); anti-stall |
| `not_freestanding_rtl=1` | Dual DoD B product = UDX+ABI; freestanding rtl SKIP / interim only |
| `soft_listen_ne_host_banner=1` | Soft listen != host banner proof; Dual DoD B stays OPEN |
| `eth banner PASS` | Product id `SSH-2.0-GreenJade_sshd` TX (DoD B host nc) |
| `eth peer early close soft PASS` | Peer FIN/EOF after banner (typical `nc`); session PASS |
| `eth KEX start PASS` | Server KEXINIT on wire |
| `eth session path PASS` | Soft session path green after banner/KEX (or early nc close) |
| `eth accept session PASS` | Accept + session handler returned OK |
| `soft eth functional_session …` | Functional session residual PASS honesty (Soft!=product) |
| `soft eth … soft_ne_product=1` | Eth tallies; Soft!=product honesty |

**Primary product line for agents/scripts:**  
`sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)`

**Dual DoD B (Soft!=product):** host `nc`/ssh to guest **TCP :22** sees
`SSH-2.0-GreenJade_sshd` (`eth banner PASS`). Soft listen != host banner.
Product path once UDX/L2 = **UDX+ABI over net doors** when product net owns
wire (not freestanding rtl; freestanding rtl SKIP). ICMP != this DoD.

Failure lines use the same prefix with **FAIL** (e.g. KEX path:
`hostkey sign FAIL`, `post-KEX session FAIL`, `live path FAIL`).

Kernel spawn markers (not from this directory):

```
sshd: live elf thr=…
sshd: live spawn PASS
```

Soft inventory (`scripts/gj-product-summary.sh`) greps `sshd: live spawn PASS`.
Hard/quick keys may also look for freestanding `live path PASS`.

## Config (rootfs)

```
/etc/ssh/sshd_config
/etc/ssh/authorized_keys   # hwtest ed25519 pubkey when staged
/usr/sbin/sshd             # freestanding ELF (also /sbin/sshd)
```

## Build

```
make sshd-gj    # → build/user/sshd.elf
make sshd       # → build/sshd (host POSIX smoke)
```

## Kernel live spawn

Embedded via `kernel/proc/sshd_embed.S` and spawned after `netstackd` in
`kernel/main.c` (outside this directory). Boot log shows the kernel markers
above; freestanding markers appear once the ELF runs its TCP :22 self-smoke.

See also: [ABI-first pivot](../../docs/ABI_FIRST_PIVOT.md) (product direction).

---

**Project:** GreenJade · Soft≠product · Dual DoD A/B **OPEN**. [root README](../../README.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
