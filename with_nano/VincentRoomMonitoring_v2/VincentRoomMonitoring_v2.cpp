/*	
By Vincent Gosselin, 2017.

The following is a program to monitor
temperature and humidity in Vincent's Room.

The state machine of the program :
1 : Wait for 10sec
2 : 
3 : Construct a string to send to InfluxDB (inside a AWS server)
4 : Send the string using the Lora Module
*/

//This is using a nice library to simplify the code.
#include "Lora_simplified.h"
//Custom tools for a better code.
#include "arduinotools.h"

//Library for lowpower
//#include "LowPower.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

//All the custom libraries
#include "arduinonanodht11.h"
#include "sharp_dust_sensor.h"
#include "GPS_v2_library.h"// Successfully reduced Dynamic memory usage from 26% to 19% in this version.
//#include "Lux_Sensor.h" //needs to lighter.
#include "MQ7_Sensor.h"
#include "O3_spec_sensor.h"
#include "arduino_soil_moisture.h"
#include <arduino_nano_DFR0198.h>

//For Deep sleep
volatile unsigned int watchdog_counter = 48;

ISR(WDT_vect) {
  watchdog_counter++;
}

// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
// From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Correct incoming amount if need be

  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCSR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCSR = bb; //Set new watchdog timeout value
  WDTCSR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}


/* Global Objects */
//Atmega328p on a Arduino nano platform is used here.

//Carbon-monoxyde sensor. Pin config: PWM is on pin D5, Pin A2 is for reading
//MQ7 MQ7_Sensor(5,2);
//Ozone sensor. Pin config: Vo is on pin A0.
O3_spec_sensor O3_spec_sensor(0); 
//Dust sensor.  Pin config: Pin D4.
sharp_dust_sensor sharp_dust_sensor(4);
//GPS. Pin config: blue wire (TX) on D8 , green wire (RX) on D9.
GPS gps_sensor(8,9);
//Luminosity sensor. Pin config: (I2C protocol) SDA to A4, SCL to A5
//LUX lux_sensor;
//Soil moisture sensor. Pin config : Pin A1.
soil_moisture soil_moisture(1);
//Soil temperature sensor. Pin config :  Pin D3.
DFR0198 DFR0198(3);
//DHT11 sensor for temperature + humidity. Using pin D2.
dht11 dht11(2);

//Main function to scan sensor/send with LoRa module.
void sensor_node_execute(void);

void setup()
{
  //Lora_init();
  //Serial.begin(38400);
  //Serial.println("Hello y'all");
  //Serial.begin(38400);
  //setup_watchdog(6); //Setup watchdog to go off after 1sec
}

void loop(void)
{
  //This is equivalent to 30 secs somehow. Needs to be calibrated.
  if((watchdog_counter > 49)){

    sleep_disable(); 
    ADCSRA |= (1<<ADEN); //Enable ADC

    watchdog_counter = 0;

    delay(1000);
    Serial.begin(38400);
    delay(1000);

    sensor_node_execute();
    //Serial.println("hello there!");
    delay(1000);

    Serial.end();
    delay(1000);

  }

  setup_watchdog(6); //Setup watchdog to go off after 1sec
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Power down everything, wake up from WDT
  sleep_enable();
  ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  sleep_mode();//Sleeps now! Wow, current is now 0.27mA!
  //sleep_disable(); 
  //ADCSRA |= (1<<ADEN); //Enable ADC

  //delay(1000);
  //Serial.begin(38400);
  //delay(1000);
  //Serial.println(" ");
  //Serial.print("Wake up at : ");Serial.println(millis(),6);
  //Serial.println("Hello world!");

  //Lora init
  // Lora_init();
  // delay(1000);

/* Sensor scanning begins */

  //CO sensor is to start first because of long acquiring time.
  // int state = MQ7_Sensor.get_preheat_state();
  // delay(1000);

  //O3 sensor.
  // O3_spec_sensor.scan();
  // delay(1000);

  // //Dust sensor
  // sharp_dust_sensor.scan();
  // delay(1000);

  // //GPS
  // gps_sensor.scan();
  // delay(1000);

  // //Luminosity sensor
  // lux_sensor.scan();
  // delay(1000);

  // //Soil moisture
  // soil_moisture.scan();
  // delay(1000);

  // //Soil temperature
  // DFR0198.scan();
  // delay(1000);

  // //Scan temperature and humidity
  // dht11.scan();
  // delay(1000);

  // //Finish CO sensor
  // while (state != 3){
  //    Serial.println(state);   
  //   delay(1000);
  //   state = MQ7_Sensor.get_preheat_state();
  // }
  // MQ7_Sensor.scan();

/* Sensor done scanning */

/* Preparing LoRa transmission */  

  // float O3_concentration = O3_spec_sensor.get_concentration(); //in ppb.
  // //int CO_concentration = MQ7_Sensor.get_CO_concentration(); //in ppm. TO BE MODIFIED.
  // float dust_concentration = sharp_dust_sensor.get_concentration(); //in micro gram per cubic meter.
  // float lat = gps_sensor.get_lat();
  // float lon = gps_sensor.get_lon();
  // unsigned int lux = lux_sensor.get_lux();
  // int soil_humidity = soil_moisture.get_moisture();
  // float soil_temperature = DFR0198.get_temperature();
  // int air_humidity = dht11.get_humidity();
  // int air_temperature = dht11.get_temperature();
  
  //Debugging
  // Serial.print("O3 concentration is : ");Serial.print(O3_concentration);Serial.println(" ppb");
  // delay(1000);
  // Serial.print("CO concentration is : ");Serial.print(CO_concentration);Serial.println(" ppm");
  // delay(1000);
  // Serial.print("Dust concentration is : ");Serial.print(dust_concentration);Serial.println(" ug/m^3");
  // delay(1000);
  // Serial.print("GPS Coordinates are -> LAT : ");Serial.print(lat,6);Serial.print("  LON : ");Serial.println(lon,6);
  // delay(1000);
  // Serial.print("Luminosity is : ");Serial.print(lux);Serial.println(" LUX");
  // delay(1000);
  // Serial.print("Soil humidity is : ");Serial.print(soil_humidity);Serial.println(" %RH");
  // delay(1000);
  // Serial.print("Soil temperature is : ");Serial.print(soil_temperature,1);Serial.println(" Degrees");
  // delay(1000);
  // Serial.print("Air humidity is : ");Serial.print(air_humidity);Serial.println(" %RH");
  // delay(1000);
  // Serial.print("Air temperature is : ");Serial.print(air_temperature);Serial.println(" Degrees");
  // delay(1000);

  //LoRA Communication
  // A -> O3 concentration
  // B -> Dust concentration
  // C -> GPS
  // D -> Lux
  // E -> Soil humidity
  // F -> Soil temperature
  // G -> Air humidity
  // H -> Air temperature

  //Trame
  // "\\!node1:A=%d:B=%d:C=%d:D=%d:E=%d:F=%d:G=%d:H=%d"

  //string to send
  // char string[50];
  // sprintf(string, "\\!node1:temperature_air=%d:humidity_air=%d", air_temperature,air_humidity);
  // Lora_send_string(string);

  //Serial.print("Die at :");Serial.println(millis(),6);
  //Serial.println(" ");

  //SLEEP for 16sec.
  // char i;
  // for(i=0;i<2;i++)
  // {
    // delay(1000);
    // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    // delay(1000);
  // }
     
}

