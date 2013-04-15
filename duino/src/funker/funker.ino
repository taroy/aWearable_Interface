#include <Servo.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SerialLCD.h>
#include <PString.h> 
#include <arduino.h>


// initialize the library
SerialLCD slcd(11,12);//this is a must, assign soft serial pins
//const int ledPin =  9;


TinyGPS gps;
SoftwareSerial nss(2, 3);

static void gpsdump(TinyGPS &gps);
static bool feedgps();
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
static void print_str_target(const char *str, int len);

char messageBuffer[20], cmd[3], range[4], pin[4], val[4], aux[4], lat[10], lon[10], led[3], disp[5];
boolean debug = false;
int index = 0;
Servo servo;
boolean inRange;
unsigned long distance1;

void setup() {
  Serial.begin(115200);
  slcd.begin();
  nss.begin(9600);
  
  
 
  
  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
  Serial.print("Sizeof(gpsobject) = "); Serial.println(sizeof(TinyGPS));
  Serial.println();
  Serial.println("Sats HDOP Latitude Longitude Fix  Date       Time       Date Alt     Course Speed Card  Distance Course Card  Chars Sentences Checksum");
  Serial.println("          (deg)    (deg)     Age                        Age  (m)     --- from GPS ----  ---- to target  ----  RX    RX        Fail");
  Serial.println("--------------------------------------------------------------------------------------------------------------------------------------");

}

/**
 * Deal with a full message and determine function to call
 */
void process() {
  index = 0;
 
  Serial.println(messageBuffer);

  Serial.println(messageBuffer[0]);
  int i = 0;
  while(i< sizeof(messageBuffer)){
    
    strncpy(cmd, messageBuffer, 2);
    cmd[2] = '\0';
    
    if(messageBuffer[i]=='r'){
      strncpy(range, messageBuffer + i + 1, 3);
      range[3] = '\0';
    }
    else if(messageBuffer[i]=='l'){
      strncpy(led, messageBuffer + i + 1, 2);
      led[2] = '\0';
    }
    else if(messageBuffer[i]=='d'){
      strncpy(disp, messageBuffer + i + 1, 4);
      disp[4] = '\0';
    }
    i++;
  }
  
  Serial.println("cmd");
  Serial.println(cmd);
  Serial.println("range");
  Serial.println(range);
  Serial.println("led");
  Serial.println(led);
  Serial.println("disp");
  Serial.println(disp);
  
  
  if (debug) {
    Serial.println(messageBuffer); }

  int cmdid = atoi(cmd);
  
  //Not using this code
  switch(cmdid) {
    case 0:  sm(pin,val);               break;
    case 1:  dw(pin,val);               break;
    case 2:  dr(pin);                   break;
    case 3:  aw(pin,val);               break;
    case 4:  ar(pin);                   break;
    case 90: autoReply();               break;
    case 98: handleServo(pin,val,aux);  break;
    case 99: toggleDebug(val);          break;
    default:                            break;
  }
  
}

void loop() {
  /**
   * Waiting for commands
   * Building the messageBuffer
   */
  
  while(Serial.available() > 0) {
    char x = Serial.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') process(); // end
    else messageBuffer[index++] = x;
   
  }
  /**
   * GPS update
   */
  bool newdata = false;
  unsigned long start = millis();
  
  // Every second we print an update - from GPS
  while (millis() - start < 1000)
  {
    if (feedgps())
      newdata = true;
  }
  
  gpsdump(gps);
  
 
}


static void gpsdump(TinyGPS &gps)
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  
  //Hardcoded locations
  static const float TARGET_LAT1 = 63.41696, TARGET_LON1 = 10.40298;
  //static const float TARGET_LAT2 = 59.13131, TARGET_LON2 = 10.216595;
  

  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 9, 5);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 10, 5);
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);

  print_date(gps);

  print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 8, 2);
  print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
  print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6);
  
  print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0UL : (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT1, TARGET_LON1), 0xFFFFFFFF, 9);
  print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : TinyGPS::course_to(flat, flon,TARGET_LAT1, TARGET_LON1), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  print_str_target(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, TARGET_LAT1, TARGET_LON1)), 6);


  //print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0UL : (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT2, TARGET_LON2), 0xFFFFFFFF, 9);
  //print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : TinyGPS::course_to(flat, flon,TARGET_LAT2, TARGET_LON2), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  //print_str_target(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, TARGET_LAT2, TARGET_LON2)), 6);
  
  distance1 = (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT1, TARGET_LON1);
  update_lcd(distance1, range);
  //update_lcd(distance1, range);
  
  //distance2 = (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT2, TARGET_LON2); 
  //Serial.println(distance2);
  //distance = getNearest(distance1, distance2);
  //Serial.println(distance);
  //in_range(distance1, range);
  
  gps.stats(&chars, &sentences, &failed);
  print_int(chars, 0xFFFFFFFF, 6);
  print_int(sentences, 0xFFFFFFFF, 10);
  print_int(failed, 0xFFFFFFFF, 9);
  Serial.println();  
  
}



//Showing the nearest device
unsigned long getNearest(unsigned long distance1, unsigned long distance2)
{
  unsigned long distance = 0;
  if (distance1 < distance2)
  {
    //in_range(distance1, range);
    Serial.println("distance1");
    distance = distance1;
  }
  else if(distance2 < distance1)
  {
    //in_range(distance2, range);
    Serial.println("distance2");
    distance = distance2;
  }
  return distance;
}

