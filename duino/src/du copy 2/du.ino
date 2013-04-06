
#include <Servo.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>


TinyGPS gps;
SoftwareSerial nss(2, 3);
int distance_to_target = 0;

static void gpsdump(TinyGPS &gps);
static bool feedgps();
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
static void print_str_target(const char *str, int len);

char messageBuffer[23], cmd[3], range[4], pin[4], val[4], aux[4], lat[10], lon[10];
boolean debug = false;
int index = 0;
Servo servo;

void setup() {
  Serial.begin(115200);
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
  
  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(range, messageBuffer + 2, 3);
  range[3] = '\0';
  strncpy(lat, messageBuffer + 5, 9);
  lat[9] = '\0';
  strncpy(lon, messageBuffer + 14, 9);
  lon[9] = '\0';
  
  Serial.println("cmd");
  Serial.println(cmd);
  Serial.println("range");
  Serial.println(range);
  Serial.println("lat");
  Serial.println(lat);
  Serial.println("lon");
  Serial.println(lon);
  
  float l = atof(lat);
  Serial.println(l);
  
  
  if (debug) {
    Serial.println(messageBuffer); }

  int cmdid = atoi(cmd);
  
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
   */
  while(Serial.available() > 0) {
    char x = Serial.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') process(); // end
    else messageBuffer[index++] = x;
  }
  
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
  static const float TARGET_LAT = 59.13131, TARGET_LON = 10.216595;
  
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
  print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0UL : (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT, TARGET_LON) / 1000, 0xFFFFFFFF, 9);
  print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : TinyGPS::course_to(flat, flon,TARGET_LAT, TARGET_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  print_str_target(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, TARGET_LAT, TARGET_LON)), 6);

  print_distance((unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT, TARGET_LON) / 1000, range);
  
  gps.stats(&chars, &sentences, &failed);
  print_int(chars, 0xFFFFFFFF, 6);
  print_int(sentences, 0xFFFFFFFF, 10);
  print_int(failed, 0xFFFFFFFF, 9);
  Serial.println();
  
}

//MARIA - Is distance within range?
static void print_distance(unsigned long val, char range[])
{
  int r = atoi(range);
  
  if(val < r) {
     Serial.println("yaaa maaan");
     Serial.println(range);
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


