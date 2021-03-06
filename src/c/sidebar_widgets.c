#include <pebble.h>
#include <math.h>
#include "settings.h"
#include "weather.h"
#include "languages.h"
#include "util.h"
#ifdef PBL_HEALTH
#include "health.h"
#endif
#include "time_date.h"
#include "sidebar_widgets.h"


bool SidebarWidgets_useCompactMode = false;
bool SidebarWidgets_fixedHeight = false;
int SidebarWidgets_xOffset;

// sidebar icons
static GDrawCommandImage* dateImage;
static GDrawCommandImage* disconnectImage;
static GDrawCommandImage* batteryImage;
static GDrawCommandImage* batteryChargeImage;

// fonts
static GFont smSidebarFont;
static GFont mdSidebarFont;
static GFont lgSidebarFont;
static GFont currentSidebarFont;
static GFont currentSidebarSmallFont;

// the widgets
static SidebarWidget batteryMeterWidget;
static int BatteryMeter_getHeight(void);
static void BatteryMeter_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget emptyWidget;
static int EmptyWidget_getHeight(void);
static void EmptyWidget_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget dateWidget;
static int DateWidget_getHeight(void);
static void DateWidget_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget currentWeatherWidget;
static int CurrentWeather_getHeight(void);
static void CurrentWeather_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget weatherForecastWidget;
static int WeatherForecast_getHeight(void);
static void WeatherForecast_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget btDisconnectWidget;
static int BTDisconnect_getHeight(void);
static void BTDisconnect_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget weekNumberWidget;
static int WeekNumber_getHeight(void);
static void WeekNumber_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget secondsWidget;
static int Seconds_getHeight(void);
static void Seconds_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget altTimeWidget;
static int AltTime_getHeight(void);
static void AltTime_draw(GContext* ctx, int xPosition, int yPosition);

static SidebarWidget beatsWidget;
static int Beats_getHeight(void);
static void Beats_draw(GContext* ctx, int xPosition, int yPosition);

#ifdef PBL_HEALTH
  static GDrawCommandImage* sleepImage;
  static GDrawCommandImage* stepsImage;
  static GDrawCommandImage* heartImage;

  static SidebarWidget healthWidget;
  static int Health_getHeight(void);
  static void Health_draw(GContext* ctx, int xPosition, int yPosition);
  static SidebarWidget sleepWidget;
  static int Sleep_getHeight(void);
  static void Sleep_draw(GContext* ctx, int xPosition, int yPosition);
  static SidebarWidget stepsWidget;
  static int Steps_getHeight(void);
  static void Steps_draw(GContext* ctx, int xPosition, int yPosition);

  static SidebarWidget heartRateWidget;
  static int HeartRate_getHeight(void);
  static void HeartRate_draw(GContext* ctx, int xPosition, int yPosition);
#endif

void SidebarWidgets_init(void) {
  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  lgSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // load the sidebar graphics
  dateImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DATE_BG);
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  #ifdef PBL_HEALTH
    sleepImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_SLEEP);
    stepsImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_STEPS);
    heartImage = gdraw_command_image_create_with_resource(RESOURCE_ID_HEALTH_HEART);
  #endif

  // set up widgets' function pointers correctly
  batteryMeterWidget.getHeight = BatteryMeter_getHeight;
  batteryMeterWidget.draw      = BatteryMeter_draw;

  emptyWidget.getHeight = EmptyWidget_getHeight;
  emptyWidget.draw      = EmptyWidget_draw;

  dateWidget.getHeight = DateWidget_getHeight;
  dateWidget.draw      = DateWidget_draw;

  currentWeatherWidget.getHeight = CurrentWeather_getHeight;
  currentWeatherWidget.draw      = CurrentWeather_draw;

  weatherForecastWidget.getHeight = WeatherForecast_getHeight;
  weatherForecastWidget.draw      = WeatherForecast_draw;

  btDisconnectWidget.getHeight = BTDisconnect_getHeight;
  btDisconnectWidget.draw      = BTDisconnect_draw;

  weekNumberWidget.getHeight = WeekNumber_getHeight;
  weekNumberWidget.draw      = WeekNumber_draw;

  secondsWidget.getHeight = Seconds_getHeight;
  secondsWidget.draw      = Seconds_draw;

  altTimeWidget.getHeight = AltTime_getHeight;
  altTimeWidget.draw      = AltTime_draw;

  #ifdef PBL_HEALTH
    healthWidget.getHeight = Health_getHeight;
    healthWidget.draw = Health_draw;

    sleepWidget.getHeight = Sleep_getHeight;
    sleepWidget.draw = Sleep_draw;

    stepsWidget.getHeight = Steps_getHeight;
    stepsWidget.draw = Steps_draw;

    heartRateWidget.getHeight = HeartRate_getHeight;
    heartRateWidget.draw = HeartRate_draw;
  #endif

  beatsWidget.getHeight = Beats_getHeight;
  beatsWidget.draw      = Beats_draw;

}

