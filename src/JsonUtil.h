#ifndef COLOR_UTIL_H
#define COLOR_UTIL_H

#include <ArduinoJson.h>
#include <FastLed.h>

void updateBooleanArrayFromJson(JsonObject& root, bool booleanArray[], uint16_t maxSize, String key);
void writeBooleanArrayToJson(JsonObject& root, bool booleanArray[], uint16_t maxSize, String key);

void updateColorFromJson(JsonObject& root, CRGB* color, CRGB def, String key = "color");
void writeColorToJson(JsonObject& root, CRGB* color, String key = "color");

void updateByteFromJson(JsonObject& root, uint8_t* value, uint8_t def, String key);
void writeByteToJson(JsonObject& root, uint8_t* value, String key);

void updateBoolFromJson(JsonObject& root, bool* value, bool def, String key);
void writeBoolToJson(JsonObject& root, bool* value, String key);

#endif
