#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_IP below, port 9080 (instead of default 80):
*/
//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "192.168.1.4:5670"     //"192.168.43.141:5670"

#ifndef STASSID
#define STASSID "jornbot-2.4G"  //"HUAWEI Mate 10 Pro"
#define STAPSK  "35481234"  //"James2519"
#endif
#define RE 0
#define DE 2

const byte soil_8in1[] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x08, 0x45, 0xEE};
byte values[22];
SoftwareSerial mod(4,5);

float MOISTURE, TEMPERATURE, EC, PH, N, P, K, SALINITY;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  SOIL_8in1();
  delay(1000);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/soil_npk_sensor"); //HTTP
    http.addHeader("Content-Type", "application/json");
    
    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
  SOIL_8in1();
  Serial.print("MOISTURE: ");
  Serial.print(MOISTURE, 1);  Serial.println(" %RH");
  Serial.print("TEMPERATURE: ");
  Serial.print(TEMPERATURE, 1); Serial.println(" C");
  Serial.print("EC: ");
  Serial.print(EC, 1);  Serial.println(" uS/cm");
  Serial.print("PH: ");
  Serial.print(PH, 1);  Serial.println(" ph");
  Serial.print("N: ");
  Serial.print(N, 0); Serial.println(" mg/kg");
  Serial.print("P: ");
  Serial.print(P, 0); Serial.println(" mg/kg");
  Serial.print("K: ");
  Serial.print(K, 0); Serial.println(" mg/kg");
  Serial.print("SALINITY: ");
  Serial.print(SALINITY, 3);  Serial.println(" ppt");
  Serial.println();
  int httpCode = http.POST("{\"Multi_parameters_soil_sensors_2\":\"{'Temperature':"+String(TEMPERATURE, 1)+","+"'Moisture\':"+String(MOISTURE,1)+","+"'Electrical_conductivity\':"+String(EC,1)+","+"'PH\':"+String(PH,1)+","+"'Nitrogen\':"+String(N,0)+","+"'Phosphorus\':"+String(P,0)+","+"'Potessium\':"+String(K,0)+","+"'Salinity\':"+String(SALINITY,0)+"}\"}");
  // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  delay(1500);
}

void SOIL_8in1(){
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(soil_8in1, sizeof(soil_8in1)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 22; i++)
    {
      values[i] = mod.read();
//      Serial.print(values[i], HEX);
//      Serial.print(" ");
    }
    Serial.println();
  }
  MOISTURE = float((uint16_t)(values[3]<<8)|(values[4])) / 10;
  TEMPERATURE = float((uint16_t)(values[5]<<8)|(values[6])) / 10;
  EC = float((uint16_t)(values[7]<<8)|(values[8])) / 10;
  PH = float((uint16_t)(values[9]<<8)|(values[10])) / 10;
  N = float((uint16_t)(values[11]<<8)|(values[12]));
  P = float((uint16_t)(values[13]<<8)|(values[14]));
  K = float((uint16_t)(values[15]<<8)|(values[16]));
  SALINITY = float((uint16_t)(values[17]<<8)|(values[18])) / 1000;

}
