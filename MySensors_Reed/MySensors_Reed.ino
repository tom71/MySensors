// EgSlimReed2
// By m26872, 2015-12-22
// Interrupt driven binary switch for Slim Node with Reed switch and external pull-up (10Mohm)
// Inspired by mysensors example:
// https://github.com/mysensors/Arduino/blob/master/libraries/MySensors/examples/BinarySwitchSleepSensor/BinarySwitchSleepSensor.ino

// Enable and select radio type attached
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#include <Vcc.h>

// #define NODE_ID 5 //12 var senaste "reed-node"-id // 110    // Use static Node_ID  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define MY_NODE_ID 5
#define SKETCH_NAME "EgSlimReed"
#define SKETCH_VERSION "2.0 2015-12-22"
#define SW_CHILD_ID 5
#define SW_PIN 3
#define BATTERY_REPORT_DAY 2   // Desired heartbeat interval when inactive. Maximum heartbeat/report interval is equal to this due to the dayCounter.
#define BATTERY_REPORT_BY_IRT_CYCLE 10  // Adjust this according to usage frequency.
#define ONE_DAY_SLEEP_TIME 86400000
#define VCC_MIN 1.9
#define VCC_MAX 3.3

int dayCounter = 0;
int irtCounter = 0;
uint8_t value;
uint8_t sentValue=2;
bool interruptReturn=false;
 
Vcc vcc;
// MySensor gw;
MyMessage msg(SW_CHILD_ID, V_TRIPPED);

void setup()  
{  
  delay(100); // to settle power for radio  
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, LOW);    // Disable internal pull-ups
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
  present(SW_CHILD_ID, S_DOOR);     
}

void loop() 
{
  if (!interruptReturn) { // Woke up by timer (or first run)
    dayCounter++; 
    if (dayCounter >= BATTERY_REPORT_DAY) {
          dayCounter = 0;
          sendBatteryReport();
    }
  }
  else {    // Woke up by pin change
      irtCounter++;
      sleep(50);       // Short delay to allow switch to properly settle
      value = digitalRead(SW_PIN);
      if (value != sentValue) {
         send(msg.set(value==HIGH ? 1 : 0));
         sentValue = value;
      }
      if (irtCounter>=BATTERY_REPORT_BY_IRT_CYCLE) {
        irtCounter=0;
        sendBatteryReport();
      }
  }

  // Sleep until something happens with the sensor,   or one sleep_time has passed since last awake.
  interruptReturn = sleep(SW_PIN-2, CHANGE, ONE_DAY_SLEEP_TIME);

} 

void sendBatteryReport() {
          float p = vcc.Read_Perc(VCC_MIN, VCC_MAX, true);
          int batteryPcnt = static_cast<int>(p);
          sendBatteryLevel(batteryPcnt);
}
