# NOTORYX Ham firmware

MeshCore firmware for licensed US amateur radio operators (FCC Part 97): the transmitting station's call sign on every frame, no encryption on air, and 33 cm / 70 cm only.

> [!WARNING]
> **Use only after a great deal of coordination.** This firmware transmits automatically on shared amateur bands, and every repeater relays under its operator's licence. Before any node goes on the air, coordinate with your frequency coordinator, the local band plan and every operator in your area. Read **[AMATEUR_OPERATION.md](AMATEUR_OPERATION.md)** first.
>
> It is in development and has not been tested on the air.

## What changes from MeshCore

Everything sits behind one build flag, `AMATEUR_RADIO`, which `platformio.ini` turns on for every board. With it off the code is stock MeshCore, so upstream updates merge cleanly.

| Part 97 rule | What the firmware does |
|---|---|
| §97.119 station ID | Every frame it transmits, including relays, ends with its call sign (8 bytes, zero padded) and is marked payload version 2. |
| No ID, no transmission | The call sign is the first word of the node name (`W1AW`, `W1AW-7`, `KD2ABC Hilltop`). Without a valid US call sign the radio drops frames instead of sending them. |
| §97.113(a)(4) no obscured content | Content goes out as plain text, padded to 16-byte blocks. The 2-byte MACs and Ed25519 signatures stay. |
| §97.301 bands | 420–450 and 902–928 MHz only, enforced by every setter and on prefs load. Default: 906.875 MHz, BW 250, SF 10, CR 5. |
| Passwords | Repeater, room and sensor logins send a one-time HMAC proof instead of the password. Remote `password`, `guest.password` and `prv.key` commands are refused over RF. |
| Separate network | Frames without a station ID (ordinary MeshCore) are ignored, so ham radios don't relay Part 15 traffic. |

Text messages are limited to 152 bytes (the station ID takes 8 bytes of every frame). The KISS modem target bypasses the mesh stack and sends no station ID: don't use it for amateur operation.

## Boards

Built and checked: Heltec V4, Heltec T114, RAK4631, RAK3401 (WisMesh 1W Booster), SenseCAP T1000-E, Station G2 and Station G3, in companion (BLE or USB) and repeater roles. The Station G2/G3 BLE companion is 5–6 KB too big for its partition, as in stock MeshCore v1.18.0; use the USB companion on those boards.

```bash
pio run -e heltec_v4_companion_radio_ble                              # one target
FIRMWARE_VERSION=v1.18.0-ham bash build.sh build-firmware heltec_v4_repeater   # versioned files in out/
```

## First boot

A freshly flashed radio **won't transmit**. Set its name to start with your call sign: in the NOTORYX Core Ham app (**Settings → Licensed operator → Set to …**), or over USB serial:

```
set name W1AW-3 Hilltop
set radio 906.875,250,10,5
reboot
```

Repeater and room-server passwords can only be changed over USB serial (`password …`), never over the air.

## Tests

- `pio test -e native`: call signs, bands, plain-text framing and login-proof rules (`test/test_amateur_station`).
- [`test/mcsim`](test/mcsim): two-hop simulator runs for relay, station ID, the transmit lock and logins.

## App

