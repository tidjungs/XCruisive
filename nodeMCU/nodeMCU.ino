#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <ServerExceed.h>
#include <SPI.h>
#include <BlynkSimpleEsp8266.h>

WidgetLCD lcd(V1);

// Setting for Server
WiFiServer server(80); // nodeMCU server : port 80
char ssid[] = "nodeMCU only";
char password[] = "";
char host[] = "10.32.176.4";
int port = 80;
String group = "readme.txt";
ServerExceed mcu(ssid, password, host, port, group, &server);

// Setting for Blynk
char auth[] = "7e80aa3cb16642bcb6d3a8cbaacd3319";
char blynk_host[] = "10.32.176.4";
int blynk_port = 18442;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  mcu.connectServer();
  Blynk.config(auth, blynk_host, blynk_port);
  BLYNK_PRINT.println("\n\n[- nodeMCU -] Connected.");
  BLYNK_PRINT.print("[- nodeMCU -] IPAddress : ");
  BLYNK_PRINT.println(WiFi.localIP());
}

String data = "";
String blynk_data = "";
String server_data = "";

void loop()
 {
  if(Serial.available()) {
  	data = Serial.readStringUntil('\r');
    data.replace("\r","");
    data.replace("\n","");
  	Serial.flush();
    if (data == "alert1"){
      lcd.print(0,0, "Alert!!!!");
    }
    if (data == "cancel"){
      lcd.clear();
    }
    splitData();
  	if(server_data != "" && server_data != "alert1" && server_data != "cancel1") mcu.sendDataFromBoardToServer(server_data);
  }
  mcu.sendDataFromServerToBoard();
  Blynk.run();
}

void splitData() {
  server_data = data.substring(0, data.indexOf(":"));
  blynk_data = data.substring(data.indexOf(":") + 1, data.length());
}

BLYNK_WRITE(V0) {
  int val = param.asInt();
  if(val == 1) {
    Serial.print("cancel1");
    String cancel = "cancel";
    lcd.clear();
    mcu.sendDataFromBoardToServer(cancel);
  }
}
