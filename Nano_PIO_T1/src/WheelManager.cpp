#include "WheelManager.h"
#include <Arduino.h>

void wmTest()
{
  Serial.begin(9600);


  
  WheelManager wm;
  wm.wheelCircumfranceInches = 84.5;

  while(true)
  {
    for (int i = 0; i < 5; i++)
    {
      wm.AddTickRaw();
      // 0.06 seconds -> 60ms -> 15hz -> 60mph ish
      unsigned long msDelay = 60; 
      delay(msDelay);
    }

    float speed = wm.GetSpeed(2);
    float distaince = wm.GetTotalDistance();
    Serial.print("WM Speed: " + String(speed));
    Serial.print("WM distaince: " + String(distaince));
    Serial.println("");



    delay(1000);
  }
}


WheelManager::WheelManager()
{
  wheelCircumfranceInches = 0.0;
  validTickCount = 0;
  MIN_MS_BETWEEN_TICKS = 50;
}

float WheelManager::GetSpeed(uint8_t durationSeconds)
{ 
  // we have a few cases:
  // 1) the last arival was more than duration away
  //    then speed is zero.
  // 2) there were more than one arrivals with in duration
  //    then speed is arrivalCount * timeSpan

  Serial.println("GS: durationSeconds: " + String(durationSeconds));

  uint32_t nowMs = millis();

  Serial.println("GS: nowMs: " + String(nowMs));

  uint16_t numberOfTicksGoneBack = 0;
  uint16_t lastTickIndex = 0; 
  uint32_t lastTickArrivalMs = 0;
  uint32_t msSenseLastTick = 0;

  bool keepGoing = true;
  while (keepGoing)
  {
    lastTickIndex = (validTickCount - 1 - numberOfTicksGoneBack) % MAX_TICK_COUNT;
    lastTickArrivalMs = validTicks[lastTickIndex];
    msSenseLastTick = nowMs - lastTickArrivalMs;

    Serial.println("GS: lastTickIndex " + String(lastTickIndex) 
      + " arrivalMs: " + String(lastTickArrivalMs));

    if(msSenseLastTick >= (durationSeconds * 1000))
    {
      Serial.println("GS: found end duration: " + String(msSenseLastTick));
      break;
      keepGoing = false;
    }

    numberOfTicksGoneBack++;
    if (numberOfTicksGoneBack == MAX_TICK_COUNT)
    {
      // we have gone all the way around the array
      // get out of here
      // knh todo - are we off by one here?
        Serial.println("GS: checked whole array: " + String(numberOfTicksGoneBack));
        keepGoing = false;
        break;
    }

    if(numberOfTicksGoneBack == validTickCount)
    {
      Serial.println("GS: examiled all valid tickes");
      break;
    }
  }  

// knh todo - might want to use the time from the first to last tick instead of first to now
// if there is more than one tick.

  uint16_t msFromNowToFirstTickInSet = nowMs - lastTickArrivalMs;
  float inchesTravled = (float(numberOfTicksGoneBack) * wheelCircumfranceInches);
  Serial.println("inchesTraveled: " + String(inchesTravled));
  Serial.println("msFromNowToFinish:" + String(msFromNowToFirstTickInSet));
  Serial.println("");

  return float(1000 * inchesTravled) / float(msFromNowToFirstTickInSet) / 17.6;
}

void WheelManager::AddTickRaw()
{  
   // AddTickRaw, check if new tick is valid
   // if valid, tick is added to deque, old ticks are bumpped oute.
   // and vlaidTickCount++;
   // A tick is valid if it arrived 50ms after the last tick
   // at 6ft cirmfurance @ 15hz => 68 mph => 0.066 seconds between ticks.

  Serial.begin(9600);

  uint32_t nowMs = millis();
  uint32_t lastArrivalTimeMs = 0;
  uint16_t lastTickIndex = 0;
  // get last tick, if it exists
  if(validTickCount > 0)
  {
    lastTickIndex = (validTickCount - 1) % MAX_TICK_COUNT;
    lastArrivalTimeMs = validTicks[lastTickIndex];
  }

  uint16_t msFromLastTick = nowMs - lastArrivalTimeMs;

  // Serial.println("wm:atr:validTickCount: " + String(validTickCount));
  // Serial.println("wm::AddTick:LastTickI:" + String(lastTickIndex));
  //Serial.println("wm::AddTick:nowMs:" + String(nowMs));
  //Serial.println("wm::AddTick:MIN_MS_BETWEEN_TICKS:" + String(MIN_MS_BETWEEN_TICKS));
  //Serial.println("wm::AddTick:lastArrivalTimeMs:" + String(lastArrivalTimeMs));
  // Serial.println("wm::AddTick:msFromLastTick:" + String(msFromLastTick));
  // Serial.println("");

  if(msFromLastTick >= MIN_MS_BETWEEN_TICKS)
  {
    uint16_t nextTickIndex = validTickCount % MAX_TICK_COUNT;

    // Serial.println("wm::AddTick:ADDING_TICK");
    // Serial.println("wm::AddTick:nextTickIndex:" + String(nextTickIndex));
    // Serial.println("");

    validTicks[nextTickIndex] = nowMs;
    validTickCount++;
  }
}

String uint64ToString(uint64_t input) {
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

float WheelManager::GetTotalDistance()
{
  // 63,360 inches per mile: 100 mi 6,336,000
  uint64_t totalInches = uint64_t(validTickCount * wheelCircumfranceInches); 
  Serial.println("td " + uint64ToString(totalInches));
  return float(totalInches / 63360);
}