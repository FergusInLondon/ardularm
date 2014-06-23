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

// BOUNDS
#define HUMI_MAX 50
#define HUMI_MIN
#define TEMP_MAX 25
#define TEMP_MIN

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

typedef struct {
  int minTemp;
  int maxTemp;
  int minHumi;
  int maxHumi;
  int temp;
  int humi;
  int tol;
} Sensors;

typedef enum {
  ALL =  0,
  TEMP = 1,
  HUMI=  2
} LCDMode;