void SidebarWidgets_deinit(void) {
  gdraw_command_image_destroy(dateImage);
  gdraw_command_image_destroy(disconnectImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);

  #ifdef PBL_HEALTH
    gdraw_command_image_destroy(stepsImage);
    gdraw_command_image_destroy(sleepImage);
    gdraw_command_image_destroy(heartImage);
  #endif
}

void SidebarWidgets_updateFonts(void) {
  if(globalSettings.useLargeFonts) {
    currentSidebarFont = lgSidebarFont;
    currentSidebarSmallFont = mdSidebarFont;
  } else {
    currentSidebarFont = mdSidebarFont;
    currentSidebarSmallFont = smSidebarFont;
  }
}

/* Sidebar Widget Selection */
SidebarWidget getSidebarWidgetByType(SidebarWidgetType type) {
  switch(type) {
    case BATTERY_METER:
      return batteryMeterWidget;
      break;
    case BLUETOOTH_DISCONNECT:
      return btDisconnectWidget;
      break;
    case DATE:
      return dateWidget;
      break;
    case ALT_TIME_ZONE:
      return altTimeWidget;
      break;
    case SECONDS:
      return secondsWidget;
      break;
    case WEATHER_CURRENT:
      return currentWeatherWidget;
      break;
    case WEATHER_FORECAST_TODAY:
      return weatherForecastWidget;
      break;
    case WEEK_NUMBER:
      return weekNumberWidget;
    #ifdef PBL_HEALTH
      case HEALTH:
        return healthWidget;
      case SLEEP:
        return sleepWidget;
      case STEP:
        return stepsWidget;
      case HEARTRATE:
        return heartRateWidget;
    #endif
    case BEATS:
      return beatsWidget;
    default:
      return emptyWidget;
      break;
  }
}

/********** functions for the empty widget **********/
static int EmptyWidget_getHeight(void) {
  return 0;
}

static void EmptyWidget_draw(GContext* ctx, int xPosition, int yPosition) {
  return;
}

/********** functions for the battery meter widget **********/

static int BatteryMeter_getHeight(void) {
  BatteryChargeState chargeState = battery_state_service_peek();

  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else if(chargeState.is_charging || !globalSettings.showBatteryPct) {
    return 14; // graphic only height
  } else {
    return (globalSettings.useLargeFonts) ? 33 : 27; // heights with text
  }
}

