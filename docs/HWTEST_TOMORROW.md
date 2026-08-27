# Hardware test — operator pack

## Fly this cut — **v0.1.185** · 8168 live PCI Command MSE|BME (login OPEN)

**0.1.185 pack of BME CF8 poke** (`ddi_door` live Command 0x04 for `10ec:8168`; UDX host `CFG_WRITE` 0x0006). Dual DoD **A/B OPEN**. Never `USBCMD.RS=1`. Hold7 IP ≠ L3. Banner / `true` ≠ login. **0.2.0** reserved.

**0.1.184 host (glass 132433 · lab `10.200.125.103`):** title **v0.1.184**; hold7 IP pin; host ping/arping **FAIL** (neigh FAILED). Overnight T0 PTY was same stamp string. **login OPEN**.

GOP dash isolate stays. Dual DoD **A/B OPEN**. Skip `nc` (hog `:22`).

| Hold | Expect after flash (185 untried) |
|------|--------|
| title | `STATUS (static) v0.1.185` |
| 1 | `M0 OK dash SKIP isolate` |
| 2 | `TE mode=hw tes=1 tt=ML slpt=1 rdy bus3 id1g` (lamp; ≠ L3) |
| 3 | `UDX xhci PASS rs=0` (Dual DoD A **OPEN**) |
| 4 | `UDX inj=<N> tx=<M> lnk=<0|1>` (photograph this) |
| 5 | `UDX te_disarm … own= rok= fovw=` |
| 6 | `UDX mac_rclm rds= te_re=` |
| 7 | `IP 10.200.125.50 :22` (static pin; ≠ ARP) |
| 8 | `DoD A=OPEN B=OPEN` |

Soft≠product. Dual DoD A **OPEN** (`need=usbcmd_rs`, never RS=1). Dual DoD B **OPEN** until host **interactive SSH login**.

**Pack:** `make hwtest-img` → `build/greenjade-hwtest.img` (ESP GPT p1 @ 1 MiB extract MATCH vs `build/`).  
KERNEL `101d024e62015dd4b824371a2858c92b44d0e85670b88d0513894432b2449336`  
sshd `471ad8a11aad7541343f1f0af71c70f0b0341543396366ba9698d1b7c777bcbb`  
rtl `89b5d3939beea3cb817ecbf7f3dfe7c76bd05b07b6a419a46641e78ad07b259f`  
xhci `665959290ade24a511fc63557df008dfca02fab646d573f6ed9b58a626428b78`  
sh `6725f1c3100271f36b4a12cf8c5c81e9bf768fa1049d6be9c638a6fda7f680b9`  
img `2f05f5e2043a4d0bcf68414074a3025d9513ab4a817ab154e65310823692442b`.

**Flash (destroys the stick):** `sudo ./scripts/install-hwtest-usb.sh /dev/sdX`  
**DUT:** G752VT · Secure Boot **off** · UEFI USB · lab **10.200.125.50**.

| Check | What to look for | Close? |
|-------|------------------|--------|
| Flash identity | GOP **STATUS (static) v0.1.185** | Confirm cut |
| Isolate | hold1 **`M0 OK dash SKIP isolate`** · no `USER KILL` / `FAULT PINNED` | Wiring. Do **not** un-isolate |
| Lab IP | hold7 **`IP 10.200.125.50 :22`** | Wiring; not L3 |
| xhci **A** | hold3 **`UDX xhci PASS rs=0`**; **RS off** | Dual DoD A **OPEN** |
| UDX L2 | hold4 **`UDX inj= tx= lnk=`** | Diagnose PHY/RX/TX |
| L3 | `ping` / `arping 10.200.125.50` | Want **PASS**. **login OPEN** |
| Interactive `ssh -t` | prompt + typed command | Dual DoD B close only then |

## Prior fly — **v0.1.184** · exec TX drain after 183 Sending command (login OPEN)

**0.1.184 packed.** Dual DoD **A/B OPEN**. GOP isolate stays. Never `USBCMD.RS=1`. `true` exit 0 ≠ login. **0.2.0** reserved.

**This cut (vs 183 glass 100643 FAIL):** `Sending command: true` **PASS**; exec CHANNEL_SUCCESS / exit-status dropped (124). Drain after each exec reply (`ssh_send_drain` == n + `ssh_tx_drain`). Stay-open **return 2**. GOP isolate stays.

**0.1.183 host (glass 100643 / 100648 · lab `10.200.125.103` · ssh UTC 151015):** `Sending command: true` **PASS**; SIGTERM 124; second ssh TIMEOUT; ping 0/3. **login OPEN**.

GOP dash isolate stays. Dual DoD **A/B OPEN**. Skip `nc` (hog `:22`).

