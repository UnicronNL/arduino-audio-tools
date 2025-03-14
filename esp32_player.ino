/**
 * Make sure that the pins are set to on, on, on, on, on
 * Make sure that PSRAM is enabled
 */

#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioLibs/AudioSourceSDMMC.h"
#include "AudioTools/AudioLibs/AudioSourceSDFAT.h"

const char *startFilePath="/";
const char* ext="*";
AudioSourceSDMMC source(startFilePath, ext);
AudioBoardStream kit(AudioKitEs8388V1);
AudioPlayer player(source, kit);

const int sdCardPin = 34;

void printMetaData(MetaDataType type, const char* str, int len){
  Serial.print("==> ");
  Serial.print(toStr(type));
  Serial.print(": ");
  Serial.println(str);
}

void next(bool, int, void*) {
   player.next();
}

void previous(bool, int, void*) {
   player.previous();
}

void startStop(bool, int, void*) {
   player.setActive(!player.isActive());
}

bool isSDCardInserted()
{
  return digitalRead(sdCardPin) == LOW;
}

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

  // SD card detection
  pinMode(sdCardPin, INPUT_PULLUP);

  if (!isSDCardInserted())
  {
    Serial.println("Please insert an SD card.");
    while (!isSDCardInserted())
    {
      delay(1000);
    }
  }

  Serial.println("SD card detected.");

  // setup output
  auto cfg = kit.defaultConfig(TX_MODE);
  kit.begin(cfg);

 // setup additional buttons 
  kit.addDefaultActions();
  kit.addAction(kit.getKey(1), startStop);
  kit.addAction(kit.getKey(4), next);
  kit.addAction(kit.getKey(3), previous);

  // setup player
  player.setBufferSize(512 * 1024);
  player.setVolume(0.5);
  player.begin();
}

void loop() {
  if (!isSDCardInserted())
  {
    Serial.println("SD card removed. Stopping playback.");
    player.setActive(false);
    while (!isSDCardInserted())
    {
      delay(1000);
    }
    Serial.println("SD card reinserted. Resuming playback.");
    player.setActive(true);
  }
  player.copy();
  kit.processActions();
}
