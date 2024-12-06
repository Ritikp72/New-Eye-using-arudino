//Defination of Library//
//************
//************
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


//************
//************
//Defination of Variables//

#define time_offset   7200 //because time pffset of Germany is UTC +2 i.e 7200 seconds
static const int RXPin = 17, TXPin = 16;
static const uint32_t GPSBaud = 4800;
char Time[]  = "TIME: 00:00 ";
char Date[]  = "DATE: 00-00-2000";
byte last_second, Second, Minute, Hour, Day, Month;
int Year;

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device
BlynkTimer timer;

float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS

char auth[] = "LTc82NJcAbDQ3WXIj-oEDRBwKxmmSQf2";              //Your Project authentication key
char ssid[] = "Wie gehts??";                                       // Name of your network (HotSpot or Router name)
char pass[] = "idontknow";                                      // Corresponding Password

unsigned int move_index = 1;       // fixed location for now

//************
//************
//Defination of Functions//
void getDateTime();
void checkGPS();
void displayInfo();

//************
//************
//Start of setup()//
void setup()
{
  Serial.begin(9600);
  Serial.println();
  ss.begin(GPSBaud);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS); // timer to check every 5s if GPS is connected.
}
//************
//************
//Start of loop()//
void loop()
{
  while (ss.available() > 0)
  {
    if (gps.encode(ss.read()))
      displayInfo();
    if (gps.time.isValid()) //checking time
    {
      Minute = gps.time.minute();
      Second = gps.time.second();
      Hour   = gps.time.hour();
    }
    if (gps.date.isValid())  //checking date
    {
      Day   = gps.date.day();
      Month = gps.date.month();
      Year  = gps.date.year();
    }
    if (last_second != gps.time.second()) // if time has changed
    {
      getDateTime();


    }
  }
  Blynk.run();
  timer.run();
}
//************
//************
//Start of checkGPS()//
void checkGPS() {
  if (gps.charsProcessed() < 15)
  {
    Serial.println(F("No GPS detected: check wiring."));
    Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}
//************
//************
//Start of displayInfo()//
void displayInfo()
{
  if (gps.location.isValid() )
  {
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon.
    float longitude = (gps.location.lng());

    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    Serial.println("-------------------------------------");
    Serial.println("-------------------------------------");
    Blynk.virtualWrite(V1, String(latitude, 6));
    Blynk.virtualWrite(V2, String(longitude, 6));
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    Blynk.virtualWrite(V3, Time, Date);

    sats = gps.satellites.value();    //get number of satellites
    Blynk.virtualWrite(V4, sats);

    bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
    Blynk.virtualWrite(V5, bearing);
    delay(2000);
  }

  Serial.println();
}
