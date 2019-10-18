// RANGIANO (range piano)
// Plays notes by changing distance
// Check readme.md for details


#include "melody.h"


// PINS
const int sensor = 12;  // Distance Sensor
const int speaker = 3; // Speaker


// WORKING RANGE of our instrument, where it will play melody
// Set it small to play with hand
// Set it large to play by walking
#define FAR 50 // cm
#define NEAR 5  // cm
const int range = FAR - NEAR;


// MELODY
// melody is set in melody.h

// to set note length in generateSound() method
// and standardize delays
#define NOTE_DURATION 100  // milliseconds

// number of notes
const int notesCount = sizeof(notes)/sizeof(notes[0]);

// length of a subrange for every note
const float noteStep = (float)range / (float)notesCount;

// to separate sequence of identical notes when played by tone()
int currentNoteIndex = 0;
int currentFreq = 0;


// DIRECTION OF PLAYING
// by default melody is played by approaching the sensor
 #define GOAWAY false   // set true to play by moving away from sensor

// METHOD OF PLAYING
// by default uses builtin tome() method: smooth, but with caveats (read method)
 #define GENERATE false  // set true to play by digitalWrite() and delay(): with pause, but more realiable



void setup() {
  // initialize serial communication:
  Serial.begin(9600);

  //  set speaker
  pinMode(speaker,OUTPUT);
  digitalWrite(speaker,LOW);
}

void loop() {
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(sensor, OUTPUT);
  digitalWrite(sensor, LOW);
  delayMicroseconds(2);
  digitalWrite(sensor, HIGH);
  delayMicroseconds(5);
  digitalWrite(sensor, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(sensor, INPUT);
  duration = pulseIn(sensor, HIGH);

  // convert the time into a distance
  // inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  Serial.println(cm);

  if (cm > NEAR && cm < FAR - 1) {
    // to calculate note index in melody array
    // we need to find distance from the "zero" of range

    // find note index depending on direction of playing
    int index = 0;
    if (GOAWAY){
      index = (cm - NEAR) / noteStep;  // play starting from lowest distance
    } else {
      index = (FAR - cm) / noteStep;    // play starting from far end of the range
    }

    // To play choose either of methods (uncomment)
    if (GENERATE) {
      generateSound(index); // generates sound
    } else {
      useTone(index); // uses builtin tone()
    }

  } else {
    // mute sound, when out of playing range
    // if sound produced by tone()
    noTone(speaker);

    // wait before pinging
    delay(NOTE_DURATION);
  }
}

void generateSound(uint8_t noteIndex)
{
    // Generate sound from note's half-period

    // convert note frequency to half of period (in microseconds)
    // period = 1/Hz/2*1000000
    float halfPeriod = 1/notes[noteIndex]/2*1000000;

    // compensate reduction of note duration because of its frequency:
    // noteDuration (in milliseconds) = X * (half of period (in milliseconds))
    int playTime = NOTE_DURATION/(halfPeriod/(float)1000);

    // generate sound
    for(int i=0;i<playTime;i++)
    {
        digitalWrite(speaker,HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(speaker,LOW);
        delayMicroseconds(halfPeriod);
    }
}

void useTone(uint8_t noteIndex)
{
    // Play by tone()
    // eliminates sound pulsation needed for ping,
    // but may interfere the signal of distance sensor

    int freq = (int)notes[noteIndex];

    if (noteIndex != currentNoteIndex && freq == currentFreq) {
      noTone(speaker);
      delay(NOTE_DURATION/2);
    }
    currentFreq = freq;
    currentNoteIndex = noteIndex;

    // play sound
    tone(speaker, freq);

    // let sound play a bit
    delay(NOTE_DURATION);
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
