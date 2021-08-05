/*
  SSL Web client

 This sketch connects to a website using SSL through a MKR NB 1500 board. Specifically,
 this example downloads the URL "https://www.arduino.cc/asciilogo.txt" and
 prints it to the Serial monitor.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe
*/

// libraries
#include <Arduino.h>
#include "SSLClient/src/SSLClient.h"
#include "MKRNB2/src/MKRNB.h"
#include "certificates.h"

// initialize the library instance
NBClient baseClient;
SSLClient client(baseClient, TAs, (size_t)2, A3, 1, SSLClient::SSL_INFO);
GPRS gprs;
NB nbAccess(true);

// URL, path and port (for example: arduino.cc)
char server[] = "google.com";
int port = 443; // port 443 is the default for HTTPS

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true; // set to false for better speed measurement

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(A4, INPUT_PULLUP);

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

  // After starting the modem with NB.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected)
  {
    if ((nbAccess.begin() == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY))
    {
      connected = true;
    }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  while (!baseClient.ready())
  {
    delay(100);
  }

  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  auto start = millis();
  // specify the server and port, 443 is the standard port for HTTPS
  if (client.connect(server, port))
  {
    auto time = millis() - start;
    Serial.print("Took: ");
    Serial.println(time);
    // Make a HTTP request:
    client.println("GET /robots.txt HTTP/1.1");
    client.println("User-Agent: SSLClientOverEthernet");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  beginMicros = micros();
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  int len = client.available();
  if (len > 0)
  {
    byte buffer[80];
    if (len > 80)
      len = 80;
    client.read(buffer, len);
    if (printWebData)
    {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected())
  {
    endMicros = micros();
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    Serial.print("Received ");
    Serial.print(byteCount);
    Serial.print(" bytes in ");
    float seconds = (float)(endMicros - beginMicros) / 1000000.0;
    Serial.print(seconds, 4);
    float rate = (float)byteCount / seconds / 1000.0;
    Serial.print(", rate = ");
    Serial.print(rate);
    Serial.print(" kbytes/second");
    Serial.println();

    // do nothing forevermore:
    while (true)
    {
      delay(1);
    }
  }
}
