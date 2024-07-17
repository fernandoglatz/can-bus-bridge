#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <mcp2515.h>

const uint32_t BAUD_RATE PROGMEM = 115200;

const uint8_t BAUD_RATE_5KBPS PROGMEM = 5;
const uint8_t BAUD_RATE_10KBPS PROGMEM = 10;
const uint8_t BAUD_RATE_20KBPS PROGMEM = 20;
const uint8_t BAUD_RATE_33KBPS PROGMEM = 33;
const uint8_t BAUD_RATE_50KBPS PROGMEM = 50;
const uint8_t BAUD_RATE_100KBPS PROGMEM = 100;
const uint8_t BAUD_RATE_125KBPS PROGMEM = 125;
const uint8_t BAUD_RATE_200KBPS PROGMEM = 200;
const uint8_t BAUD_RATE_250KBPS PROGMEM = 250;
const uint16_t BAUD_RATE_500KBPS PROGMEM = 500;
const uint16_t BAUD_RATE_1000KBPS PROGMEM = 1000;

const uint8_t CLOCK_20MHZ PROGMEM = 20;
const uint8_t CLOCK_16MHZ PROGMEM = 16;
const uint8_t CLOCK_8MHZ PROGMEM = 8;

const uint16_t INTERVAL_1HZ = 1000;
const uint8_t INTERVAL_5HZ = 200;

unsigned long lastTime1Hz = 0;
unsigned long lastTime5Hz = 0;

uint8_t availableChannels = 0;
uint8_t availableBridges = 0;

struct CanBusChannel {
    uint8_t pin;
    uint8_t channel;
    bool readable;
};

struct CanBusBridge {
    uint8_t channelA;
    uint8_t channelB;
    bool bidirectional;
};

struct CanBusChannel channels[5];
struct CanBusBridge bridges[5];

void (*restart)(void) = 0;

void logInfo(const String& message, const String& extra) {
    Serial.print(F("["));
    Serial.print(millis());
    Serial.print(F("] INFO - "));
    Serial.print(message);
    Serial.println(extra);
}

void logWarn(const String& message, const String& extra) {
    Serial.print(F("["));
    Serial.print(millis());
    Serial.print(F("] WARN - "));
    Serial.print(message);
    Serial.println(extra);
}

void logError(const String& message, const String& extra) {
    Serial.print(F("["));
    Serial.print(millis());
    Serial.print(F("] ERROR - "));
    Serial.print(message);
    Serial.println(extra);
}

void sendSerialMessage(JsonDocument* json) {
    String jsonString;
    serializeJson(*json, Serial);
    Serial.println("");
}

void execute1HzTask() {
    if (availableChannels == 0) {
        JsonDocument eventJson;
        eventJson[F("event")] = F("missing-setup");
        sendSerialMessage(&eventJson);
    }
}

void execute5HzTask() {
}

void processCanMessageSend(const String& input) {
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
        logError(F("Received message with no data: "), input);

    } else {
        struct CanBusChannel* channel = NULL;
        for (int i = 0; i < availableChannels; i++) {
            if (channels[i].channel == channelInt) {
                channel = &channels[i];
                break;
            }
        }

        if (channel == NULL) {
            logError(F("Received message for unknown channel: "), input);

        } else {
            MCP2515 mcp2515(channel->pin);
            mcp2515.sendMessage(&canFrame);
        }
    }
}

