#include <SoftwareSerial.h>

#include <SPI.h>
#include <SD.h>


#define RX_PIN 7
#define TX_PIN 8

#define SD_CS_PIN 4

SoftwareSerial swSerial(RX_PIN, TX_PIN);


#define BUFFER_SIZE 256
#define MAX_NUM_OF_DELIMIERS 20

char sentenceBuffer[BUFFER_SIZE];
unsigned delimitersIndexes[MAX_NUM_OF_DELIMIERS];


unsigned sentenceCount = 0;
unsigned symbolCount = 0;

unsigned si = 0;  // index in sentence buffer
unsigned di = 0;  // index in delimiters indexes array

struct RMCdata
{
  byte hour;
  byte minute;
  byte second;
  byte millisecond;

  byte day;
  byte month;
  byte year;

  byte latDegrees;
  byte latMinutes;
  unsigned long latMinutesFract;

  unsigned short lngDegrees;
  byte lngMinutes;
  unsigned long lngMinutesFract;
  
  char nsIndicator;
  char ewIndicator;

  bool valid;
} rmcData;


void parseSentence(void);
void dumpRMC(void);

void setup()
{
  swSerial.begin(9600);
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  if (!SD.begin(SD_CS_PIN)) {
    while (1) ;
  }

  Serial.println(F("SD initialized"));

  // clear RMC data
  rmcData.hour = 0;
  rmcData.minute = 0;
  rmcData.second = 0;
  rmcData.millisecond = 0;

  rmcData.day = 0;
  rmcData.month = 0;
  rmcData.year = 0;

  rmcData.latDegrees = 0;
  rmcData.latMinutes = 0;
  rmcData.latMinutesFract = 0;

  rmcData.lngDegrees = 0;
  rmcData.lngMinutes = 0;
  rmcData.lngMinutesFract = 0;
  
  rmcData.nsIndicator = 'U';
  rmcData.ewIndicator = 'U';
  
  rmcData.valid = false;

  Serial.println(F("Setup complete"));
}

void loop()
{
  unsigned long t = millis();
  
  do {
    while (swSerial.available()) 
    {
      char x = swSerial.read();
      if (x != -1) 
      {
        if (x == '$')
        {
          si = 0;
          di = 0;
        }
        else if (x == 13)
        {
          if (si < BUFFER_SIZE)
            sentenceBuffer[si] = '\0';
          else
            sentenceBuffer[BUFFER_SIZE - 1] = '\0';

          /* 
          Serial.print("-- NMEA sentence: ");
          for (const char* p = sentenceBuffer; *p; p++) {
            Serial.print(*p);
          }
          Serial.println("");
          */
          parseSentence();
          sentenceCount++;

          if (rmcData.valid)
          {
/*            char str[128];
            sprintf(str, "%02d/%02d/%02d %02d:%02d:%02d.%02d  Lat: %02d%02d.%05lu %c Lng: %03d%02d.%05lu %c",
              rmcData.day, rmcData.month, rmcData.year, 
              rmcData.hour, rmcData.minute, rmcData.second, rmcData.millisecond,
              rmcData.latDegrees, rmcData.latMinutes, rmcData.latMinutesFract, rmcData.nsIndicator, 
              rmcData.lngDegrees, rmcData.lngMinutes, rmcData.lngMinutesFract, rmcData.ewIndicator);
            Serial.println(str);*/
            dumpRMC();
          }
          
        }
        else if (x != 10)
        {
          if (si < BUFFER_SIZE)
          {
            sentenceBuffer[si] = x;
            if (x == ',' || x == '*')
            {
              if (di < MAX_NUM_OF_DELIMIERS)
                delimitersIndexes[di++] = si;
            }
            si++;
          }
        }
        symbolCount++;
      }
    }
  } while ((millis() - t) < 1000);

  Serial.print(F("symbols: "));
  Serial.println(symbolCount);
  Serial.print(F("sentences: "));
  Serial.println(sentenceCount);
}

