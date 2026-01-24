  /*
    server strana pre Android aplikáciu, pripojenie cez BT, final code
    meranie prúdu a napatia cez PZEM004 senzor
  */
  
  char junk;
  String inputString = "";
  String buzzer = "OFF";
  
  #include <SoftwareSerial.h>
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include "DHT.h"
  //#include "EmonLib.h"    // CT sensor library
  #include <PZEM004T.h>     // PZEM sensor library
  
  PZEM004T pzem(10,11);  // (RX,TX) connect to TX,RX of PZEM
  IPAddress ip(192,168,1,1);

  // (RX,TX) connect to TX,RX of BT
  #define txPin 8
  #define rxPin 13
  #define myLED LED_BUILTIN
  #define BUFFER_SIZE 64
  #define DHTPIN 3
  #define DHTTYPE DHT22
  
  //pro teploměr
  #define ONE_WIRE_BUS 4
  
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);
  
  DeviceAddress suction_Thermometer = { 0x28, 0xFF, 0x40, 0x37, 0xA8, 0x15, 0x01, 0xF1 };
  DeviceAddress evaporating_Thermometer = { 0x28, 0xFF, 0x3C, 0xE1, 0x63, 0x15, 0x03, 0x9B };
  DeviceAddress pressure_Thermometer = { 0x28, 0xFF, 0xFA, 0x2E, 0x90, 0x15, 0x01, 0x53 };
  DeviceAddress condensing_Thermometer = { 0x28, 0xFF, 0xB7, 0x4F, 0xA8, 0x15, 0x04, 0x8D };
  DeviceAddress PoolWater_1 = { 0x28, 0xFF, 0x5A, 0xA6, 0xA5, 0x16, 0x05, 0x7A };
  DeviceAddress PoolWater_2 = { 0x28, 0xFF, 0x91, 0x82, 0xA6, 0x16, 0x03, 0x6F };
  
  DHT dht(DHTPIN, DHTTYPE);
  float temperature;
  float t;
  float h;
  boolean DebugMode = false;
  String stringContainingCode;
  
  SoftwareSerial BT_Serial(rxPin, txPin); // RX, TX
  char myChar ;
  unsigned char buf[BUFFER_SIZE] = {0};
  unsigned char len = 0;
  long lastReading = 0;
  int interval = 5000;
  
  // Simple Sounds
  int speakerPin = 9;     // set the buzzer control digital IO pin
  
  int numTones = 10;
  int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
  //            mid C  C#   D    D#   E    F    F#   G    G#   A

  //EnergyMonitor emon1;
  //EnergyMonitor emon2;

  void setup()                    // run once, when the sketch starts
  {
  
    Serial.begin(9600);
    pzem.setAddress(ip);
    
    Serial.print("Serial Ready! DebugMode= ");
    Serial.println(DebugMode);
  
    BT_Serial.begin(9600);            // set the baud rate to 9600, same should be of your Serial Monitor
    BT_Serial.println("Bluetooth Ready!");
  
    sensors.begin();
    dht.begin();
    delay(1000);
  
    //#define TEMPERATURE_PRECISION 9 // 9 bits 0.5°C 93.75 ms
    sensors.setResolution(suction_Thermometer, 8);
    sensors.setResolution(evaporating_Thermometer, 8);
    sensors.setResolution(pressure_Thermometer, 8);
    sensors.setResolution(condensing_Thermometer, 8);
    //#define TEMPERATURE_PRECISION 10 // 10 bits 0.25°C 187.5 m
    sensors.setResolution(PoolWater_1, 10);
    sensors.setResolution(PoolWater_2, 10);
  
    pinMode(myLED, OUTPUT);
    digitalWrite(myLED, LOW);
  
    pinMode(speakerPin, OUTPUT);   // set speakerPin as output
    
    /*emon1.current(1, 29);             // Current: input pin, calibration.
    emon2.current(2, 29);             // Current: input pin, calibration.
    */
  }
  
  void loop()
  {
    /*Serial.print("lastReading= ");
      Serial.println(lastReading);
      Serial.print("millis= ");
      Serial.println(millis());
      Serial.print("delta= ");
      Serial.println(millis() - lastReading);
      Serial.println();
    */
    if (millis() - lastReading > interval)
    {
      stringContainingCode = "{ \"temperature\":";
      GetDHT();
      stringContainingCode = stringContainingCode + (t);
      stringContainingCode = stringContainingCode + ", \"humidity\":";
      stringContainingCode = stringContainingCode + (h) + ", \"";
      //stringContainingCode = stringContainingCode + (h);
  
      // Watt measurement
      
      float v = pzem.voltage(ip);
      if (v < 0.0) v = 0.0;
      Serial.print(v);Serial.print("V; ");
    
      float i = pzem.current(ip);
      if(i >= 0.0){ Serial.print(i);Serial.print("A; "); }
      
      float p = pzem.power(ip);
      if(p >= 0.0){ Serial.print(p);Serial.print("W; "); }
      
      /*float e = pzem.energy(ip);
      if(e >= 0.0){ Serial.print(e);Serial.print("Wh; "); }
      */
      Serial.println();
    
      delay(1000);
      
      stringContainingCode = stringContainingCode + "P\":"+ p + ", \"";

      // End of Watt measuremet

  
      GetTemperature(evaporating_Thermometer, "Te1");
      GetTemperature(suction_Thermometer, "Te2");
      GetTemperature(pressure_Thermometer, "Tp");
      GetTemperature(condensing_Thermometer, "Tc");
      GetTemperature(PoolWater_1, "Tv1");
      GetTemperature(PoolWater_2, "Tv2");

      
      stringContainingCode = stringContainingCode.substring(0, stringContainingCode.length() - 3) + " }";   // po načíatní poslednej T treba odseknuť --, "--

      BT_Serial.println(stringContainingCode);
      BT_Serial.flush();
  
  
      lastReading = millis();
      Serial.println("---prave prebehlo meranie");
      Serial.println(stringContainingCode);
  
    }
  
    if ((t < 20.5) && (buzzer == "OFF")) {
      alarm();
    }
  
    if (BT_Serial.available()) {
      while (BT_Serial.available())
      {
        char inChar = (char)BT_Serial.read(); //read the input
        inputString += inChar;        //make a string of the characters coming on serial
      }
      BT_Serial.println(inputString);
      while (BT_Serial.available() > 0)
      {
        junk = BT_Serial.read() ;  // clear the serial buffer
      }
      if (inputString == "a") {       //in case of 'a' turn the LED on
        digitalWrite(myLED, HIGH);
        buzzer = "ON";
      } else if (inputString == "b") { //incase of 'b' turn the LED off
        digitalWrite(myLED, LOW);
        buzzer = "OFF";
      }
      inputString = "";
    }

    
  }
  
  void GetTemperature(DeviceAddress deviceAddress, String Device_No) {
    sensors.requestTemperatures();
    temperature = sensors.getTempC(deviceAddress);
    delay(200);                           // when TEMPERATURE_PRECISION is 10 (10 bits = 0.25°C)  Mode Resol Conversion time is about 187.5 ms
  
    if (DebugMode) {
      /*
        if (deviceAddress == PoolWater_2) {
        Serial.println("korekcia PoolWater_2");
        Serial.print("temperature_old= ");
        Serial.println(temperature);
        temperature = temperature - 0.25;
        Serial.print("temperature_new= ");
        Serial.println(temperature);
        }*/
  
      if (temperature == -127.00) {
        Serial.println("Error getting temperature");
      } else {
        Serial.print("temperature= ");
        Serial.println(temperature);
      }
    }
  
  
    stringContainingCode = stringContainingCode + (Device_No) + "\":";
    if (temperature == -127.00) {
      stringContainingCode = stringContainingCode + "\"n-a\", \"";
    } else {
      // korekcia na rozdiel v meraní senzorov teploty bazénovej vody (ktorá existuje pri vypnutom čerpadle vzhľadom na nepresnosť DS20 pri nastavenej
      // citlivosti merania TEMPERATURE_PRECISION is 10 (10 bits = 0.25°C))
      if (deviceAddress == PoolWater_2) {
        temperature = temperature - 0.25;
      }
      stringContainingCode = stringContainingCode + (temperature) + ", \"";
    }
  }
  
  void GetDHT() {
    h = dht.readHumidity();
    t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      h = 0;
      t = 0;
      //return;
    }
  }
  
  void bufferData(char c) {
    if (len < BUFFER_SIZE) {
      buf[len++] = c;
    } // TODO warn, or send data
  }
  
  
  void alarm() {
    for (int i = 0; i < 80; i++) {  // make a sound
      digitalWrite(speakerPin, HIGH); // send high signal to buzzer
      delay(1); // delay 1ms
      digitalWrite(speakerPin, LOW); // send low signal to buzzer
      delay(1);
    }
    delay(50);
    for (int j = 0; j < 100; j++) { //make another sound
      digitalWrite(speakerPin, HIGH);
      delay(2); // delay 2ms
      digitalWrite(speakerPin, LOW);
      delay(2);
    }
    delay(100);
    return;
  }
