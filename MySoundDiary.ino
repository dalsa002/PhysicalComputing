
// Daniah Alsaleh
// Physical Computing / Saskia
// Dec-2017
// Project Name : My Sound Diary
// The sound diary is about recording some of the sounds I hear every day. It is made of a cube with 5 watercolour  paper interfaces using
// conductive paint and watercolour paint. When you touch a side of the cube, it trigers a sound that I encounter during my day.
// Inspiration for this came from meditation where they encourage you to close your eyes and listen to the various sounds.
// As our contemprary life focuses only on the visual and ignores all other senses. 

// The sketch is from adafruit website, a learn page created by Lady Ada with examples. And from
// project 13 in the Arduino book. Combining the 2 sketches gave me the sound diary project.



//Libraries
#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include <WaveUtil.h>
#include <WaveHC.h>
#include <CapacitiveSensor.h> 

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// threshold for turning the sound on 
int threshold = 400;



// I have 5 input for 5 sound waves, one input for each sound wave file

CapacitiveSensor capSensorA3 = CapacitiveSensor(A2, A3);  // in inout  A2
CapacitiveSensor capSensor6 = CapacitiveSensor(7, 6);     // in inout  7
CapacitiveSensor capSensor8 = CapacitiveSensor(9, 8);     // in  input 9
CapacitiveSensor capSensor12 = CapacitiveSensor(A5, 12);  // in input  A5
CapacitiveSensor capSensorA1 = CapacitiveSensor(A0, A1);  //in in input  A3


//checking if my SD card is working

void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while (1);
}


void setup() {
  
  // set up serial port
  Serial.begin(9600);

  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while (1);                           // then 'halt' - do nothing!
  }


  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part))
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while (1);                           // then 'halt' - do nothing!
  }

  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);    // FAT16 or FAT32?

  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while (1);                            // then 'halt' - do nothing!
  }

  putstring_nl("Ready!");


}


// Main code to run the wave files

void loop() {
  
  long sensorValueA3 = capSensorA3.capacitiveSensor(30);
  long sensorValue6 = capSensor6.capacitiveSensor(30); 
  long sensorValueA1 = capSensorA1.capacitiveSensor(30);
  long sensorValue8 = capSensor8.capacitiveSensor(30);
  long sensorValue12 = capSensor12.capacitiveSensor(30);

  //if my touch thresh hold is above 400 play file, else stop it


  if (sensorValue6 > threshold) {
    playcomplete("wave1.WAV");
  }
  
  //
  if (sensorValueA3 > threshold) {
    playcomplete("wave2.WAV");
  }
  //
  if (sensorValue8 > threshold) {
    playcomplete("wave3.WAV");
  }
  //
  if (sensorValueA1 > threshold) {
    playcomplete("wave4.WAV");
  }
  //
  if (sensorValue12 > threshold) {
    playcomplete("wave5.WAV");
  }



}


void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
    // do nothing while its playing
  }
  // now its done playing
}


void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }

  // ok time to play! start playback
  wave.play();
}
