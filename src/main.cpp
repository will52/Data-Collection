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
int catchup = 0;

void setup() {
    // Open serial communications:
    Serial.begin(9600);

    //initialise accelerometer
    if(!IMU.begin()){
		Serial.print("IMU init failed");
		while(1)
			;
	}

    //initialise SD card module
    pinMode(10,OUTPUT);
    if (!SD.begin(10)) {
        Serial.println("SD init failed");
        while (1)
            ;
    }
    Serial.println("initialization done.");

    pinMode(2,INPUT_PULLUP);
}

void loop() {
    //check start time
    int start = micros();

    //check for button press (falling edge)
    button = digitalRead(2);
    bool pressed = false;
    if(button == LOW && oldButton == HIGH){
        Serial.println("pressed");
        pressed = true;
    }
    oldButton = button;

    //file handling
    if(record){
        saveData();
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

    //timing handling for 104Hz
    int timeTaken = micros() - start;
    int delay = 9600 - timeTaken - catchup;
    if(!record) delay = 9600;
    if(delay<0){
        catchup = delay*-1;
    }else{
        catchup = 0;
        delayMicroseconds(delay);
    }
}

void saveData() {
    //read xyz from accelerometer
    float data[3];
    IMU.readAcceleration(data[0], data[1], data[2]);
    //write data to file
    size_t written = dataFile.write((char*) &data, 12);
	if(written != 12){
		Serial.print("Error writting data to file");
		while(1)
			;
	}
}

void createFile(){
    //creat numbered file for data storage
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
    //close file to save data
	dataFile.close();
}