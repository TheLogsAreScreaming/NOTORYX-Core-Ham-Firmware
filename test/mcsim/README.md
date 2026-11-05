# Amateur mode in the mcsim simulator

Two-hop tests for the Part 97 changes, run in [mcsim](https://github.com/usrflo/mcsim) at commit `a360002`. W1AW and KD2ABC can only hear each other through a repeater.

| Test | Expect |
|---|---|
| `ham_relay.yaml` + `ham_dm_and_channel.yaml` | The DM is readable on air ("DM 1 from W1AW"), frames are payload version 2, W1AW's frames end in `W1AW`, the repeater's relays end in `N0RPT`, and KD2ABC delivers the DM to its app. |
| `ham_relay.yaml` + `ham_channel.yaml` | The channel message "W1AW: CH 1 from W1AW" is relayed by N0RPT and received by KD2ABC. |
| `ham_no_call.yaml` + `ham_dm_and_channel.yaml` | The repeater is named "Hilltop" (no call sign): it hears the DM, logs `no valid call sign in node name, not transmitting`, and sends nothing. |
| `ham_login.yaml` + `ham_login_admin.yaml` | W1AW logs into repeater `N0RPT` and room server `N0RS Room` (mcsim sets both admin passwords to `admin` over serial): both log `Login success!`, and no frame on air contains `admin`. |
| `ham_login.yaml` + `ham_login_wrong.yaml` | The same with the password `wrong`: the repeater logs `Invalid password`, the room server `Incorrect room password`. |

## Run

```bash
git clone https://github.com/usrflo/mcsim.git && cd mcsim && git checkout a360002
git apply ../path/to/test/mcsim/mcsim.patch     # build amateur firmware; v1.18 companion shim
rm -rf MeshCore && mkdir MeshCore && git -C ../NOTORYX-Core-Ham-Firmware archive HEAD | tar -x -C MeshCore
cp ../path/to/test/mcsim/*.yaml examples/topologies/
cargo build --release
./target/release/mcsim run examples/topologies/ham_relay.yaml examples/topologies/ham_dm_and_channel.yaml --duration 2m --seed 1 -o trace.json
```

Each `PACKET` event in `trace.json` has `packet_hex`; its last 8 bytes are the transmitting station's call sign. After swapping firmware, run `cargo clean --release -p mcsim-firmware` first: archived files keep old timestamps, so the firmware won't otherwise recompile.

The simulator runs one firmware build, so it can't show that stock MeshCore frames are ignored. That is covered by the unit tests and needs a real-radio check.
