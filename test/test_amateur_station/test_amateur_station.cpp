#include <gtest/gtest.h>
#include <string.h>
#include "helpers/AmateurStation.h"
#include "Packet.h"
#include "Utils.h"

using namespace mesh;

static bool call(const char* s) { return AmateurStation::isUsCallsign(s, strlen(s)); }

TEST(AmateurCallsign, AcceptsUsFormats) {
  EXPECT_TRUE(call("W1AW"));     // 1x2
  EXPECT_TRUE(call("K7A"));      // 1x1
  EXPECT_TRUE(call("N0ABC"));    // 1x3
  EXPECT_TRUE(call("KD2ABC"));   // 2x3
  EXPECT_TRUE(call("WA6XY"));    // 2x2
  EXPECT_TRUE(call("AL7Q"));     // AA-AL block
  EXPECT_TRUE(call("kd2abc"));   // case-insensitive
}

TEST(AmateurCallsign, RejectsOthers) {
  EXPECT_FALSE(call(""));
  EXPECT_FALSE(call("W1"));        // no suffix
  EXPECT_FALSE(call("1AW"));       // no prefix
  EXPECT_FALSE(call("ABC1D"));     // three letter prefix
  EXPECT_FALSE(call("AM1X"));      // A prefix outside AA-AL
  EXPECT_FALSE(call("VE3ABC"));    // not a US prefix
  EXPECT_FALSE(call("W1ABCD"));    // four letter suffix
  EXPECT_FALSE(call("W12AB"));     // two digits
  EXPECT_FALSE(call("W1A2"));      // digit in suffix
  EXPECT_FALSE(call("NONAME"));
}

TEST(AmateurCallsign, TakesCallFromNodeName) {
  char out[AMATEUR_CALL_MAX + 1];
  EXPECT_EQ(4, AmateurStation::callsignOf("W1AW", out));
  EXPECT_STREQ("W1AW", out);
  EXPECT_EQ(6, AmateurStation::callsignOf("kd2abc-7", out));
  EXPECT_STREQ("KD2ABC", out);
  EXPECT_EQ(4, AmateurStation::callsignOf("N0XY Hilltop repeater", out));
  EXPECT_STREQ("N0XY", out);

  EXPECT_EQ(0, AmateurStation::callsignOf("Heltec Repeater", out));
  EXPECT_EQ(0, AmateurStation::callsignOf("NONAME", out));
  EXPECT_EQ(0, AmateurStation::callsignOf("W1AWXYZ123", out));
  EXPECT_EQ(0, AmateurStation::callsignOf("-W1AW", out));
  EXPECT_FALSE(AmateurStation::isValidNodeName(NULL));
  EXPECT_TRUE(AmateurStation::isNodeNameAllowed("W1AW Base"));
  EXPECT_FALSE(AmateurStation::isNodeNameAllowed("Base W1AW"));
}

TEST(AmateurBands, OnlySeventyAndThirtyThreeCm) {
  EXPECT_TRUE(AmateurStation::isAmateurFreqMHz(906.875f));
  EXPECT_TRUE(AmateurStation::isAmateurFreqMHz(902.0f));
  EXPECT_TRUE(AmateurStation::isAmateurFreqMHz(928.0f));
  EXPECT_TRUE(AmateurStation::isAmateurFreqMHz(433.5f));
  EXPECT_FALSE(AmateurStation::isAmateurFreqMHz(869.618f));   // EU Part 15 style preset
  EXPECT_FALSE(AmateurStation::isAmateurFreqMHz(928.1f));
  EXPECT_FALSE(AmateurStation::isAmateurFreqMHz(419.9f));
  EXPECT_FALSE(AmateurStation::isAmateurFreqMHz(2400.0f));
  EXPECT_TRUE(AmateurStation::isAmateurFreqKHz(910525));
  EXPECT_FALSE(AmateurStation::isAmateurFreqKHz(869525));
  EXPECT_FALSE(AmateurStation::isFreqAllowedMHz(915.0f + 20.0f));
}

TEST(AmateurPlainText, EncryptCopiesAndPadsToBlocks) {
  uint8_t key[CIPHER_KEY_SIZE] = { 1, 2, 3 };
  const char* msg = "CQ CQ de W1AW";   // 13 bytes
  uint8_t out[32];
  memset(out, 0xEE, sizeof(out));

  int n = Utils::encrypt(key, out, (const uint8_t*)msg, strlen(msg));
  EXPECT_EQ(CIPHER_BLOCK_SIZE, n);
  EXPECT_EQ(0, memcmp(out, msg, strlen(msg)));   // readable on air
  for (int i = strlen(msg); i < n; i++) EXPECT_EQ(0, out[i]);
  EXPECT_EQ(0xEE, out[n]);   // nothing written past the padded length

  uint8_t back[32];
  EXPECT_EQ(n, Utils::decrypt(key, back, out, n));
  EXPECT_EQ(0, memcmp(back, msg, strlen(msg)));
}

TEST(AmateurPlainText, ExactBlockNeedsNoPadding) {
  uint8_t key[CIPHER_KEY_SIZE] = { 0 };
  uint8_t src[32], out[32];
  for (int i = 0; i < 32; i++) src[i] = i + 1;
  EXPECT_EQ(32, Utils::encrypt(key, out, src, 32));
  EXPECT_EQ(0, memcmp(out, src, 32));
}

TEST(AmateurFrame, RawLengthCountsStationId) {
  Packet pkt;
  pkt.header = ROUTE_TYPE_FLOOD;
  pkt.path_len = 0;
  pkt.payload_len = 20;
  EXPECT_EQ(2 + 20 + STATION_ID_SIZE, pkt.getRawLength());
  EXPECT_EQ(8, STATION_ID_SIZE);
  EXPECT_EQ(176, MAX_PACKET_PAYLOAD);
  EXPECT_STREQ("", pkt.heard_station);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
