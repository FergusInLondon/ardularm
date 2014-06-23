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

// Component Objects
LiquidCrystal lcd( LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );

// Variables
LCDMode lcd_mode = ALL;
Sensors sensors;

void setup()
{
  pinMode(FMQ,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GRE,OUTPUT);

  Sensors.minTemp = TEMP_MIN;
  Sensors.maxTemp = TEMP_MAX;
  Sensors.minHumi = HUMI_MIN;
  Sensors.maxHumi = HUMI_MAX;

  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

//
void loop()
{

  keyIncrementer( BTN_ADJUST_HUMI, Sensors.minHumi, Sensors.maxHumi, &maxHumi );
  keyIncrementer( BTN_ADJUST_TEMP, Sensors.minTemp, Sensors.maxTemp, &maxtemp );
  keyIncrementer( BTN_ADJUST_LCD, 0, 2, lcdOutput );

  keyHumidityAdjust();		// Check for Humidity Interaction
  keyTempAdjust();			  // Check for Temperature Interaction
  keyLCDModeScan();       // CHeck for Reset Interaction

  while(! getDHTValues( DHT_SENSOR ) );

  // BEGIN DEBUG:
  DEBUG_SEND("temp:");
  DEBUG_SEND(temp);
  DEBUG_SEND("humi:");
  DEBUG_SEND(humi);
  DEBUG_SEND("tol:");
  DEBUG_SEND(tol);
  // EOF DEBUG.

  // If temperature is too high
  if(temp > maxTemp){
    digitalWrite(LED_GRE,HIGH); 	// Green LED (9) ON (For when temperature is *too high*? GREEN?)
    digitalWrite(FMQ,LOW);    		// Buzzer (10) on LOW
  }else{
    digitalWrite(LED_GRE,LOW);   	// Green LED OFF
    digitalWrite(FMQ,HIGH);
  }

  // If humidity is too high
  if(humi > maxHumi){
    digitalWrite(LED_RED,HIGH);  	// Red LED ON (7)
    digitalWrite(FMQ,LOW);  	    // Buzzer on LOW (13)
  }else{
    digitalWrite(LED_RED,LOW);  	// Red LED OFF (7)
    digitalWrite(FMQ, HIGH); 	   // Buzzer on HIGH (13)
  }

  // Get Smoke status... Then do nothing with the value apart
  //  from output to LCD?!
  int val;
  val=analogRead(0);

  // BEGIN DEBUG:
  DEBUG_SEND("smo:");
  DEBUG_SEND(val);
  // EOF DEBUG.

  delay(100);

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

// Check btnPin for a press, if so - increment associated var,
//  do bounds checking - lowering if required.
void keyIncrementer( int btnPin, int lower, int upper, int &cur )
{
  if( buttonCheck( btnPin ) )
  {
    cur++;
    if( cur > upper ) cur = lower;
  }
}

// Check for a button press on a given PIN.
bool buttonCheck( int pin )
{
	if( analogRead(pin) > 600 )
	{
		delay(20);
		if( analogRead(pin) > 600 )
		{
			while( analogRead(pin) > 600 ); // Blocks execution until button is released.
			return true;
		}
	}
	return false;
}

// Get the values from the DHT; adheres to the protocol found in the
//  DHT11 Lib. ( http://playground.arduino.cc/main/DHT11Lib )
bool getDHTValues( int pin )
{
	unsigned long time;		// Time
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;

	delay(200);					// Delay
	pinMode( pin,OUTPUT );		// Set DHT11 Signal (8) to output
	digitalWrite( pin, LOW );		// Write low.
	delay(20);					// Wait 20ms
	digitalWrite( pin, HIGH );	// Write High
	delayMicroseconds(40);		// delay 40ms
	digitalWrite( pin, LOW );		// Write LOw
	pinMode( pin,INPUT );			// Set to input

	if(! checkForDHTAcknowledgement( pin, LOW ) )	return false;
	if(! checkForDHTAcknowledgement( pin, HIGH ) )	return false;

	for(int i=0;i<40;i++)
	{
		if(! checkForDHTAcknowledgement( pin, LOW ) )	return false;

		time = micros();
		if(! checkForDHTAcknowledgement( pin, HIGH ) )	return false;

		if( (micros() - time) > 40 ) bits[idx] != (1 << cnt );
		if( cnt == 0 )
		{
			cnt = 7;
			idx++;
		} else {
			cnt--;
		}
	}

	Sensors.humi = bits[0];
	Sensors.temp = bits[2];
	Sensors.tol = bits[4];

	if( bits[0] + bits[2] != tol )
	{
		DEBUG_SEND("DHT11 REPORTS INVALID VALUES.");	// Provide visual feedback?
	    return false;
	}

	return true;
}

// Check for acknowledgement from the DHT.
inline bool checkForDHTAcknowledgement( int pin, uint8_t level )
{
	// Check for acknowledgement; otherwise timeout.
	unsigned int loopCnt = 10000;
	while( digitalRead(pin) == level )
	{
		if( loopCnt-- == 0 )
		{
			DEBUG_SEND("DHT11 TIMED OUT.");	// Provide visual feedback?
			return false;
		}
	}
}

void lcd_OutputAll()
{
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("TEMP:  C");
  lcd.setCursor(9,0);
  lcd.print("SMO:");
  lcd.setCursor(0,1);
  lcd.print("HUMI:  %");
  lcd.setCursor(9,1);
  lcd.setCursor(5,0);
  lcd.print(temp);
  lcd.setCursor(5,1);
  lcd.print(humi);
  lcd.setCursor(13,0);
  lcd.print(val,DEC);
}

void lcd_OutputTemp()
{
  lcd.begin(16,2);
  lcd.print("ALARM TEMP: ");
  lcd.setCursor(11,0);
  lcd.print(maxTemp);
}

void lcd_OutputHumi()
{
  lcd.begin(16,2);
  lcd.print("ALARM HUMI: ");	// Perhaps display the current val as well as the max val?
  lcd.setCursor(12,0);
  lcd.print(maxHumi);
}