| Hold | Expect after flash (184 untried) |
|------|--------|
| title | `STATUS (static) v0.1.184` |
| 1 | `M0 OK dash SKIP isolate` |
| 2 | `TE MODE=NONE TES=0 TT=ML SLPT=1 RDY BUS3 ID16 TE_DISARM` |
| 3 | `UDX xhci PASS rs=0 IMAN=1 IRQ=PASS CCS=1` (Dual DoD A **OPEN**) |
| 4 | `UDX inj=47 tx=2 lnk=1` |
| 5 | `UDX TE_DISARM FOVW OWN=16 ROK=0 FOVW=1 C=1` |
| 6 | `UDX MAC_RCLM RDS=1 TE-RE=1` |
| 7 | `IP 10.200.125.50 :22` |
| 8 | `DoD A=OPEN B=OPEN` |

Soft≠product. Dual DoD A **OPEN** (`need=usbcmd_rs`, never RS=1). Dual DoD B **OPEN** for **interactive login**. **0.2.0** reserved.

**Pack:** `make hwtest-img` → `build/greenjade-hwtest.img` (ESP GPT p1 @ 1 MiB extract MATCH vs `build/`).  
KERNEL `9241ec46385ebf0c731bec736a4e163376dbc8bc2df9176d109294991392096c`  
sshd `254b23617afbd672d96b9473e49940cf5ff465fc3df38794e78a6e9bb98765bc`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `69f47a511ca459d035c74ea35563a7c0489d1c4e43eaf164dc2b8c240c925ae4`.

**Flash (destroys the stick):** `sudo ./scripts/install-hwtest-usb.sh /dev/sdX`  
**DUT:** G752VT · Secure Boot **off** · UEFI USB · lab **10.200.125.50**.

| Check | What to look for | Close? |
|-------|------------------|--------|
| Flash identity | GOP **STATUS (static) v0.1.184** | Confirm cut |
| Isolate | hold1 **`M0 OK dash SKIP isolate`** · no `USER KILL` / `FAULT PINNED` | Wiring. Do **not** un-isolate |
| Lab IP | hold7 **`IP 10.200.125.50 :22`** | Wiring |
| xhci **A** | hold3 **`UDX xhci PASS rs=0 … ccs=1`**; **RS off** | Dual DoD A **OPEN** |
| L3 | `ping` / `arping 10.200.125.50` | Want **PASS** before and after two `ssh`. **login OPEN** |
| Banner / PK_OK / SUCCESS / `Sending command` | as on **0.1.183** | Proven-class. **login OPEN** |
| First `ssh … true` | **exit 0** | **Untried** this stamp. **login OPEN** |
| Second `ssh … true` | still exit 0 | **Untried** this stamp. **login OPEN** |
| Interactive `ssh` | MOTD; no dash `$` | **Not** this cut. **login OPEN** |
| Shell / dash | real `/bin/sh` | **Not** this cut. **login OPEN** |

**Not claimed:** interactive dash, interactive SSH login, Dual DoD A/B close, bar3, **0.2.0**. Soft≠product. **G-AC-1.**

```sh
make hwtest-img
ls -lh build/greenjade-hwtest.img
./scripts/gj-image-version.sh --report
sudo ./scripts/install-hwtest-usb.sh /dev/sdX
```

Host (skip `nc`):
```sh
ping -c 5 10.200.125.50
ssh -o BatchMode=yes -o ConnectTimeout=10 jay@10.200.125.50 true; echo exit:$?
ssh -o BatchMode=yes -o ConnectTimeout=10 jay@10.200.125.50 true; echo exit:$?
ping -c 3 10.200.125.50
```

---

## Prior cut — **v0.1.183** (do not flash) · OPEN_CONFIRM TX drain after 182 miss CLOSE

**0.1.183 host (glass 100643 / 100648 · lab `10.200.125.103` · ssh UTC 151015):** title **v0.1.183**; isolate; xhci `PASS rs=0`. Ping before **5/5**. First `ssh … true`: SUCCESS **PASS**; **`Sending command: true` PASS**; SIGTERM 124. Second ssh connect TIMEOUT 255. Ping after **0/3**. **login OPEN**.

**Pack (do not flash):** KERNEL `f24d04d39ed189ee648d3376216f7beb2969981d0bcc1eee0ba7111f92c15d9a`  
sshd `c1d538b6429bae1423290500b8c478b14d1d43ffcf19b0cb37923a52f81e832e`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `9b336e89c14c1898faedd04894e04ae4a4f48c6ea01f69e867dda76b3a10c0f8`.

---

## Prior cut — **v0.1.182** (do not flash) · OPEN_CONFIRM TX retry after 181 miss

**0.1.182 host (glass 090849 / 090856 / 092206 · lab `10.200.125.103` · ssh UTC 141559):** title **v0.1.182**; isolate; xhci `PASS rs=0`. Ping before **5/5**. First and second `ssh … true`: SUCCESS **PASS**; `new session`; **no** `Sending command`; 124. Serial: **CHANNEL_OPEN session PASS** → **CHANNEL miss CLOSE**. Ping after **1/3**. **login OPEN**.

