#include <DFPlayerMini_Fast.h>
#include "LedControl.h" // Include the LedControl library
#include <Keypad.h>
#include <FastLED.h>
#include <EEPROM.h>

//led strip
#define DATA_PIN    2
#define NUM_LEDS    8
#define BRIGHTNESS  250
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

void(* resetFunc) (void) = 0;
// Define the array of leds
CRGB leds[NUM_LEDS];

//sound
DFPlayerMini_Fast myMP3;

bool isPaused = 0;
bool trackPlayed = false; // Flag to keep track of whether the track has been played

//Keypad
const uint8_t ROW_NUM = 4; //four rows
const uint8_t COLUMN_NUM = 3; //three columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {7, 8, 9}; //connect to the column pinouts of the keypad
const char redareInreg[] PROGMEM = "8";
const char raspunsIar[] PROGMEM = "1";
const char altAp[] PROGMEM = "2";
const char codDemo[] PROGMEM = "321";
const char password_2[] PROGMEM = "100";
const char password_7[] PROGMEM = "130";
const char password_5[] PROGMEM = "330";
const char password_6[] PROGMEM = "310";
bool cod2Used = 0;
bool cod7Used = 0;
bool cod5Used = 0;
bool cod6Used = 0;
bool offLed = false;
String input_password = "";
bool stateCode = 0;
bool keypadEnabledCodApartamente = false;
bool ledOn = false;
unsigned long ledStartTime = 0;
const unsigned long LED_ON_DURATION = 100;
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// Pins for the DIN, CLK, and CS of the LED matrix drivers
const uint8_t DIN_PIN = 51;
const uint8_t CLK_PIN = 52;
const uint8_t CS_PIN = 49;
const uint8_t NUM_MATRICES = 4; // Number of daisy-chained matrices

uint8_t currentMatrix = 3;
uint8_t currentChoice;
unsigned long currentTime;
uint8_t currentLed = 1;
uint8_t currentAp;
uint8_t currentApNr;
uint8_t currentIntreb;
const uint8_t max1 = 0;
const uint8_t max2 = 1;
const uint8_t max3 = 2;
const uint8_t max4 = 3;

//BECURI
const uint8_t LED_PIN1R = 22;
const uint8_t LED_PIN1V = 23;
const uint8_t LED_PIN2R = 24;
const uint8_t LED_PIN2V = 25;
const uint8_t LED_PIN3R = 26;
const uint8_t LED_PIN3V = 27;
const uint8_t LED_PIN4R = 28;
const uint8_t LED_PIN4V = 29;
const uint8_t LED_PIN5R = 30;
const uint8_t LED_PIN5V = 31;
const uint8_t LED_PIN6R = 32;
const uint8_t LED_PIN6V = 33;
const uint8_t LED_PIN7R = 34;
const uint8_t LED_PIN7V = 35;
const uint8_t LED_PIN8R = 36;
const uint8_t LED_PIN8V = 37;
const uint8_t LED_PINI1 = 38;
const uint8_t LED_PINI2 = 39;
const uint8_t LED_PINI3 = 40;
const uint8_t LED_PINI4 = 41;
const uint8_t LED_PINPOL1 = 14;
const uint8_t LED_PINPOL2 = 15;
const uint8_t LED_PINKEY = 17;

// array of LedControl objects for the four drivers
LedControl lcMatrix[4] = {
  LedControl(DIN_PIN, CLK_PIN, CS_PIN, 4), // First driver
  LedControl(DIN_PIN, CLK_PIN, CS_PIN, 4), // Second driver
  LedControl(DIN_PIN, CLK_PIN, CS_PIN, 4), // Third driver
  LedControl(DIN_PIN, CLK_PIN, CS_PIN, 4)  // Fourth driver
};

// Define the pattern
byte human[] = {
  0b00010000,
  0b00001000,
  0b11101010,
  0b00111111,
  0b11101010,
  0b00001000,
  0b00010000,
  0b00000000
};
// images for 8x8 matrix
const uint8_t IMAGES[][8] = {
{
  0b00000000,
  0b01111110,
  0b10000001,
  0b10000001,
  0b10000001,
  0b01111110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001000,
  0b00000100,
  0b00000010,
  0b11111111,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b11000100,
  0b10100010,
  0b10010001,
  0b10001001,
  0b10001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b01000010,
  0b10011001,
  0b10011001,
  0b01101110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b00001111,
  0b00001000,
  0b00001000,
  0b11111111,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b10001111,
  0b10001001,
  0b10001001,
  0b11111001,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b11111111,
  0b10001001,
  0b10001001,
  0b11111001,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b11111111,
  0b00000001,
  0b00000001,
  0b00000001,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b11111111,
  0b10001001,
  0b10001001,
  0b11111111,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000000,
  0b10001111,
  0b10001001,
  0b10001001,
  0b11111111,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00100000,
  0b00010010,
  0b00010000,
  0b00010000,
  0b00010010,
  0b00100000,
  0b00000000
},{
  0b00000000,
  0b00010000,
  0b11101010,
  0b00111111,
  0b11101010,
  0b00010000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00100000,
  0b10010100,
  0b11111100,
  0b10010100,
  0b00100000,
  0b00000000,
  0b00000000
},{
  0b00010000,
  0b00100010,
  0b01000000,
  0b01000000,
  0b01000000,
  0b00100010,
  0b00010000,
  0b00000000
},{
  0b00010000,
  0b00110010,
  0b01010000,
  0b01010000,
  0b01010000,
  0b00110010,
  0b00010000,
  0b00000000
},{
  0b00010000,
  0b00001000,
  0b11101010,
  0b00011111,
  0b00101010,
  0b01001000,
  0b10000100,
  0b00000000
},{
  0b10000100,
  0b01001000,
  0b00101010,
  0b00011111,
  0b11101010,
  0b00001000,
  0b00010000,
  0b00000000
}};
const uint8_t IMAGES_LEN = sizeof(IMAGES)/8;
bool patternsDisplayed = false;
bool patternLaughDisplayed = false;

//scor 
 LedControl lcScor = LedControl(12, 11, 10, 1);

int Digits[10] = {B01111110, B00110000, B01101101, B01111001,
                  B00110011, B01011011, B01011111, B01110000,
                  B01111111, B01111011};

