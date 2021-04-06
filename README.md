# arduino_gps_tracker
Simple GPS tracker

The project is a simple GPS tracker based on the Arduino UNO board (Rev. 3 to be exact).
As a GPS receiver I use small and cheap GPS sensor Beitian BN-200.

GPS receiver provides data in form of NMEA sentences. The most commonly used are:

xxGGA - Global positioning system fixed data
xxGLL - Geographic position - latitude / longitude
xxGSA - GNSS DOP and active satellites
xxGSV - GNSS satellites in view
xxRMC - Recommended minimum specific GNSS data
xxVTG - Course over ground and ground speed

Where xx is talker ID, which identifies the source of information. The most common are:
GA - Galileo (European Union)
GB - BeiDou (China)
GL - GLONASS (Russia)
GP - GPS (United States)
GN - combination of GPS and GLONASS

In accordance with documentation Beitian supports the all sentences mentioned above.


The program receives data from the GPS receiver via software-emulated serial port.
Data received as NMEA sentences is parsed and only interested information are stored into the file on SD-card.
Currently these are:
- UTC timestamp
- Latitude (degrees minutes)
- Longitude (degrees minutes)

Everytime when program is started it creates a new data file on SD-card. 
The name of the file reflects the day and time when the first GPS fix has been done. 
Filename is limited by 8 characters so I didn't include month and year parts into it.

Links (NMEA):
 - http://www.satsleuth.com/GPS_NMEA_sentences.aspx
 - https://brandidowns.com/?p=77
 
Links:
 - https://makersportal.com/blog/2019/9/4/arduino-gps-tracker