static void BatteryMeter_draw(GContext* ctx, int xPosition, int yPosition) {

  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  char batteryString[6];
  int batteryPositionY = yPosition;

  if(SidebarWidgets_fixedHeight){
    if(!globalSettings.showBatteryPct || chargeState.is_charging) {
      batteryPositionY += (FIXED_WIDGET_HEIGHT / 2) - 12;
    } else {
      batteryPositionY += 3;
    }
  } else {
    // correct for vertical empty space on battery icon
    batteryPositionY -= 5;
  }

  if (batteryImage) {
    util_image_draw(ctx, batteryImage, xPosition + 3 + SidebarWidgets_xOffset, batteryPositionY);
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      util_image_draw_inverted_color(ctx, batteryChargeImage, xPosition + 3 + SidebarWidgets_xOffset, batteryPositionY);
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, globalSettings.iconStrokeColor);

    #ifdef PBL_COLOR
      if(battery_percent <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
      }
    #endif

    graphics_fill_rect(ctx, GRect(xPosition + 6 + SidebarWidgets_xOffset, 8 + batteryPositionY, width, 8), 0, GCornerNone);
  }

  // never show battery % while charging, because of this issue:
  // https://github.com/freakified/TimeStylePebble/issues/11
  if(globalSettings.showBatteryPct && !chargeState.is_charging) {
    int textOffsetY;
    GFont batteryFont;

    if(!globalSettings.useLargeFonts) {
      batteryFont = smSidebarFont;
      if(SidebarWidgets_fixedHeight) {
        textOffsetY = 25;
      } else {
        textOffsetY = 18;
      }

      // put the percent sign on the opposite side if turkish
      snprintf(batteryString, sizeof(batteryString),
               (globalSettings.languageId == LANGUAGE_TR) ? "%%%d" : "%d%%",
               battery_percent);
    } else {
      batteryFont = lgSidebarFont;
      if(SidebarWidgets_fixedHeight) {
        textOffsetY = 18;
      } else {
        textOffsetY = 14;
      }

      snprintf(batteryString, sizeof(batteryString), "%d", battery_percent);
    }
    graphics_draw_text(ctx,
                       batteryString,
                       batteryFont,
                       GRect(xPosition - 4 + SidebarWidgets_xOffset, textOffsetY + batteryPositionY, 38, 20),
                       GTextOverflowModeFill,
                       GTextAlignmentCenter,
                       NULL);
  }
}

/********** current date widget **********/

static int DateWidget_getHeight(void) {
  if(globalSettings.useLargeFonts) {
    return (SidebarWidgets_useCompactMode) ? 42 : 62;
  } else  {
    return (SidebarWidgets_useCompactMode) ? 41 : 58;
  }
}

static void DateWidget_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  // compensate for extra space that appears on the top of the date widget
  yPosition -= (globalSettings.useLargeFonts) ? 10 : 7;

  // first draw the day name
  graphics_draw_text(ctx,
                     time_date_currentDayName,
                     currentSidebarFont,
                     GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  // next, draw the date background
  // (an image in normal mode, a rectangle in large font mode)
  if(!globalSettings.useLargeFonts) {
    if(dateImage) {
      util_image_draw(ctx, dateImage, xPosition + 3 + SidebarWidgets_xOffset, yPosition + 23);
    }
  } else {
    graphics_context_set_fill_color(ctx, globalSettings.iconStrokeColor);
    graphics_fill_rect(ctx, GRect(xPosition + 2 + SidebarWidgets_xOffset, yPosition + 30, 26, 22), 2, GCornersAll);

    graphics_context_set_fill_color(ctx, globalSettings.iconFillColor);
    graphics_fill_rect(ctx, GRect(xPosition + 4 + SidebarWidgets_xOffset, yPosition + 32, 22, 18), 0, GCornersAll);
  }

  // next, draw the date number
  graphics_context_set_text_color(ctx, globalSettings.iconStrokeColor);

  int yOffset = 0;
  yOffset = globalSettings.useLargeFonts ? 24 : 26;

  graphics_draw_text(ctx,
                     time_date_currentDayNum,
                     currentSidebarFont,
                     GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + yOffset, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);


   // switch back to normal color for the rest
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  // don't draw the month if we're in compact mode
  if(!SidebarWidgets_useCompactMode) {
    yOffset = globalSettings.useLargeFonts ? 48 : 47;

    graphics_draw_text(ctx,
                       time_date_currentMonth,
                       currentSidebarFont,
                       GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + yOffset, 40, 20),
                       GTextOverflowModeFill,
                       GTextAlignmentCenter,
                       NULL);
  }


}

/********** current weather widget **********/

static int CurrentWeather_getHeight(void) {
  if(globalSettings.useLargeFonts) {
    return 44;
  } else {
    return 42;
  }
}

