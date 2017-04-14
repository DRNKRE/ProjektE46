#include <iostream>
#include <Windows.h>
#include "Serial.h"
#include "sharedmemory.h"

bool FoundMemory = true;

#define MAP_OBJECT_NAME "$pcars$"

int main(void)
{


    std::cout << "Starting Projekt46..." << std::endl << std::endl;

    Serial port("COM3");

    int game_load_attempt=0;

    // Open the memory-mapped file
    std::cout << "Looking for game..." << std::endl;

    //Attempt to load the game instantly.  Just in case the game is already running.
	HANDLE fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );

    //If game isn't found. Give the user a chance to open the game.
    while (fileHandle == NULL && game_load_attempt < 10) {

        //Wait a few seconds and check again.
        Sleep(5000);
        game_load_attempt+=1;
        fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );

        //Check again
        if (fileHandle == NULL) {
            std::cout << "Game not found: Attempt " << game_load_attempt << " of 10." << std::endl;
        }

    }
    if (fileHandle == NULL) {
            std::cout << "Game failed to load." << std::endl;
            CloseHandle( fileHandle );
            return 1;
    }

	// Get the data structure
	const SharedMemory* sharedData = (SharedMemory*)MapViewOfFile( fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory) );
	if (sharedData == NULL)
	{
		std::cout << "Could not map view of file " << GetLastError() << std::endl;

		CloseHandle( fileHandle );
		return 1;
	}

    float Engine_RPM = 0.0;
    int Transmission_Gear=0;

    const unsigned int SIZE_OF_DATA = 3;
    unsigned char msgA[SIZE_OF_DATA];

    //Init Variables
    msgA[0] = 0xFF;
    msgA[1] = 0;
    msgA[2] = 0;
    while(1){


                if (FoundMemory) {
                    Engine_RPM = sharedData -> mRpm;
                    Transmission_Gear = (sharedData -> mGear) + 1;
                }

                msgA[0]=0xFF;
                msgA[1]=(char) ( Engine_RPM / 39.37 );
                msgA[2]=(char) ( Transmission_Gear );

                bool writeSuccess = port.WriteData(msgA, SIZE_OF_DATA);

                Sleep(100);
    }

    UnmapViewOfFile( sharedData );
	CloseHandle( fileHandle );
    //system("PAUSE");
}