**Pack (do not flash):** KERNEL `bc4a4a5a69a6e972ffff18cd592688a53141ff29f49d7f748dae7eb2ae4f7434`  
sshd `ca9d8ad79f4d6981864f2b460b89a608c5db44c90463813823ee10b5f968d54a`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `68223e54af12d741089b00458142b67a7d4eaff6195b75a1adb8f48b3b7cd91d`.

---

## Prior cut — **v0.1.181** (do not flash) · OPEN_CONFIRM wait after 180 flake

**0.1.181 host (glass 161810 / 161815 · lab `10.200.125.103` · ssh UTC 212009):** title **v0.1.181**; isolate; xhci `PASS rs=0 iman=1 irq=PASS ccs=1`; hold4 `inj=675 tx=0 lnk=1`; hold5 `own=16 rok=0 fovw=1`; hold7 `IP 10.200.125.50 :22`; hold8 `DoD A=OPEN B=OPEN`. Ping before: 4/5 (20% loss; one 372 ms). arping 4 replies MAC `2C:56:DC:0B:6A:13`. First `ssh … true` (`timeout` 45): SUCCESS **PASS**; channel `new session`; **no** `Sending command: true`; SIGTERM 124. Second `ssh … true`: banner + NEWKEYS + ACCEPT + continue-list + offering key; **no** PK_OK; 124. Ping after: **3/3** (slow 555–1050 ms). **This cut's ask FAIL this boot.** **login OPEN**. Dual DoD **A/B OPEN**.

**Pack (do not flash):** KERNEL `08e89f70eabb0877b09c23838ad4924a42f47e8bb6093deb642acdfe6519aa31`  
sshd `6150449bdfd2f9a37c993f7010804496890599a88a580bcc73f94b796d526d12`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `8a284b44179e4f2284db83344a9ab837d76eec44e4ee08eca6db5aaa4eddae1f`.

---

## Prior cut — **v0.1.180** (do not flash) · exec stub flush after SUCCESS

**0.1.180 re-test (glass 143315 / 143319 · lab `10.200.125.103`):** title **v0.1.180**; isolate; xhci `PASS rs=0 iman=1 irq=PASS ccs=1`; hold4 `inj=1279 tx=0 lnk=1`; hold5 `own=16 rok=0 fovw=1`; hold7 `IP 10.200.125.50 :22`; hold8 `DoD A=OPEN B=OPEN`. Ping 5/5 (~4 ms, last 578 ms). arping 2/4 MAC `2C:56:DC:0B:6A:13`. First `ssh … true` (`timeout` 45): SUCCESS **PASS**; **no** `Sending command: true`; SIGTERM 124. Second `ssh … true`: SUCCESS **PASS** + `Sending command: true` then SIGTERM 124 (no `exit-status`). Ping after: **0/3**. **This cut's ask FAIL this boot.** **login OPEN**. Dual DoD **A/B OPEN**.

**0.1.180 earlier probe (glass 130127 / 130131):** same stamp. First `ssh … true` **exit 0** (~18 s) **PASS** that probe. Second `ssh` hung after PK_OK (timeout 124). **login OPEN**.

**Second `ssh` leftover (both probes):** stay-open `return 2` leaves the first fd without TCP CLOSE. **login OPEN**.

**Pack (do not flash):** KERNEL `11a9f32836b21da437c6065297a352ceecd299b8814c60d7790ad82c4a140b1e`  
sshd `ddaa8a2c18554f923ed3c78a66e53d05777c9bf0adb10ebb4715e9c7d6c40502`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `5aaa62588bb70856151398a2cdcee853fdc33409b048968e3e6fccc97f0d0f05`.

---

## Prior cut — **v0.1.179** (do not flash) · RFC 4254 after SUCCESS

**0.1.179 host:** SUCCESS **PASS**; `Sending command: true` then timeout 124. OPEN_CONFIRM reached client; exec CHANNEL_SUCCESS dropped (TCP CLOSE, no flush). Glass 111905/111910. **login OPEN**.

**Pack (do not flash):** KERNEL `fca03a64cd21fb7c4b31818a0b26fc457d2886e0089b30b7a91548ba8a3ad162`  
sshd `cd1973af32613c0f92dbbbd0d41854ab3ffdff991cff21ae9dcc0ddd1a4342cc`  
rtl `b4ac65aad72b42a344ab39f7a76bc961b7533127d2649003dad83c0ef5a7b9bc`  
xhci `7d22d214788b1bd3f13f63d182c3f51b9c5556fbf4f0d400ac61c75edd4760f0`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `f098c827f7ac3a66c35fff5d023c429d0246204fe427d55aa9c69ead9bd527f7`.

