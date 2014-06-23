/*
** DHT11 Sensor Methods
*/

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
