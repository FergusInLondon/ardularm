/* 
** Source code to ICStation's 
**  "Temperature & Humidity & Smoke Detector Alarm System Kit"
**   http://www.icstation.com/product_info.php?products_id=3665#.U4IX4h89jC0
**
** Original Source code available from:
**  http://www.icstation.com/newsletter/eMarketing/alarm%20system%20code.txt
*/

#include<LiquidCrystal.h>
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
void setup()
{ 
  pinMode(fmq,OUTPUT);
  pinMode(FMQ,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(led,OUTPUT);
  Serial.begin(9600);
}
void loop()
{ HH();
  TT();
  keyScan();
  bgn:
  delay(200);
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
  delay(20);
  digitalWrite(pin,HIGH);
  delayMicroseconds(40);
  digitalWrite(pin,LOW);
  pinMode(pin,INPUT);
  loopCnt=10000;
  while(digitalRead(pin) != HIGH)
  {
    if(loopCnt-- == 0)
    {
      Serial.println("HIGH");
      goto bgn;
    }
  }
  
  loopCnt=30000;
  while(digitalRead(pin) != LOW)
  {
    if(loopCnt-- == 0)
    {
      Serial.println("LOW");
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
   
humi=chr[0]*128+chr[1]*64+chr[2]*32+chr[3]*16+chr[4]*8+chr[5]*4+chr[6]*2+chr[7];
   
temp=chr[16]*128+chr[17]*64+chr[18]*32+chr[19]*16+chr[20]*8+chr[21]*4+chr[22]*2+chr[23];
tol=chr[32]*128+chr[33]*64+chr[34]*32+chr[35]*16+chr[36]*8+chr[37]*4+chr[38]*2+chr[39];
  Serial.print("temp:");
  Serial.println(temp);
  Serial.print("humi:");
  Serial.println(humi);
  Serial.print("tol:");
  Serial.println(tol);

 if(temp>T)
 {
   digitalWrite(LED,HIGH);
   digitalWrite(FMQ,LOW);
  }else{
 digitalWrite(LED,LOW);
 digitalWrite(FMQ,HIGH);
 } 
 if(humi>H)
 {
   digitalWrite(led,HIGH);
   digitalWrite(fmq,LOW);
  }else{
 digitalWrite(led,LOW);
 digitalWrite(fmq,HIGH);
 }
 
 int val;
    val=analogRead(0);
    Serial.print("smo:");
    Serial.println(val,DEC);
    delay(100);
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
if(flag==1)
{  
  lcd.begin(16,2);
  lcd.print("ALARM TEMP:");
  lcd.setCursor(11,0);
  lcd.print(T);

}
if(flag==2)
{  
  lcd.begin(16,2);
  lcd.print("ALARM  HUMI:");
  lcd.setCursor(12,0);
  lcd.print(H);

}
}
void keyScan()
{
  if(analogRead(BUTTON)>600)
   {
     delay(20);
     if(analogRead(BUTTON)>600)
     {
       flag++;
       if(flag>=3)
         flag=0;
       while(analogRead(BUTTON)>600);
     }
   }
}
void HH()
{
  if(analogRead(B)>600)
   {
     delay(20);
     if(analogRead(B)>600)
     {
       H++;
       if(H>=61)
         H=40;
       while(analogRead(B)>600);
     }
   }
}
void TT()
{
  if(analogRead(BU)>600)
   {
     delay(20);
     if(analogRead(BU)>600)
     {
       T++;
       if(T>=31)
         T=20;
       while(analogRead(BU)>600);
     }
   }
}
