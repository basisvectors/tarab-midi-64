// based on code and hardware implementation from 
// https://github.com/rheslip/Twisty-MIDI-Controller

#include "MIDIUSB.h"

#define NPOTS 16 // number of pots
#define NPAGES 4 // number of pages
uint16_t pot[NPOTS]; // pot readings
uint16_t lastpots[NPOTS]; // old pot readings
bool potlock[NPOTS]; // when pots are locked it means they must change by MIN_POT_CHANGE to register
uint8_t CC[NPOTS]; // CC values
uint8_t pageCC[NPAGES][NPOTS]; // CC values stored per page - for pot pickup
uint8_t channel=0; // MIDI channel - starts at 0
uint8_t page=0; // CC page 0-3
uint8_t leds=0;
long button2timer;
bool shift=0;  // alt button mode
int mode;      // CC message mode or LFO mode
// button debounce variables
uint8_t button1State = HIGH;
uint8_t button1LastState = HIGH;
unsigned long button1LastDebounceTime = 0;
uint8_t button2State = HIGH;
uint8_t button2LastState = HIGH;
unsigned long button2LastDebounceTime = 0;
#define CCMODE 0

#define RED_PIN 3 // LED output pins
#define ORANGE_PIN 5
#define YELLOW_PIN 9
#define GREEN_PIN 10
#define RED 1   // LED bit masks
#define ORANGE 2
#define YELLOW 8
#define GREEN 4
#define LEDFLASH 100 // flash rate
#define BUTTON1 4  // change bank button
#define BUTTON2 7  // change channel button
#define BUTTON2_TIME 2000 // increment timer
#define DEBOUNCE 10 // button debounce time in ms

#define MUX_IN A7 // port we read values from MUX
#define BASE_CC 16  // lowest CC number to use
#define MIN_POT_CHANGE 25 // pot reading must change by this in order to register


#define POT_AVERAGING 5 //average to use in LFO page 

// midi related stuff
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


// set analog mux address for reading pots
void muxaddr(int addr) {
  digitalWrite(A0, addr & 1);
  digitalWrite(A1, addr & 2);
  digitalWrite(A2, addr & 4);
  digitalWrite(A3, addr & 8);  
}

// flag all pot values as locked ie they have to change more than MIN_POT_CHANGE to register
void lockpots(void) {
  for (int i=0; i<NPOTS;++i) potlock[i]=1;
}

// sample analog pot input and do filtering. 
// if pots are locked, change value only if pot crosses the stored CC value for the current page (pot pickup)
uint16_t sample_pot(uint8_t potnum) {
    int val=0;
    muxaddr(potnum);
    for (int j=0; j<POT_AVERAGING;++j) val+=analogRead(MUX_IN); // read the A/D a few times and average for a more stable value
    val=val/POT_AVERAGING;
    if (potlock[potnum]) {
      // pot pickup: check if current pot value has crossed the stored CC value for this page
      uint8_t currentCC = val / 8; // convert to CC range 0-127
      uint8_t storedCC = pageCC[page][potnum];
      uint8_t lastCC = lastpots[potnum] / 8;
      
      // unlock if pot has crossed the stored value (moved from one side to the other)
      if ((lastCC <= storedCC && currentCC >= storedCC) || (lastCC >= storedCC && currentCC <= storedCC)) {
        potlock[potnum]=0;   // flag pot no longer locked
        pot[potnum]=lastpots[potnum]=val; // save the new reading
      }
      else {
        lastpots[potnum]=val; // update last reading but keep pot locked
        val=pot[potnum]; // return the old locked value
      }
    }
    else {
      if (abs(lastpots[potnum]-val) > 7) lastpots[potnum]=val; // even if pot is unlocked, make sure pot has moved at least 8 counts so CC values don't jump around
      else val=lastpots[potnum];
      pot[potnum]=val; // pot is unlocked so save the reading
    }
    return val;
}

