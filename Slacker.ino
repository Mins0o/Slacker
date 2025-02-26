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

class WatchFace : public Watchy {  // inherit and extend Watchy class
  using Watchy::Watchy;

 public:
  void drawWatchFace() {  // override this method to customize how the watch
                          // face looks

    int16_t x1, y1;
    uint16_t w, h;
    String textstring;
    bool light = true;  // left this here if someone wanted to tweak for dark

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
    weatherData currentWeather = getWeatherData();
    String temperature = String(currentWeather.temperature);
    int16_t weatherConditionCode = currentWeather.weatherConditionCode;
    textstring = temperature;
    textstring += "|";
    textstring += (int)floor(currentWeather.min_temp);
    textstring += "|";
    textstring += (int)ceil(currentWeather.max_temp);
    display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
    display.setTextColor(light ? GxEPD_BLACK : GxEPD_WHITE);

    display.setCursor(12, 62);
    display.print(textstring);
    display.println(" `C");
    display.setCursor(12, 62);
    forecastFetched ? display.print("_") : 0;
    display.setCursor(0, 49);
    weatherUpdated_ ? display.print("_") : 0;

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
};

WatchFace m(settings);  // instantiate your watchface

void setup() {
  m.init();  // call init in setup
}

void loop() {
  // this should never run, Watchy deep sleeps after init();
}
