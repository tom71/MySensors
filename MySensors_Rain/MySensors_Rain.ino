// EgSlimRain
// By m26872, 2015-12-22
// Connect RainSensor on A0
// Inspired by mysensors example:
// https://github.com/mysensors/Arduino/blob/master/libraries/MySensors/examples/BinarySwitchSleepSensor/BinarySwitchSleepSensor.ino

#include <MySensor.h>
#include <SPI.h>
#include <Vcc.h>

#define NODE_ID 12 //12 var senaste "reed-node"-id // 110    // Use static Node_ID  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define SKETCH_NAME "EgSlimRain"
#define SKETCH_VERSION "2.0 2016-06-19"
#define SW_CHILD_ID 1
#define ANALOG_PIN 0
#define BATTERY_REPORT_DAY 2   // Desired heartbeat interval when inactive. Maximum heartbeat/report interval is equal to this due to the dayCounter.
#define BATTERY_REPORT_BY_IRT_CYCLE 10  // Adjust this according to usage frequency.
#define VCC_MIN 1.9
#define VCC_MAX 3.3

int dayCounter = 0;
int irtCounter = 0;
int lastRange = -1;

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

Vcc vcc;
MySensor gw;
MyMessage msg(SW_CHILD_ID, V_RAINRATE);

void sendBatteryReport();

void setup() {
  delay(100); // to settle power for radio
  gw.begin(NULL, NODE_ID);
  gw.sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
  gw.present(SW_CHILD_ID, S_RAIN);
}

void loop() {
  irtCounter++;

  // read the sensor on analog A0:
  int sensorReading = analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  if (range != lastRange) {
    gw.send(msg.set(sensorReading));
    lastRange = range;
  }

  if (irtCounter >= BATTERY_REPORT_BY_IRT_CYCLE) {
    irtCounter = 0;
    sendBatteryReport();
  }

  gw.sleep(10000);
}

void sendBatteryReport() {
  float p = vcc.Read_Perc(VCC_MIN, VCC_MAX, true);
  int batteryPcnt = static_cast<int>(p);
  gw.sendBatteryLevel(batteryPcnt);
}
