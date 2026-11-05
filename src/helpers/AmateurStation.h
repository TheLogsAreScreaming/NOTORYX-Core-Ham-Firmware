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
// Login without a password on air: the login data is this byte followed by
// AMATEUR_LOGIN_PROOF_SIZE bytes of
//   HMAC-SHA256(key = password, timestamp (4, LE) || server pub key || client pub key)
// in place of the password. Binding both keys stops a proof being replayed to
// another server, and the server's rising-timestamp check stops it being replayed
// to the same one. It authenticates; it hides nothing (Part 97.113(a)(4)).
#define AMATEUR_LOGIN_PROOF        0x04   // below ' ', and not 0 or an ANON_REQ_TYPE_* code
#define AMATEUR_LOGIN_PROOF_SIZE   16

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

  /**
   * \brief  writes the login proof (marker byte + AMATEUR_LOGIN_PROOF_SIZE bytes) for
   *    'password' into 'dest'.
   * \returns  bytes written: 1 + AMATEUR_LOGIN_PROOF_SIZE
   */
  static int writeLoginProof(uint8_t* dest, const char* password, uint32_t timestamp,
                             const uint8_t* server_pub, const uint8_t* client_pub);

  /**
   * \brief  whether login 'data' (as received after the timestamp) proves 'password'.
   *    Empty data matches only an empty password. In AMATEUR_RADIO builds anything
   *    other than a proof is refused; elsewhere the plain password is compared.
   */
  static bool loginMatches(const uint8_t* data, const char* password, uint32_t timestamp,
                           const uint8_t* server_pub, const uint8_t* client_pub);

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
