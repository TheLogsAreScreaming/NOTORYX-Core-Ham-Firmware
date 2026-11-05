#include "AmateurStation.h"

#include <string.h>
#include <SHA256.h>

namespace mesh {

static char upper(char c) { return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c; }
static bool isLetter(char c) { c = upper(c); return c >= 'A' && c <= 'Z'; }
static bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool AmateurStation::isUsCallsign(const char* s, int len) {
  if (s == NULL || len < 3 || len > AMATEUR_CALL_MAX) return false;

  int prefix = 0;   // letters before the district digit
  while (prefix < len && isLetter(s[prefix])) prefix++;
  if (prefix < 1 || prefix > 2 || prefix >= len || !isDigit(s[prefix])) return false;

  int suffix = len - prefix - 1;
  if (suffix < 1 || suffix > 3) return false;
  for (int i = prefix + 1; i < len; i++) {
    if (!isLetter(s[i])) return false;
  }

  char first = upper(s[0]);
  if (prefix == 1) return first == 'K' || first == 'N' || first == 'W';

  char second = upper(s[1]);
  if (first == 'A') return second >= 'A' && second <= 'L';   // AA-AL only
  return first == 'K' || first == 'N' || first == 'W';
}

int AmateurStation::callsignOf(const char* node_name, char* dest) {
  int len = 0;
  while (node_name[len] != 0 && node_name[len] != '-' && node_name[len] != ' ') {
    if (++len > AMATEUR_CALL_MAX) return 0;
  }
  if (!isUsCallsign(node_name, len)) return 0;

  for (int i = 0; i < len; i++) dest[i] = upper(node_name[i]);
  dest[len] = 0;
  return len;
}

bool AmateurStation::isAmateurFreqMHz(float mhz) {
  return (mhz >= 420.0f && mhz <= 450.0f) || (mhz >= 902.0f && mhz <= 928.0f);
}

static void loginTag(uint8_t* tag, const char* password, uint32_t timestamp,
                     const uint8_t* server_pub, const uint8_t* client_pub) {
  const uint8_t* key = (const uint8_t*)password;
  size_t key_len = strlen(password);
  SHA256 sha;
  sha.resetHMAC(key, key_len);
  sha.update(&timestamp, 4);   // little-endian on every supported MCU
  sha.update(server_pub, PUB_KEY_SIZE);
  sha.update(client_pub, PUB_KEY_SIZE);
  sha.finalizeHMAC(key, key_len, tag, AMATEUR_LOGIN_PROOF_SIZE);
}

int AmateurStation::writeLoginProof(uint8_t* dest, const char* password, uint32_t timestamp,
                                    const uint8_t* server_pub, const uint8_t* client_pub) {
  dest[0] = AMATEUR_LOGIN_PROOF;
  loginTag(&dest[1], password, timestamp, server_pub, client_pub);
  return 1 + AMATEUR_LOGIN_PROOF_SIZE;
}

bool AmateurStation::loginMatches(const uint8_t* data, const char* password, uint32_t timestamp,
                                  const uint8_t* server_pub, const uint8_t* client_pub) {
  if (data[0] == 0) return password[0] == 0;
#if AMATEUR_RADIO
  if (data[0] != AMATEUR_LOGIN_PROOF || password[0] == 0) return false;
  uint8_t expected[AMATEUR_LOGIN_PROOF_SIZE];
  loginTag(expected, password, timestamp, server_pub, client_pub);
  uint8_t diff = 0;   // compare every byte, so timing doesn't show how much matched
  for (int i = 0; i < AMATEUR_LOGIN_PROOF_SIZE; i++) diff |= expected[i] ^ data[1 + i];
  return diff == 0;
#else
  return strcmp((const char*)data, password) == 0;
#endif
}

}
