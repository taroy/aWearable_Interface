#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SerialLCD.h>
#include <PString.h> 
#include <stdlib.h> 


// initialize the library
SerialLCD slcd(7,8);//this is a must, assign soft serial pins


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
boolean inRange;
unsigned long distance;
unsigned long degree;
char *dir_coor;
#define LED 9




void setup() {
  Serial.begin(115200);
  slcd.begin();
  Serial.println("SETUP");
  pinMode(LED, OUTPUT);
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
  
  if(messageBuffer[0]=='m'){

    int i = 1;
    while(i< sizeof(messageBuffer)){
    
      strncpy(cmd, messageBuffer + 1, 2);
      cmd[2] = '\0';
    
      strncpy(range, messageBuffer + 3, 3);
      range[3] = '\0';

      if(messageBuffer[i]=='l'){
         strncpy(led, messageBuffer + i, 2);
        led[2] = '\0';
      }
      else if(messageBuffer[i]=='d'){
        strncpy(disp, messageBuffer + i, 4);
        disp[4] = '\0';
      }
      i++;
    }
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
  //static const float TARGET_LAT1 = 63.417217, TARGET_LON1 = 10.40195;
  static const float TARGET_LAT1 = 63.411428, TARGET_LON1 = 10.412507;
  //#define TARGET_LAT1 63.41696
  //#define TARGET_LON1 10.40298

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
  
  degree = TinyGPS::course_to(flat, flon,TARGET_LAT1, TARGET_LON1);
  distance = (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT1, TARGET_LON1);
  dir_coor = (char *)TinyGPS::cardinal(TinyGPS::course_to(flat, flon, TARGET_LAT1, TARGET_LON1));
  inRange = in_range((unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT1, TARGET_LON1), range);
  

  
  if(inRange){ 
    if(disp[0]=='d')
    {
      update_lcd(distance, range, TARGET_LAT1, TARGET_LON1, dir_coor, flat, flon);
    }
    if(led[0]=='l')
    {
      Serial.println("led[0]=='l'");
      update_LED(range);
    }
  }
  else
  {
    slcd.clear();
  }

  
  gps.stats(&chars, &sentences, &failed);
  print_int(chars, 0xFFFFFFFF, 6);
  print_int(sentences, 0xFFFFFFFF, 10);
  print_int(failed, 0xFFFFFFFF, 9);
  Serial.println();  
  
}


//MARIA - Is distance within range?
boolean in_range(unsigned long val, char range[])
{ 
  char buffer[12];
  PString(buffer, sizeof(buffer), val);
  int r = atoi(range); 

  if(r!=0) {
    Serial.println(r);

    if(val < r) 
    {
      Serial.println("true");
      inRange = true;
      Serial.println("in range");
    }
    else
    {
      inRange = false;
      Serial.println("not in range");
    }
  }
  else if(r==0)
  {
     Serial.println("r er null");
  }
  
  return inRange; 
}

static void update_lcd(unsigned long val, char range[],const float TARGET_LAT, const float TARGET_LON, char *dir_coor, float flat, float flon)
{
  
  Serial.println(*dir_coor);

  char dist[12];
  PString(dist, sizeof(dist), val);
  char dir[5];
  PString(dir, sizeof(dir), dir_coor);
  char deg[5];
  PString(deg, sizeof(deg), degree);
  char targlat[10];
  dtostrf(TARGET_LAT, 9, 6, targlat);
  char targlon[10];
  dtostrf(TARGET_LON, 9, 6, targlon);
  char yourlat[10];
  dtostrf(flat, 9, 6, yourlat);
  char yourlon[10];
  dtostrf(flon, 9, 6, yourlon);

  
  slcd.clear();
  slcd.home();
  
  // d1
  if(disp[1]=='1'){
    Serial.println("display distance");
    slcd.print("Distance :");
    slcd.print(dist);  
    slcd.print("m ");

    // d12
    if(disp[2]=='2')
    {
      Serial.println("display direction");
      slcd.setCursor(0, 1);
      slcd.print("Direction :");
      slcd.print(dir); 
      delay(1000);
      
      // d123
      if(disp[3]=='3')
      {
        slcd.clear();
        slcd.home();
        slcd.print("Friend ");
        slcd.setCursor(0, 1);
        slcd.print("coordinates: ");
    
        delay(500);
        slcd.clear();
        slcd.home();
        slcd.print("lat :");
        slcd.print(targlat); 
        slcd.setCursor(0, 1);
        slcd.print("lon :");
        slcd.print(targlon);
    
        delay(1000);
        slcd.clear();
        slcd.home();
        slcd.print("Your ");
        slcd.setCursor(0, 1);
        slcd.print("coordinates: ");

        delay(500);
        slcd.clear();
        slcd.home();
        slcd.print("lat :");
        slcd.print(yourlat); 
        slcd.setCursor(0, 1);
        slcd.print("lon :");
        slcd.print(yourlon);
      }
    }
  
    // d13
    else if(disp[2]=='3')
    {
        delay(1000);
        slcd.clear();
        slcd.home();
        slcd.print("Friend ");
        slcd.setCursor(0, 1);
        slcd.print("coordinates: ");
    
        delay(500);
        slcd.clear();
        slcd.home();
        slcd.print("lat :");
        slcd.print(targlat); 
        slcd.setCursor(0, 1);
        slcd.print("lon :");
        slcd.print(targlon);
    
        delay(1000);
        slcd.clear();
        slcd.home();
        slcd.print("Your ");
        slcd.setCursor(0, 1);
        slcd.print("coordinates: ");

        delay(500);
        slcd.clear();
        slcd.home();
        slcd.print("lat :");
        slcd.print(yourlat); 
        slcd.setCursor(0, 1);
        slcd.print("lon :");
        slcd.print(yourlon);
      }
  }
    

  
  // d2
  else if(disp[1]=='2')
  {
    Serial.println("display direction");
    slcd.home();
    slcd.print("Direction :");
    slcd.print(dir);  
    
    // d23
    if(disp[2]=='3'){
      Serial.println("display coordinates");
      delay(1000);
      slcd.clear();
      slcd.home();
      slcd.print("Friend ");
      slcd.setCursor(0, 1);
      slcd.print("coordinates: ");
    
      delay(500);
      slcd.clear();
      slcd.home();
      slcd.print("lat :");
      slcd.print(targlat); 
      slcd.setCursor(0, 1);
      slcd.print("lon :");
      slcd.print(targlon);
    
      delay(1000);
      slcd.clear();
      slcd.home();
      slcd.print("Your ");
      slcd.setCursor(0, 1);
      slcd.print("coordinates: ");

      delay(500);
      slcd.clear();
      slcd.home();
      slcd.print("lat :");
      slcd.print(yourlat); 
      slcd.setCursor(0, 1);
      slcd.print("lon :");
      slcd.print(yourlon);

    }
  }
  
  // d3
  else if(disp[1]=='3')
  {
    Serial.println("display coordinates");
    slcd.home();
    slcd.print("Friend ");
    slcd.setCursor(0, 1);
    slcd.print("coordinates: ");
    
    delay(500);
    slcd.clear();
    slcd.home();
    slcd.print("lat :");
    slcd.print(targlat); 
    slcd.setCursor(0, 1);
    slcd.print("lon :");
    slcd.print(targlon);
    
    delay(1000);
    slcd.clear();
    slcd.home();
    slcd.print("Your ");
    slcd.setCursor(0, 1);
    slcd.print("coordinates: ");

    delay(500);
    slcd.clear();
    slcd.home();
    slcd.print("lat :");
    slcd.print(yourlat); 
    slcd.setCursor(0, 1);
    slcd.print("lon :");
    slcd.print(yourlon);
  }
  
  feedgps();
}

static void update_LED(char range[])
{
  //int r = atoi(range); 
  int r = 9;
  if(led[1]=='1')
  {
    digitalWrite(LED, HIGH);
  }
  else if(led[1]=='2')
  {
    digitalWrite(LED, HIGH);
    if(r < 100)
    {
      Serial.println("r er mindre enn 100");
      digitalWrite(LED, LOW);
      delay(500);
      digitalWrite(LED, HIGH);
      delay(500);
      
      if(r < 50)
      {
        digitalWrite(LED, LOW);
        delay(200);
        digitalWrite(LED, HIGH);
        delay(200);
        
        if(r < 20)
        {
          digitalWrite(LED, LOW);
          delay(100);
          digitalWrite(LED, HIGH);
          delay(100);
          
          if(r < 10)
          {
            digitalWrite(LED, LOW);
            delay(50);
            digitalWrite(LED, HIGH);
            delay(50);
   
          }
        }
      }
    }
  }
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
  //Serial.println("print_str: const char *str");
  //Serial.println(*str);
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
    
  feedgps();
}

//calls blinking_lights
static void print_str_target(const char *str, int len)
{
  //Serial.println("print_str_target: const char *str");
  //Serial.println(*str);
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
  



