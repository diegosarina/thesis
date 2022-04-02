#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>


#define SS 18 // SPI SS Pin
#define RST 14
#define DI0 26

#define RH_MESH_MAX_MESSAGE_LEN 50
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 driver(SS, DI0);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(driver, SERVER_ADDRESS);

uint8_t data[] = "I am the server";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void setup()
{
    Serial.begin(115200);
    if (!manager.init())
        Serial.println("init failed");
    driver.setCADTimeout(500);
}

void loop()
{
    uint8_t len = sizeof(buf);
    uint8_t from;
    
    if (manager.recvfromAckTimeout(buf, &len, 3000, &from))
    {
        Serial.print("got request from : 0x");
        Serial.print(from, HEX);
        Serial.print(": ");
        Serial.println((char *)buf);

        if (buf[0] == 'S')
        {
            Serial.println("Server discovery received");
            // Send a reply back to the originator client
            if (manager.sendtoWait(data, sizeof(data), from) != RH_ROUTER_ERROR_NONE)
            {
                Serial.println("sendtoWait failed");
            }
        }
        else
        {
            Serial.printf("Uploading data: %s ...\n", buf);
        }
    }
    else
    {
        Serial.println("loop");
    }
}
