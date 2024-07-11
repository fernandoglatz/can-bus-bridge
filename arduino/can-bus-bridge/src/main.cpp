#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <mcp2515.h>

const char ACTION[] = "action";
const char RESTART[] = "restart";
const char GET_INFO[] = "get-info";
const char GET_CHANNELS[] = "get-channels";
const char SETUP[] = "setup";

const char EVENT[] = "event";
const char EVENT_CHANNELS[] = "channels";
const char EVENT_INFO[] = "info";
const char EVENT_SETUP_COMPLETED[] = "setup-completed";
const char EVENT_MISSING_SETUP[] = "missing-setup";

const char MILLIS[] = "millis";
const char VERSION[] = "version";
const char VERSION_VALUE[] = "1.0.0";

const unsigned long BAUD_RATE PROGMEM = 115200;

const unsigned int INTERVAL_1HZ PROGMEM = 1000;
const unsigned char INTERVAL_5HZ PROGMEM = 200;

unsigned long lastTime1Hz = 0;
unsigned long lastTime5Hz = 0;

unsigned char availableChannels = 0;

struct CanBusChannel {
    unsigned char pin;
    unsigned char channel;
    int baudrate;
    unsigned char clock;
    bool readable;
};

struct CanBusChannel channels[5];

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

void sendSerialMessage(DynamicJsonDocument json) {
    String jsonString;
    serializeJson(json, Serial);
    Serial.println("");
}

void execute1HzTask() {
    if (availableChannels == 0) {
        DynamicJsonDocument eventJson(8);
        eventJson[EVENT] = EVENT_MISSING_SETUP;
        sendSerialMessage(eventJson);
    }
}

void execute5HzTask() {
}

void processCanMessageSend(String input) {
    struct can_frame canFrame;

    int inputLength = input.length();
    int spaceIndex = input.indexOf(' ');
    int channelInt = input.substring(2, spaceIndex).toInt();

    int secondSpaceIndex = input.indexOf(' ', spaceIndex + 1);
    String idHex = input.substring(spaceIndex + 1, secondSpaceIndex);
    canFrame.can_id = strtol(idHex.c_str(), NULL, 16);

    String data = input.substring(secondSpaceIndex + 1);

    int currentColonIndex = secondSpaceIndex;
    int bytes = 0;
    for (int i = 0; i < 8; i++) {
        int colonIndex = input.indexOf(':', currentColonIndex + 1);
        if (colonIndex == -1 && inputLength > currentColonIndex + 1) {
            colonIndex = inputLength;
        }

        if (colonIndex > -1) {
            String datahex = input.substring(currentColonIndex + 1, colonIndex);
            canFrame.data[i] = strtol(datahex.c_str(), NULL, 16);

            currentColonIndex = colonIndex;
            bytes++;
        } else {
            break;
        }
    }

    canFrame.can_dlc = bytes;

    if (bytes == 0) {
        logError("Received message with no data: " + input);
    } else {
        struct CanBusChannel* channel = NULL;
        for (int i = 0; i < availableChannels; i++) {
            if (channels[i].channel == channelInt) {
                channel = &channels[i];
                break;
            }
        }

        if (channel == NULL) {
            logError("Received message for unknown channel: " + input);

        } else {
            MCP2515 mcp2515(channel->pin);
            mcp2515.sendMessage(&canFrame);
        }
    }
}

void processAction(DynamicJsonDocument receivedJson) {
    String action = receivedJson[ACTION];

    if (action == RESTART) {
        logInfo("Restarting...");
        delay(1000);
        restart();

    } else if (action == GET_INFO) {
        DynamicJsonDocument infoJson(32);
        infoJson[EVENT] = EVENT_INFO;
        infoJson[VERSION] = VERSION_VALUE;
        infoJson[MILLIS] = millis();
        sendSerialMessage(infoJson);

    } else if (action == GET_CHANNELS) {
        DynamicJsonDocument channelsJson(128);
        channelsJson[EVENT] = EVENT_CHANNELS;
        JsonArray channelsArray = channelsJson.createNestedArray(EVENT_CHANNELS);

        for (int i = 0; i < availableChannels; i++) {
            JsonObject channelJson = channelsArray.createNestedObject();
            channelJson["pin"] = channels[i].pin;
            channelJson["channel"] = channels[i].channel;
            channelJson["baudrate"] = channels[i].baudrate;
            channelJson["clock"] = channels[i].clock;
            channelJson["readable"] = channels[i].readable;
        }

        sendSerialMessage(channelsJson);

    } else if (action == SETUP) {
        JsonObject channelJson = receivedJson["channel"];

        unsigned char pin = channelJson["pin"];
        unsigned char channel = channelJson["channel"];
        int baudrate = channelJson["baudrate"];
        unsigned char clock = channelJson["clock"];
        bool readable = channelJson["readable"];

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

        logInfo("Configuring channel " + String(channel) + " on pin " + String(pin) + " with baudrate " + String(baudrate) + " and clock " + String(clock) + " and readable " + String(readable));

        MCP2515 mcp2515(pin);
        mcp2515.reset();
        mcp2515.setBitrate(canSpeed, canClock);
        mcp2515.setNormalMode();

        CanBusChannel newChannel = {pin, channel, baudrate, clock, readable};
        channels[availableChannels] = newChannel;

        availableChannels++;

        DynamicJsonDocument setupJson(8);
        setupJson[EVENT] = EVENT_SETUP_COMPLETED;
        sendSerialMessage(setupJson);

    } else {
        logError("Unknown action: " + action);
    }
}

void processSerialInput() {
    String input = Serial.readStringUntil('\n');

    if (input.startsWith("CH")) {
        processCanMessageSend(input);

    } else if (input.length() == 1) {
        Serial.println("");

    } else {
        DynamicJsonDocument receivedJson(256);
        deserializeJson(receivedJson, input);
        String action = receivedJson[ACTION];

        if (action != "") {
            processAction(receivedJson);

        } else {
            logInfo("Received from serial: " + input);
        }
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
        struct CanBusChannel channel = channels[i];
        struct can_frame canFrame;

        MCP2515 mcp2515(channel.pin);

        if (mcp2515.readMessage(&canFrame) == MCP2515::ERROR_OK) {
            int pid = canFrame.can_id;
            int bits = canFrame.can_dlc;

            if (bits > 0 && channel.readable) {
                char pidHex[9];
                sprintf(pidHex, "%X", pid);

                String message = "CH" + String(channel.channel) + " " + millis() + " " + String(pidHex) + " ";

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