static void CurrentWeather_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  if (Weather_currentWeatherIcon) {
    util_image_draw(ctx, Weather_currentWeatherIcon, xPosition + 3 + SidebarWidgets_xOffset, yPosition);
  }

  // draw weather data only if it has been set
  if(Weather_weatherInfo.currentTemp != INT32_MIN) {

    int currentTemp = Weather_weatherInfo.currentTemp;

    if(!globalSettings.useMetric) {
      currentTemp = roundf(currentTemp * 1.8f + 32);
    }

    char tempString[8];

    // in large font mode, omit the degree symbol and move the text
    if(!globalSettings.useLargeFonts) {
      snprintf(tempString, sizeof(tempString), " %d°", currentTemp);

      graphics_draw_text(ctx,
                         tempString,
                         currentSidebarFont,
                         GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + 24, 38, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);
    } else {
      snprintf(tempString, sizeof(tempString), " %d", currentTemp);

      graphics_draw_text(ctx,
                         tempString,
                         currentSidebarFont,
                         GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + 20, 35, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);
    }
  } else {
    // if the weather data isn't set, draw a loading indication
    graphics_draw_text(ctx,
                       "...",
                       currentSidebarFont,
                       GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition, 38, 20),
                       GTextOverflowModeFill,
                       GTextAlignmentCenter,
                       NULL);
  }
}

/***** Bluetooth Disconnection Widget *****/

static int BTDisconnect_getHeight(void) {
  return 22;
}

static void BTDisconnect_draw(GContext* ctx, int xPosition, int yPosition) {
  if(disconnectImage) {
    util_image_draw(ctx, disconnectImage, xPosition + 3 + SidebarWidgets_xOffset, yPosition);
  }
}

/***** Week Number Widget *****/

static int WeekNumber_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else {
    return (globalSettings.useLargeFonts) ? 31 : 26;
  }
}

