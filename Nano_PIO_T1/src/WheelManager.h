#include <avr/io.h>
#include <Arduino.h>

#ifndef WHEEL_MANAGER_H_
#define WHEEL_MANAGER_H_

void wmTest();

class WheelManager
{
  public:
    // Default constructor
    WheelManager();

   // wheelCircumfranceInches must be stored in non volital memory
   float wheelCircumfranceInches;

   // tickMaxAgeSeconds defines the max age of a tick before booting it out of the deque.
   // When adding a tick to the deque, any ticks older than this get booted out.
   // This helps facilitate getting the average.   
   //static const uint8_t tickMaxAgeSeconds = 30;
   // dont need if using a circlular array of adiquate size

  //static float MIN_MS_BETWEEN_TICKS = 0.050f;
  uint16_t MIN_MS_BETWEEN_TICKS;

   // valitTickCount, count of all valid ticks.
   // used to calculate total distance.
   uint32_t validTickCount;

    // 6ft cirm at 68 mph => 15 hz
    // 15 hz * 30 seconds = 450
    // knh tood - change after testing
   static const uint16_t MAX_TICK_COUNT = 10;

    // last tick index = validTickCount - 1 % MAX_TICK_COUNT
   uint32_t validTicks[MAX_TICK_COUNT];

   // gets the average speed over the last time period of durationSeconds.
   // durationSeconds must be less or equal to tickMaxAgeSeconds.
   // returns zero by default
    float GetSpeed(uint8_t durationSeconds);

    float GetTotalDistance();

   // AddTickRaw, check if new tick is valid
   // if valid, tick is added to deque, old ticks are bumpped oute.
   // and vlaidTickCount++;
   // A tick is valid if it arrived 50ms after the last tick
   // at 6ft cirmfurance @ 15hz => 68 mph => 0.066 seconds between ticks.
    void AddTickRaw();
   
};
#endif
