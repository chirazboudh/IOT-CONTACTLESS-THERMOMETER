#include <LiquidCrystal.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MLX90614.h>
float temp=0;
float Covid = 38.5; 
int sens = 0 ; // a digital input for the sens Button 
int urgent;
//                Rs E  D4 D5 D6 D7
LiquidCrystal lcd(4, 5, 3, 8, 7, 6);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char *ssid = "ContactLess_thermometer";
const char *password = "ContactLess_thermometer";
IPAddress ipadd (203,114,5,2);
IPAddress mask (255,255,255,0);
AsyncWebServer server(80);
void etatt(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "ET-"+String(eepromReadFloat(100))+":"+String(urgent));
}
void setup() {
  mlx.begin();          // Sensor configuration
  Serial.begin(9600);   // Serial communication config 
 lcd.begin (16,2);     // dispaly on classical cristal lcd 2 lines 16 caraters/line
  pinMode (sens,INPUT);
  EEPROM.begin(500);
   WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ipadd,ipadd,mask);
  Serial.println("");
  Serial.println("");
  Serial.print("Configuring access point...");
  Serial.println(ssid);
  
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println(WiFi.macAddress());

//**************************************************//
//*********** Protocole Communication **************//
//**************************************************//  
      server.on("/etat", HTTP_GET, etatt);
      server.begin();
      Serial.println("HTTP server started"); 
}

void display (int line_lcd, int col_lcd, float value)

{
  // line_lcd for cursor line 1 or 2
  // col_lcd for cursor position 0..15
 lcd.setCursor(line_lcd,col_lcd);
 lcd.print("Temp = ");
 lcd.print(value);
 Serial.print("Temperature = "); 
 Serial.print(temp);
}
float eepromReadFloat(int address){
   union u_tag {
     byte b[4];
     float fval;
   } u;   
   u.b[0] = EEPROM.read(address);
   u.b[1] = EEPROM.read(address+1);
   u.b[2] = EEPROM.read(address+2);
   u.b[3] = EEPROM.read(address+3);
   return u.fval;
}
void eepromWriteFloat(int address, float value){
   union u_tag {
     byte b[4];
     float fval;
   } u;
   u.fval=value;
 
   EEPROM.write(address  , u.b[0]);
   EEPROM.write(address+1, u.b[1]);
   EEPROM.write(address+2, u.b[2]);
   EEPROM.write(address+3, u.b[3]);
   EEPROM.commit();
}
void loop() {
 // Read value from sensor 
if (sens) { // read only if the botton read is pressed.
  temp = mlx.readAmbientTempC();
  eepromWriteFloat(100,temp);
 // Serial.println(temp);  // remove first comments foor tests
 // display value on LCD starting at 0.0
 display(0,0, temp);

  if (temp>Covid){
   urgent=1;
    lcd.print(" !");
    Serial.println(" !"); }
    else urgent=0;
 }
 else {
  urgent=0;
    temp=0;
    eepromWriteFloat(100,temp);
  display(0,0, 0);
 }

 delay(100);
}