Use **[NOTORYX Core Ham](https://github.com/TheLogsAreScreaming/NOTORYX-Core-Ham)**: it reads ham frames, offers only ham-band presets and walks you through licensed operator setup. Ordinary MeshCore apps can connect but will offer Part 15 settings the firmware refuses.

---

## About MeshCore

MeshCore is a lightweight, portable C++ library that enables multi-hop packet routing for embedded projects using LoRa and other packet radios. It is designed for developers who want to create resilient, decentralized communication networks that work without the internet.

## 🔍 What is MeshCore?

MeshCore now supports a range of LoRa devices, allowing for easy flashing without the need to compile firmware manually. Users can flash a pre-built binary using tools like Adafruit ESPTool and interact with the network through a serial console.
MeshCore provides the ability to create wireless mesh networks, similar to Meshtastic and Reticulum but with a focus on lightweight multi-hop packet routing for embedded projects. Unlike Meshtastic, which is tailored for casual LoRa communication, or Reticulum, which offers advanced networking, MeshCore balances simplicity with scalability, making it ideal for custom embedded solutions, where devices (nodes) can communicate over long distances by relaying messages through intermediate nodes. This is especially useful in off-grid, emergency, or tactical situations where traditional communication infrastructure is unavailable.

## ⚡ Key Features

* Multi-Hop Packet Routing
  * Devices can forward messages across multiple nodes, extending range beyond a single radio's reach.
  * Supports up to a configurable number of hops to balance network efficiency and prevent excessive traffic.
  * Nodes use fixed roles where "Companion" nodes are not repeating messages at all to prevent adverse routing paths from being used.
* Supports LoRa Radios – Works with Heltec, RAK Wireless, and other LoRa-based hardware.
* Decentralized & Resilient – No central server or internet required; the network is self-healing.
* Low Power Consumption – Ideal for battery-powered or solar-powered devices.
* Simple to Deploy – Pre-built example applications make it easy to get started.

## 🎯 What Can You Use MeshCore For?

* Off-Grid Communication: Stay connected even in remote areas.
* Emergency Response & Disaster Recovery: Set up instant networks where infrastructure is down.
* Outdoor Activities: Hiking, camping, and adventure racing communication.
* Tactical & Security Applications: Military, law enforcement, and private security use cases.
* IoT & Sensor Networks: Collect data from remote sensors and relay it back to a central location.

## 🚀 How to Get Started

- Watch the [MeshCore QuickStart Playlist](https://www.youtube.com/watch?v=iaFltojJrAc&list=PLshzThxhw4O4WU_iZo3NmNZOv6KMrUuF9) by The Comms Channel
- Watch the [MeshCore Technical Presentation](https://www.youtube.com/watch?v=OwmkVkZQTf4) by Liam Cottle.
- Read through our [Frequently Asked Questions](./docs/faq.md) and [Documentation](https://docs.meshcore.io).
- Flash the MeshCore firmware on a supported device.
- Connect with a supported client.

For developers:

- Install [PlatformIO](https://docs.platformio.org) in [Visual Studio Code](https://code.visualstudio.com).
- Clone and open the MeshCore repository in Visual Studio Code.
- See the example applications you can modify and run:
  - [Companion Radio](./examples/companion_radio) - For use with an external chat app, over BLE, USB or Wi-Fi.
  - [KISS Modem](./examples/kiss_modem) - Serial KISS protocol bridge for host applications. ([protocol docs](./docs/kiss_modem_protocol.md))
  - [Simple Repeater](./examples/simple_repeater) - Extends network coverage by relaying messages.
  - [Simple Room Server](./examples/simple_room_server) - A simple BBS server for shared Posts.
  - [Simple Secure Chat](./examples/simple_secure_chat) - Secure terminal based text communication between devices.
  - [Simple Sensor](./examples/simple_sensor) - Remote sensor node with telemetry and alerting.

The Simple Secure Chat example can be interacted with through the Serial Monitor in Visual Studio Code, or with a Serial USB Terminal on Android.

## ⚡️ MeshCore Flasher

We have prebuilt firmware ready to flash on supported devices.

- Launch https://meshcore.io/flasher
- Select a supported device
- Flash one of the firmware types:
  - Companion, Repeater or Room Server
- Once flashing is complete, you can connect with one of the MeshCore clients below.

## 📱 MeshCore Clients

**Companion Firmware**

The companion firmware can be connected to via BLE, USB or Wi-Fi depending on the firmware type you flashed.

- Web: https://app.meshcore.nz
- Android: https://play.google.com/store/apps/details?id=com.liamcottle.meshcore.android
- iOS: https://apps.apple.com/us/app/meshcore/id6742354151?platform=iphone
- NodeJS: https://github.com/liamcottle/meshcore.js
- Python: https://github.com/fdlamotte/meshcore-cli

**Repeater and Room Server Firmware**

The repeater and room server firmware can be set up via USB in the web config tool.

- https://config.meshcore.io

They can also be managed via LoRa in the mobile app by using the Remote Management feature.

## 🛠 Hardware Compatibility

MeshCore is designed for devices listed in the [MeshCore Flasher](https://meshcore.io/flasher)

## 📜 License

MeshCore is open-source software released under the MIT License. You are free to use, modify, and distribute it for personal and commercial projects.

## Contributing

Please submit PR's using 'dev' as the base branch!
For minor changes just submit your PR and we'll try to review it, but for anything more 'impactful' please open an Issue first and start a discussion. It is better to sound out what it is you want to achieve first, and try to come to a consensus on what the best approach is, especially when it impacts the structure or architecture of this codebase.

Here are some general principles you should try to adhere to:
* Keep it simple. Please, don't think like a high-level lang programmer. Think embedded, and keep code concise, without any unnecessary layers.
* No dynamic memory allocation, except during setup/begin functions.
* Use the same brace and indenting style that's in the core source modules. (A .clang-format is probably going to be added soon, but please do NOT retroactively re-format existing code. This just creates unnecessary diffs that make finding problems harder)

Help us prioritize! Please react with thumbs-up to issues/PRs you care about most. We look at reaction counts when planning work.

### Running unit tests

To run unit tests, run the following command:

```bash
pio test --environment native --verbose
```

## Road-Map / To-Do

There are a number of fairly major features in the pipeline, with no particular time-frames attached yet. In very rough chronological order:
- [X] Companion radio: UI redesign
- [X] Repeater + Room Server: add ACL's (like Sensor Node has)
- [X] Standardise Bridge mode for repeaters
- [ ] Repeater/Bridge: Standardise the Transport Codes for zoning/filtering
- [X] Core + Repeater: enhanced zero-hop neighbour discovery
- [ ] Core: round-trip manual path support
- [ ] Companion + Apps: support for multiple sub-meshes (and 'off-grid' client repeat mode)
- [ ] Core + Apps: support for LZW message compression
- [ ] Core: dynamic CR (Coding Rate) for weak vs strong hops
- [ ] Core: new framework for hosting multiple virtual nodes on one physical device
- [ ] V2 protocol spec: discussion and consensus around V2 packet protocol, including path hashes, new encryption specs, etc

## 📞 Get Support

- Report bugs and request features on the [GitHub Issues](https://github.com/meshcore-dev/meshcore/issues) page.
- Find additional guides and components on [my site](https://buymeacoffee.com/ripplebiz).
- Join [MeshCore Discord](https://meshcore.gg) to chat with the developers and get help from the community.