void processAction(JsonDocument receivedJson) {
    String action = receivedJson[F("action")];

    if (action == F("restart")) {
        logInfo(F("Restarting..."), "");
        delay(1000);
        restart();

    } else if (action == F("get-info")) {
        JsonDocument infoJson;
        infoJson[F("event")] = F("info");
        infoJson[F("version")] = F("1.0.0");
        infoJson[F("millis")] = millis();
        sendSerialMessage(&infoJson);

    } else if (action == F("get-channels")) {
        JsonDocument channelsJson;
        JsonArray channelsArray = channelsJson[F("channels")].to<JsonArray>();
        for (int i = 0; i < availableChannels; i++) {
            JsonObject channelJson = channelsArray.add<JsonObject>();
            channelJson[F("pin")] = channels[i].pin;
            channelJson[F("channel")] = channels[i].channel;
            channelJson[F("readable")] = channels[i].readable;
        }
        sendSerialMessage(&channelsJson);

    } else if (action == F("get-bridges")) {
        JsonDocument bridgeAggJson;
        JsonArray bridgesArray = bridgeAggJson[F("bridges")].to<JsonArray>();
        for (int i = 0; i < availableBridges; i++) {
            JsonObject bridgeJson = bridgesArray.add<JsonObject>();
            bridgeJson[F("channelA")] = bridges[i].channelA;
            bridgeJson[F("channelB")] = bridges[i].channelB;
        }
        sendSerialMessage(&bridgeAggJson);

    } else if (action == F("add-channel")) {
        JsonObject channelJson = receivedJson[F("channel")];

        uint8_t pin = channelJson[F("pin")];
        uint8_t channel = channelJson[F("channel")];
        uint16_t baudrate = channelJson[F("baudrate")];
        uint8_t clock = channelJson[F("clock")];
        bool readable = channelJson[F("readable")];

        CAN_SPEED canSpeed;
        switch (baudrate) {
            case BAUD_RATE_5KBPS:
                canSpeed = CAN_5KBPS;
                break;
            case BAUD_RATE_10KBPS:
                canSpeed = CAN_10KBPS;
                break;
            case BAUD_RATE_20KBPS:
                canSpeed = CAN_20KBPS;
                break;
            case BAUD_RATE_33KBPS:
                canSpeed = CAN_33KBPS;
                break;
            case BAUD_RATE_50KBPS:
                canSpeed = CAN_50KBPS;
                break;
            case BAUD_RATE_100KBPS:
                canSpeed = CAN_100KBPS;
                break;
            case BAUD_RATE_125KBPS:
                canSpeed = CAN_125KBPS;
                break;
            case BAUD_RATE_200KBPS:
                canSpeed = CAN_200KBPS;
                break;
            case BAUD_RATE_250KBPS:
                canSpeed = CAN_250KBPS;
                break;
            case BAUD_RATE_1000KBPS:
                canSpeed = CAN_1000KBPS;
                break;
            case BAUD_RATE_500KBPS:
            default:
                canSpeed = CAN_500KBPS;
                break;
        }

        CAN_CLOCK canClock;
        switch (clock) {
            case CLOCK_20MHZ:
                canClock = MCP_20MHZ;
                break;
            case CLOCK_16MHZ:
                canClock = MCP_16MHZ;
                break;
            case CLOCK_8MHZ:
            default:
                canClock = MCP_8MHZ;
                break;
        }

        MCP2515 mcp2515(pin);
        mcp2515.reset();
        mcp2515.setBitrate(canSpeed, canClock);
        mcp2515.setNormalMode();

        CanBusChannel newChannel = {pin, channel, readable};
        channels[availableChannels] = newChannel;

        availableChannels++;

        JsonDocument eventJson;
        eventJson[F("event")] = F("add-channel-completed");
        sendSerialMessage(&eventJson);

    } else if (action == F("set-filter")) {
        uint8_t channel = receivedJson[F("channel")];
        bool extended = receivedJson[F("extended")];
        uint32_t mask = receivedJson[F("mask")];
        uint32_t filter = receivedJson[F("filter")];

        for (int i = 0; i < availableChannels; i++) {
            if (channels[i].channel == channel) {
                struct CanBusChannel channelObj = channels[i];

                MCP2515 mcp2515(channelObj.pin);
                mcp2515.setFilterMask(MCP2515::MASK0, extended, mask);
                mcp2515.setFilter(MCP2515::RXF0, extended, filter);
                mcp2515.setFilterMask(MCP2515::MASK1, extended, mask);
                break;
            }
        }

        JsonDocument eventJson;
        eventJson[F("event")] = F("set-filter-completed");
        sendSerialMessage(&eventJson);

    } else if (action == F("add-bridge")) {
        uint8_t channelA = receivedJson[F("channelA")];
        uint8_t channelB = receivedJson[F("channelB")];
        bool bidirectional = receivedJson[F("bidirectional")];

        CanBusBridge newBridge = {channelA, channelB, bidirectional};
        bridges[availableBridges] = newBridge;

        availableBridges++;

        JsonDocument eventJson;
        eventJson[F("event")] = F("add-bridge-completed");
        sendSerialMessage(&eventJson);

    } else {
        logError(F("Unknown action: "), action);
    }
}

void processSerialInput() {
    String input = Serial.readStringUntil('\n');

    if (input.startsWith("CH")) {
        processCanMessageSend(input);

    } else if (input.length() == 1) {
        Serial.println("");

    } else {
        JsonDocument receivedJson;
        deserializeJson(receivedJson, input);
        String action = receivedJson[F("action")];

        if (action != "" && action != F("null")) {
            processAction(receivedJson);

        } else {
            logWarn(F("Received from serial: "), input);
        }
    }
}

void bridgeFrame(const uint8_t& channel, struct can_frame* canFrame) {
    for (int j = 0; j < availableBridges; j++) {
        struct CanBusBridge bridge = bridges[j];
        if (bridge.channelA == channel) {
            struct CanBusChannel destinationChannel = channels[bridge.channelB];
            MCP2515 destinationMcp2515(destinationChannel.pin);
            destinationMcp2515.sendMessage(canFrame);

        } else if (bridge.bidirectional && bridge.channelB == channel) {
            struct CanBusChannel destinationChannel = channels[bridge.channelA];
            MCP2515 destinationMcp2515(destinationChannel.pin);
            destinationMcp2515.sendMessage(canFrame);
        }
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    logInfo(F("Starting..."), "");

    SPI.begin();

    logInfo(F("Started"), "");
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
            int bytes = canFrame.can_dlc;

            if (bytes > 0 && channel.readable) {
                char pidHex[9];
                sprintf(pidHex, "%X", pid);

                Serial.print("CH");
                Serial.print(channel.channel);
                Serial.print(' ');
                Serial.print(millis());
                Serial.print(' ');
                Serial.print(pidHex);
                Serial.print(' ');

                for (int i = 0; i < bytes; i++) {
                    int byteValue = canFrame.data[i];

                    if (i > 0) {
                        Serial.print(':');
                    }

                    char byteHex[3];
                    sprintf(byteHex, "%02X", byteValue);
                    Serial.print(byteHex);
                }

                Serial.println();

                bridgeFrame(channel.channel, &canFrame);
            }
        }
    }
}
