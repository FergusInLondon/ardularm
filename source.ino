/* 
** Source code to ICStation's 
**  "Temperature & Humidity & Smoke Detector Alarm System Kit"
**   http://www.icstation.com/product_info.php?products_id=3665#.U4IX4h89jC0
**
** Original Source code available from:
**  http://www.icstation.com/newslecheckTemper/eMarketing/alarm%20system%20code.txt
*/

#include <LiquidCrystal.h>
#include "ardualarm.h"
#include "sense.h"

// Component Objects
LiquidCrystal lcd( LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );

// Variables
LCDMode lcd_mode = ALL;
Sensors sensors;
uint8_t state = 0;

// Setup
void setup()
{
  // Correctly set output pins.
  pinMode(FMQ,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GRE,OUTPUT);

  // Set hard coded min and max range.
  sensors.sensMinTemp = TEMP_LOW;
  sensors.sensMaxTemp = TEMP_HIG;
  sensors.sensMinHumi = HUMI_LOW;
  sensors.sensMaxHumi = HUMI_HUMI;

  // Set user defined alarm range.
  sensors.maxTemp = TEMP_DEF;
  sensors.maxHumi = HUMI_DEF;

  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

// Main Loop
void loop()
{
  keyHumidityAdjust();		// Check for Humidity Interaction
  keyTempAdjust();			// Check for Temperature Interaction
  keyLCDModeScan();			// CHeck for Reset Interaction

  // Read from sensors
  int maxTries = 5;
  while( (! getDHTValues( DHT_SENSOR )) || ( maxTries > 0) ) maxTries--;
  getSmokeReading();

  // BEGIN DEBUG:
  DEBUG_SEND("temp:");
  DEBUG_SEND(sensors.temp);
  DEBUG_SEND("humi:");
  DEBUG_SEND(sensors.humi);
  DEBUG_SEND("tol:");
  DEBUG_SEND(sensors.tol);
  // EOF DEBUG.

  // Delay, and then update our state from our new sensor readings
  delay(100);
  updateState();

  // Check for current LCD Output Mode
  switch( lcd_mode ){
    case TEMP:
      lcd_OutputTemp();
      break;

    case HUMI:
      lcd_OutputHumi();
      break;

    case ALL:
    default:
      lcd_OutputAll();
      break;
  }
}


/*
** STATE MANIPULATION AND BEHAVIOUR
*/


// Given an updated set of sensor values, perform bitwise operations on 
//  our state mask to produce a new state variable. If this is different
//  from our previous state, trigger an update to all outputs.
void updateState( )
{
  uint8_t newState;

  // Set state variables for sensors
  newState |= ( sensors.humi > sensors.maxHumi )  << 8;
  newState |= ( sensors.temp > sensors.maxTemp )  << 6;
  newState |= ( sensors.smoke )	<< 4;

  // One of the First 4 bits has been activated;
  //  meaning a sensor has detected a dangerous reading.
  if( newState & 0x0F ){
    bool toggleAll = false;
    if( newState & IN_SMOKE ) toggleAll = true;
    if( newState & IN_HUMI_H || toggleAll ) newState |= 1 << 1;
    if( newState & IN_TEMP_H || toggleAll ) newState |= 1 << 2; 
  }

  // If our state is different than the last reading, then lets update
  //  our global state and refresh our outputs.
  if( newState != state ){
    state = newState;
    updateIndicators();
  }
}

// Update all our outputs (Buzzer, Red LED and Green LED) from our
//  state bitmask.
void updateIndicators()
{
  digitalWrite( LED_GRE, ( state & OUT_G_LED ) );
  digitalWrite( LED_RED, ( state & OUT_R_LED ) );
  // Buzzer output is peculiar; high = off, low = on.
  digitalWrite( FMQ, (! state & OUT_BUZZE ) );
}


/*
** INPUT DETECTION/PROCESSING METHODS
*/


// Detect button presses on LCD output mode adjustment button.
void keyLCDModeScan()
{
  // Some typecasting voodoo here, beware.
  int mode = (int)lcd_mode;

  keyIncrementer(
    BTN_ADJUST_LCD,
    0,
    lcd_mode_count,
    currentMode
  );

  lcd_mode = (LCDMode)current_mode;
}

// Detect button presses on humdity adjustment button.
void keyHumidityAdjust()
{
  keyIncrementer(
    BTN_ADJUST_HUMI, 
    sensors.sensMinHumi,
    sensors.sensMaxHumi,
    sensors.userMaxHumi
  );
}

// Detect button presses on temperature adjustment button.
void keyTempAdjust()
{
  keyIncrementer(
    BTN_ADJUST_TEMP, 
    sensors.sensMinTemp,
    sensors.sensMaxTemp,
    sensors.userMaxTemp
  );
}