---

## Prior cut — **v0.1.178** (do not flash) · SUCCESS after PK_OK

**0.1.178 host:** `Authenticated … using "publickey"` **PASS**. Hung at session channel. Glass 095502/095511. **login OPEN**.

---

## Prior cut — **v0.1.177** (do not flash) · USERAUTH 128-miss

**0.1.177 host:** `Server accepts key` **PASS**. SUCCESS OPEN. Glass 214007/214016.

---

## Prior cut — **v0.1.176** (do not flash) · USERAUTH miss stay-open

**0.1.176 host:** second banner **PASS**; continue-list **PASS** (second ssh); no PK_OK. Glass 184011/184014.

---

## Prior cut — **v0.1.175** (do not flash) · publickey PK_OK + SUCCESS

**0.1.175 host:** first ssh ACCEPT then hang; second banner TIMEOUT. Glass 171630/171637.

---

## Prior cut — **v0.1.174** (do not flash) · USERAUTH_FAILURE

**0.1.174 host:** `Authentications that can continue: publickey` **PASS**. `Permission denied (publickey)`. Glass 163643/163646.

---

## Prior cut — **v0.1.173** (do not flash) · AEAD packet_length % 8

**0.1.173 host:** `SSH2_MSG_SERVICE_ACCEPT received` **PASS** (first and second `ssh`). No MAC incorrect. Timeout after ACCEPT. Glass 151749/151753.

---

## Prior cut — **v0.1.172** (do not flash) · RFC 4253 K2 expand

**0.1.172 host:** NEWKEYS both ways **PASS** (first and second `ssh`). Then `padding error: need 28 block 8 mod 4` + `MAC incorrect`. No `SERVICE_ACCEPT`. Ping/arping 5/5. Glass 141116/141122.

**This cut:** K2 = HASH(K||H||K1). Dual DoD **A/B OPEN**.

| Hold | Glass |
|------|--------|
| title | `STATUS (static) v0.1.172` |

KERNEL `dfad0c62f3959338ba1b8312eb3caf3c6791eb945dd976ecad9e4f8bfb5b9afd`  
sshd `975f2b11f6be84a9a51998c09ad53c124cf884b2aba3efa50e6a76b880d2866d`.

---

## Prior cut — **v0.1.171** (do not flash) · NEWKEYS seq + ACCEPT hog

**0.1.170 host:** NEWKEYS both ways **PASS**, no `SERVICE_ACCEPT`, no `MAC incorrect`, timeout 124. Second `ssh`/`nc` banner-exchange **TIMEOUT** (listen hog). Ping stayed up.

**This cut:** consume type 30 from `have` (no RX-seq double-count); arm AEAD only after type 21; `recv_pkt` fail-closed on bad length/MAC; type-5 miss returns 2 so park ACCEPT lives. Dual DoD **A/B OPEN**. SERVICE_ACCEPT ≠ login.

| Hold | Glass |
|------|--------|
| title | `STATUS (static) v0.1.171` |
| 1 | `M0 OK dash SKIP isolate` |
| 2 | TE |
| 3 | `UDX xhci …` (scratchpad SKIP or progress; Dual DoD A **OPEN**) |
| 4 | `UDX inj= tx= lnk=` |
| 5 | `UDX … own rok fovw` |
| 6 | `UDX mac_rclm` |
| 7 | `IP 10.200.125.50 :22` |
| 8 | `DoD A=OPEN B=OPEN` |

Soft≠product. Dual DoD A **OPEN** (`need=usbcmd_rs`, never RS=1). Dual DoD B **OPEN** for **interactive login**. Type 31 + good signature ≠ login. **0.2.0** reserved.

**Pack:** `make hwtest-img` → `build/greenjade-hwtest.img` (ESP extract MATCH).  
KERNEL `2e3db0ba0ee3393eb7d46e066bb825ff6db5c3d710515f61fa22cf364f9a3958`  
sshd `d3e060e2fcb5c1a805660c066558d0f82885d8f1c29a4391e58e7998b57c7e54`  
rtl `511e8bfefce67ed699c1d1848f4c8ac6cce2bb3f49c69c04e3a077f925936e66`  
xhci `c5a8d00006921be3fd5bff64c4344d5c3cc9ba18b309c8c1f0039492f650ed92`  
sh `11e79a696d0d74598263bc4b64c7ad6a0f3f4d4486c6c386f80768beb96f210b`  
img `38c99fc1e2561494967b04f7d5dc2d823a3265bc02c45b64b69b8c7c4ce14a65`.
**Flash (destroys the stick):** `sudo ./scripts/install-hwtest-usb.sh /dev/sdX`  
**DUT:** G752VT · Secure Boot **off** · UEFI USB · lab **10.200.125.50**.

