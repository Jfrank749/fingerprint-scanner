/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to 
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
#include <LiquidCrystal.h>          //the liquid crystal library contains commands for printing to the display

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   // tell the RedBoard what pins are connected to the display

uint8_t id;

const int SCANNING = 1;
const int REGISTRATION = 2;
                 
int mode=SCANNING;

void setup()  
{
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  lcd.print("Fingerprint Lock");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  //lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  if (finger.verifyPassword()) {
    lcd.print("Found sensor!");
  } else {
    lcd.print("Didn't find ");
    lcd.setCursor(0, 1);
    lcd.print("sensor :(");
    
    delay(1); 
  }
  /*lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("valid finger...");
 delay(5000);*/
}


void loop()                     // run over and over again
{
  if (digitalRead(5) == HIGH) {
    // trying to recognize registered print
    id = getFingerprintIDez();
    lcd.clear();
    lcd.setCursor(0,0);
    if (id == -1) {
      //rejected
      lcd.print("Unrecognized id");
    } else {
      // ID can be printed
      lcd.print("Recognized id #");
      lcd.print(id); 
    }
    delay(1000);            //don't need to run this at full speed.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Waiting to scan");
    delay(1000);
  } else {
    //lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
    lcd.clear();                      //clear the display
    lcd.setCursor(0, 0); // set cursor to top left corner
    lcd.print("Ready to enroll");
    lcd.setCursor(0, 1);
    id = getNextId();
    lcd.print("fingerprint id ");
    lcd.print(id);
    delay(1000);
    //lcd.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    //id = readnumber();
    if (id == 0) {// ID #0 not allowed, try again!
       return;
    }
    //lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
    lcd.clear();                      //clear the display
    lcd.setCursor(0, 0); // set cursor to top left corner
    lcd.print("Enrolling ID #");
    lcd.print(id);
    
    while (!  getFingerprintEnroll() );
  }
}

uint8_t getNextId() {
  // we're going to look through all of the possible
  // memory locations [1..127] for the first one that
  // doesn't have a good fingerprint already stored
  // in it.
  for (uint8_t i = 1; i < 128; i++) {
    // load the ith fingerprint model into memory
    if (finger.loadModel(i) != FINGERPRINT_BADLOCATION) {
      // check if we can convert that model into an image
      if (finger.image2Tz() != FINGERPRINT_OK) {
        // if we can't, then that's the one to replace!
        return i;
      }
    }
  }
}
 
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
      lcd.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.println("Could not find fingerprint features");
      return p;
    default:
      lcd.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    lcd.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.println("Did not find a match");
    return p;
  } else {
    lcd.println("Unknown error");
    return p;
  }   
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  // found a match!
  lcd.print("Found Finger #"); lcd.print(finger.fingerID); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
    //lcd.clear();
  //lcd.setCursor(0,0);
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  //lcd.print("Found ID #"); lcd.print(finger.fingerID); 
  //delay (10000);
  return (int) finger.fingerID;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("Waiting for"); 
  lcd.setCursor(0, 1); 
  lcd.print ("finger");
  delay (1000);
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print ("to enroll");
  lcd.setCursor(0, 1); // set cursor to top left corner
  lcd.print ("as #"); lcd.print(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.print("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.print("Imaging error");
      break;
    default:
      Serial.print("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("ID "); lcd.print(id);
  p = -1;
  lcd.clear();
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("Place same");
  lcd.setCursor(0,1);
  lcd.print("finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
      lcd.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
  lcd.print("Creating model for #");  lcd.print(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
  lcd.setCursor(0, 0); // set cursor to top left corner
    lcd.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
