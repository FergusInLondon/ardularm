/*
** Source code to ICStation's
**  "Temperature & Humidity & Smoke Detector Alarm System Kit"
**   http://www.icstation.com/product_info.php?products_id=3665#.U4IX4h89jC0
**
** Original Source code available from:
**  http://www.icstation.com/newslecheckTemper/eMarketing/alarm%20system%20code.txt
*/

#include <LiquidCrystal.h>

//
// CONSTANTS
//

// - STATES
#define LCD_OUTPUT_ALL 0
#define LCD_OUTPUT_TEMP 1
#define LCD_OUTPUT_HUMI 2

// - PINS
#define BTN_ADJUST_HUMI 1
#define BTN_ADJUST_TEMP 2
#define BTN_ADJUST_LCD 3
#define LED_RED 7
#define LED_GRE 9
#define FMQ 13
#define DHT_SENSOR 8

// - LCD PINS (Hitache HD44780 Compat.)
#define LCD_ENABLE 11
#define LCD_RS 12
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#ifdef DEBUG
#define DEBUG_SEND( msg ) DEBUG_SEND( msg );
#else
#define DEBUG_SEND( msg )
#endif

// Component Objects
LiquidCrystal lcd( LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );

// Variables
int temp, humi, tol;	// Hold readings from DHT11
int lcdOutput=0;		// State variable: Contains LCD Output Mode

int maxHumi=50;			// Humidity Threshold
int maxTemp=25;			// Temperature Threshold

//
void setup()
{
  pinMode(FMQ,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GRE,OUTPUT);

  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

//
void loop()
{
  keyHumidityAdjust();		// Check for Humidity Interaction
  keyTempAdjust();			// Check for Temperature Interaction
  keyLCDModeScan();			// CHeck for Reset Interaction

  while(! getDHTValues( DHT_SENSOR ) );

  // BEGIN DEBUG:
  Serial.print("temp:");
  DEBUG_SEND(temp);
  Serial.print("humi:");
  DEBUG_SEND(humi);
  Serial.print("tol:");
  DEBUG_SEND(tol);
  // EOF DEBUG.

  // If temperature is too high
  if(temp > maxTemp)
  {
    digitalWrite(LED_GRE,HIGH); 	// Green LED (9) ON (For when temperature is *too high*? GREEN?)
    digitalWrite(FMQ,LOW);    		// Buzzer (10) on LOW
  }else{
    digitalWrite(LED_GRE,LOW);   	// Green LED OFF
    digitalWrite(FMQ,HIGH);
  }

  // If humidity is too high
  if(humi > maxHumi)
  {
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
  if( LCD_OUTPUT_ALL == lcdOutput )
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

  } else if( LCD_OUTPUT_TEMP == lcdOutput ) {

    lcd.begin(16,2);
    lcd.print("ALARM TEMP: ");
    lcd.setCursor(11,0);
    lcd.print(maxTemp);

  } else if( LCD_OUTPUT_HUMI == lcdOutput ) {

    lcd.begin(16,2);
    lcd.print("ALARM HUMI: ");	// Perhaps display the current val as well as the max val?
    lcd.setCursor(12,0);
    lcd.print(maxHumi);

  }
}


// Detect BUTTON presses on Pin A3, and switch the LCD display
// output format.
void keyLCDModeScan()
{
	if( buttonCheck( BTN_ADJUST_LCD ) )
	{
		lcdOutput++;
		if( lcdOutput > 2 ) lcdOutput = 0;
	}
}

// Detect BUTTON presses on Pin A1, and increase humidity threshold.
//  Should humidity be set above 61, revert back to 20.
//  Some form of UI feedback (LCD and DEBUG?) would be nice.
void keyHumidityAdjust()
{
	if( buttonCheck( BTN_ADJUST_HUMI ) )
	{
		maxHumi++;
		if( maxHumi > 60 ) maxHumi = 40;
	}
}

// Detect BUTTON presses on Pin A2, and increase temperature
//  threshold. Should temperature be set above 31, revert back
//  to 20.
//  Some form of UI feedback (LCD and DEBUG?) would be nice.
void keyTempAdjust()
{
	if( buttonCheck( BTN_ADJUST_TEMP ) )
	{
		maxTemp++;
		if( maxTemp > 30 ) maxTemp = 20;
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

	humi = bits[0];
	temp = bits[2];
	tol = bits[4];

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