| Check | What to look for | Close? |
|-------|------------------|--------|
| Flash identity | GOP **STATUS (static) v0.1.171** | Confirm cut |
| Isolate | hold1 **`M0 OK dash SKIP isolate`** · no `USER KILL` / `FAULT PINNED` | Wiring |
| Lab IP | hold7 **`IP 10.200.125.50 :22`** (not `0.0.0.0`) | Wiring |
| xhci **A** | hold3 **`UDX xhci PASS rs=0 … ccs=1`** (re-fly); **RS off** | Dual DoD A **OPEN** |
| Live net | hold4 inj/tx climb · `lnk=1` | Soft |
| L3 | `ping` / `arping 10.200.125.50` | **PASS** on **0.1.171** (ping 5/5 @ ~3.5 ms; arping 5/5 MAC `2C:56:DC:0B:6A:13`) |
| Banner | `ssh` → `SSH-2.0-GreenJade_sshd` | **PASS** on **0.1.171** first and second `ssh`. **login OPEN** |
| KEXINIT | `ssh -v` — server type 20 | **PASS** on **0.1.171** |
| ECDH_REPLY | type 31 | **PASS** on **0.1.171** |
| Signature | **no** `incorrect signature`; host key `ssh-ed25519 SHA256:dBeUIEbH…` | **PASS** on **0.1.171**; **login OPEN** |
| NEWKEYS | `SSH2_MSG_NEWKEYS sent` + `received` | **PASS** on **0.1.171** (first and second `ssh`) |
| STATE pane | `M0 OK` **before** `sshd :22` · last `idle` (not `M0 IDLE` after sshd) | Wiring (not in 084406/084415 crop) |
| SERVICE_ACCEPT | `SSH2_MSG_SERVICE_ACCEPT received` · no `MAC incorrect` | **OPEN** on **0.1.171** — no ACCEPT line; no `MAC incorrect`. **login OPEN** |
| Second banner | after first `ssh`, second `ssh` still sees `SSH-2.0-GreenJade_sshd` | **PASS** on **0.1.171** (listen hog **fixed**); **login OPEN** |
| Optional | `Authentications that can continue` is **not** this cut. **Not** login. | Soft |

**Not claimed:** interactive dash, interactive SSH login, Dual DoD A/B close, bar3, **0.2.0**. Soft≠product. **G-AC-1.**

```sh
make hwtest-img
ls -lh build/greenjade-hwtest.img
./scripts/gj-image-version.sh --report
sudo ./scripts/install-hwtest-usb.sh /dev/sdX
```

---

## Prior cut — **v0.1.140** (do not flash) · isolate in KERNEL, GOP flood

**0.1.139 glass:** `#PF I=1` RIP=`0x20000FA0` → kernel `#UD` **HALTED**.

**0.1.140 glass + host:** stamp matches; `USER KILL #PF I=1` (pre-isolate pack) then isolate pack still **0.1.140**; kernel stayed up; ping/arping/banner **PASS**. LOG wiped by park-live heartbeats.

---

## Prior cut — **v0.1.139** · dash as `/bin/sh` (do not flash)

**Pack:** `make hwtest-img` → `build/greenjade-hwtest.img`.  
**Flash (destroys the stick):** `sudo ./scripts/install-hwtest-usb.sh /dev/sdX`  
**DUT:** G752VT · Secure Boot **off** · UEFI USB · lab **10.200.125.50**.

| Check | What to look for | Close? |
|-------|------------------|--------|
| Flash identity | GOP **STATUS (static) v0.1.139** · serial `main: image version=0.1.139` | Confirm cut |
| Product `/bin/sh` | serial `shell: live dash /bin/sh` · ESP `rootfs-full/bin/sh` is `dash.elf` | Wiring only |
| execve `/bin/sh` | serial `linux: execve /bin/sh dash embed` if something execs sh | Soft |
| L3 net | `arping` / `ping 10.200.125.50` | Already proven on 0.1.138 |
| sshd | `nc -v -w 3 10.200.125.50 22` → `SSH-2.0-GreenJade_sshd` | Banner proven; **login OPEN** |

**Not claimed:** interactive dash on GOP/serial, Dual DoD A/B close, bar3. Soft≠product. **G-AC-1.** Old smoke shell is `bin/greenjade-shell` only.

```sh
make dash
make hwtest-img
ls -lh build/greenjade-hwtest.img
./scripts/gj-image-version.sh --report
sudo ./scripts/install-hwtest-usb.sh /dev/sdX
```

---

# Hardware test — operator pack (2026-08-01, historical)

**Goal:** boot GreenJade freestanding **and/or** Linux inventory on real DUT; capture evidence; do not over-claim.  
**Product tip:** `makefile_max=26800` (Wave 126). **bar3 remains OPEN** (Steam client / Top-50 still not claimed).  
**G752VT Linux inventory (2026-08-01):** **PASS** on `GJ-LNX-ESP`. Freestanding in-kernel xHCI/rtl = **soft scaffold** (high-water ~stage 15); **product priority = Linux ABI + virtio T0**, not freestanding class-driver thrash. Map: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

