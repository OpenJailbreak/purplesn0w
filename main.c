//purplesn0w by geohot

#include <stdio.h>
#include "crc32.h"

#define CRC_LOCKDOWND 0xF127A167

//#define TEST

#ifdef TEST
#define PATH_LOCKDOWND "lockdownd"
#else
#define PATH_LOCKDOWND "/usr/libexec/lockdownd"
#endif

#define CRC_COMMCENTER 0xFB5608C7

#ifdef TEST
#define PATH_COMMCENTER "CommCenter"
#else
#define PATH_COMMCENTER "/System/Library/PrivateFrameworks/CoreTelephony.framework/Support/CommCenter"
#endif

const unsigned char PATCH_COMMCENTER[] = {
0xF8,0xB5,0x2E,0x1C,0x7C,0x36,0x32,0x6A,0x05,0x2A,0x0C,0xD0,0x01,0x32,0x32,0x62,
0x06,0xA1,0x00,0x91,0x29,0x1C,0x12,0x4A,0x00,0x23,0x12,0x4C,0xA0,0x47,0xF8,0xBC,
0x08,0xBC,0x11,0x4B,0x18,0x47,0x11,0x4C,0xA0,0x47,0xF8,0xBD,0x2B,0x78,0x6C,0x6F,
0x67,0x3D,0x31,0x2C,0x22,0x67,0x65,0x6F,0x68,0x6F,0x74,0x73,0x61,0x74,0x69,0x74,
0x61,0x67,0x61,0x69,0x6E,0x61,0x6C,0x6C,0x70,0x74,0x69,0x6E,0x6A,0x65,0x63,0x74,
0x7A,0x64,0x4C,0x4A,0x40,0x68,0x61,0x68,0x61,0x69,0x55,0x52,0x20,0x22,0x00,0x00,
0xA1,0xED,0x01,0x00,0x71,0xF9,0x01,0x00,0x89,0xEE,0x01,0x00,0xBD,0xF5,0x05,0x00};

#ifdef TEST
#define PATH_ACTIVATIONRECORD "wildcard_record.plist"
#else
#define PATH_ACTIVATIONRECORD "/var/root/Library/Lockdown/activation_records/wildcard_record.plist"
#endif

const char TICKET[] = "A8009FE5000090E5000050E32400000A00000FE1C00080E300F021E14304A0E38C109FE50126A0E388409FE534FF2FE184009FE50010A0E3001080E5103F11EEFF30C3E3103F01EE0000A0E10000A0E10000A0E10000A0E10000A0E3431DA0E3002091E5000081E550109FE5002081E5170F08EE104F11EEFF40C4E3074084E3104F01EE0000A0E3150F07EE0000A0E10000A0E10000A0E100000FE1C000C0E300F021E10000A0E314409FE514FF2FE12C183C2000003020C83614202C180C430C080000595352200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

char buf[1000000];
int len;
FILE *f;

int main() {
  printf("purplesn0w by geohot\n");
  printf("thanks to oranav for at+xlog exploit\n");
  printf("the \"true\" unlock solution\n");
  gen_crc_table();
// check for activation record
  FILE* ar = fopen(PATH_ACTIVATIONRECORD, "rb");
  if(ar == 0) { printf("must have wildcard activation record to use...i am working on this. Sorry\n"); return -1; }
  fclose(ar);

// read lockdownd
  f = fopen(PATH_LOCKDOWND, "rb");
  if(f == 0) { printf("lockdownd not found\n"); return -1; }
  len = fread(buf, 1, 1000000, f);
  fclose(f);
  unsigned int crc_lockdownd = update_crc(0, buf, len);
  if(crc_lockdownd == 0xED460150) { printf("already applied\n"); return -1; }
  if(crc_lockdownd != CRC_LOCKDOWND) { printf("lockdownd bad crc\n"); return -1; }
// patch it
  buf[0x5dce] = 0x00; buf[0x5dcf] = 0x20;
// write it back
  f = fopen(PATH_LOCKDOWND, "wb");
  fwrite(buf, 1, len, f);
  fclose(f);

// read CommCenter
  f = fopen(PATH_COMMCENTER, "rb");
  if(f == 0) { printf("CommCenter not found\n"); return -1; }
  len = fread(buf, 1, 1000000, f);
  fclose(f);
  if(update_crc(0, buf, len) != CRC_COMMCENTER) { printf("CommCenter bad crc or already applied\n"); return -1; }
// patch it
  buf[0x1de5e] = 0x6E;
  buf[0x1de60] = 0xB9; buf[0x1de61] = 0xF8;
  memcpy(&buf[0x8bfd4], PATCH_COMMCENTER, sizeof(PATCH_COMMCENTER));
// write it back
  f = fopen(PATH_COMMCENTER, "wb");
  fwrite(buf, 1, len, f);
  fclose(f);

// read activation record
  f = fopen(PATH_ACTIVATIONRECORD, "rb");
  if(f == 0) { printf("activation record not found, wtf\n"); return -1; }
  len = fread(buf, 1, 1000000, f);
  fclose(f);
  // no crc check here
// patch it
  int i;
  for(i = 0; i < (len-strlen("WildcardTicket")); i++) {
    if(memcmp(&buf[i], "WildcardTicket", strlen("WildcardTicket")) == 0)
      break;
  }
  if(i == (len-strlen("WildcardTicket"))) { printf("WildcardTicket not found\n"); return -1; }
  memcpy(&buf[i+0x13], TICKET, strlen(TICKET));
// write it back
  f = fopen(PATH_ACTIVATIONRECORD, "wb");
  fwrite(buf, 1, len, f);
  fclose(f);

  printf("enjoy your purplesn0w unlocked iPhone\n");
  return 0;
}
