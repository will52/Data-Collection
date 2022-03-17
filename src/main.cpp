#include <Arduino.h>
#include <Nano33BLE_System.h>
#include <LSM9DS1.h>
#include <SD.h>
#include <SPI.h>
#include "main.h"

File dataFile;
PinStatus button = HIGH;
PinStatus oldButton = HIGH;
char record = false;
char ledCount = 0;
bool led = false;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }

    if(!IMU.begin()){
		Serial.print("IMU init failed");
		while(1)
			;
	}

    if (!SD.begin(10)) {
        Serial.println("SD init failed");
        while (1)
            ;
    }
    Serial.println("initialization done.");

    pinMode(2,INPUT_PULLUP);
}

void loop() {
    button = digitalRead(2);
    bool pressed = false;
    if(button == LOW && oldButton == HIGH){
        Serial.println("pressed");
        pressed = true;
    }
    oldButton = button;
    if(record){
        saveData();
        // ledCount++;
        // if(ledCount>20){
        //     led = !led;
        //     ledCount = 0;
        //     digitalWrite(LED_BUILTIN, led);
        // }
        if(pressed){
            closeFile();
            record = false;
        }
    }else{
        if(pressed){
            createFile();
            record = true;
        }
    }
    delayMicroseconds(9615); //delay needed to run above code at 104Hz, ignoring the time to run the code
}

void saveData() {
    float data[3];
    IMU.readAcceleration(data[0], data[1], data[2]);
    size_t written = dataFile.write((char*) &data, 12);
	if(written != 12){
		Serial.print("Error writting data to file");
		while(1)
			;
	}
}

void createFile(){
	int fileNum = 1;
	String filename = String(fileNum) + ".dat";
	while(SD.exists(filename)){
		fileNum++;
		filename = String(fileNum) + ".dat";
	}
    Serial.println(filename);
	dataFile = SD.open(filename, FILE_WRITE);
    if(!dataFile){
        Serial.println("Error opening file");
        while(1)
            ;
    }
}

void closeFile(){
	dataFile.close();
}