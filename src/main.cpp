#include <Arduino.h>
#include <Nano33BLE_System.h>
#include <LSM9DS1.h>
#include <SD.h>
#include <SPI.h>

File dataFile;
PinStatus button;
PinStatus oldButton = LOW;
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

    pinMode(3,OUTPUT);
    digitalWrite(3, HIGH);
    pinMode(4, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    button = digitalRead(4);
    char pressed = false;
    if(button>oldButton){
        pressed = true;
    }
    oldButton = button;
    if(record){
        saveData();
        ledCount++;
        if(ledCount>20){
            led = !led;
            ledCount = 0;
            digitalWrite(LED_BUILTIN, led);
        }
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
    char written = dataFile.write((char*) &data, 12);
	if(written != 12){
		Serial.print("Error writting data to file");
		while(1)
			;
	}
}

void createFile(){
	int fileNum = 1;
	String filename = String(fileNum+".dat");
	while(SD.exists(filename)){
		fileNum++;
		filename = String(fileNum+".dat");
	}
	dataFile = SD.open(filename, FILE_WRITE);
}

void closeFile(){
	dataFile.close();
}
