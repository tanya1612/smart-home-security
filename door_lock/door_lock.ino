#include <Keypad.h> //  For Keypad
#include <Servo.h>  //  For Servo
#include <LiquidCrystal.h>  //  For LCD
#include <SoftwareSerial.h> //  For ESP

//  ESP
SoftwareSerial esp(18, 19);// RX, TX

//  Wifi credentials
const String ssid = "your-ssid";  // Update here
const String wifipass = "you-password"; // Update here
//  ThingSpeak API Setting
const String host = "api.thingspeak.com";
const String port = "80";

//  LCD
const byte rs = 12, en = 11;
LiquidCrystal lcd (rs, en, 14, 15, 16, 17);

//  Keypad
const byte row = 4, col = 4;
const byte rowPin[row] = {5, 4, 3, 2};
const byte colPin[col] = {9, 8, 7, 6};
const char keys[row][col] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad kyd (makeKeymap(keys), colPin, rowPin, col, row);

//  Servo
const int motorPin = 10;
const int motorDelay = 15;
int pos = 0;
Servo motor;

//  Door credentials
#define LEN 4
String master = "1234", pass = "0000";
char key;
int indx = 0, wratmpt = 0;
bool doorOpen = false, newPass = false;
int freq[4][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
bool firstTime = true;

//  Send AT command
void SndCmd(String cmd, char* reply, int tym)
{
  boolean found = false;
  int ctym = 0;
  Serial.println(cmd);
  while(ctym < tym)
  {
    esp.println(cmd);
    if(esp.find(reply))
    {
      found = true;
      break;
    }
    ctym++;
  }
  if(found == true) Serial.println("OK");
  else  Serial.println("Fail");
}

//  Connect to wifi
void WifiConnect()
{
  SndCmd("AT+CWMODE=1", "OK", 5);
  SndCmd("AT+CWJAP=\""+ssid+"\",\""+wifipass+"\"", "OK", 5);
  SndCmd("AT+CIPMUX=0", "OK", 5);
}

//  Send a Mail
void SndMail()
{
  String data = "GET https://api.thingspeak.com/update?api_key=\your-api-key="+master;  //  Update here
  SndCmd("AT+CIPSTART=\"TCP\",\""+host+"\","+port, "OK", 15);
  SndCmd("AT+CIPSEND="+String(data.length()+2), ">", 4);
  esp.println(data);
}

//  Reset Pin
int GetMax(int ix)
{
  int max = 0, pos = 0;
  for(int i = 0; i < 10; ++i)
  {
    if(freq[ix][i] > max)
    {
      max = freq[ix][i];
      pos = i;
    }
  }
  return pos;
}

void ResetPass()
{
  wratmpt = 0;
  int ipass = 0;
  if(firstTime)
  {
    firstTime = false;
    for(int i = 0; i < LEN; ++i)
    {
      int digit = random(0, 10);
      ++freq[i][digit];
      ipass = (ipass*10) + digit;
    }
  }
  else
  {
    for(int i = 0; i < LEN; ++i)
    {
      int digit = GetMax(i);
      ++freq[i][digit];
      ipass = (ipass*10) + digit;
    }
  }
  master = String(ipass);
  SndMail();
}

//  Wrong Pin action
void WrongPass()
{
  ++wratmpt;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("WRONG PASSWORD");
  if(wratmpt <= 5)  delay(5000);
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("WAIT 10 SECONDS");
    delay(10000);
    wratmpt = 0;
  }
}

//  Get Password from user
int GetPass()
{
  pass = "";
  lcd.clear();
  lcd.setCursor(1, 0);
  if(newPass) lcd.print("NEW PASSWORD");
  else  lcd.print("ENTER PASSWORD");
  lcd.setCursor(6, 1);
  lcd.print("****");
  lcd.setCursor(6, 1);
  while (indx < LEN)
  {
    do
    {
      key = kyd.getKey();
    } while (key == NO_KEY || key == 'A' || key == 'C' || key == 'D' || key == '*' || key == '#');
    if(key == 'B')
    {
      ResetPass();
      return 1;
    }
    else
    {
      pass += key;
      ++indx;
      lcd.print(key);
    }
  }
  indx = 0;
  return 0;
}

//  Compare password
bool ComparePass()
{
  for (int i = 0; i < LEN; ++i)
    if (master[i] != pass[i])  return false;
  return true;
}

//  Update the frequency matrix
void UpdateFreq()
{
  for(int i = 0; i < LEN; ++i)
  {
    int digit = master[i] - '0';
    ++freq[i][digit];
  }
}

//  Change Pin
void ChangePass()
{
  if(GetPass() == 1)
  {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.println("RESET");
    delay(2000);
  }
  else if(ComparePass())
  {
    wratmpt = 0;
    newPass = true;
    GetPass();
    master = pass;
    UpdateFreq();
    pass = "";
    newPass = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD CHANGED");
  }
  else  WrongPass();
}

//  Open the door
void DoorOpen()
{
  wratmpt = 0;
  for (pos = 0; pos <= 45; pos += 1) 
  {
    motor.write(pos);
    delay(motorDelay);
  }
  motor.write(90);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("DOOR OPEN");
  doorOpen = true;
  pass = "";
}

//  Close the door
void DoorClose()
{ 
  for (pos = 180; pos >= 135; pos -= 1)
  {
    motor.write(pos);
    delay(motorDelay);
  }
  motor.write(90);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("DOOR CLOSE");
  delay(1000);
  doorOpen = false;
  pass = "";
}

//  Setup
void setup()
{
  //  Serial monitor and ESP
  Serial.begin(9600);
  esp.begin(9600);

  //  Servo motor
  motor.attach(motorPin);
  motor.write(90);

  //  LCD
  for (int i = 14; i <= 17; ++i) pinMode(i, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WELCOME!");
  lcd.setCursor(3, 1);
  lcd.print("DOOR LOCK");

  //  Internet service
  WifiConnect();
  
  //  For random seed
  digitalWrite(13, HIGH);
  while(digitalRead(13) == HIGH);
  randomSeed(millis()%1000);
  
  ResetPass();
  delay(2000);
}

//  Loop
void loop()
{
  if (doorOpen)
  {
    do
    {
      key = kyd.getKey();
    } while (key == NO_KEY);
    if (key == '*') DoorClose();
    else if(key == 'C') ChangePass();
  }
  else
  {
    if(GetPass() == 1)
    {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.println("RESET");
      delay(2000);
    }
    else
    {
      if (ComparePass())  DoorOpen();
      else  WrongPass();
    }
  }
}
