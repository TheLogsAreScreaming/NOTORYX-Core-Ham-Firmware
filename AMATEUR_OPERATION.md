# Coordinate before you transmit

**NOTORYX Ham firmware is for licensed amateur radio operators. Put it on the air only after a great deal of coordination with the people and services around you.** A mesh is not one station: every node you flash, and every repeater, transmits automatically, on a shared band, under someone's licence.

## Why it needs coordination

- **The bands are shared.** In the US, 33 cm (902–928 MHz) belongs first to government radiolocation, and it is also used by industrial (ISM) equipment, location services and millions of licence-free Part 15 devices, including ordinary MeshCore and Meshtastic radios. 70 cm (420–450 MHz) carries repeaters, satellites, ATV and weak-signal work, and government radar has priority. Amateurs are secondary on both: we must not cause harmful interference.
- **It transmits by itself.** Repeaters relay automatically, and every relay is a transmission under that repeater's call sign. Its control operator is responsible for all of it.
- **The signal is wide and busy.** The default LoRa setting uses 250 kHz and sends regularly. In a band plan, that needs a slot that won't sit on top of someone else.
- **It is a separate network.** Ham nodes ignore normal MeshCore traffic, and normal MeshCore radios can't read ham traffic. Mixing the two in one area splits the mesh and confuses users.
- **Nothing is private.** Every message, including direct messages, is readable by anyone with a receiver.

## Before the first transmission

1. **Licence and call sign.** Every operator needs a US amateur licence (Technician or higher). Set the node name to start with your call sign; the radio won't transmit until you do.
2. **Talk to your frequency coordinator.** Contact your regional frequency coordinator or repeater council, and check the local band plan. Pick a coordinated channel. The 906.875 MHz default is a starting point, not a coordinated frequency.
3. **Agree on one setting.** Every operator in the area must use the same frequency, bandwidth, spreading factor and coding rate. Write it down and share it.
4. **Plan every repeater.** Each needs a licensed control operator, a known location, and a way to shut it down quickly, both remotely and in person.
5. **Check where you are.** Some areas are excluded or limited (47 CFR §97.303), for example parts of Colorado, Wyoming, Texas and New Mexico on 33 cm, and 420–430 MHz north of Line A near the Canadian border.
6. **Start small.** Test on a dummy load or at minimum power first. Use the least power that works (§97.313), and do an RF exposure evaluation (§97.13) before running higher power.
7. **Don't bridge.** Don't connect the ham mesh to Part 15 networks, MQTT or the internet without working through third-party traffic rules (§97.115).

This is not legal advice. The control operator of each station is responsible for what it transmits. Check the current text of 47 CFR Part 97 before you operate.
