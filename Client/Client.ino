#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>

#define SS 18 // SPI SS Pin
#define RST 14
#define DI0 26

#define RH_MESH_MAX_MESSAGE_LEN 50

#define NODE_ADDRESS 1

// Singleton instance of the radio driver
RH_RF95 driver(SS, DI0);
// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(driver, NODE_ADDRESS);

uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

uint8_t server = 255;

void setup()
{
    Serial.begin(115200);
    if (!manager.init())
        Serial.println("init failed");

    driver.setCADTimeout(500);

    while (server == 255)
    {
        server = findServer(manager);
    }
    Serial.printf("Server found: %d\n", server);
    delay(2000);
}

uint8_t data[] = "Holis";

void loop()
{
    Serial.println("Sending data..");
    if (manager.sendtoWait(data, sizeof(data), server) != RH_ROUTER_ERROR_NONE)
    {
        Serial.printf("Error trying to send data to server %d\n", server);
    }

    delay(2000);
}

uint8_t findServer(RHMesh manager)
{
    Serial.println("Finding server");
    uint8_t data[] = "S";

    if (manager.sendtoWait(data, sizeof(data), RH_BROADCAST_ADDRESS) != RH_ROUTER_ERROR_NONE)
    {
        Serial.println("sendtoWait failed. Are the intermediate mesh servers running?");
        return 255;
    }
    // It has been reliably delivered to the next node.
    // Now wait for a reply from the ultimate server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (!manager.recvfromAckTimeout(buf, &len, 3000, &from))
    {
        Serial.println("No reply...");
        return 255;
    }

    Serial.print("got reply from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char *)buf);

    return from;
}