static void WeekNumber_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  int yTextPosition = SidebarWidgets_fixedHeight ? yPosition + 6 : yPosition - 4;
  yTextPosition = globalSettings.useLargeFonts ? yTextPosition - 2 : yTextPosition;

  // note that it draws "above" the y position to correct for
  // the vertical padding
  graphics_draw_text(ctx,
                     wordForWeek[globalSettings.languageId],
                     currentSidebarSmallFont,
                     GRect(xPosition - 4 + SidebarWidgets_xOffset, yTextPosition, 38, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  yTextPosition = SidebarWidgets_fixedHeight ? yPosition + 15 : yPosition;
  yTextPosition = globalSettings.useLargeFonts ? yTextPosition + 6 : yTextPosition + 9;

  graphics_draw_text(ctx,
                     time_date_currentWeekNum,
                     currentSidebarFont,
                     GRect(xPosition + SidebarWidgets_xOffset, yTextPosition, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

/***** Seconds Widget *****/

static int Seconds_getHeight(void) {
  return 14;
}

static void Seconds_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  graphics_draw_text(ctx,
                     time_date_currentSecondsNum,
                     lgSidebarFont,
                     GRect(xPosition + SidebarWidgets_xOffset, yPosition - 10, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

/***** Weather Forecast Widget *****/

static int WeatherForecast_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else {
    return (globalSettings.useLargeFonts) ? 63 : 60;
  }
}

static void WeatherForecast_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  //srand(time(NULL));
  //Weather_setForecastCondition(rand() % 12);

  if(Weather_forecastWeatherIcon) {
    util_image_draw(ctx, Weather_forecastWeatherIcon, xPosition + 3 + SidebarWidgets_xOffset, yPosition + 1);
  }

  // draw weather data only if it has been set
  if(Weather_weatherForecast.highTemp != INT32_MIN) {

    int highTemp = Weather_weatherForecast.highTemp;
    int lowTemp  = Weather_weatherForecast.lowTemp;

    if(!globalSettings.useMetric) {
      highTemp = roundf(highTemp * 1.8f + 32);
      lowTemp  = roundf(lowTemp * 1.8f + 32);
    }

    char tempString[8];

    graphics_context_set_fill_color(ctx, globalSettings.sidebarTextColor);

    // in large font mode, omit the degree symbol and move the text
    if(!globalSettings.useLargeFonts) {
      snprintf(tempString, sizeof(tempString), " %d°", highTemp);

      graphics_draw_text(ctx,
                         tempString,
                         smSidebarFont,
                         GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + 23, 38, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);

      graphics_fill_rect(ctx, GRect(xPosition + 6 + SidebarWidgets_xOffset, 8 + yPosition + 30, 18, 1), 0, GCornerNone);

      snprintf(tempString, sizeof(tempString), " %d°", lowTemp);

      graphics_draw_text(ctx,
                         tempString,
                         smSidebarFont,
                         GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + 35, 38, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);
    } else {
      snprintf(tempString, sizeof(tempString), "%d", highTemp);

      graphics_draw_text(ctx,
                         tempString,
                         smSidebarFont,
                         GRect(xPosition + SidebarWidgets_xOffset, yPosition + 23, 30, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);

      graphics_fill_rect(ctx, GRect(xPosition + 6 + SidebarWidgets_xOffset, 8 + yPosition + 30, 18, 1), 0, GCornerNone);

      snprintf(tempString, sizeof(tempString), "%d", lowTemp);

      graphics_draw_text(ctx,
                         tempString,
                         smSidebarFont,
                         GRect(xPosition + SidebarWidgets_xOffset, yPosition + 35, 30, 20),
                         GTextOverflowModeFill,
                         GTextAlignmentCenter,
                         NULL);
    }
  } else {
    // if the weather data isn't set, draw a loading indication
    graphics_draw_text(ctx,
                       "...",
                       currentSidebarFont,
                       GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition, 38, 20),
                       GTextOverflowModeFill,
                       GTextAlignmentCenter,
                       NULL);
  }
}

/***** Alternate Time Zone Widget *****/

static int AltTime_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else {
    return (globalSettings.useLargeFonts) ? 31 : 26;
  }
}

static void AltTime_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  int yMod = SidebarWidgets_fixedHeight ? 6 : - 5;
  yMod = globalSettings.useLargeFonts ? yMod - 2 : yMod;

  graphics_draw_text(ctx,
                     globalSettings.altclockName,
                     currentSidebarSmallFont,
                     GRect(xPosition + SidebarWidgets_xOffset, yPosition + yMod, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  yMod = SidebarWidgets_fixedHeight ? 16 : 0;
  yMod = (globalSettings.useLargeFonts) ? yMod + 5 : yMod + 8;

  graphics_draw_text(ctx,
                     time_date_altClock,
                     currentSidebarFont,
                     GRect(xPosition - 1 + SidebarWidgets_xOffset, yPosition + yMod, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

/***** Health Widget *****/

#ifdef PBL_HEALTH
static int Health_getHeight(void) {
  if(Health_sleepingToBeDisplayed()) {
    return Sleep_getHeight();
  } else {
    return Steps_getHeight();
  }
}

static void Health_draw(GContext* ctx, int xPosition, int yPosition) {
  // check if we're showing the sleep data or step data

  if(Health_sleepingToBeDisplayed()) {
    Sleep_draw(ctx, xPosition, yPosition);
  } else {
    Steps_draw(ctx, xPosition, yPosition);
  }
}

static int Sleep_getHeight(void) {
  return 44;
}

static void Sleep_draw(GContext* ctx, int xPosition, int yPosition) {
  if(sleepImage) {
    util_image_draw(ctx, sleepImage, xPosition + 3 + SidebarWidgets_xOffset, yPosition - 7);
  }

  // get sleep in seconds
  HealthValue sleep_seconds = globalSettings.healthUseRestfulSleep ? Health_getRestfulSleepSeconds() : Health_getSleepSeconds();

  char hours_text[4];
  char minutes_text[4];

  seconds_to_minutes_hours_text(sleep_seconds, hours_text, minutes_text);

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);
  graphics_draw_text(ctx,
                     hours_text,
                     mdSidebarFont,
                     GRect(xPosition - 2 + SidebarWidgets_xOffset, yPosition + 14, 34, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  graphics_draw_text(ctx,
                     minutes_text,
                     smSidebarFont,
                     GRect(xPosition - 2 + SidebarWidgets_xOffset, yPosition + 30, 34, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

}

static int Steps_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else {
    return 32;
  }
}

static void Steps_draw(GContext* ctx, int xPosition, int yPosition) {
  if(stepsImage) {
    int yIconPosition = SidebarWidgets_fixedHeight ? yPosition + 2 : yPosition - 7;

    util_image_draw(ctx, stepsImage, xPosition + 3 + SidebarWidgets_xOffset, yIconPosition);
  }

  char steps_text[8];
  bool use_small_font = false;

  if(globalSettings.healthActivityDisplay == DISTANCE) {
    HealthValue distance = Health_getDistanceWalked();
    MeasurementSystem unit_system = health_service_get_measurement_system_for_display(HealthMetricWalkedDistanceMeters);

    // format distance string
    if(unit_system == MeasurementSystemMetric) {
      distance_to_metric_text(distance, steps_text);

      if(distance > 9999) {
        use_small_font = true;
      }
    } else {
      distance_to_imperial_text(distance, steps_text);
    }
  } else if(globalSettings.healthActivityDisplay == STEPS) {
    HealthValue steps = Health_getSteps();

    steps_to_text(steps, steps_text);
  } else if(globalSettings.healthActivityDisplay == DURATION) {
    HealthValue active_seconds = Health_getActiveSeconds();

    seconds_to_text(active_seconds, steps_text);
  } else { // KCALORIES
    HealthValue active_kcalories = Health_getActiveKCalories();

    kCalories_to_text(active_kcalories, steps_text);
  }

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  int yTextPosition = yPosition;

  if(SidebarWidgets_fixedHeight) {
    if(globalSettings.useLargeFonts) {
      yTextPosition += 26;
    } else {
      yTextPosition += 24;
    }
  } else {
    yTextPosition += 13;
  }

  graphics_draw_text(ctx,
                     steps_text,
                     (use_small_font) ? smSidebarFont : mdSidebarFont,
                     GRect(xPosition - 2 + SidebarWidgets_xOffset, yTextPosition, 35, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

static int HeartRate_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else if(globalSettings.useLargeFonts) {
    return 40;
  } else {
    return 38;
  }
}

static void HeartRate_draw(GContext* ctx, int xPosition, int yPosition) {
  if(heartImage) {
    int yIconPosition = SidebarWidgets_fixedHeight ? yPosition + 3 : yPosition;

    util_image_draw(ctx, heartImage, xPosition + 3 + SidebarWidgets_xOffset, yIconPosition);
  }

  int yOffset = globalSettings.useLargeFonts ? 17 : 20;

  if(SidebarWidgets_fixedHeight) {
    yOffset += 4;
  }

  HealthValue heart_rate = Health_getHeartRate();
  char heart_rate_text[8];

  snprintf(heart_rate_text, sizeof(heart_rate_text), "%li", heart_rate);

  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);
  graphics_draw_text(ctx,
                     heart_rate_text,
                     currentSidebarFont,
                     GRect(xPosition - 5 + SidebarWidgets_xOffset, yPosition + yOffset, 38, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

#endif

/***** Beats (Swatch Internet Time) widget *****/

static int Beats_getHeight(void) {
  if(SidebarWidgets_fixedHeight) {
    return FIXED_WIDGET_HEIGHT;
  } else {
    return (globalSettings.useLargeFonts) ? 31 : 26;
  }
}

static void Beats_draw(GContext* ctx, int xPosition, int yPosition) {
  graphics_context_set_text_color(ctx, globalSettings.sidebarTextColor);

  int yMod = SidebarWidgets_fixedHeight ? 6 : - 5;
  yMod = globalSettings.useLargeFonts ? yMod - 2 : yMod;
  graphics_draw_text(ctx,
                     "@",
                     currentSidebarSmallFont,
                     GRect(xPosition + SidebarWidgets_xOffset, yPosition + yMod, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  yMod = SidebarWidgets_fixedHeight ? 16 : 0;
  yMod = (globalSettings.useLargeFonts) ? yMod + 5 : yMod + 8;

  graphics_draw_text(ctx,
                     time_date_currentBeats,
                     currentSidebarFont,
                     GRect(xPosition + SidebarWidgets_xOffset, yPosition + yMod, 30, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}
