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


// Component Objects
LiquidCrystal lcd( LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7 );

// Variables
int temp, humi, tol;	// Hold readings from DHT11
int lcdOutput=0;		// State variable: Contains LCD Output Mode

int H=50;				// Humidity Threshold
int T=25;				// Temperature Threshold

unsigned int loopCnt;	// Loop Counter
int chr[40] = {0};		// Buffer, Store DHT11 Output as binary
unsigned long time;		// Time

uint8_t bits[5];
uint8_t cnt = 7;
uint8_t idx = 0;

//
void setup()
{ 
  pinMode(FMQ,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GRE,OUTPUT);

  // BEGIN DEBUG:
  Serial.begin(9600);
  // EOF DEBUG.
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
  Serial.println(temp);
  Serial.print("humi:");
  Serial.println(humi);
  Serial.print("tol:");
  Serial.println(tol);
  // EOF DEBUG.

  // If temperature is too high
  if(temp>T)
  {
  	// Green LED (9) ON (For when temperature is *too high*? GREEN?)
    digitalWrite(LED_GRE,HIGH);
    // Buzzer (10) on LOW
    digitalWrite(FMQ,LOW);
  }else{
  	// Green LED OFF
    digitalWrite(LED_GRE,LOW);
    // Buzzer on HIGH
    digitalWrite(FMQ,HIGH);
  } 

  // If humidity is too high
  if(humi>H)
  {
  	// Red LED ON (7)
    digitalWrite(LED_RED,HIGH);
    // Buzzer on LOW (13)
    digitalWrite(fmq,LOW);
  }else{
  	// Red LED OFF (7)
    digitalWrite(LED_RED,LOW);
    // Buzzer on HIGH (13)
    digitalWrite(fmq,HIGH);
  }

  // Get Smoke status... Then do nothing with the value apart 
  //  from output to LCD?!
  int val;
  val=analogRead(0);

  // BEGIN DEBUG:
  Serial.print("smo:");
  Serial.println(val,DEC);
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
    lcd.print("TEMP: ");
    lcd.setCursor(11,0);
    lcd.print(T);

  } else if( LCD_OUTPUT_HUMI == lcdOutput ) {

    lcd.begin(16,2);
    lcd.print("HUMI: ");
    lcd.setCursor(12,0);
    lcd.print(H);

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
		H++;
		if( H > 60 ) H = 40;
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
		T++;
		if( T > 30 ) T = 20;
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
bool getDHTValues( pin )
{
  delay(200);					// Delay
  pinMode( pin,OUTPUT );		// Set DHT11 Signal (8) to output
  digitalWrite( pin, LOW );		// Write low.
  delay(20);					// Wait 20ms
  digitalWrite( pin, HIGH );	// Write High
  delayMicroseconds(40);		// delay 40ms
  digitalWrite( pin, LOW );		// Write LOw
  pinMode( pin,INPUT );			// Set to input


  if(! checkForDHTAcknowledgement( pin, LOW ) )		return false;
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
		// BEGIN DEBUG:
	    Serial.println("DHT11 REPORTS INVALID VALUES.");	// Provide visual feedback?
	    // EOF DEBUG.
	    return false;
  }

  return true;
}

// Check for acknowledgement from the DHT.
inline bool checkForDHTAcknowledgement( int pin, uint8_t level )
{
  // Check for acknowledgement; otherwise timeout.
  loopCnt = 10000;
  while( digitalRead(pin) == level )
  {
  	if( loopCnt-- == 0 )
  	{
	  // BEGIN DEBUG:
      Serial.println("DHT11 TIMED OUT.");	// Provide visual feedback?
      return false;
      // EOF DEBUG.
  	}
  }
}