#include <Arduino.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "gen/src/protos/net_message.pb.h"

typedef _dev_buoys_NetMessage NetMessage;

void setup()
{
  Serial.begin(9600);
  Serial.println("Proto check start.");
}

void printHex(uint8_t num)
{
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

void loop()
{
  Serial.println("Encoding.");

  NetMessage mymessage = dev_buoys_NetMessage_init_zero;
  mymessage.payload = {4, {0xde, 0xad, 0xbe, 0xef}};
  mymessage.has_payload = true;

  uint8_t buffer[512];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  pb_encode(&stream, dev_buoys_NetMessage_fields, &mymessage);

  delay(1000);

  Serial.println("Decoding.");

  pb_istream_t istream = pb_istream_from_buffer(buffer, sizeof(buffer));
  NetMessage decoded_message = dev_buoys_NetMessage_init_zero;
  pb_decode(&istream, dev_buoys_NetMessage_fields, &decoded_message);

  for (int i = 0; i < sizeof(decoded_message.payload); i++)
  {
    printHex(decoded_message.payload.bytes[i]);
  }

  Serial.println();

  delay(1000);
}