// set LEDs state based on bit mask
void setleds(uint8_t setting) {
  if (setting &1) digitalWrite(RED_PIN, 1);
  else digitalWrite(RED_PIN, 0); 
  if (setting &2) digitalWrite(ORANGE_PIN, 1);
  else digitalWrite(ORANGE_PIN, 0); 
  if (setting &4) digitalWrite(YELLOW_PIN, 1);
  else digitalWrite(YELLOW_PIN, 0); 
  if (setting &8) digitalWrite(GREEN_PIN, 1);
  else digitalWrite(GREEN_PIN, 0); 
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, OUTPUT);    // mux addresses
  pinMode(A1, OUTPUT);  
  pinMode(A2, OUTPUT);  
  pinMode(A3, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);  
  pinMode(BUTTON2, INPUT_PULLUP);    
  pinMode(RED_PIN, OUTPUT);  
  pinMode(GREEN_PIN, OUTPUT);  
  pinMode(ORANGE_PIN, OUTPUT);  
  pinMode(YELLOW_PIN, OUTPUT); 
  setleds(1<<page);  // show CC page number using LED - red=1, orange=2, yellow=3, green=4

  for (int i=0; i<NPOTS;++i) {
    sample_pot(i); // initialize pot settings
    uint8_t initCC = pot[i] / 8; // convert to CC range
    for (int p=0; p<NPAGES; ++p) {
      pageCC[p][i] = initCC; // initialize all pages with current pot positions
    }
  }
  lockpots(); // set high initial movement threshold
  mode=CCMODE;
}
  
void loop() {
  uint8_t reading;
  int i,j;


  // --- New page and channel logic ---
  uint8_t button1Reading = digitalRead(BUTTON1);
  uint8_t button2Reading = digitalRead(BUTTON2);

  // Debounce both buttons
  if (button1Reading != button1LastState) button1LastDebounceTime = millis();
  if (button2Reading != button2LastState) button2LastDebounceTime = millis();

  bool button1Pressed = false;
  bool button2Pressed = false;
  static bool channelMode = false;

  if ((millis() - button1LastDebounceTime) > DEBOUNCE) {
    if (button1Reading != button1State) {
      button1State = button1Reading;
      if (button1State == LOW) button1Pressed = true;
    }
  }
  if ((millis() - button2LastDebounceTime) > DEBOUNCE) {
    if (button2Reading != button2State) {
      button2State = button2Reading;
      if (button2State == LOW) button2Pressed = true;
    }
  }
  button1LastState = button1Reading;
  button2LastState = button2Reading;

  // Both buttons pressed: enter/exit channel mode
  static bool prevBothPressed = false;
  bool bothPressed = (button1Reading == LOW && button2Reading == LOW);
  if (bothPressed && !prevBothPressed) {
    channelMode = !channelMode; // toggle channel mode
    button2timer = millis();
  }
  prevBothPressed = bothPressed;

  if (!channelMode) {
    // Page up/down
    if (button2Pressed && !bothPressed) {
      page++;
      if (page > 3) page = 0;
      lockpots();
      setleds(1<<page);
    }
    if (button1Pressed && !bothPressed) {
      if (page == 0) page = 3; else page--;
      lockpots();
      setleds(1<<page);
    }
  } else {
    // Channel change mode
    // Show channel number while in channel mode
    if ((millis()/LEDFLASH) & 1) setleds(channel+1);
    else setleds(0);
    // Increment channel with button1, decrement with button2
    if (button1Pressed && !bothPressed) {
      channel = (channel + 1) & 0xf;
      setleds(channel+1);
      button2timer = millis();
    }
    if (button2Pressed && !bothPressed) {
      channel = (channel == 0) ? 15 : channel - 1;
      setleds(channel+1);
      button2timer = millis();
    }
    // Exit channel mode if no button pressed for BUTTON2_TIME
    if ((millis() - button2timer) > BUTTON2_TIME && !bothPressed && !button1Pressed && !button2Pressed) {
      channelMode = false;
      setleds(1<<page);
    }
  }
      
  if (mode==CCMODE ) { // send CC messages if pots change
    for (i=0;i<NPOTS;++i) {
      if (CC[i] != (reading=sample_pot(i)/8)) { // convert pot value 0-1023 to CC value 0-127
       CC[i]=reading;
        pageCC[page][i]=reading; // store CC value for this page for pot pickup
        controlChange(channel,BASE_CC+i+page*16, CC[i]);
        MidiUSB.flush();      // forces message to be sent now
      }
    }
  }
  
}
