#include "ESP8266.h"

#define SSID        ""
#define PASSWORD    ""

ESP8266 wifi(Serial1);
float latitude = 41.075000;
float longitude = 14.327000;
void setup(void)
{
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    // aspetta finchè non ottieniamo una connessione all'hotspot
    wifi.joinAP(SSID, PASSWORD);

    Serial.print("setup end\r\n");

}

void loop(void)
{
  String hello_string = costruisci_richiesta();
  uint8_t buffer[1024];
  char hello[1024];
  hello_string.toCharArray(hello, hello_string.length());
	Serial.print(hello);
  wifi.createTCP("projectlisa.ml", 80);
  
  //char *form_data = "serial=2&latitude=41.072075&longitude=14.331375";
  //char *hello = "POST /post_position.php HTTP/1.1\r\nHost: projectlisa.ml\r\nConnection: close\r\nserial=2&latitude=41.072069&longitude=14.331341\r\n\r\n";
  //BASEchar *hello = "GET / HTTP/1.1\r\nHost: projectlisa.ml\r\nConnection: close\r\n\r\n";
  //char *hello = "GET /post_position.php HTTP/1.1\r\nHost: projectlisa.ml\r\nConnection: close\r\n\r\n";
  
  //"POST /post_position.php HTTP/1.1\r\nHost: projectlisa.ml\r\nConnection: keep-alive\r\n\r\n"
  //sprintf(hello, "%s%s%s%s%d%s%s%s%s%s%s","POST /post_position.php HTTP/1.1\r\n", "Host: projectlisa.ml\r\n", "Connection: keep-alive\r\n", 
  //"Content-Lenght: ", strlen(form_data), "\r\n", "Cache-Control: no-cache\r\n", "Content-Type: application/x-www-form-urlencoded\r\n", "\r\n", form_data, "\r\n");
  
  wifi.send((const uint8_t *)hello, hello_string.length());

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0)
  {
    Serial.print("Received:");
    for (uint32_t i = 0; i < len; i++)
    {
      Serial.print((char)buffer[i]);
    }
    Serial.print("\r\n");
  }
  Serial.print(hello_string);
  latitude += 0.000010; 
  longitude += 0.000010;
}

String costruisci_richiesta()
  {

    String richiesta_http;

    richiesta_http.concat("GET /post_position.php?");
    richiesta_http.concat("serial=2");
    richiesta_http.concat("&latitude=" + String(latitude, 6));
    richiesta_http.concat("&longitude=" + String(longitude, 6) + " ");
    richiesta_http.concat("HTTP/1.1\r\n");
    richiesta_http.concat("Host: " + String("projectlisa.ml") + "\r\n");
    richiesta_http.concat("Connection: close\r\n");
    richiesta_http.concat("\r\n\r\n");

    return richiesta_http;
  }

