#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <mcp2515.h>

const String ACTION = "action";
const String RESTART = "restart";
const String GET_INFO = "get-info";
const String SETUP = "setup";

const String EVENT = "event";
const String EVENT_INFO = "info";
const String EVENT_SETUP_COMPLETED = "setup-completed";
const String EVENT_MISSING_SETUP = "missing-setup";

const String VERSION = "version";
const String VERSION_VALUE = "1.0.0";

const unsigned long BAUD_RATE = 115200;

const unsigned long INTERVAL_1HZ = 1000;
const unsigned long INTERVAL_5HZ = 200;

unsigned long lastTime1Hz = 0;
unsigned long lastTime5Hz = 0;
int availableChannels = 0;

struct CanBusChannel {
    int channel;
    int pin;
};

struct CanBusChannel channels[10];

//{"action":"setup","channels":[{"pin":10,"channel":0,"baudrate":500,"clock":8}]}
//{"action":"restart"}

void (*restart)(void) = 0;

void logInfo(String message) {
    Serial.println("[" + String(millis()) + "] INFO - " + message);
}

void logWarn(String message) {
    Serial.println("[" + String(millis()) + "] WARN - " + message);
}

void logError(String message) {
    Serial.println("[" + String(millis()) + "] ERROR - " + message);
}

void sendSerialMessage(JsonDocument json) {
    String jsonString;
    serializeJson(json, jsonString);
    Serial.println(jsonString);
}

void execute1HzTask() {
    if (availableChannels == 0) {
        JsonDocument eventJson;
        eventJson[EVENT] = EVENT_MISSING_SETUP;
        sendSerialMessage(eventJson);
    }
}

void execute5HzTask() {
}

void processSerialInput() {
    String input = Serial.readStringUntil('\n');

    JsonDocument receivedJson;
    deserializeJson(receivedJson, input);
    String action = receivedJson[ACTION];

    if (action == RESTART) {
        restart();

    } else if (action == GET_INFO) {
        JsonDocument event;
        event[EVENT] = EVENT_INFO;
        event[VERSION] = VERSION_VALUE;
        sendSerialMessage(event);

    } else if (action == SETUP) {
        JsonArray channelsJson = receivedJson["channels"];
        availableChannels = 0;

        for (JsonVariant channelJson : channelsJson) {
            int pin = channelJson["pin"];
            int channel = channelJson["channel"];
            long baudrate = channelJson["baudrate"];
            long clock = channelJson["clock"];

            CAN_SPEED canSpeed;
            switch (baudrate) {
                case 5:
                    canSpeed = CAN_5KBPS;
                    break;
                case 10:
                    canSpeed = CAN_10KBPS;
                    break;
                case 20:
                    canSpeed = CAN_20KBPS;
                    break;
                case 33:
                    canSpeed = CAN_33KBPS;
                    break;
                case 50:
                    canSpeed = CAN_50KBPS;
                    break;
                case 100:
                    canSpeed = CAN_100KBPS;
                    break;
                case 125:
                    canSpeed = CAN_125KBPS;
                    break;
                case 200:
                    canSpeed = CAN_200KBPS;
                    break;
                case 250:
                    canSpeed = CAN_250KBPS;
                    break;
                case 1000:
                    canSpeed = CAN_1000KBPS;
                    break;
                case 500:
                default:
                    canSpeed = CAN_500KBPS;
                    break;
            }

            CAN_CLOCK canClock;
            switch (clock) {
                case 20:
                    canClock = MCP_20MHZ;
                    break;
                case 16:
                    canClock = MCP_16MHZ;
                    break;
                case 8:
                default:
                    canClock = MCP_8MHZ;
                    break;
            }

            logInfo("Configuring channel " + String(channel) + " on pin " + String(pin) + " with baudrate " + String(baudrate) + " and clock " + String(clock));

            MCP2515 mcp2515(pin);
            mcp2515.reset();
            mcp2515.setBitrate(canSpeed, canClock);
            mcp2515.setNormalMode();

            CanBusChannel newChannel = {channel, pin};
            channels[availableChannels] = newChannel;

            availableChannels++;
        }

        JsonDocument event;
        event[EVENT] = EVENT_SETUP_COMPLETED;
        sendSerialMessage(event);

    } else {
        logInfo("Received from serial: " + input);
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    logInfo("Starting...");

    SPI.begin();

    logInfo("Started");
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastTime1Hz >= INTERVAL_1HZ) {
        execute1HzTask();
        lastTime1Hz = currentMillis;
    }

    if (currentMillis - lastTime5Hz >= INTERVAL_5HZ) {
        execute5HzTask();
        lastTime5Hz = currentMillis;
    }

    if (Serial.available() > 0) {
        processSerialInput();
    }

    for (int i = 0; i < availableChannels; i++) {
        struct can_frame canFrame;
        struct CanBusChannel channel = channels[i];

        MCP2515 mcp2515(channel.pin);

        if (mcp2515.readMessage(&canFrame) == MCP2515::ERROR_OK) {
            int pid = canFrame.can_id;
            int bits = canFrame.can_dlc;

            if (bits > 0) {
                char pidHex[9];
                sprintf(pidHex, "%X", pid);

                String message = "CH" + String(channel.channel) + " " + String(pidHex) + " ";

                for (int i = 0; i < bits; i++) {
                    int byteValue = canFrame.data[i];

                    if (i > 0) {
                        message = message + ":";
                    }

                    char byteHex[3];
                    sprintf(byteHex, "%02X", byteValue);
                    message = message + byteHex;
                }

                Serial.println(message);
            }
        }
    }
}