void parseRMC(void)
{
  unsigned idx = 0;

  // check 'Status' field
  idx = delimitersIndexes[1] + 1;
  if (delimitersIndexes[2] - idx == 1 && sentenceBuffer[idx] == 'A')
  {
    // parse 'UTC time' field
    idx = delimitersIndexes[0] + 1;
    if (delimitersIndexes[1] - idx == 9)
    {
      rmcData.hour = sentenceBuffer[idx++] - '0';
      rmcData.hour *= 10;
      rmcData.hour += sentenceBuffer[idx++] - '0';

      rmcData.minute = sentenceBuffer[idx++] - '0';
      rmcData.minute *= 10;
      rmcData.minute += sentenceBuffer[idx++] - '0';

      rmcData.second = sentenceBuffer[idx++] - '0';
      rmcData.second *= 10;
      rmcData.second += sentenceBuffer[idx++] - '0';

      // need to skip decimal separator
      // so use prefix increment instead of postfix one
      rmcData.millisecond = sentenceBuffer[++idx] - '0';
      rmcData.millisecond *= 10;
      rmcData.millisecond += sentenceBuffer[++idx] - '0';
    }
    else
    {
      return;
    }

    // parse 'Latitude' field and N/S indicator
    idx = delimitersIndexes[2] + 1;
    if (delimitersIndexes[3] - idx == 10)
    {
      rmcData.latDegrees = sentenceBuffer[idx++] - '0';
      rmcData.latDegrees *= 10;
      rmcData.latDegrees += sentenceBuffer[idx++] - '0';
      
      rmcData.latMinutes = sentenceBuffer[idx++] - '0';
      rmcData.latMinutes *= 10;
      rmcData.latMinutes += sentenceBuffer[idx++] - '0';

      // need to skip decimal separator
      // so use prefix increment instead of postfix one      
      rmcData.latMinutesFract = sentenceBuffer[++idx] - '0';
      rmcData.latMinutesFract *= 10;
      rmcData.latMinutesFract += sentenceBuffer[++idx] - '0';
      rmcData.latMinutesFract *= 10;
      rmcData.latMinutesFract += sentenceBuffer[++idx] - '0';
      rmcData.latMinutesFract *= 10;
      rmcData.latMinutesFract += sentenceBuffer[++idx] - '0';
      rmcData.latMinutesFract *= 10;
      rmcData.latMinutesFract += sentenceBuffer[++idx] - '0';
    }
    else
    {
      return;
    }

    idx = delimitersIndexes[3] + 1;
    if (delimitersIndexes[4] - idx == 1 && 
      (sentenceBuffer[idx] == 'S' || sentenceBuffer[idx] == 'N'))
    {
      rmcData.nsIndicator = sentenceBuffer[idx];      
    }
    else
    {
      return;
    }   

    // parse 'Longitude' field and E/W indicator
    idx = delimitersIndexes[4] + 1;
    if (delimitersIndexes[5] - idx == 11)
    {
      rmcData.lngDegrees = sentenceBuffer[idx++] - '0';
      rmcData.lngDegrees *= 10;
      rmcData.lngDegrees += sentenceBuffer[idx++] - '0';
      rmcData.lngDegrees *= 10;
      rmcData.lngDegrees += sentenceBuffer[idx++] - '0';
            
      rmcData.lngMinutes = sentenceBuffer[idx++] - '0';
      rmcData.lngMinutes *= 10;
      rmcData.lngMinutes += sentenceBuffer[idx++] - '0';

      // need to skip decimal separator
      // so use prefix increment instead of postfix one           
      rmcData.lngMinutesFract = sentenceBuffer[++idx] - '0';
      rmcData.lngMinutesFract *= 10;
      rmcData.lngMinutesFract = sentenceBuffer[++idx] - '0';
      rmcData.lngMinutesFract *= 10;
      rmcData.lngMinutesFract = sentenceBuffer[++idx] - '0';
      rmcData.lngMinutesFract *= 10;
      rmcData.lngMinutesFract = sentenceBuffer[++idx] - '0';
      rmcData.lngMinutesFract *= 10;
      rmcData.lngMinutesFract = sentenceBuffer[++idx] - '0';
    }
    else
    {
      return;
    }

    idx = delimitersIndexes[5] + 1;
    if (delimitersIndexes[6] - idx == 1 && 
      (sentenceBuffer[idx] == 'E' || sentenceBuffer[idx] == 'W'))
    {
      rmcData.ewIndicator = sentenceBuffer[idx];
    }
    else
    {
      return;
    }

    // parse 'UTC date' field
    idx = delimitersIndexes[8] + 1;
    if (delimitersIndexes[9] - idx == 6)
    {
      rmcData.day = sentenceBuffer[idx++] - '0';
      rmcData.day *= 10;
      rmcData.day += sentenceBuffer[idx++] - '0';

      rmcData.month = sentenceBuffer[idx++] - '0';
      rmcData.month *= 10;
      rmcData.month += sentenceBuffer[idx++] - '0';

      rmcData.year = sentenceBuffer[idx++] - '0';
      rmcData.year *= 10;
      rmcData.year += sentenceBuffer[idx++] - '0';
    }
    else
    {
      return;
    }

    // now assume that the message is valid, even if other fields are no present
    rmcData.valid = true;
  }
}

void dumpRMC(void)
{
  static char filename[18] = { '\0' };
  static bool fnameInit = false;
  if (rmcData.valid)
  {
    if (!fnameInit) 
    {
      sprintf(filename, "%02d%02d%02d%02d.csv",  // ddhhmmss.csv
              rmcData.day, rmcData.hour, rmcData.minute, rmcData.second);
      fnameInit = true;
    }

    File file = SD.open(filename, FILE_WRITE);    
    if (file)
    {
      char row[64];
      sprintf(row, "%02d/%02d/%02d,%02d:%02d:%02d.%02d,%02d%02d.%05lu,%03d%02d.%05lu", 
          rmcData.year, rmcData.month, rmcData.day, rmcData.hour, rmcData.minute, rmcData.second, rmcData.millisecond,
          rmcData.latDegrees, rmcData.latMinutes, rmcData.latMinutesFract, rmcData.lngDegrees, rmcData.lngMinutes, rmcData.lngMinutesFract);
      file.println(row);          
      file.close();  
    }
    else
    {
      Serial.println(F("Could not open file to write."));
    }
  }
}

void parseSentence(void)
{
  if (!strncmp(&sentenceBuffer[2], "RMC", 3))
  {
    parseRMC();

/*
    Serial.print("-- RMC sentence: ");
    for (const char* p = sentenceBuffer; *p; p++) {
      Serial.print(*p);
    }
    Serial.println("");    */
  }
}
