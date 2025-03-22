/*
Slacker
Version 1.2.1
https://github.com/uCBill/Slacker
Linux like watchface by Bill Eichner
Daylight Savings Time is configured in the settings.h section

Based on Bahn-for-Watchy
https://github.com/BraininaBowl/Bahn-for-Watchy

Based on SmatWatch
https://github.com/theRealc2c2/SmartWatch

Face for Watchy watch
https://watchy.sqfmi.com
*/

#include <Watchy.h>  //include the Watchy library

#include "LiberationSansNarrow_Bold8pt7b.h"
#include "Teko_Regular20pt7b.h"
#include "icons.h"
#include "prompt.h"
#include "settings.h"

#define ACTIVE_LOW 1
RTC_DATA_ATTR bool forecastFetched_g = false;
RTC_DATA_ATTR bool weatherUpdated_g = false;
RTC_DATA_ATTR int weatherIntervalCounter_g = -1;
RTC_DATA_ATTR weatherData currentWeather_g;
RTC_DATA_ATTR float min_temp_g;
RTC_DATA_ATTR float max_temp_g;
RTC_DATA_ATTR bool light =
    true;  // left this here if someone wanted to tweak for dark

class WatchFace : public Watchy {  // inherit and extend Watchy class
  using Watchy::Watchy;
  long gmtOffset = 0;

  String weatherQueryURL;
  String weatherResponse;
  String forecastQueryURL;
  String forecastResponse;