## DUT #1 — ASUS ROG G752VT-RH71

| Field | Value |
|-------|--------|
| **Model** | ASUS ROG **G752VT-RH71** (17.3″ gaming laptop, ~2015–2016) |
| **BIOS** | **G752VT.307** (confirmed on DUT run) |
| **CPU** | Intel **Core i7-6700HQ** (Skylake, 4C/8T) — **x86_64** OK |
| **RAM** | **16 GiB** DDR4 (upgradeable to 64 GiB) |
| **GPU** | NVIDIA **GeForce GTX 970M** 3 GiB + Intel HD 530 (Optimus-era) |
| **Display** | 17.3″ FHD — firmware **GOP** only for freestanding console |
| **Storage** | M.2 NVMe + 2.5″ SATA common — HCL **T1 soft probes** only, **not** full product drivers |
| **USB HC** | PCI **`0000:00:14.0`** · **`8086:a12f`** Intel 100 Series USB 3.0 xHCI |
| **USB ports** | **USB 3.0 only** (no pure USB 2.0 jacks) — SuperSpeed path required for stick log |
| **IOMMU** | **Translated** under Linux (VT-d) — freestanding DMA must match |
| **Net** | Realtek/Intel wired + Wi‑Fi — **not** virtio; no LAN product path on first freestanding boot |
| **Audio** | Realtek HDA — freestanding multi-stream smoke ≠ laptop codec product |
| **Firmware** | **UEFI** (Aptio); Secure Boot present — **disable** for GreenJade / Linux sticks |

### USB topology (Linux inventory PASS — 2026-08-01)

| Hub | Layout | Devices of interest |
|-----|--------|---------------------|
| **Bus 1 HS** 16 ports | Internal HS tree | **1-4** webcam · **1-8** ASUS HID · **1-9** Intel BT · **1-10** keyboard |
| **Bus 2 SS** 10 ports | SuperSpeed tree | Stick on **SS port 5**: **`13fe:6400`**, **usb-storage BOT** |

Prefer a physical USB3 jack that enumerates as **SS port 5** for freestanding MSC log media. Full note: [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md).

### Firmware keys (ASUS ROG G752)

| Action | Key (typical) |
|--------|----------------|
| Boot menu (one-shot) | **Esc** (or **F8** on some ROG SKUs) at power-on |
| Setup (BIOS/UEFI) | **F2** at power-on |
| Boot override | Setup → **Boot** → USB / UEFI: USB name |

**Required settings before USB boot:**

1. **Secure Boot → Disabled** (neither GreenJade nor the Alpine inventory image is MS-signed for this path).
2. Prefer **UEFI** boot of the stick (not CSM-only “Legacy USB” unless Multiboot ISO needs it).
3. If present: **Fast Boot → Disabled** (so Esc/F2 work reliably).
4. **Launch CSM** / Legacy: leave **Off** for pure UEFI USB; if freestanding UEFI stalls, try **live ISO** with CSM **Enabled** for Multiboot hybrid.
5. Discrete GPU: leave default; first bring-up only needs **UEFI GOP** (do not expect GTX 970M 3D).

### Freestanding vs Linux (honest)

| Surface | Status on G752VT (2026-08-01) |
|---------|-------------------------------|
| **Linux inventory stick** | **PASS** — probes write report to **`GJ-LNX-ESP`**, machine poweroffs |
| **Freestanding xHCI/net** | **Soft scaffold** (~stage 15 GET_CONFIG; rtl8168 lab) — **not** T1 product bar |
| **Product direction** | **Linux ABI + virtio T0** + later userspace/DDI drivers (no GPL ship) |
| **IOMMU** | Linux: **translated**; product DMA = **window caps** (P-DMA) |
| **UEFI / M0** | Freestanding pack ready; confirm on panel / ESP logs per run |
| **Serial** | **No DB9** on this chassis — use panel bars + remount stick logs |
| **Steam / bar3** | Media **READY** on freestanding stick only — **bar3 OPEN**; Linux inventory **≠** bar3 |

**Hard rule:** Linux `RESULT: PASS` and freestanding soft greps are **not** Steam client, **not** Deck Top 50, **not** full T1 product HCL close.

### Next engineering targets (do not claim done)

1. **Linux ABI Option C** — apps on virtio (sshd/netstackd/shell); cold personality to userspace.  
2. **DDI** — PCI/IRQ/DMA caps for userspace / out-of-tree dual-license drivers.  
3. **G752 inventory** — keep Linux stick as topology oracle (SS port 5, a12f, translated IOMMU).  
4. **bar3** — remains **OPEN** until client + matrix evidence.  
5. Freestanding xHCI/rtl — optional soft only; stop treating panel stages as product DoD.

