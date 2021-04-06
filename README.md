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


Links (NMEA):
 - http://www.satsleuth.com/GPS_NMEA_sentences.aspx
 - https://brandidowns.com/?p=77
 
Links:
 - https://makersportal.com/blog/2019/9/4/arduino-gps-tracker