void sensor_node_execute(void){

  Serial.println(" ");
  delay(1000);
  Serial.print("Wake up at : ");Serial.println(millis(),6);
  delay(1000);
  //Serial.println("Hello world!");

  //Lora init
  Lora_init();
  delay(1000);

/* Sensor scanning begins */

  //CO sensor is to start first because of long acquiring time.
  // int state = MQ7_Sensor.get_preheat_state();
  // delay(1000);

  //O3 sensor.
  O3_spec_sensor.scan();
  delay(1000);

  //Dust sensor
  sharp_dust_sensor.scan();
  delay(1000);

  //GPS
  gps_sensor.scan();
  delay(1000);

  // //Luminosity sensor
  // lux_sensor.scan();
  // delay(1000);

  //Soil moisture
  soil_moisture.scan();
  delay(1000);

  //Soil temperature
  DFR0198.scan();
  delay(1000);

  //Scan temperature and humidity
  dht11.scan();
  delay(1000);

  // //Finish CO sensor
  // while (state != 3){
  //    Serial.println(state);   
  //   delay(1000);
  //   state = MQ7_Sensor.get_preheat_state();
  // }
  // MQ7_Sensor.scan();

/* Sensor done scanning */

/* Preparing LoRa transmission */  

  float O3_concentration = O3_spec_sensor.get_concentration(); //in ppb.
  //int CO_concentration = MQ7_Sensor.get_CO_concentration(); //in ppm. TO BE MODIFIED.
  float dust_concentration = sharp_dust_sensor.get_concentration(); //in micro gram per cubic meter.
  float lat = gps_sensor.get_lat();
  float lon = gps_sensor.get_lon();
  // //unsigned int lux = lux_sensor.get_lux();
  int soil_humidity = soil_moisture.get_moisture();
  float soil_temperature = DFR0198.get_temperature();
  int air_humidity = dht11.get_humidity();
  int air_temperature = dht11.get_temperature();
  
  //Debugging
  // Serial.print("O3 concentration is : ");Serial.print(O3_concentration);Serial.println(" ppb");
  // delay(1000);
  // Serial.print("CO concentration is : ");Serial.print(CO_concentration);Serial.println(" ppm");
  // delay(1000);
  // Serial.print("Dust concentration is : ");Serial.print(dust_concentration);Serial.println(" ug/m^3");
  // delay(1000);
  // Serial.print("GPS Coordinates are -> LAT : ");Serial.print(lat,6);Serial.print("  LON : ");Serial.println(lon,6);
  // delay(1000);
  // Serial.print("Luminosity is : ");Serial.print(lux);Serial.println(" LUX");
  // delay(1000);
  // Serial.print("Soil humidity is : ");Serial.print(soil_humidity);Serial.println(" %RH");
  // delay(1000);
  // Serial.print("Soil temperature is : ");Serial.print(soil_temperature,1);Serial.println(" Degrees");
  // delay(1000);
  // Serial.print("Air humidity is : ");Serial.print(air_humidity);Serial.println(" %RH");
  // delay(1000);
  // Serial.print("Air temperature is : ");Serial.print(air_temperature);Serial.println(" Degrees");
  // delay(1000);

  //LoRA Communication
  // A -> O3 concentration
  // B -> Dust concentration
  // C -> GPS
  // D -> Lux
  // E -> Soil humidity
  // F -> Soil temperature
  // G -> Air humidity
  // H -> Air temperature

  // Trame
  // "\\!node1:A=%d:B=%d:C=%d:D=%d:E=%d:F=%d:G=%d:H=%d"

  //string to send
  char string[50];
  sprintf(string, "\\!node1:temperature_air=%d:humidity_air=%d", air_temperature,air_humidity);
  Lora_send_string(string);

  delay(1000);
  Serial.print("Die at :");Serial.println(millis(),6);
  delay(1000);
  Serial.println(" ");
}


