/* 
** Source code to ICStation's 
**  "Temperature & Humidity & Smoke Detector Alarm System Kit"
**   http://www.icstation.com/product_info.php?products_id=3665#.U4IX4h89jC0
**
** Original Source code available from:
**  http://www.icstation.com/newslecheckTemper/eMarketing/alarm%20system%20code.txt
*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);
int temp;
int humi;
int tol;
int j;
int H=50;
int T=25;
int B=1;
int flag=0;
int led=7;
int LED=9;
int BU=2;
int BUTTON=3;
int FMQ=13;
int fmq=10;
unsigned int loopCnt;
int chr[40] = {0};
unsigned long time;
#define pin 8

//
void setup()
{ 
  pinMode(fmq,OUTPUT);
  pinMode(FMQ,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(led,OUTPUT);

  // BEGIN DEBUG:
  Serial.begin(9600);
  // EOF DEBUG.
}

//
void loop()
{
  keyHumidityAdjust();		// Check for Humidity Interaction
  keyTempAdjust();			// Check for Temperature Interaction
  keyResetScan();			// CHeck for Reset Interaction


  bgn:								// BEGIN LABEL.

  /*
  ** BEGIN HUMIDITY AND TEMPERATURE SENSING (DHT11 - PIN 8)
  */

  // EXTRACT BELOW LOGIC TO SOMEWHERE MORE SANE.
  delay(200);						// Delay
  pinMode(pin,OUTPUT);				// Set DHT11 Signal (8) to output
  digitalWrite(pin,LOW);			// Write low.
  delay(20);						// Wait 20ms
  digitalWrite(pin,HIGH);			// Write High
  delayMicroseconds(40);			// delay 40ms
  digitalWrite(pin,LOW);			// Write LOw
  pinMode(pin,INPUT);				// Set to input
  loopCnt=10000;
  
  while(digitalRead(pin) != HIGH)
  {
    if(loopCnt-- == 0)
    {
	  // BEGIN DEBUG:
      Serial.println("HIGH");
      // EOF DEBUG.
      goto bgn;
    }
  }
  
  loopCnt=30000;
  while(digitalRead(pin) != LOW)
  {
    if(loopCnt-- == 0)
    {
	  // BEGIN DEBUG:
      Serial.println("LOW");
      // EOF DEBUG.
      goto bgn;
    }
  }
 
  for(int i=0;i<40;i++)
  {
    while(digitalRead(pin) == LOW)
    {}
    time = micros();
    while(digitalRead(pin) == HIGH)
    {}
    if (micros() - time >50)
    {
      chr[i]=1;
    }else
    {
      chr[i]=0;
    }
  }
  

  // TODO: WEIRD BINARY CONVERSION.
  humi=chr[0]*128+chr[1]*64+chr[2]*32+chr[3]*16+chr[4]*8+chr[5]*4+chr[6]*2+chr[7];
  temp=chr[16]*128+chr[17]*64+chr[18]*32+chr[19]*16+chr[20]*8+chr[21]*4+chr[22]*2+chr[23];
  tol=chr[32]*128+chr[33]*64+chr[34]*32+chr[35]*16+chr[36]*8+chr[37]*4+chr[38]*2+chr[39];

  /*
  ** EOF DHT11 SENSING
  */

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
    digitalWrite(LED,HIGH);
    // Buzzer (10) on LOW
    digitalWrite(FMQ,LOW);
  }else{
  	// Green LED OFF
    digitalWrite(LED,LOW);
    // Buzzer on HIGH
    digitalWrite(FMQ,HIGH);
  } 

  // If humidity is too high
  if(humi>H)
  {
  	// Red LED ON (7)
    digitalWrite(led,HIGH);
    // Buzzer on LOW (13)
    digitalWrite(fmq,LOW);
  }else{
  	// Red LED OFF (7)
    digitalWrite(led,LOW);
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

  // If All is OK; Draw variables on LCD.
  if(flag==0)
  {
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("TEMP:  C");
    lcd.setCursor(9,0);
    lcd.print("SMO:");
    lcd.setCursor(0,1);
    lcd.print("HUMI:  %");
    lcd.setCursor(9,1);
    //lcd.print("BEP:"); 
    lcd.setCursor(5,0);
    lcd.print(temp);  
    lcd.setCursor(5,1);
    lcd.print(humi);
    lcd.setCursor(13,0);
    lcd.print(val,DEC);
  }

  // If temperature is too high, draw alert on LCD.
  if(flag==1)
  {  
    lcd.begin(16,2);
    lcd.print("ALARM TEMP:");
    lcd.setCursor(11,0);
    lcd.print(T);
  }

  // If humidity is too high, draw alert on LCD.
  if(flag==2)
  {
    lcd.begin(16,2);
    lcd.print("ALARM  HUMI:");
    lcd.setCursor(12,0);
    lcd.print(H);
  }
}


// Detect BUTTON presses on Pin A3, and - I think - reset the alarm?
//  Doesn't actually make a great deal of sense to be honest, alarm
//  will automatically go off again unless the environmental factor
//  is rectified, and then the alarm will stop automatically anyway.
void keyScan()
{
	if( buttonCheck( BUTTON ) )
	{
		flag++;
		if( flag > 2 ) flag = 0;
	}
}

// Detect BUTTON presses on Pin A1, and increase humidity threshold.
//  Should humidity be set above 61, revert back to 20.
//  Some form of UI feedback (LCD and DEBUG?) would be nice.
void keyHumidityAdjust()
{
	if( buttonCheck( B ) )
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
	if( buttonCheck( BU ) )
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