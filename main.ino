/////////////////////
// PhD project 1.0v//
// MajedRayan-PRCX //
// +201000737708   //
/////////////////////
// SD datalogger
#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;
//----------------->> <<.--------------//
//  temperature & Humidity
#include "DHT.h"
#define DHTPIN0 2  // what digital pin we're connected to
#define DHTPIN1 3
#define DHTTYPE0 DHT22  // DHT 22
#define DHTTYPE1 DHT22  // DHT 22
DHT dht0(DHTPIN0, DHTTYPE0);
DHT dht1(DHTPIN1, DHTTYPE1);
//----------------->> <<.--------------//
//Solar Radiation
#define ANALOG_PIN A0    // Analog pin
#define RESISTANCE 10    // Resistance in thousands of ohms
#define PANEL_LENGTH 60  // Length of solar cell in mm
#define PANEL_WIDTH 20   // Width of solar cell in mm
volatile float Area;
volatile float Power;
volatile float Radiation;
//----------------->> <<.--------------//
//Watchdog Timer
#include "Timers.h"
Timers guard_timer;
bool data_received = false;
//----------------->> <<.--------------//
const int Mq9 = 8;  // MQ-9 sensor
//----------------->> <<.--------------//
boolean debug = true;  // governer
//----------------->> <<.--------------//


void setup() {
  guard_timer.start(5000);  //time in ms for whatchdog

  if (debug) Serial.begin(9600);  // Searial
  if (debug) Serial.println("DHTxx test!");
  dht0.begin();         // DHT 0
  dht1.begin();         // DHT 1
  pinMode(Mq9, INPUT);  // MQ-9 sensor pinMode
                        // SD report
                        /* if (!SD.begin(chipSelect)) {
    if (debug) Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  if (debug) Serial.println("card initialized."); */
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h0 = dht0.readHumidity();
  float h1 = dht1.readHumidity();
  float t0 = dht0.readTemperature();
  float t1 = dht1.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h0) || isnan(t0)) {
    if (debug) Serial.println("Failed to read from DHT0 sensor!");
    return;
  }

  if (isnan(h1) || isnan(t1)) {
    if (debug) Serial.println("Failed to read from DHT1 sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic0 = dht0.computeHeatIndex(t0, h0, false);
  float hic1 = dht1.computeHeatIndex(t1, h1, false);
  //////////////////////////////////////////////////////////////////////////
  if (debug) Serial.print("Humidity0: ");
  if (debug) Serial.print(h0);
  if (debug) Serial.print(" %\t");
  if (debug) Serial.print("Temperature0: ");
  if (debug) Serial.print(t0);
  if (debug) Serial.print(" *C ");
  if (debug) Serial.print("Heat index0: ");
  if (debug) Serial.print(hic0);
  if (debug) Serial.println(" *C ");
  delay(2000);
  Serial.println("NEXT");
  if (debug) Serial.print("Humidity1: ");
  if (debug) Serial.print(h1);
  if (debug) Serial.print(" %\t");
  if (debug) Serial.print("Temperature1: ");
  if (debug) Serial.print(t1);
  if (debug) Serial.print(" *C ");
  if (debug) Serial.print("Heat index1: ");
  if (debug) Serial.print(hic1);
  if (debug) Serial.println(" *C ");
  delay(2000);
  Serial.println("NEXT");
  //////////////////////////////////////////////////////////////////////////
  // Solar
  Area = PANEL_LENGTH * PANEL_WIDTH / (100 * 100);      // we are dividing by 10000 get the area in square meters
  Power = pow(analogRead(ANALOG_PIN), 2) / RESISTANCE;  // Calculating power
  Radiation = Power / Area;
  char *msg;
  sprintf(msg, " The Solar Radiation is %f W/M2 ", Radiation);  // Generating message to be printed
  if (debug) Serial.println(msg);
  delay(1000);
  //////////////////////////////////////////////////////////////////

  // MQ-9 Sensor
  int alarm = 0;
  float sensor_volt;
  float RS_gas;
  float ratio;
  //-Replace the name "R0" with the value of R0 in the demo of First Test -/
  float R0 = 0.91;
  int MQ9Value = analogRead(A0);
  sensor_volt = ((float)MQ9Value / 1024) * 5.0;
  RS_gas = (5.0 - sensor_volt) / sensor_volt;  // Depend on RL on yor module
  ratio = RS_gas / R0;                         // ratio = RS/R0
                                               //------------------------------------------------------------/
  if (debug) Serial.print("sensor_volt = ");
  if (debug) Serial.println(sensor_volt);
  if (debug) Serial.print("RS_ratio = ");
  if (debug) Serial.println(RS_gas);
  if (debug) Serial.print("Rs/R0 = ");
  if (debug) Serial.println(ratio);
  if (debug) Serial.print("\n\n");
  alarm = digitalRead(Mq9);
  delay(1000);
  // logging data
  String dataString = "";
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }
  /* String datalog = String(t0) + "," + String(t1) + "," + String(h0) + "," + String(h1) + "," + String(hic0) + "," + String(hic1) + "," + String(sensor_volt) + "," + String(RS_gas) + "," + String(ratio) + "," + String(Mq9) + "," + String(Radiation) ;
  //datalog += ",";

  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(datalog);
    dataFile.close();
    // print to the serial port too:
    if (debug) Serial.println(datalog);
  }
  // if the file isn't open, pop up an error:
  else {
    if (debug) Serial.println("error opening datalog.csv");
  }*/
  //----------------->> <<.--------------//

  //----------------->> <<.--------------//
  // Watchdog
  if (data_received) {
    guard_timer.restart();
  }
  if (guard_timer.available()) {
    guard_timer.stop();
    //Send alert that timer elapsed
    guard_timer.start(5000);
  }
}