---

## Two sticks — clear operator paths

**Use separate sticks or re-image between modes.** Labels differ; do not expect `GJ-PERSIST` on the Linux image or `GJ-LNX-ESP` on freestanding.

### Stick A — GreenJade freestanding (`GREENJADE` + `GJ-PERSIST`)

```sh
cd /home/jay/Documents/knano   # or your clone
ls -lh build/greenjade-hwtest.img
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL   # pick stick; NOT sda/sdb RAID
# DESTROYS the entire device:
sudo make install-hwtest-usb DEV=/dev/sdX
```

**On DUT:**

1. AC power preferred; insert stick (USB3 jack).  
2. Power on → spam **Esc** → **UEFI: … USB**.  
3. Watch panel: `GJ-EFI` / progress bars; keyboard may be soft-only.  
4. Expect freestanding **stage 11** hang on xHCI control until that target lands — still collect whatever logs exist.

**After run — collect freestanding logs:**

```sh
sudo umount /run/media/$USER/* 2>/dev/null; true
sudo mkdir -p /mnt/gj-esp /mnt/gj-persist
sudo mount -L GREENJADE /mnt/gj-esp    # or /dev/sdX1
cat /mnt/gj-esp/EFI/GREENJADE/BOOT.LOG   # UEFI loader tee (pre-EBS)
cat /mnt/gj-esp/EFI/GREENJADE/KLOG.TXT   # kernel xHCI MSC stick log (if path up)
sudo umount /mnt/gj-esp
sudo mount -L GJ-PERSIST /mnt/gj-persist
ls /mnt/gj-persist/logs /mnt/gj-persist/steam
cat /mnt/gj-persist/steam/STATUS         # READY = media only — bar3 OPEN
sudo umount /mnt/gj-persist
```

Optional lab-host SSH enable (serial bridge host, not GreenJade LAN on this laptop):

```sh
sudo make hwtest-ssh-setup
```

### Stick B — Linux inventory (`GJ-LNX-ESP` + `GJ-LNX-ROOT`)

When freestanding is stuck (stage 11) or you need a fresh PCI/USB map:

```sh
cd /home/jay/Documents/knano
sudo make linux-hwtest-img                 # build/linux-hwtest.img (~2 GiB)
lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL
sudo make install-linux-hwtest DEV=/dev/sdX
```

**On DUT:**

1. Secure Boot **off** → UEFI boot stick.  
2. Console: `GreenJade Linux HWTEST — starting full loop` (or similar).  
3. Wait until machine **powers off** (leave stick inserted).  

**After poweroff — collect Linux report:**

```sh
sudo mkdir -p /mnt/esp
sudo mount -L GJ-LNX-ESP /mnt/esp
ls /mnt/esp/GJ-HWTEST/reports/
cat /mnt/esp/GJ-HWTEST-RESULT.txt
cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt
# Note: xHCI 8086:a12f, Bus1 HS map, Bus2 SS port 5 stick 13fe:6400 BOT, IOMMU translated
sudo umount /mnt/esp
```

This is **Linux dmesg/lspci/lsusb evidence**, not freestanding product PASS. See `scripts/linux-hwtest/README.md`.

### Fallback — Multiboot live ISO (if freestanding UEFI stalls)

```sh
sudo dd if=build/greenjade-live.iso of=/dev/sdX bs=4M status=progress conv=fsync
# or QEMU check first:
./scripts/run-live-iso.sh
```

---

## What is ready on this lab host

