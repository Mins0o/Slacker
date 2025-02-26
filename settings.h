/* Arduino IDE Tool Settings:
Board: "ESP32 Dev Module"
Upload Speed: "921600"
CPU Frequency: "240MHz (WiFi/BT)"
Flash Frequency: "80MHz"
Flash Size: "4MB (32Mb)"
Partition Scheme: "HUGE APP (3MB No OTA/1MB Spiffs)"
Core Debug Level: "None"
PSRAM: "Diabled"
Arduino Runs On: "Core 1"
Events Runs On: "Core 1"
Port: "/dev/ttyUSB0"
 */

#ifndef SETTINGS_H
#define SETTINGS_H

// Weather Settings
#define CITY_ID \
  "5128581"  // New York City https://openweathermap.org/current#cityid
#define OPENWEATHERMAP_APIKEY \
  "none"  // use your own API key :)
#define OPENWEATHERMAP_URL                  \
  "http://api.openweathermap.org/data/2.5/" \
  "weather?id={cityID}&units={units}&appid={apiKey}"  // open weather api
#define TEMP_UNIT "metric"  // metric = Celsius , imperial = Fahrenheit
#define TEMP_LANG "en"
#define WEATHER_UPDATE_INTERVAL \
  30             // must be greater than 5, measured in minutes
#define PING 10  // see if network is available
// NTP Settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600 * -4  // New York is UTC -5, Daylight Savings is -4
#define DST_OFFSET_SEC 3600 * 0

watchySettings settings{
    .cityID = CITY_ID,
    .weatherAPIKey = OPENWEATHERMAP_APIKEY,
    .weatherURL = OPENWEATHERMAP_URL,
    .weatherUnit = TEMP_UNIT,
    .weatherLang = TEMP_LANG,
    .weatherUpdateInterval = WEATHER_UPDATE_INTERVAL,
    .ntpServer = NTP_SERVER,
    .gmtOffset = GMT_OFFSET_SEC + DST_OFFSET_SEC,
};

#endif