 public:
  void drawWatchFace() {  // override this method to customize how the watch
                          // face looks

    int16_t x1, y1;
    uint16_t w, h;
    String textstring;

    // resets step counter at midnight everyday
    if (currentTime.Hour == 00 && currentTime.Minute == 00) {
      sensor.resetStepCounter();
    }

    // ** DRAW **

    // drawbg
    display.fillScreen(light ? GxEPD_WHITE : GxEPD_BLACK);
    display.setFont(&LiberationSansNarrow_Bold8pt7b);
    display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextWrap(false);

    // draw slacker text
    display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(0, 10);
    display.print("sk@darkstar:/~");
    display.setCursor(0, 23);
    display.print("$ steps; weather");
    display.setCursor(0, 36);
    display.print("total steps: ");  // use step # for total
    display.setCursor(0, 49);
    display.print("temp(--|L|H):    climate:");
    display.setCursor(0, 75);
    display.print("sk@darkstar:/~");
    display.setCursor(0, 89);
    display.print("$ date");

    // draw day of week
    textstring = dayShortStr(currentTime.Wday);
    textstring += " ";

    // draw day, month, year
    textstring += currentTime.Day;
    textstring += " ";
    textstring += monthShortStr(currentTime.Month);
    textstring += " ";
    textstring += currentTime.Year - 30;
    textstring += " ";

    // draw time
    display.setFont(&LiberationSansNarrow_Bold8pt7b);
    if (currentTime.Hour > 0 && currentTime.Hour < 10) {
      textstring += "0";
    } else {
      textstring += "";
    }
    textstring += currentTime.Hour;

    textstring += ":";
    if (currentTime.Minute < 10) {
      textstring += "0";
    } else {
      textstring += "";
    }
    textstring += currentTime.Minute;

    textstring += ":";
    if (currentTime.Second < 10) {
      textstring += "0";
    } else {
      textstring += "";
    }
    textstring += currentTime.Second;

    // this section adds AM or PM to the display
    if (currentTime.Hour >= 12) {
      textstring +=
          " PM EDT";  // List of US Time Zones: EST, CST, MST, PST, AKST
    } else {
      textstring += " AM EDT";  // List of US Daylight Savings Time Zones: EDT,
                                // CDT, MDT, PDT, AKDT
    }
    // To change Time Zones, including Daylight Savings Time, see the settings.h
    // section

    display.setCursor(0, 103);
    display.print(textstring);

    // drawTimeBold
    display.setFont(&Teko_Regular20pt7b);
    if (currentTime.Hour > 0 && currentTime.Hour <= 12) {
      textstring = currentTime.Hour;
    } else if (currentTime.Hour < 1) {
      textstring = 12;
    } else {
      textstring = ((currentTime.Hour + 11) % 12) + 1;
    }

    textstring += ":";
    if (currentTime.Minute < 10) {
      textstring += "0";
    } else {
      textstring += "";
    }
    textstring += currentTime.Minute;

    // this section adds AM or PM to the display
    if (currentTime.Hour >= 12) {
      textstring += "PM";
    } else {
      textstring += "AM";
    }

    display.setCursor(107, 23);
    display.print(textstring);

    // draw battery
    display.setFont(&LiberationSansNarrow_Bold8pt7b);
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();

    if (VBAT >= 4.2) {
      batteryLevel = 100.0;
    } else if (VBAT >= 3.3) {
      batteryLevel = 100.0 * (VBAT - 3.3) / 0.9;
    }
    display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(0, 116);
    display.print("sk@darkstar:/~");
    display.setCursor(0, 130);
    display.print("$ upower -i /UPW/dev/btry_BT0");
    display.setCursor(0, 144);
    display.print("voltage:");
    display.setCursor(90, 144);
    display.print(VBAT);
    display.print(" V");
    display.setCursor(0, 158);
    display.print("percentage:");
    display.setCursor(90, 158);
    display.print(batteryLevel);
    display.setCursor(143, 158);
    display.print("%");
    display.setCursor(0, 173);
    display.print("technology:");
    display.setCursor(90, 173);
    display.print("lithium-pol");
    display.setCursor(0, 187);
    display.print("sk@darkstar:/~");
    display.setCursor(0, 199);
    display.print("$ ");
    // draw image
    display.drawBitmap(12, 189, epd_bitmap_prompt, 7, 14,
                       light ? GxEPD_BLACK : GxEPD_WHITE);

    // draw steps
    display.setFont(&LiberationSansNarrow_Bold8pt7b);
    textstring = sensor.getCounter();
    display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(75, 36);
    display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);
    display.print(textstring);
    currentWeather_g = getWeatherData();
    String temperature = String(currentWeather_g.temperature);
    int16_t weatherConditionCode = currentWeather_g.weatherConditionCode;
    textstring = temperature;
    textstring += "|";
    textstring += (int)floor(min_temp_g);
    textstring += "|";
    textstring += (int)ceil(max_temp_g);
    display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
    display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);

    display.setCursor(35, 62);
    display.print(textstring);
    display.println(" `C");
    display.setCursor(35, 62);
    forecastFetched_g ? display.print("_") : 0;
    display.setCursor(0, 49);
    weatherUpdated_g ? display.print("_") : 0;

    const unsigned char* weatherIcon;

    if (WIFI_CONFIGURED) {
      // https://openweathermap.org/weather-conditions
      if (weatherConditionCode > 801) {  // Cloudy
        weatherIcon = cloudy;
      } else if (weatherConditionCode == 801) {  // Few Clouds
        weatherIcon = cloudsun;
      } else if (weatherConditionCode == 800) {  // Clear
        weatherIcon = sunny;
      } else if (weatherConditionCode >= 700) {  // Atmosphere
        weatherIcon = atmosphere;
      } else if (weatherConditionCode >= 600) {  // Snow
        weatherIcon = snow;
      } else if (weatherConditionCode >= 500) {  // Rain
        weatherIcon = rain;
      } else if (weatherConditionCode >= 300) {  // Drizzle
        weatherIcon = drizzle;
      } else if (weatherConditionCode >= 200) {  // Thunderstorm
        weatherIcon = thunderstorm;
      }
    } else {
      weatherIcon = chip;
    }
    display.drawBitmap(147, 33, weatherIcon, 48, 32,
                       light ? GxEPD_BLACK : GxEPD_WHITE);
    display.drawBitmap(169, 144, WIFI_CONFIGURED ? wifi : wifioff, 26, 18,
                       light ? GxEPD_BLACK : GxEPD_WHITE);

    // debug
    //    display.fillScreen(0x7f);
    //    display.setCursor(0, 15);
    //    display.setTextWrap(true);
    ////    display.println(weatherQueryURL);
    ////    display.println(weatherResponse);
    ////    display.println(forecastQueryURL);
    ////    display.println(forecastResponse);
    //    display.setTextWrap(false);
  }

  weatherData getWeatherData() {
    return _getWeatherData(settings.cityID, settings.lat, settings.lon,
                           settings.weatherUnit, settings.weatherLang,
                           settings.weatherURL, settings.weatherAPIKey,
                           settings.weatherUpdateInterval = 30);
  }

  weatherData _getWeatherData(const String& cityID, const String& lat,
                              const String& lon, const String& units,
                              const String& lang, const String& url,
                              const String& apiKey, uint8_t updateInterval) {
    currentWeather_g.isMetric = units == String("metric");
    if (weatherIntervalCounter_g <
        0) {  //-1 on first run, set to updateInterval
      weatherIntervalCounter_g = updateInterval;
    }
    if (weatherIntervalCounter_g >=
        updateInterval) {  // only update if WEATHER_UPDATE_INTERVAL has elapsed
                           // i.e. 30 minutes
      if (connectWiFi()) {
        HTTPClient http;  // Use Weather API for live data if WiFi is connected
        http.setConnectTimeout(1500);  // 3 second max timeout
        weatherQueryURL = url;
        weatherResponse = "";
        if (cityID != "") {
          weatherQueryURL.replace("{cityID}", cityID);
        } else {
          weatherQueryURL.replace("{lat}", lat);
          weatherQueryURL.replace("{lon}", lon);
        }
        weatherQueryURL.replace("{units}", units);
        weatherQueryURL.replace("{lang}", lang);
        weatherQueryURL.replace("{apiKey}", apiKey);
        http.begin(weatherQueryURL.c_str());
        int httpResponseCode = http.GET();
        if (httpResponseCode == 200) {
          weatherResponse = http.getString();
          JSONVar responseObject = JSON.parse(weatherResponse);
          currentWeather_g.temperature = (double)responseObject["main"]["temp"];
          currentWeather_g.weatherConditionCode =
              int(responseObject["weather"][0]["id"]);
          currentWeather_g.weatherDescription =
              JSONVar::stringify(responseObject["weather"][0]["main"]);
          currentWeather_g.external = true;
          breakTime((time_t)(int)responseObject["sys"]["sunrise"],
                    currentWeather_g.sunrise);
          breakTime((time_t)(int)responseObject["sys"]["sunset"],
                    currentWeather_g.sunset);
          // sync NTP during weather API call and use timezone of lat & lon
          gmtOffset = int(responseObject["timezone"]);
          syncNTP(gmtOffset);
        } else {
          // http error
        }
        http.end();
        // turn off radios
        WiFi.mode(WIFI_OFF);
        btStop();
      } else {  // No WiFi, use internal temperature sensor
        uint8_t temperature = sensor.readTemperature();  // celsius
        if (!currentWeather_g.isMetric) {
          temperature = temperature * 9. / 5. + 32.;  // fahrenheit
        }
        currentWeather_g.temperature = temperature;
        currentWeather_g.weatherConditionCode = 800;
        currentWeather_g.external = false;
      }
      if (connectWiFi()) {
        float min_temp_backup = min_temp_g;
        float max_temp_backup = max_temp_g;
        min_temp_g = currentWeather_g.temperature;
        max_temp_g = currentWeather_g.temperature;
        getMinMaxTemperature(min_temp_g, max_temp_g, weatherQueryURL);
        if (!forecastFetched_g) {
          min_temp_g = min_temp_backup;
          max_temp_g = max_temp_backup;
        }
        WiFi.mode(WIFI_OFF);
        btStop();
      }
      weatherIntervalCounter_g = 0;
      weatherUpdated_g = true;
    } else {
      weatherIntervalCounter_g++;
      weatherUpdated_g = false;
      forecastFetched_g = false;
    }
    return currentWeather_g;
  }

  void getMinMaxTemperature(float& minTemp, float& maxTemp,
                            const String& finished_url) {
    HTTPClient http;  // Use Weather API for live data if WiFi is connected
    http.setConnectTimeout(1500);  // 3 second max timeout
    forecastQueryURL = finished_url;
    forecastQueryURL.replace("2.5/weather", "2.5/forecast");
    forecastQueryURL += "&cnt=10";
    http.begin(forecastQueryURL.c_str());
    int httpResponseCode = http.GET();
    forecastResponse = "failed";
    if (httpResponseCode == 200) {
      forecastFetched_g = true;
      forecastResponse = http.getString();
      JSONVar responseObject = JSON.parse(forecastResponse);
      auto forecast_list = responseObject["list"];
      size_t list_size = forecast_list.length();
      float temp_min = minTemp;
      float temp_max = maxTemp;
      for (int ii = 0; ii < list_size && ii < 10; ++ii) {
        JSONVar forecast = forecast_list[ii]["main"];
        temp_min = (double)forecast["temp_min"];
        temp_max = (double)forecast["temp_max"];
        if (temp_min < minTemp) {
          minTemp = temp_min;
        }
        if (temp_max > maxTemp) {
          maxTemp = temp_max;
        }
      }
    } else {
      // http error
      forecastResponse += httpResponseCode;
      forecastResponse += http.getString();
      forecastFetched_g = false;
    }
    http.end();
  }

  void handleButtonPress() {
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    // Menu Button
    if (wakeupBit & MENU_BTN_MASK) {
      if (guiState ==
          WATCHFACE_STATE) {  // enter menu state if coming from watch face
        showMenu(menuIndex, false);
      } else if (guiState == MAIN_MENU_STATE) {  // if already in menu, then
                                                 // select menu item
        switch (menuIndex) {
          case 0:
            showAbout();
            break;
          case 1:
            showBuzz();
            break;
          case 2:
            showAccelerometer();
            break;
          case 3:
            setTime();
            break;
          case 4:
            setupWifi();
            break;
          case 5:
            showUpdateFW();
            break;
          case 6:
            showSyncNTP();
            break;
          default:
            break;
        }
      } else if (guiState == FW_UPDATE_STATE) {
        updateFWBegin();
      }
    }
    // Back Button
    else if (wakeupBit & BACK_BTN_MASK) {
      if (guiState ==
          MAIN_MENU_STATE) {  // exit to watch face if already in menu
        RTC.read(currentTime);
        showWatchFace(false);
      } else if (guiState == APP_STATE) {
        showMenu(menuIndex, false);  // exit to menu if already in app
      } else if (guiState == FW_UPDATE_STATE) {
        showMenu(menuIndex, false);  // exit to menu if already in app
      } else if (guiState == WATCHFACE_STATE) {
        RTC.clearAlarm();
        RTC.read(currentTime);
        showWatchFace(true);
        return;
      }
    }
    // Up Button
    else if (wakeupBit & UP_BTN_MASK) {
      if (guiState == MAIN_MENU_STATE) {  // increment menu index
        menuIndex--;
        if (menuIndex < 0) {
          menuIndex = MENU_LENGTH - 1;
        }
        showMenu(menuIndex, true);
      } else if (guiState == WATCHFACE_STATE) {
        light = light ? false : true;
        RTC.read(currentTime);
        showWatchFace(true);
        return;
      }
    }
    // Down Button
    else if (wakeupBit & DOWN_BTN_MASK) {
      if (guiState == MAIN_MENU_STATE) {  // decrement menu index
        menuIndex++;
        if (menuIndex > MENU_LENGTH - 1) {
          menuIndex = 0;
        }
        showMenu(menuIndex, true);
      } else if (guiState == WATCHFACE_STATE) {
        return;
      }
    }

    /***************** fast menu *****************/
    bool timeout = false;
    long lastTimeout = millis();
    pinMode(MENU_BTN_PIN, INPUT);
    pinMode(BACK_BTN_PIN, INPUT);
    pinMode(UP_BTN_PIN, INPUT);
    pinMode(DOWN_BTN_PIN, INPUT);
    while (!timeout) {
      if (millis() - lastTimeout > 5000) {
        timeout = true;
      } else {
        if (digitalRead(MENU_BTN_PIN) == ACTIVE_LOW) {
          lastTimeout = millis();
          if (guiState ==
              MAIN_MENU_STATE) {  // if already in menu, then select menu item
            switch (menuIndex) {
              case 0:
                showAbout();
                break;
              case 1:
                showBuzz();
                break;
              case 2:
                showAccelerometer();
                break;
              case 3:
                setTime();
                break;
              case 4:
                setupWifi();
                break;
              case 5:
                showUpdateFW();
                break;
              case 6:
                showSyncNTP();
                break;
              default:
                break;
            }
          } else if (guiState == FW_UPDATE_STATE) {
            updateFWBegin();
          }
        } else if (digitalRead(BACK_BTN_PIN) == ACTIVE_LOW) {
          lastTimeout = millis();
          if (guiState ==
              MAIN_MENU_STATE) {  // exit to watch face if already in menu
            RTC.read(currentTime);
            showWatchFace(false);
            break;  // leave loop
          } else if (guiState == APP_STATE) {
            showMenu(menuIndex, false);  // exit to menu if already in app
          } else if (guiState == FW_UPDATE_STATE) {
            showMenu(menuIndex, false);  // exit to menu if already in app
          } else if (guiState == WATCHFACE_STATE) {
            RTC.clearAlarm();
            RTC.read(currentTime);
            showWatchFace(true);
          }
        } else if (digitalRead(UP_BTN_PIN) == ACTIVE_LOW) {
          lastTimeout = millis();
          if (guiState == MAIN_MENU_STATE) {  // increment menu index
            menuIndex--;
            if (menuIndex < 0) {
              menuIndex = MENU_LENGTH - 1;
            }
            showFastMenu(menuIndex);
          } else if (guiState == WATCHFACE_STATE) {
            light = light ? false : true;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
          }
        } else if (digitalRead(DOWN_BTN_PIN) == ACTIVE_LOW) {
          lastTimeout = millis();
          if (guiState == MAIN_MENU_STATE) {  // decrement menu index
            menuIndex++;
            if (menuIndex > MENU_LENGTH - 1) {
              menuIndex = 0;
            }
            showFastMenu(menuIndex);
          }
        }
      }
    }
  }
};

WatchFace m(settings);  // instantiate your watchface

void setup() {
  m.init();  // call init in setup
}

void loop() {
  // this should never run, Watchy deep sleeps after init();
}
