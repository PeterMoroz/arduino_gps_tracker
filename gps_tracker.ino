/*
 * https://www.elithecomputerguy.com/2020/06/arduino-gps-adafruit-ultimate-gps-coordinates-and-speed-display-on-lcd/
 */

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

//ETCG Notes - LCD I2C Screen
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);


// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 8
// Connect the GPS RX (receive) pin to Digital 7

// you can change the pin numbers to match your wiring:
SoftwareSerial swSerial(8, 7);  // rx-pin, tx-pin
Adafruit_GPS gps(&swSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true


void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);  // col, row
  // lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0, 1);
  // lcd.print("Arduino LCM IIC");

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");
  
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  gps.begin(9600);  

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  gps.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  swSerial.println(PMTK_Q_RELEASE);
  
}

uint32_t timer = millis();
void loop() {
  // put your main code here, to run repeatedly:

  char c = gps.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (gps.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(gps.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!gps.parse(gps.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  } 


  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (gps.hour < 10) { Serial.print('0'); }
    Serial.print(gps.hour, DEC); Serial.print(':');
    if (gps.minute < 10) { Serial.print('0'); }
    Serial.print(gps.minute, DEC); Serial.print(':');
    if (gps.seconds < 10) { Serial.print('0'); }
    Serial.print(gps.seconds, DEC); Serial.print('.');
    if (gps.milliseconds < 10) {
      Serial.print("00");
    } else if (gps.milliseconds > 9 && gps.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(gps.milliseconds);
    Serial.print("Date: ");
    Serial.print(gps.day, DEC); Serial.print('/');
    Serial.print(gps.month, DEC); Serial.print("/20");
    Serial.println(gps.year, DEC);
    Serial.print("Fix: "); Serial.print((int)gps.fix);
    Serial.print(" quality: "); Serial.println((int)gps.fixquality);
    if (gps.fix) {
      Serial.print("Location: ");
      Serial.print(gps.latitude, 8); Serial.print(gps.lat);
      Serial.print(", ");
      Serial.print(gps.longitude, 8); Serial.println(gps.lon);
      //ETCG Notes -- The Following Gives Coordinates in formant you can plug into
      //Google Maps (Degrees)
      Serial.println("Location in Degrees");
      Serial.print(gps.latitudeDegrees, 8);
      Serial.print(", ");
      Serial.println(gps.longitudeDegrees, 8);

      Serial.print("Speed (knots): "); Serial.println(gps.speed);
      Serial.print("Angle: "); Serial.println(gps.angle);
      Serial.print("Altitude: "); Serial.println(gps.altitude);
      Serial.print("Satellites: "); Serial.println((int)gps.satellites);

//        //ETCG Notes -- Print to LCD Screen
//        lcd.clear();
//        lcd.setCursor(0,1);
//        lcd.print("Spd: ");
//        lcd.setCursor(5,1);
//        float mph;
//        mph = (gps.speed) * 1.15;
//        lcd.print(mph);
//        lcd.setCursor(0,2);
//        lcd.print("Lat: ");
//        lcd.setCursor(5,2);
//        lcd.print(gps.latitudeDegrees, 8);
//        lcd.setCursor(0,3);
//        lcd.print("Lon: ");
//        lcd.setCursor(5,3);
//        lcd.print(gps.longitudeDegrees, 8);
    } else {
//        lcd.clear();
//        lcd.setCursor(0,1);
//        lcd.print("No Signal");
    }
//      lcd.setCursor(0,0);
//      lcd.print("Satellites: ");
//      lcd.setCursor(12,0);
//      lcd.print((int)gps.satellites);
    }  
}
