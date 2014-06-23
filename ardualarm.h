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
#define SMO_SENSOR 0

// - LCD PINS (Hitache HD44780 Compat.)
#define LCD_ENABLE 11
#define LCD_RS 12
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

// BOUNDS
#define HUMI_MAX 50
#define TEMP_MAX 25

/*
** MACROS
*/

// DEBUG TO SERIAL
#ifdef DEBUG
#define DEBUG_SEND( msg ) Serial.print( msg );
#else
#define DEBUG_SEND( msg )
#endif

/*
** TYPE DEFINITIONS
*/

// Container for Sensor values
typedef struct {
  // Sensor Hardware specific ranges.
  int sensMinTemp;
  int sensMaxTemp;
  int sensmMinHumi;
  int sensMaxHumi;

  // Sensor User defined ranges.
  int maxTemp;
  int maxHumi;

  // Sensor Readings
  bool smoke;
  int temp = 0;
  int humi = 0;
  int tol = 0;
} Sensors;


// LCD Output Modes
typedef enum {
  ALL  =  0,  // DISPLAY ALL SETTINGS
  TEMP =  1,  // DISPLAY TEMPERATURE
  HUMI =  2   // DISPLAY HUMIDITY
} LCDMode;

const int lcd_num_count = HUMI - ALL;

// State enumeration - allows us to hold the entirety of our state in an 8bit int.
enum State {
  OUT_R_LED = 0x01,   // OUTPUT:  RED LED
  OUT_G_LED = 0x02,   // OUTPUT:  GREEN LED
  OUT_BUZZE = 0x04,   // OUTPUT:  BUZZER
  IN_SMOKE  = 0x08,   // INPUT:   SMOKE SENSOR
  //  N/A   = 0x10,   // FUTURE:  USER DEFINED MIN TEMPERATURE
  IN_TEMP_H = 0x20,   // STATUS:  TEMPERATURE TOO HIGH
  //  N/A   = 0x40,   // FUTURE:  USER DEFINED MIN HUMIDITY
  IN_HUMI_H = 0x08    // STATUS:  HUMDITIY TOO HIGH
};


/*
** INPUT METHODS
*/


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


/*
** OUTPUT METHODS
*/


/*
** LCD OUTPUT METHODS.
*/


// Output all current values.
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
  lcd.print( ( sensors.temp > 0 ) ? sensors.temp : 'ERR' );
  lcd.setCursor(5,1);
  lcd.print( ( sensors.humi > 0 ) ? sensors.humi : 'ERR' );
  lcd.setCursor(13,0);
  lcd.print( (sensors.smoke) ? 1 : 0 );
}

// Output *alarm temperature* only; not current temperature
//  TODO: Output both.
void lcd_OutputTemp()
{
  lcd.begin(16,2);
  lcd.print("ALARM TEMP: ");  // Perhaps display the current val as well as the max val?!?
  lcd.setCursor(11,0);
  lcd.print(maxTemp);
}

// Output *alarm humidity* only; not current humidity
//  TODO: Output both.
void lcd_OutputHumi()
{
  lcd.begin(16,2);
  lcd.print("ALARM HUMI: ");  // Perhaps display the current val as well as the max val?!?
  lcd.setCursor(12,0);
  lcd.print(maxHumi);
}
