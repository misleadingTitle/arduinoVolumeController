#include <Evc_pt2257.h>
#include <IRremote.h>
/*
  Sketch example for using PT2257 - Electronic Volume Controller IC.
  Created by Victor NPB, December 9, 2014.
  Connect the IC to the following pins, you can change it in the .c file.
  Pull-up resistors are required on both pins (4K7 or 10K should be fine).
  SCL -> A0
  SDA -> A1
  Volume Attenuation are expressed as unsigned integers
  evc_setVolume(0)   set both channels to 0dB of attenuation (max volume)
  evc_setVolume(79)  set both channels to -79dB of attenuation (min volume)
  evc_setVolumeRight() same as above but set just one channel
  evc_setVolumeLeft()  same as above but set just one channel
  evc_mute(true)     mute both channels to -95dB
  evc_mute(false)    unmute both channels to last set attenuation
  evc_off()          off (-79dB)
*/

const int muteButtonPin = 2;     // the number of the pushbutton pin
const int volUpButtonPin = 3;     // the number of the pushbutton pin
const int volDownButtonPin = 4;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int debounceTimeout = 100;
const int irPin = 5;

IRrecv irrecv(irPin);
decode_results irresults;

long int lastPushTime;
// variables will change:
int muteButtonState = 0;         // variable for reading the pushbutton status
int volUpButtonState = 0;         // variable for reading the pushbutton status
int volDownButtonState = 0;         // variable for reading the pushbutton status

int muteState = 0;
int volume = 0;
void setup() {
  // put your setup code here, to run once:
  evc_init();
  Serial.begin(9600);
  evc_mute(muteState);
  evc_setVolume(volume);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(muteButtonPin, INPUT);
  pinMode(volUpButtonPin, INPUT);
  pinMode(volDownButtonPin, INPUT);
  irrecv.enableIRIn();
}

int mutePrevState = LOW;
int volUpPrevState = LOW;
int volDownPrevState = LOW;
int lastIrCommand = 0;
void pressMute() {
  Serial.print("MUTEPRESS");
  Serial.print("\r\n");
  digitalWrite(ledPin, HIGH);
  mutePrevState = HIGH;
  if (muteState == 0) {
    muteState = 1;
  } else {
    muteState = 0;
  }
  evc_mute(muteState);
}

void pressVolUp() {
  Serial.print("volUpPRESS");
  Serial.print("\r\n");
  digitalWrite(ledPin, HIGH);
  if (volume > 0) {
    evc_setVolume(--volume);
    Serial.print(volume);
    Serial.print("\r\n");
  }
}

void pressVolDown() {
  Serial.print("volDownPRESS");
  Serial.print("\r\n");
  if (volume < 79) {
    evc_setVolume(++volume);
    Serial.print(volume);
    Serial.print("\r\n");
  }
}

void loop() {
  static int i = 0;

  // read the state of the pushbutton value:
  muteButtonState = digitalRead(muteButtonPin);
  volUpButtonState = digitalRead(volUpButtonPin);
  volDownButtonState = digitalRead(volDownButtonPin);
  long int currentTime = millis();
  if (muteButtonState != mutePrevState) {
    if ((currentTime - lastPushTime) > debounceTimeout) {
      lastPushTime = currentTime;
      if (muteButtonState == HIGH) {
        pressMute();
      } else {
        digitalWrite(ledPin, LOW);
        mutePrevState = LOW;
      }
    }
  }


  if (volUpButtonState != volUpPrevState) {
    if ( (currentTime - lastPushTime) > debounceTimeout) {
      lastPushTime = currentTime;
      if (volUpButtonState == HIGH) {
        pressVolUp();
      } else {
        digitalWrite(ledPin, LOW);
        volUpPrevState = LOW;
      }
    }
  }


  if (volDownButtonState != volDownPrevState) {
    if ( (currentTime - lastPushTime) > debounceTimeout) {
      lastPushTime = currentTime;
      if (volDownButtonState == HIGH) {
        pressVolDown();
      } else {
        volDownPrevState = LOW;
      }
    }
  }


  if (irrecv.decode(&irresults))
  {
    Serial.println("remote:");
    Serial.println(irresults.value);
    if (irresults.value == 551502015) {
      Serial.print("IRUP");
      pressVolUp();
      lastIrCommand = 1;
    } else if (irresults.value == 4294967295) {
      Serial.print("IRAGAIN");
      if (lastIrCommand == 1) {
        pressVolUp();
      } else if (lastIrCommand == 2) {
        pressVolDown();
      }
    } else if (irresults.value == 551534655) {
      Serial.print("IRDOWN");
      pressVolDown();
      lastIrCommand = 2;
    } else {
      Serial.print("IRUNUSED");
    }
    Serial.print("\r\n");
    irrecv.resume();

  }


}


