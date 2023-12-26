#include <Wire.h>
#include <AHTxx.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

float ahtValue;

AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);


#include <LiquidCrystal.h>

const int rs = 4, en = 16, d4 = 17, d5 = 5, d6 = 18, d7 = 19;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


/**************************************************************************/
/*
    setup()

    Main setup
*/
/**************************************************************************/
void setup()
{

  lcd.begin(16, 2);  // formats display to 16 x 2

  #if defined(ESP8266)
  WiFi.persistent(false);  //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();  //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  #endif

  Serial.begin(115200);
  Serial.println();
  
  while (aht20.begin() != true)
  {
    Serial.println(F("AHT2x not connected or fail to load calibration coefficient")); //(F()) save string to flash & keeps dynamic memory free

    delay(5000);
  }

  Serial.println(F("AHT20 OK"));

  //Wire.setClock(400000); //experimental I2C speed! 400KHz, default 100KHz
}


/**************************************************************************/
/*
    loop()

     Main loop
*/
/**************************************************************************/
void loop()
{
  int LCDTemp = aht20.readTemperature(); // Tempature Value
  int LCDHum = aht20.readHumidity(); // Humidity Value

  lcd.print("Temp: ");  // Displays words "Temp:"
  lcd.print(LCDTemp); // Displays Temp Int Value

  lcd.setCursor(0, 1); // sets to write on the second line

  lcd.print("Hum: "); // Displays words "Hum:"
  lcd.print(LCDHum); // Displays Hum Int Value

  lcd.setCursor(0, 0); // resets to write on the first line (The second parameter is the line you are writing on, it is indexed at 0)



  /* DEMO - 1, every temperature or humidity call will read 6-bytes over I2C, total 12-bytes */
  Serial.println();
  Serial.println(F("DEMO 1: read 12-bytes"));

  ahtValue = aht20.readTemperature(); //read 6-bytes via I2C, takes 80 milliseconds

  Serial.print(F("Temperature...: "));
  
  if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(ahtValue);
    Serial.println(F(" +-0.3C"));
  }
  else
  {
    printStatus(); //print temperature command status

    if   (aht20.softReset() == true) Serial.println(F("reset success")); //as the last chance to make it alive
    else                             Serial.println(F("reset failed"));
  }

  delay(2000); //measurement with high frequency leads to heating of the sensor, see NOTE

  ahtValue = aht20.readHumidity(); //read another 6-bytes via I2C, takes 80 milliseconds

  Serial.print(F("Humidity......: "));
  
  if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(ahtValue);
    Serial.println(F(" +-2%"));
  }
  else
  {
    printStatus(); //print humidity command status
  }

  delay(2000); //measurement with high frequency leads to heating of the sensor, see NOTE

  /* DEMO - 2, temperature call will read 6-bytes via I2C, humidity will use same 6-bytes */
  Serial.println();
  Serial.println(F("DEMO 2: read 6-byte"));

  ahtValue = aht20.readTemperature(); //read 6-bytes via I2C, takes 80 milliseconds

  Serial.print(F("Temperature: "));
  
  if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(ahtValue);
    Serial.println(F(" +-0.3C"));
  }
  else
  {
    printStatus(); //print temperature command status
  }

  ahtValue = aht20.readHumidity(AHTXX_USE_READ_DATA); //use 6-bytes from temperature reading, takes zero milliseconds!!!

  Serial.print(F("Humidity...: "));
  
  if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(ahtValue);
    Serial.println(F(" +-2%"));
  }
  else
  {
    printStatus(); //print temperature command status not humidity!!! RH measurement use same 6-bytes from T measurement
  }

  delay(10000); //recomended polling frequency 8sec..30sec
}


/**************************************************************************/
/*
    printStatus()

    Print last command status
*/
/**************************************************************************/
void printStatus()
{
  switch (aht20.getStatus())
  {
    case AHTXX_NO_ERROR:
      Serial.println(F("no error"));
      break;

    case AHTXX_BUSY_ERROR:
      Serial.println(F("sensor busy, increase polling time"));
      break;

    case AHTXX_ACK_ERROR:
      Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_DATA_ERROR:
      Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_CRC8_ERROR:
      Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
      break;

    default:
      Serial.println(F("unknown status"));    
      break;
  }
}