/*
//MARIA - Is distance within range?
boolean in_range(unsigned long val, char range[])
{
  char buffer[12];
  char buffer2[12];
  val = val;
  PString(buffer, sizeof(buffer), val);
  PString(buffer2, sizeof(buffer2), range);
  int r = atoi(range); 
  Serial.println("range");
  Serial.println(buffer2);
  Serial.println("distance");
  Serial.println(buffer);
  //slcd.begin();
  slcd.print("hei");
  if(r!=0) {
    Serial.println(r);

    if(val < r) 
    {
      Serial.println("true");
      inRange = true;
      Serial.println("in range");
      //Updating LCD
      
      //slcd.print("distance :");
      //slcd.print(buffer);  
      //slcd.print("m");
      //slcd.setCursor(0, 1);
      //slcd.print("range :");
      //slcd.print(buffer2);
    }
    else
    {
      inRange = false;
      //slcd.print("not in range");
      Serial.println("not in range");
    }
  }
  else if(r==0){
     Serial.println("r er null");
  }
  
  return inRange; 
}
*/


static void update_lcd(unsigned long val, char range[])
{
  Serial.println("update lcd");
  char buffer[12];
  PString(buffer, sizeof(buffer), val);
  slcd.clear();
  delay(1000);
  slcd.setCursor(0,0);
  slcd.print("Distance :");
  slcd.print(buffer);  
  slcd.print("m");
    
  feedgps();

}



static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  feedgps();
  
}

static void print_float(float val, float invalid, int len, int prec)
{
  char sz[32];
  if (val == invalid)
  {
    strcpy(sz, "*******");
    sz[len] = 0;
        if (len > 0) 
          sz[len-1] = ' ';
    for (int i=7; i<len; ++i)
        sz[i] = ' ';
    Serial.print(sz);
    
  }
  else
  {
    Serial.print(val, prec);
    
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1);
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(" ");
  }
  feedgps();
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("*******    *******    ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d   ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  feedgps();
}


static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
    
  feedgps();
}

//calls blinking_lights
static void print_str_target(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
}

 
static bool feedgps()
{
  while (nss.available())
  {
    if (gps.encode(nss.read()))
      return true;
  }
  return false;
}



/**
 * Toggle debug mode
 * @param char val value for enabling or disabling debugger (0 = false, 1 = true)
 */
void toggleDebug(char *val) {
  if (atoi(val) == 0) {
    debug = false;
    Serial.println("goodbye");
  } else {
    debug = true;
    Serial.println("hello");
  }
}

void autoReply() {
   Serial.println('Is Dave there?'); 
}

/**
 * Set pin mode
 * @param char pin identifier for pin
 * @param char val set pit to OUTPUT or INPUT
 */
void sm(char *pin, char *val) {
  if (debug) {
    Serial.println("sm"); }
    
  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {  
    if (atoi(val) == 0) {
      pinMode(p, OUTPUT);
    } else {
      pinMode(p, INPUT);
    }
  }
}

/**
 * Digital write
 * @param char pin identifier for pin
 * @param char val set pin to HIGH or LOW
 */
void dw(char *pin, char *val) {
  if (debug) {
    Serial.println("dw"); }
    
  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {  
    pinMode(p, OUTPUT);
    if (atoi(val) == 0) {
      digitalWrite(p, LOW);
    } else {
      digitalWrite(p, HIGH);
    }
  }
}

/**
 * Digital read
 * @param char pin pin identifier
 */
void dr(char *pin) {
  if (debug) {
    Serial.println("dr"); }
    
  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {
    pinMode(p, INPUT);
    int oraw = digitalRead(p);
    char m[7];
    sprintf(m, "%02d::%02d", p,oraw);
    Serial.println(m);
  }
}

/**
 * Analog read
 * @param char pin pin identifier
 */
void ar(char *pin) {
  if (debug) {
    Serial.println("ar"); }

  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {
    pinMode(p, INPUT); // don't want to sw
    int rval = analogRead(p);
    char m[8];
    sprintf(m, "%s::%03d", pin, rval);
    Serial.println(m);
  }
}  

/*
 * Analog write
 * @param char pin pin identifier
 * @param char val value to write 
 */
void aw(char *pin, char *val) {
  if (debug) {
    Serial.println("aw"); }
    
  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {
    pinMode(p, OUTPUT);
    analogWrite(p, atoi(val));
  }
}

int getPin(char *pin) { //Converts to A0-A5, and returns -1 on error
  int ret = -1;
  if (pin[0] == 'A' || pin[0] == 'a') {
    switch(pin[1]) {
      case '0': ret = A0; break;
      case '1': ret = A1; break;
      case '2': ret = A2; break;
      case '3': ret = A3; break;
      case '4': ret = A4; break;
      case '5': ret = A5; break;
      default:            break;
    }
  } else {
    ret = atoi(pin);
    if (ret == 0 && (pin[0] != '0' || pin[1] != '0')) {
      ret = -1; }
  }
  
  return ret;
}
  

/*
 * Handle Servo commands
 * attach, detach, write, read, writeMicroseconds, attached
 */
void handleServo(char *pin, char *val, char *aux) {
  if (debug) {
    Serial.println("ss"); }
    
  int p = getPin(pin);
  if (p == -1 && debug) {
    Serial.println("badpin"); 
  } else {
    Serial.println("got signal");
    if (atoi(val) == 0) {
      servo.detach();
    } else if (atoi(val) == 1) {
      servo.attach(p);
      Serial.println("attached");
    } else if (atoi(val) == 2) {
      Serial.println("writing to servo");
      Serial.println(atoi(aux));
      servo.write(atoi(aux));
    }  
  }
}