int dig2, dig3;
unsigned long previousMillis = 0;
const unsigned long delaytime = 450;
//vector rasp corect
const uint8_t v[8] = {3,2,1,2,2,1,1,2}; // apartamentele 5 si 8 au rasp automat 2 care iti da 2 puncte
//gameState values
uint8_t gameState = 0;//pana la joc propriuzis
const uint8_t priza =0;
const uint8_t initiala = 2;
const uint8_t instructiuni = 3;
const uint8_t demo = 4;
const uint8_t joc = 5;

//inGameState values
uint8_t inGameState = 0; // in joc propriuzis
const uint8_t inceput = 0;
const uint8_t alegAp = 1;
const uint8_t apartament = 2;
const uint8_t intrebare = 3;
const uint8_t raspuns = 4;
const uint8_t cifru = 5;
const uint8_t intrebFinal = 6;
const uint8_t povesteRaspCorect = 7;
const uint8_t gameOver = 8;
const uint8_t win = 9;

//audio files - stareBoxa values
uint8_t stareBoxa = 1; 
const uint8_t crimeSound = 1;
const uint8_t politie = 2;
const uint8_t initialaSound = 4;
const uint8_t instructiuniSound = 5;
const uint8_t demoAp = 6;
const uint8_t demoIntrebare = 7;
const uint8_t demoRasp1 = 8;
const uint8_t demoRasp2 = 9;
const uint8_t demoRasp3 = 10;
const uint8_t demoCodGresit = 13;
const uint8_t demoCod2 = 14;
const uint8_t demoCodCorect = 15;
const uint8_t demoBravo = 16;
const uint8_t challange = 17;
const uint8_t narator1 = 20;
const uint8_t apartament2 = 21;
const uint8_t intrebare2 = 22;
const uint8_t ap2Aleg2 = 23;
const uint8_t aleg1 = 26;
const uint8_t aleg2 = 27;
const uint8_t aleg3 = 28;
const uint8_t raspunsGresit = 29;
const uint8_t raspunsCorect = 30;
const uint8_t alegereAp = 31;
const uint8_t finalRaspuns = 32;
const uint8_t gameDone = 36;
const uint8_t gameWon = 37;
const uint8_t apartament1 = 38;
const uint8_t intrebare1 = 39;
const uint8_t ap1Pov = 40;
const uint8_t apartament3 = 41;
const uint8_t intrebare3 = 42;
const uint8_t ap3Pov = 43;
const uint8_t apartament4 = 44;
const uint8_t apartament5 = 45;
const uint8_t apartament6 = 46;
const uint8_t apartament7 = 47;
const uint8_t apartament8 = 48;
const uint8_t intrebare4 = 49;
const uint8_t intrebare58 = 50;
const uint8_t intrebare6 = 51;
const uint8_t intrebare7 = 52;
const uint8_t ap4Pov = 53;
const uint8_t ap58Pov = 54;
const uint8_t ap6Pov = 55;
const uint8_t ap7Pov = 56;
const unsigned long crimeSoundDuration = 16050;
const unsigned long politieDuration = 4000;
const unsigned long titluDuration = 3000;
const unsigned long finalAlesDuration = 700;
const unsigned long instructiuniSoundDuration = 220000;
const unsigned long demoApDuration = 10000;
const unsigned long demoRaspDuration = 10000;
const unsigned long demoCodDuration = 4500;
const unsigned long demoCodCorectDur = 5500;
const unsigned long apelDuration = 19000;
const unsigned long naratorDuration =  17000;
unsigned long intrebDuration;
unsigned long apartamentDuration;
unsigned long raspDuration;
const unsigned long alesDuration = 3500;
const unsigned long gameOverDuration = 8000;
const unsigned long gameWonDuration = 17000;
unsigned long startLastTime = 0;
const unsigned long delayBetweenLEDs = 100;
const unsigned long delayBetweenLEDs2 = 110;

unsigned long lastChangeTime = 0;
uint8_t scorDemo = 20;
uint8_t scor = 0;
uint8_t vizAp[8] = {0,0,0,0,0,0,0,0};
const int highScoreAddress=0;// EEPROM address to store high score
const unsigned long displayInterval = 500; // Flashing interval in milliseconds
bool isHighScore = 0;
bool highScoreFlag = false; 
unsigned long highScoreStartTime = 0; 
enum DisplayState {
  DISPLAY_OFF,
  DISPLAY_ON
};

DisplayState currentState = DISPLAY_OFF;
int scoreToDisplay = 0;
bool displayOn = true; // Start with the display on