| Artifact | Path | Notes |
|----------|------|--------|
| Product kernel | `build/greenjade.elf` (~8.5 MiB) | Multiboot **smoke PASS** (M0 OK, UD=0, quick-keys) |
| UEFI loader | `build/GreenJade.efi` (~55 KiB) | Loads `KERNEL.ELF`; OVMF may #UD after handoff — real DUT TBD |
| **Freestanding HW USB image** | `build/greenjade-hwtest.img` (**2.0 GiB**) | GPT: **`GREENJADE`** + **`GJ-PERSIST`** (logs/ssh/**steam READY**) |
| **Linux inventory image** | `build/linux-hwtest.img` | GPT: **`GJ-LNX-ESP`** + **`GJ-LNX-ROOT`** — G752VT loop **works** |
| Hybrid live ISO | `build/greenjade-live.iso` (~92 MiB) | Multiboot2 + EFI fallback |
| ESP stage tree | `build/esp/` | `BOOTX64.EFI` + `KERNEL.ELF` + user ELFs |
| SSH keys | `build/hwtest-keys/id_ed25519` | Freestanding GJ-PERSIST / lab host |
| Steam media | `build/steam-stage/steam/STATUS` = **READY** | Media bootstrap only — **≠ client launch** |

**Preflight (this host, 2026-08-01):**

- Multiboot: `timeout 180 scripts/run-qemu.sh build/greenjade.elf` → **gj-quick-keys PASS**.
- Steam: `./scripts/steam-bar3-check.sh` → **READY** (**bar3 still OPEN**).
- Linux hwtest loop: `make linux-hwtest-img` → DUT reports on **`GJ-LNX-ESP`** (**PASS inventory** on G752VT).
- Freestanding on DUT: **stage 11** control after address still the freestanding gap.

**No USB stick is plugged into this lab box by default.** Plug the stick before write; never target RAID `sda`/`sdb`.

---

## Optional serial (rare on this laptop)

G752VT has **no convenient COM**. If you later have USB-TTL on another DUT:

```sh
mkdir -p /tmp/gj-hw-logs
picocom -b 115200 /dev/ttyUSB0 | tee /tmp/gj-hw-logs/serial-$(date -u +%Y%m%dT%H%MZ).txt
./scripts/gj-product-summary.sh /tmp/gj-hw-logs/serial-….txt
./scripts/gj-quick-keys.sh /tmp/gj-hw-logs/serial-….txt
```

### Expect (freestanding greps, when path runs)

- UEFI: `GJ-EFI` then `KERNEL.ELF loaded` / `M0 OK`
- Multiboot ISO: `source=MULTIBOOT2` then `M0 OK`
- Product: `sshd: live spawn PASS`, `scsi_mid: live spawn PASS`, `shell: live spawn PASS` (virtio lab; laptop NIC may miss)
- Soft: `TRAP #UD` count **0** (or note any #UD)
- xHCI: stage markers through address; **stage 11 control** is the known freestanding fail point on this HC

---

## Steam honesty (do not over-claim)

| Media / result | Meaning |
|----------------|---------|
| `steam/STATUS=READY` | Bootstrap tree on freestanding stick |
| Linux inventory **PASS** | Host Linux saw hardware — **≠** freestanding MSC/HID product |
| **bar3 OPEN** | Client not launched; Deck Top 50 still **NOT-TRIED × 50** |
| Product lamps | **0** |

Do **not** mark matrix rows PASS from media READY or Linux inventory alone. See [STEAM_HWTEST.md](STEAM_HWTEST.md), [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md).

## Success criteria

**Minimum — freestanding “DUT boot attempted”:**

- [ ] Stick boots firmware on **G752VT** (UEFI USB or Multiboot hybrid)
- [ ] Evidence of loader/kernel progress (panel STATUS capture and/or ESP `BOOT.LOG` / `KLOG.TXT`)
- [ ] Note hang stage if any (**stage 11** expected until `a12f` control lands)
- [ ] Record: model **G752VT**, BIOS **G752VT.307** (or current F2 → Main), RAM, boot path, USB port used

**Minimum — Linux inventory (already achieved 2026-08-01; re-run after HW changes):**

- [ ] `linux-hwtest-img` written; DUT poweroffs after loop
- [ ] `GJ-LNX-ESP` has `GJ-HWTEST-RESULT.txt` + `reports/LATEST.txt`
- [ ] Report still shows **`8086:a12f`**, SS stick port, IOMMU translated (or document deltas)

Stretch (product still open):

- [ ] Freestanding past stage 11 control on **`a12f`**
- [ ] Soft greps NVMe / AHCI / USB HC (≠ product storage)
- [ ] BOT MSC stick log under freestanding
- [ ] Steam tree on GJ-PERSIST (media only — **not** bar3)

## Rebuild commands

```sh
make greenjade.efi
make build/greenjade.elf
make userland
make steam-stage          # STATUS=READY if build/steam-tree present
make hwtest-img           # → build/greenjade-hwtest.img
make live-iso             # → build/greenjade-live.iso
sudo make linux-hwtest-img
./scripts/steam-bar3-check.sh
timeout 180 scripts/run-qemu.sh build/greenjade.elf
```

## Honesty / open bars

- Soft continuum high-water **makefile_max=26800** ≠ runtime product complete  
- Soft deepen **retgradientangle** / **retblendangle** ≠ bar3  
- Linux inventory PASS ≠ freestanding xHCI product  
- Real-DUT freestanding targets: **SS control a12f**, **IOMMU**, **port priority**  
- **bar3 OPEN** — Steam client / Top-50 still not claimed  
- Priorities: Security → Performance → Portability → Readability  

## Related docs

- [HCL.md](HCL.md) — tiers + install checklist + G752VT map summary  
- [G752VT_LINUX_HWTEST.md](G752VT_LINUX_HWTEST.md) — USB/xHCI map · freestanding vs Linux · dual sticks  
- [STEAM_HWTEST.md](STEAM_HWTEST.md) — Steam option 2/3 media  
- [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) — bar3 ceiling  
