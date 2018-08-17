#include <SoftwareSerial.h>
SoftwareSerial ESPserial(2, 3);
//SoftwareSerial GPSSerial(0, 1);// RX | TX
#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
     
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();
 
void setup() 
{
    //Serial.begin(9600);
     GPS.begin(115200);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
 // GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  
  // Ask for firmware version
 // GPSSerial.println(PMTK_Q_RELEASE);// communication with the host computer
  //  while (!Serial)   { ; }
   // Serial.println(" GPS COmmand sent");
// 
    // Start the software serial for communication with the ESP8266
    ESPserial.begin(9600);  
  
        
}
void loop() {
  
 
    // listen for user input and send it to the ESP8266
  // Serial.println("Waiting for GPS");
      
  // String readString;
     char c = GPS.read();
  // if you want to debug, this is a good time to do it!
 /* if (GPSECHO)
    if (c) Serial.print(c);*/
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
   // Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    /*Serial.print("\nTime: "+ millis());
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);*/
   // Serial.print("Fix: "); Serial.print((int)GPS.fix);
    //Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
       String latitude=String(GPS.latitude,4) + GPS.lat;
        String longitude=String(GPS.longitude,4) + GPS.lon;
     // Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      String coord= coordinate2dec(latitude,longitude);
      Serial.println("WRiting " + coord + "   To ESP");
       ESPserial.print( coord ); 
       delay(15000);
     
    }
     else
      {
        //Serial.println("GPS is not fixed");
        ESPserial.write("19.02n72.85e");
        delay(15000);
      }
  }
   
   
 }
 String coordinate2dec(String lati,String longi)
{
  String coord;
    
    //j=0;
    String lat_degree=lati.substring(0,2);
    
    
    String lat_minut=lati.substring(2,9);
   
    String long_degree=longi.substring(0,2);
   
    String long_minut=longi.substring(2,9);
    
     float minut= lat_minut.toFloat();
     minut=minut/60.0;
     float degree=lat_degree.toFloat();
     lati=degree+minut;
     
     minut= long_minut.toFloat();
     minut=minut/60.0;
     degree=long_degree.toFloat();
     longi=degree+minut;
     //Serial.println("lati"+ lati);
     //Serial.println("longi"+ longi);
     lati.concat("N");
    longi.concat("E");
    coord= lati+longi;
    return coord;
}