void setup() {
  //Serial.begin(9600);

  Serial1.begin(9600); // Hardware Serial 1 on pins 18 (RX1) and 19 (TX1)
  myMP3.begin(Serial1, true);
  delay(1000); // Wait for 1 second to allow the DFPlayer Mini to initialize

  //Serial.println("Setting volume to 25");
  myMP3.volume(22);

  for (int driverIndex = 0; driverIndex < 4; driverIndex++) {
    lcMatrix[driverIndex].shutdown(driverIndex, false);
    lcMatrix[driverIndex].setIntensity(driverIndex, 4);
    lcMatrix[driverIndex].clearDisplay(driverIndex);
  }
  pinMode(LED_PIN1R, OUTPUT);
  pinMode(LED_PIN1V, OUTPUT);
  pinMode(LED_PIN2R, OUTPUT);
  pinMode(LED_PIN2V, OUTPUT);
  pinMode(LED_PIN3R, OUTPUT);
  pinMode(LED_PIN3V, OUTPUT);
  pinMode(LED_PIN4R, OUTPUT);
  pinMode(LED_PIN4V, OUTPUT);
  pinMode(LED_PIN5R, OUTPUT);
  pinMode(LED_PIN5V, OUTPUT);
  pinMode(LED_PIN6R, OUTPUT);
  pinMode(LED_PIN6V, OUTPUT);
  pinMode(LED_PIN7R, OUTPUT);
  pinMode(LED_PIN7V, OUTPUT);
  pinMode(LED_PIN8R, OUTPUT);
  pinMode(LED_PIN8V, OUTPUT);
  pinMode(LED_PINI1, OUTPUT);
  pinMode(LED_PINI2, OUTPUT);
  pinMode(LED_PINI3, OUTPUT);
  pinMode(LED_PINI4, OUTPUT);
  pinMode(LED_PINPOL1, OUTPUT);
  pinMode(LED_PINPOL2, OUTPUT);
  pinMode(LED_PINKEY, OUTPUT);

  for (int driverIndex = 0; driverIndex < 4; driverIndex++) {
    lcMatrix[driverIndex].shutdown(driverIndex, false);

    // Set the brightness level here (0 to 15)
    lcMatrix[driverIndex].setIntensity(driverIndex, 5); // Adjust the brightness level as needed

    lcMatrix[driverIndex].clearDisplay(driverIndex);
  }
   lcScor.shutdown(0, false);
   lcScor.setIntensity(0, 5);
   lcScor.clearDisplay(0);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(LED_PINPOL1, OUTPUT);
  pinMode(LED_PINPOL2, OUTPUT);
 input_password.reserve(10); // maximum input characters
 turnOffAllPolice();
 turnOffDisplay();
  keypad.setHoldTime(800);               // Default is 1000mS
  keypad.setDebounceTime(100);           // Default is 50mS
}
void loop() {

  currentTime = millis();
  if (gameState == priza){
    lightShow();
    playSpecificTrackOnce(crimeSound);
     turnOffDisplay();
   if (currentTime - startLastTime >= crimeSoundDuration + titluDuration) {
    trackPlayed = false;   
    gameState = initiala;
    stareBoxa = initialaSound;
    startLastTime = currentTime;
   }
    
  }

  if( gameState == initiala){
     turnOffDisplay();
    turnOffAllLEDs();
    for (int i = 0; i < 4; i++) {
    lcMatrix[i].clearDisplay(i);
    }
     playSpecificTrackOnce(initialaSound);
    proccesKeypadInputStart();
    startLastTime = currentTime;
  }
  if(gameState == instructiuni){
   digitalWrite(LED_PINKEY, LOW);
   playSpecificTrackOnce(instructiuniSound);
    if (currentTime - startLastTime >= instructiuniSoundDuration) {
    trackPlayed = false; 
    gameState = initiala;
    stareBoxa = initialaSound;
    startLastTime = currentTime;
  }
}

 if(gameState == demo){
  processKeypadInputCodApartamente(); 

   if( stareBoxa == demoAp){
   playSpecificTrackOnce(demoAp);
   displayNumber(scorDemo);
 }
   if(stareBoxa == demoIntrebare){  
     playSpecificTrackOnce(demoIntrebare);
 }

   if(stareBoxa == demoRasp1){ 
    playSpecificTrackOnce(demoRasp1);
    
    displayNumber(scorDemo);
    }
   if(stareBoxa == demoRasp2){ 
    playSpecificTrackOnce(demoRasp2);
    displayNumber(scorDemo);
    }
    if(stareBoxa == demoRasp3){ 
    playSpecificTrackOnce(demoRasp3);
    displayNumber(scorDemo);

    }
    if(stareBoxa == demoCodGresit){ 
    playSpecificTrackOnce(demoCodGresit);
    displayNumber(scorDemo);
     if (currentTime - startLastTime >= demoCodDuration) {
    stareBoxa = demoCod2;
    trackPlayed = false; 
    startLastTime = currentTime;
     }}

    if(stareBoxa == demoCod2)
    {
       playSpecificTrackOnce(demoCod2);

    }
    if(stareBoxa == demoCodCorect ){ playSpecificTrackOnce(demoCodCorect);
    displayNumber(scorDemo);
    digitalWrite(LED_PINI1,HIGH);
     if (currentTime - startLastTime >= demoCodCorectDur) {
    stareBoxa = demoBravo;
    trackPlayed = false; 
    startLastTime = currentTime;
     }}
    
     if(stareBoxa == demoBravo){
       turnOffAllLEDs();
       playSpecificTrackOnce(demoBravo);
       keypadEnabledCodApartamente = false;
        if (currentTime - startLastTime >= demoCodCorectDur) {
        stareBoxa = initialaSound;
        gameState = initiala;
        trackPlayed = false; 
        startLastTime = currentTime;
     }
  }

}

if(gameState == joc){
displayNumber(scor);

if(inGameState == inceput){
  if(stareBoxa == challange){
    playSpecificTrackOnce(challange);
    digitalWrite(LED_PIN2V,HIGH);
    for (int i = 0; i < 4; i++) {
    lcMatrix[i].clearDisplay(i);
    }
    
     if (currentTime - startLastTime >= apelDuration) {
    trackPlayed = false; 
    stareBoxa = politie;
    startLastTime = currentTime;
   }
 }


  if(stareBoxa == politie){
    turnOffAllLEDs();
    playSpecificTrackOnce(politie);
    police();
     if (currentTime - startLastTime >= politieDuration) {
    trackPlayed = false; 
    stareBoxa = narator1;
    startLastTime = currentTime;
  }
}

  if(stareBoxa == narator1){
    turnOffAllPolice();
    playSpecificTrackOnce(narator1);
    if (currentTime - startLastTime >= naratorDuration) {
    trackPlayed = false; 
    inGameState = alegAp;
    startLastTime = currentTime;
    enableKeypadCodApartamente();
  }
}
}

  if(inGameState == alegAp){
    turnOffAllLEDs();
     enableKeypadCodApartamente();
    playSpecificTrackOnce(alegereAp);
    processKeypadInputCodApartamente();
  }

 if(inGameState == cifru){ processKeypadInputCodApartamente();}
  if(inGameState == intrebFinal){ processKeypadInputCodApartamente(); if (currentTime - startLastTime >= finalAlesDuration){ enableKeypadCodApartamente();} }

  if(inGameState == apartament){
      digitalWrite(LED_PINI1, LOW);
      digitalWrite(LED_PINI2, LOW);
      digitalWrite(LED_PINI3, LOW);
      digitalWrite(LED_PINI4, LOW);
     processKeypadInputCodApartamente();
     playSpecificTrackOnce(currentAp);
   
  if (currentTime - startLastTime >= apartamentDuration) {
    trackPlayed = false; 
    inGameState = intrebare;
    startLastTime = currentTime;
  }  

  }
  if(inGameState == intrebare){

    if(currentAp == apartament2)
    { stareBoxa = intrebare2; currentAp = apartament2; currentIntreb = intrebare2;  }
    if(currentAp == apartament1)
    { stareBoxa = intrebare1; currentAp = apartament1; currentIntreb = intrebare1;  }
    if(currentAp == apartament3)
    { stareBoxa = intrebare3; currentAp = apartament3; currentIntreb = intrebare3;  }
    if(currentAp == apartament4)
    { stareBoxa = intrebare4; currentAp = apartament4; currentIntreb = intrebare4;  }
    if(currentAp == apartament5)
    { stareBoxa = intrebare58; currentAp = apartament5; currentIntreb = intrebare58;  }
    if(currentAp == apartament6)
    { stareBoxa = intrebare6; currentAp = apartament6; currentIntreb = intrebare6;  }
    if(currentAp == apartament7)
    { stareBoxa = intrebare7; currentAp = apartament7; currentIntreb = intrebare7;  }
    if(currentAp == apartament8)
    { stareBoxa = intrebare58; currentAp = apartament8; currentIntreb = intrebare58; }
      playSpecificTrackOnce(currentIntreb);

    if (currentTime - startLastTime >= intrebDuration) {
    trackPlayed = false; 
    inGameState = raspuns;
    startLastTime = currentTime;
    currentChoice = 0;
  } 

}

  if(inGameState == raspuns){
        processKeypadInputCodApartamente();
        if(currentChoice != 0)
        playSpecificTrackOnce(currentChoice);

  if (currentTime - startLastTime >= alesDuration) {
    trackPlayed = false;
    startLastTime = currentTime;
    if(stareBoxa == raspunsCorect){ inGameState = povesteRaspCorect; stareBoxa = 1; trackPlayed = false; startLastTime = currentTime;} 
    if(stareBoxa == raspunsGresit){inGameState = povesteRaspCorect;stareBoxa = raspunsGresit; trackPlayed = false; startLastTime = currentTime;}
    startLastTime = currentTime;
  } 
    
  }
  if(inGameState == povesteRaspCorect)
  {
    if(currentAp == apartament2 && stareBoxa == 1 ){playSpecificTrackOnce(ap2Aleg2);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
    if(currentAp == apartament1 && stareBoxa == 1 ){playSpecificTrackOnce(ap1Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
    if(currentAp == apartament3 && stareBoxa == 1 ){playSpecificTrackOnce(ap3Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
        if(currentAp == apartament4 && stareBoxa == 1 ){playSpecificTrackOnce(ap4Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
            if(currentAp == apartament5 && stareBoxa == 1 ){playSpecificTrackOnce(ap58Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
            if(currentAp == apartament6 && stareBoxa == 1 ){playSpecificTrackOnce(ap6Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
            if(currentAp == apartament7 && stareBoxa == 1 ){playSpecificTrackOnce(ap7Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
            if(currentAp == apartament8 && stareBoxa == 1 ){playSpecificTrackOnce(ap58Pov);
    if (currentTime - startLastTime >= raspDuration){stareBoxa = raspunsCorect; trackPlayed = false; startLastTime = currentTime;
    }}
      if(stareBoxa == raspunsCorect) {playSpecificTrackOnce(raspunsCorect);
  if (currentTime - startLastTime >= demoCodCorectDur)
  { inGameState= alegAp; trackPlayed = false; startLastTime = currentTime;
  }
  }
     if(stareBoxa == raspunsGresit){playSpecificTrackOnce(raspunsGresit);processKeypadInputCodApartamente();}

}

     if(inGameState == gameOver){ playSpecificTrackOnce(gameDone); lightShow2(); if (currentTime - startLastTime >= gameOverDuration){resetFunc(); } }
     if(inGameState == win){  
    if (!highScoreFlag) {
    highScoreFlag = true; // Set the flag to true to indicate it's been executed
    highScoreStartTime = millis(); // Store the current time
    saveHighScore(scor);

  }
  
  if (highScoreFlag && (millis() - highScoreStartTime >= 3500)) {
    // Turn off the LEDs
    if(offLed == false)
    {turnOffLEDs();}
  }
  displayNumber(scor);if (currentTime - startLastTime >= 2000) {  playSpecificTrackOnce(gameWon);police();}if (currentTime - startLastTime >= 9700) { offLed = true;lightShow3();if (currentTime - startLastTime >= gameWonDuration + titluDuration){resetFunc();}}
}
}
 }



void turnOnLEDs(bool nr) {
  if (1) {
    digitalWrite(LED_PIN1V, HIGH);
    digitalWrite(LED_PIN2V, HIGH);
    digitalWrite(LED_PIN3V, HIGH);
    digitalWrite(LED_PIN4V, HIGH);
    digitalWrite(LED_PIN5V, HIGH);
    digitalWrite(LED_PIN6V, HIGH);
    digitalWrite(LED_PIN7V, HIGH);
    digitalWrite(LED_PIN8V, HIGH);
  } else {
    digitalWrite(LED_PIN1R, HIGH);
    digitalWrite(LED_PIN2R, HIGH);
    digitalWrite(LED_PIN3R, HIGH);
    digitalWrite(LED_PIN4R, HIGH);
    digitalWrite(LED_PIN5R, HIGH);
    digitalWrite(LED_PIN6R, HIGH);
    digitalWrite(LED_PIN7R, HIGH);
    digitalWrite(LED_PIN8R, HIGH);
  }
}

void turnOffLEDs() {
 
    digitalWrite(LED_PIN1V, LOW);
    digitalWrite(LED_PIN2V, LOW);
    digitalWrite(LED_PIN3V, LOW);
    digitalWrite(LED_PIN4V, LOW);
    digitalWrite(LED_PIN5V, LOW);
    digitalWrite(LED_PIN6V, LOW);
    digitalWrite(LED_PIN7V, LOW);
    digitalWrite(LED_PIN8V, LOW);
    digitalWrite(LED_PIN1R, LOW);
    digitalWrite(LED_PIN2R, LOW);
    digitalWrite(LED_PIN3R, LOW);
    digitalWrite(LED_PIN4R, LOW);
    digitalWrite(LED_PIN5R, LOW);
    digitalWrite(LED_PIN6R, LOW);
    digitalWrite(LED_PIN7R, LOW);
    digitalWrite(LED_PIN8R, LOW);

}

//  playSpecificTrackOnce(#);
void playSpecificTrackOnce(uint16_t trackNum)
{
  if (!trackPlayed)
  {
    //Serial.print("Playing track ");
    //Serial.println(trackNum);
    myMP3.play(trackNum);
    trackPlayed = true; // Set the flag to indicate that the track has been played
  }
}

void displayHumanOnMatrix(int index){
    if(inGameState != intrebFinal && inGameState !=gameOver && inGameState!= gameWon) {
  for (int i = 0; i < 4; i++) {
      if (i != index) {
        lcMatrix[i].clearDisplay(i);
      }
    }
  lcMatrix[index].setRow(index, 0, human[0]);
  lcMatrix[index].setRow(index, 1, human[1]);
  lcMatrix[index].setRow(index, 2, human[2]);
  lcMatrix[index].setRow(index, 3, human[3]);
  lcMatrix[index].setRow(index, 4, human[4]);
  lcMatrix[index].setRow(index, 5, human[5]);
  lcMatrix[index].setRow(index, 6, human[6]);
  lcMatrix[index].setRow(index, 7, human[7]);
  }
}
unsigned long lastDisplayTime = 0;
int displayState = 0;
void displayPatternsOnce() {
  unsigned long currentTime = millis();

  if (currentTime - lastDisplayTime >= 2000) { 
    lastDisplayTime = currentTime;

    switch (displayState) {
      case 0:
        // Display the 11th pattern on max1 for 2 seconds
        displayPatternOnMatrix(max1, 10); // Display pattern 10 (0-based index) on max1
        displayState++;
        break;

      case 1:
        // Display the 11th pattern on max2 for 1 second
        displayPatternOnMatrix(max2, 10); // Display pattern 10 on max2
        displayState++;
        break;

      case 2:
        // Display the 11th pattern on max3 for 1 second
        displayPatternOnMatrix(max3, 10); // Display pattern 10 on max3
        displayState++;
        break;

      case 3:
        // Display the 12th pattern on max4 for 2 seconds
        displayPatternOnMatrix(max4, 11); // Display pattern 11 on max4
        displayState++;
        break;

      case 4:
        // Display the 13th pattern on max4 for 2 seconds
        displayPatternOnMatrix(max4, 12); // Display pattern 12 on max4
        displayState++;
        break;

      default:
        // Exit the function when all patterns are displayed
        patternsDisplayed = true;
        break;
    }
  }
}

unsigned long lastDisplayTime2 = 0;
int displayState2 = 0;
void displayPatternLaughOnce() {
  unsigned long currentTime = millis();

  if (currentTime - lastDisplayTime2 >= 300) { 
    lastDisplayTime2 = currentTime;

    switch (displayState2) {
      case 0:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;

      case 1:
   
        displayPatternOnMatrix(max4, 14); 
        displayState2++;
        break;

      case 2:
   
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;

      case 3:
    
        displayPatternOnMatrix(max4, 14);
        displayState2++;
        break;

      case 4:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;
      
      case 5:
     
        displayPatternOnMatrix(max4, 14); 
        displayState2++;
        break;
      
      case 6:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;

      case 7:
     
        displayPatternOnMatrix(max4, 14); 
        displayState2++;
        break;
      
      case 8:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;
      case 9:
     
        displayPatternOnMatrix(max4, 14); 
        displayState2++;
        break;
      
      case 10:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;

      case 11:
     
        displayPatternOnMatrix(max4, 14); 
        displayState2++;
        break;
      
      case 12:
     
        displayPatternOnMatrix(max4, 13); 
        displayState2++;
        break;

      default:
        patternLaughDisplayed = true;
        break;
    }
  }
}
unsigned long lastDisplayTime3 = 0;
int displayState3 = 0;
void displayPatternDance() {
  unsigned long currentTime = millis();

  if (currentTime - lastDisplayTime3 >= 500) { 
    lastDisplayTime3 = currentTime;

    switch (displayState3) {
      case 0:
     
        displayPatternOnMatrix(max1, 15); 
        displayState3++;
        break;

      case 1:
   
        displayPatternOnMatrix(max1, 16); 
        displayState3++;
        break;

      case 2:
   
        displayPatternOnMatrix(max2, 15); 
        displayState3++;
        break;

      case 3:
    
        displayPatternOnMatrix(max2, 16);
        displayState3++;
        break;

      case 4:
     
        displayPatternOnMatrix(max3, 15); 
        displayState3++;
        break;
      
      case 5:
     
        displayPatternOnMatrix(max3, 16); 
        displayState3++;
        break;
      
      case 6:
     
        displayPatternOnMatrix(max4, 15); 
        displayState3++;
        break;

      case 7:
     
        displayPatternOnMatrix(max4, 16); 
        displayState3++;
        break;
      
      default:
        displayState3 = 0;
        break;
    }
  }
}
void displayPatternOnMatrix(int matrixIndex, int patternIndex) {
  if (matrixIndex >= 0 && matrixIndex < NUM_MATRICES) {
    for (int i = 0; i < NUM_MATRICES; i++) {
      if (i != matrixIndex) {
        lcMatrix[i].clearDisplay(i); // Turn off all other matrices
      }
    }
    for (int row = 0; row < 8; row++) {
      lcMatrix[matrixIndex].setRow(matrixIndex, row, IMAGES[patternIndex][row]);
    }
  }
}
void displayNumber(int number) {
  if (number < 0 || number > 99) {
    // Handle out-of-range numbers here, if needed
    return;
  }

  dig2 = number / 10;  // This calculates the tens digit
  dig3 = number % 10;  // This calculates the ones digit

  // Display only on dig2 and dig3
  lcScor.setRow(0, 1, Digits[dig2]);
  lcScor.setRow(0, 2, Digits[dig3]);
}
void turnOffDisplay() {
  for (int digit = 0; digit < 4; digit++) {
    lcScor.setChar(0, digit, ' ', false);  // Clear all segments on each digit
  }
}
void police() {
  static int phase = 0; // 0 for Red, 1 for Blue
  static unsigned long previousPoliceMillis = 0;
  const long policeInterval = 160; // Interval for police light changes in milliseconds

  unsigned long currentMillis = millis();

  if (currentMillis - previousPoliceMillis >= policeInterval) {
    previousPoliceMillis = currentMillis;

    for (int i = 0; i < NUM_LEDS / 2; i++) {
      if (phase == 0) {
        leds[i] = CRGB::Red;
        leds[i + NUM_LEDS / 2] = CRGB::Black;
        digitalWrite(LED_PINPOL1, HIGH);
        digitalWrite(LED_PINPOL2, HIGH);
      } else {
        leds[i] = CRGB::Black;
        leds[i + NUM_LEDS / 2] = CRGB::Blue;
        digitalWrite(LED_PINPOL1, LOW);
        digitalWrite(LED_PINPOL2, LOW);
      }
    }
    // Set the middle two LEDs to black
    leds[NUM_LEDS / 2 - 1] = CRGB::Black;
    leds[NUM_LEDS / 2] = CRGB::Black;

    FastLED.show();

    // Toggle the phase
    phase = 1 - phase;
  }
}

void turnOffAllPolice() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(LED_PINPOL1, LOW);
  digitalWrite(LED_PINPOL2, LOW);
}


void proccesKeypadInputStart()
{
  if (keypadEnabledCodApartamente) 
    return; 
  char key = keypad.getKey();
  unsigned long currentMillisLed = millis();
  if (key){
    digitalWrite(LED_PINKEY, HIGH);
    ledOn = true;
    ledStartTime = currentMillisLed;
    //Serial.println(key);
     if(key == '5') { gameState = initiala; keypadEnabledCodApartamente = false; trackPlayed = false; stareBoxa = initialaSound;  } 
     if(key == '6') { /* gameState = apartament;*/}
     if(key == '*') {gameState = joc; turnOffAllLEDs();
    turnOffDisplay();trackPlayed = false; stareBoxa = challange; keypadEnabledCodApartamente = true; digitalWrite(LED_PINKEY, LOW);} 
     if(key == '0') {gameState = demo; scorDemo = 20; trackPlayed = false; stareBoxa = demoAp; keypadEnabledCodApartamente = true;   digitalWrite(LED_PINKEY, LOW);}
     if(key == '#') {gameState = instructiuni; trackPlayed = false; stareBoxa = instructiuniSound;}
 }
 if (!key && ledOn && currentMillisLed - ledStartTime >= LED_ON_DURATION) {
    digitalWrite(LED_PINKEY, LOW);
    ledOn = false;
  }

}

void processKeypadInputCodApartamente() {
    if (!keypadEnabledCodApartamente) {
    return; // If keypad is disabled, don't process input
  }
  char key = keypad.getKey();
  unsigned long currentMillisLed = millis();
  
  if (key){
    
    digitalWrite(LED_PINKEY, HIGH);
    ledOn = true;
    ledStartTime = currentMillisLed;
    //Serial.println(key);
   if (stateCode == 0)
    {
     if(inGameState == intrebFinal){
        if(key != '4' && key != '0'  && key!= '*' && key != '#' && key != '9')
        { scor = 0; if (currentTime - startLastTime >= alesDuration) { inGameState = gameOver; startLastTime = currentTime;trackPlayed = false;} }
        else if(key == '4') { scor = scor + 3*sumaApNeviz();if (currentTime - startLastTime >= alesDuration) { inGameState = win;startLastTime = currentTime;trackPlayed = false;}}
      } 

     if(key == '8') {   
 
   displayHumanOnMatrix(max1); 
   digitalWrite(LED_PIN8R, HIGH); 

   if(gameState == demo) {

    trackPlayed = false; 
    stareBoxa = demoIntrebare;
    startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
    
  }
  else {}
}
     if(key == '9' && inGameState!=intrebFinal) {
       if(inGameState != alegAp && (gameState == demo || gameState == joc))
        pauseMP3();
        else
        if(gameState == joc && inGameState == alegAp)
        {inGameState = cifru; trackPlayed= false;  digitalWrite(LED_PINI1, LOW);
  digitalWrite(LED_PINI2, LOW);
  digitalWrite(LED_PINI3, LOW);
  digitalWrite(LED_PINI4, LOW); }
      }

      // 
      if(key == '0' && inGameState!=intrebFinal){if(inGameState == alegAp) inGameState = intrebFinal; disableKeypadCodApartamente(); key = 'none'; trackPlayed = false;}
     if(key == '7'){  if(inGameState != alegAp && (gameState == demo || gameState == joc)) {resumeMP3();}
     if(gameState == joc && inGameState == alegAp){
      currentMatrix= max1;
      displayHumanOnMatrix(max1); 
      digitalWrite(LED_PIN7R, HIGH); 

      currentAp = apartament7;
      apartamentDuration =165000;
      intrebDuration =  7000;
      raspDuration = 45000;
      currentApNr = 6;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[6] = 1;
     }}
     if(key == '1') {/*if (inGameState!=alegAp)*/
     if(gameState == demo)
     { 
    trackPlayed = false; 
    stareBoxa = demoRasp1;
    scorDemo=scorDemo-4;
    startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
    turnOffDisplay();
     }
      if(gameState == joc && inGameState == alegAp){
      currentMatrix= max4;
      displayHumanOnMatrix(max4); 
      digitalWrite(LED_PIN1R, HIGH); 
  
      apartamentDuration =103000;
      raspDuration = 18000;
      intrebDuration =  7000;
      currentAp = apartament1;
      currentApNr = 0;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[0] = 1;
     }
     if(inGameState == raspuns) { 
       currentChoice = aleg1;
       if(v[currentApNr] == 1){ scor = scor + 2; stareBoxa = raspunsCorect; trackPlayed = false;} else {if(scor >= 4) scor= scor - 4; else scor = 0; stareBoxa = raspunsGresit;trackPlayed = false;}
     
     }

    }
     if(key == '2') {
     if(gameState == demo)
     { 
    trackPlayed = false; 
    stareBoxa = demoRasp2;
    startLastTime = currentTime;
    scorDemo=scorDemo-4;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
    turnOffDisplay();
     }
     if(gameState == joc && inGameState == alegAp){
      currentMatrix= max4;
      displayHumanOnMatrix(max4); 
      digitalWrite(LED_PIN2R, HIGH); 

      apartamentDuration =155000;
      intrebDuration =  7000;
      raspDuration = 9900;
      currentAp = apartament2;
      currentApNr = 1;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[1] = 1;
     }
     if(inGameState == raspuns) { 
       currentChoice = aleg2;
       if(v[currentApNr] == 2){ scor = scor + 2; stareBoxa = raspunsCorect; trackPlayed = false;} else {if(scor >= 4) scor= scor - 4; else scor = 0; stareBoxa = raspunsGresit;trackPlayed = false;}
     
     }
    }
     if(key == '3') {
     if(gameState == demo)
     { 
    trackPlayed = false; 
    stareBoxa = demoRasp3;
    startLastTime = currentTime;
    scorDemo=scorDemo + 2;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
    turnOffDisplay();
     }
          if(gameState == joc && inGameState == alegAp){
      currentMatrix= max3;
      displayHumanOnMatrix(max3); 
      digitalWrite(LED_PIN3R, HIGH); 
 
      currentAp = apartament3;
      apartamentDuration =54500;
      raspDuration = 17500;
      intrebDuration =  7000;
      currentApNr = 2;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[2] = 1;
     }
      if(inGameState == raspuns) { 
      currentChoice = aleg3;
       if(v[currentApNr] == 3){ scor = scor + 2; stareBoxa = raspunsCorect; trackPlayed = false;} else { if(scor >= 4) scor= scor - 4; else scor = 0; stareBoxa = raspunsGresit;trackPlayed = false;}
     
     }
    }

    if(key == '4'){

      if(gameState == joc && inGameState == alegAp){
      currentMatrix= max3;
      displayHumanOnMatrix(max3); 
      digitalWrite(LED_PIN4R, HIGH); 

      currentAp = apartament4;
      apartamentDuration =32000;
      raspDuration = 55000;
      intrebDuration =  7000;
      currentApNr = 3;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[3] = 1;
     }

    }
        if(key == '5'){

      if(gameState == joc && inGameState == alegAp){
      currentMatrix= max2;
      displayHumanOnMatrix(max2); 

      currentAp = apartament5;
      apartamentDuration =94000;
      raspDuration = 1100;
      intrebDuration =  3500;
      currentApNr = 4;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[4] = 1;
     }

    }
     if(key == '6'){

      if(gameState == joc && inGameState == alegAp){
      currentMatrix= max2;
      displayHumanOnMatrix(max2); 

      currentAp = apartament6;
      currentApNr = 5;
      apartamentDuration =32000;
      raspDuration = 25000;
      intrebDuration =  7000;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[5] = 1;
     }

    }

   if(key == '8'){

      if(gameState == joc && inGameState == alegAp){
      currentMatrix= max1;
      displayHumanOnMatrix(max1); 
      digitalWrite(LED_PIN8R, HIGH); 
      currentAp = apartament8;
      apartamentDuration =3500;
      raspDuration = 1100;
      intrebDuration =  3500;
      currentApNr = 7;
      trackPlayed = false; 
      startLastTime = currentTime;
      if(ledOn) digitalWrite(LED_PINKEY, LOW);
      vizAp[7] = 1;
     }

    }
    if( key!='*' && stateCode == 0 && inGameState == alegAp)
    { inGameState = apartament;}

    }   
    if(key == '*' && inGameState != intrebFinal) {
      stateCode = 1;
      input_password = ""; // Reset the input password

    } else if(key == '#' && inGameState != intrebFinal) {
       displayHumanOnMatrix(currentMatrix);
      if (compareProgmemString(redareInreg, input_password.c_str())){ if(gameState == demo){ stareBoxa = demoAp; trackPlayed = false; startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);}
    if(gameState == joc){ inGameState = apartament; trackPlayed = false; startLastTime = currentTime;currentChoice = 0; }
    
    }
          if (compareProgmemString(raspunsIar, input_password.c_str())){ if(gameState == demo){ stareBoxa = demoIntrebare; trackPlayed = false; startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);}
    else {inGameState = intrebare; trackPlayed = false; startLastTime = currentTime; currentChoice = 0; }
    
    }
        if (compareProgmemString(altAp, input_password.c_str())){
          inGameState = alegAp; trackPlayed = false; startLastTime = currentTime; currentChoice = 0; 

        }
    if(inGameState == cifru)
      checkPassword();

      if(stareBoxa == demoRasp3 || stareBoxa == demoCod2)
       checkPasswordDemo();
       stateCode = 0;
    }
    else {
      if(inGameState == cifru){
      displayPatternOnMatrix(currentMatrix,key-'0');
      }
      input_password += key; // Append new character to input password string
    }
  }
   
  if (!key && ledOn && currentMillisLed - ledStartTime >= LED_ON_DURATION) {
    digitalWrite(LED_PINKEY, LOW);
    ledOn = false;
  }
}


void disableKeypadCodApartamente() {
  keypadEnabledCodApartamente = false;
}

void enableKeypadCodApartamente() {
  keypadEnabledCodApartamente = true;
}

void checkPassword() {
  if(inGameState == cifru){
  if(compareProgmemString(password_2, input_password.c_str())|| compareProgmemString(password_7, input_password.c_str()) ||compareProgmemString(password_5, input_password.c_str()) || compareProgmemString(password_6, input_password.c_str()) ) {
   if(compareProgmemString(password_2, input_password.c_str()) ){
     if(cod2Used == 0){
     scor = scor + 2;
     digitalWrite(LED_PINI1,HIGH);
    
     }
 if (currentTime - startLastTime >= titluDuration) { cod2Used = 1;}
   }

 if(compareProgmemString(password_7, input_password.c_str()) ){
     if(cod7Used == 0){
     scor = scor + 2;
     digitalWrite(LED_PINI2,HIGH);
    
     }
 if (currentTime - startLastTime >= titluDuration) { cod7Used = 1;}
   }
      if(compareProgmemString(password_5, input_password.c_str()) ){
     if(cod5Used == 0){
     scor = scor + 2;
     digitalWrite(LED_PINI3,HIGH);
    
     }
 if (currentTime - startLastTime >= titluDuration) { cod5Used = 1;}
   }
   if(compareProgmemString(password_6, input_password.c_str()) ){
     if(cod6Used == 0){
     scor = scor + 2;
     digitalWrite(LED_PINI4,HIGH);
    
     }
 if (currentTime - startLastTime >= titluDuration) { cod6Used = 1;}
   }
  } else {
    if(scor >= 2)
    scor = scor -2;
  }
  input_password = ""; // Reset the input password
   if (currentTime - startLastTime >= titluDuration) {
  inGameState = alegAp;
  startLastTime = currentTime;
  trackPlayed = false;
   }
  }
}
void checkPasswordDemo() {
  if(compareProgmemString(codDemo, input_password.c_str())) {
    scorDemo = scorDemo + 2;
    stareBoxa= demoCodCorect;
    trackPlayed = false; 
    startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
  } else {
    scorDemo = scorDemo - 2;
    stareBoxa= demoCodGresit;
    trackPlayed = false; 
    startLastTime = currentTime;
    if(ledOn) digitalWrite(LED_PINKEY, LOW);
  }

  input_password = ""; // Reset the input password
}

void lightShow2(){if(patternsDisplayed == false) {displayPatternsOnce();startLastTime = currentTime;} else {lightShow4();if (currentTime - startLastTime >= 5550)if(patternLaughDisplayed == false){displayPatternLaughOnce();}} } 
void lightShow3(){ lightShow();displayPatternDance();}
void lightShow() {
  unsigned long currentTime = millis();

  // Check if it's time to change the LED state
  if (currentTime - lastChangeTime >= delayBetweenLEDs) {
    // Turn off the current LED
    digitalWrite(getGreenPin(currentLed), LOW);
    digitalWrite(getRedPin(currentLed), LOW);

    // Move to the next LED
    currentLed++;
    if (currentLed > 12) {
      currentLed = 1;
    }

    // Turn on the new LED
    digitalWrite(getGreenPin(currentLed), HIGH);
    digitalWrite(getRedPin(currentLed+1), HIGH);

    // Update the last change time
    lastChangeTime = currentTime;
  }
}
void lightShow4() {
  unsigned long currentTime = millis();

  // Check if it's time to change the LED state
  if (currentTime - lastChangeTime >= delayBetweenLEDs2) {
    // Turn off the current LED
    digitalWrite(getRedPin(currentLed), LOW);
    digitalWrite(getRedPin(currentLed+1), LOW);

    // Move to the next LED
    currentLed++;
    if (currentLed > 12) {
      currentLed = 1;
    }

    // Turn on the new LED
    digitalWrite(getRedPin(currentLed), HIGH);
    digitalWrite(getRedPin(currentLed+1), HIGH);

    // Update the last change time
    lastChangeTime = currentTime;
  }
}
int getGreenPin(int ledNumber) {
  // Map LED number to the corresponding green pin
  switch (ledNumber) {
    case 1: return LED_PIN1V;
    case 2: return LED_PIN2V;
    case 3: return LED_PIN3V;
    case 4: return LED_PIN4V;
    case 5: return LED_PIN5V;
    case 6: return LED_PIN6V;
    case 7: return LED_PIN7V;
    case 8: return LED_PIN8V;
    case 9: return LED_PINI1;
    case 10: return LED_PINI2;
    case 11: return LED_PINI3;
    case 12: return LED_PINI4;
    default: return -1; // Return -1 for unknown LED number
  }
}

int getRedPin(int ledNumber) {
  // Map LED number to the corresponding red pin
  switch (ledNumber) {
    case 1: return LED_PIN1R;
    case 2: return LED_PIN2R;
    case 3: return LED_PIN3R;
    case 4: return LED_PIN4R;
    case 5: return LED_PIN5R;
    case 6: return LED_PIN6R;
    case 7: return LED_PIN7R;
    case 8: return LED_PIN8R;
    default: return -1; // Return -1 for unknown LED number
  }
}
void turnOffAllLEDs() {
  digitalWrite(LED_PIN1R, LOW);
  digitalWrite(LED_PIN1V, LOW);
  digitalWrite(LED_PIN2R, LOW);
  digitalWrite(LED_PIN2V, LOW);
  digitalWrite(LED_PIN3R, LOW);
  digitalWrite(LED_PIN3V, LOW);
  digitalWrite(LED_PIN4R, LOW);
  digitalWrite(LED_PIN4V, LOW);
  digitalWrite(LED_PIN5R, LOW);
  digitalWrite(LED_PIN5V, LOW);
  digitalWrite(LED_PIN6R, LOW);
  digitalWrite(LED_PIN6V, LOW);
  digitalWrite(LED_PIN7R, LOW);
  digitalWrite(LED_PIN7V, LOW);
  digitalWrite(LED_PIN8R, LOW);
  digitalWrite(LED_PIN8V, LOW);
  if(inGameState != alegAp){
  digitalWrite(LED_PINI1, LOW);
  digitalWrite(LED_PINI2, LOW);
  digitalWrite(LED_PINI3, LOW);
  digitalWrite(LED_PINI4, LOW);
  }
}
void pauseMP3() {
  if(isPaused == 0){
  myMP3.pause();
  isPaused = 1;
  }
}

// Function to resume the MP3 playback
void resumeMP3() {
  if(isPaused == 1){
  myMP3.resume();
  isPaused = 0;
  }
}

bool compareProgmemString(const char* str1, const char* str2) {
  uint8_t i = 0;
  while (true) {
    char c1 = pgm_read_byte(str1 + i);
    char c2 = str2[i];
    if (c1 == '\0' && c2 == '\0') {
      return true; // Both strings are identical
    }
    if (c1 != c2) {
      return false; // Strings are different
    }
    i++;
  }
}
static int suma;
int sumaApNeviz(){
  for(int i=0;i<8;i++)
if( vizAp[i]==0)
suma++;
return suma;
}

void saveHighScore(int currentScore) {
  uint8_t previousHighScore;
  EEPROM.get(highScoreAddress, previousHighScore); // Read the previous high score from EEPROM
   
  if (currentScore >= previousHighScore) {
    isHighScore =1;
    turnOnLEDs(1);
    EEPROM.put(highScoreAddress, currentScore);
    }
  else turnOnLEDs(0);
    
  }

