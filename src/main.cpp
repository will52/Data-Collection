/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <LSM9DS1.h>
#include <SD.h>
#include <SPI.h>

File dataFile;

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

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    dataFile = SD.open("test.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (dataFile) {
        Serial.print("Writing to test.txt...");
        dataFile.println("testing 1, 2, 3.");
        // close the file:
        dataFile.close();
        Serial.println("done.");
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }

    // re-open the file for reading:
    dataFile = SD.open("test.txt");
    if (dataFile) {
        Serial.println("test.txt:");

        // read from the file until there's nothing else in it:
        while (dataFile.available()) {
            Serial.write(dataFile.read());
        }
        // close the file:
        dataFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
}

void loop() {
    // nothing happens after setup
}

void saveData() {
	float x, y, z;
	IMU.readAcceleration(x,y,z);
	char written = 0;
	written += dataFile.write((char*) &x,4);
	written += dataFile.write((char*) &y,4);
	written += dataFile.write((char*) &z,4);
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
