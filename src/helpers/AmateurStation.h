#pragma once

#include <stdint.h>
#include <MeshCore.h>

/*
 * NOTORYX amateur (FCC Part 97) station rules, shared by every node role.
 *
 *  - Station ID (97.119): the station's call sign is the leading word of its node
 *    name ("W1AW", "W1AW-7", "KD2ABC Hilltop"). It is sent, zero padded to
 *    AMATEUR_CALL_MAX bytes, at the end of every frame the station transmits.
 *  - Frequency privileges (97.301): transmit only inside 70cm or 33cm.
 */
namespace mesh {

class AmateurStation {
public:
  /**
   * \returns  true if the 'len' chars at 's' form a US amateur call sign:
   *    one or two letter prefix (K, N or W, optionally followed by a letter, or AA-AL),
   *    one district digit, then a one to three letter suffix. Upper or lower case.
   */
  static bool isUsCallsign(const char* s, int len);

  /**
   * \brief  copies the call sign that leads 'node_name' into 'dest', upper-cased and
   *    NUL terminated. The call sign ends at the first '-', ' ' or end of string.
   * \param  dest  at least AMATEUR_CALL_MAX + 1 bytes
   * \returns  length of the call sign, or 0 if the name doesn't start with one
   */
  static int callsignOf(const char* node_name, char* dest);

  static bool isValidNodeName(const char* node_name) {
    char call[AMATEUR_CALL_MAX + 1];
    return node_name != NULL && callsignOf(node_name, call) > 0;
  }

  /** \returns  true if 'mhz' lies in 420-450 MHz (70cm) or 902-928 MHz (33cm) */
  static bool isAmateurFreqMHz(float mhz);
  static bool isAmateurFreqKHz(uint32_t khz) { return isAmateurFreqMHz(khz / 1000.0f); }

  // Checks for settings code. Outside AMATEUR_RADIO builds they allow everything.
  static bool isFreqAllowedMHz(float mhz) {
#if AMATEUR_RADIO
    return isAmateurFreqMHz(mhz);
#else
    return true;
#endif
  }
  static bool isNodeNameAllowed(const char* node_name) {
#if AMATEUR_RADIO
    return isValidNodeName(node_name);
#else
    return true;
#endif
  }
};

}
