#pragma once

#include <chrono>
#include <conio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <string>
#include <vector>

static const std::string kCoordsToken = "+WbTUl8YMswC7S1LqwIr9w==QrfceQJhpBv442Ze";
static const int8_t kESC = 27;
static const int8_t kHoursToTimeShift = 6;
static const int8_t kHoursInaDay = 24;

enum Days_to_Show {
  one = 1,
  three = 3,
  seven = 7,
  fourthteen = 14,
  sixteen = 16,
};

Days_to_Show& operator++(Days_to_Show& d) {
  switch(d) {
    case one:
      d = three;
      break;
    case three:
      d = seven;
      break;
    case seven:
      d = fourthteen;
      break;
    case fourthteen:
      d = sixteen;
      break;
    case sixteen:
      break;
    default:
      break;
  }
  return d;
}

Days_to_Show& operator--(Days_to_Show& d) {
  switch(d) {
    case one:
      break;
    case three:
      d = one;
      break;
    case seven:
      d = three;
      break;
    case fourthteen:
      d = seven;
      break;
    case sixteen:
      d = fourthteen;
    default:
      break;
  }
  return d;
}

struct Coordinates {
  double latitude = 0;
  double longitude = 0;
  friend std::ostream& operator<<(std::ostream& ostream, const Coordinates& coords) {
    ostream << "(Latitude, Longitude): (" << coords.latitude << ' ';
    ostream << coords.longitude << ")\n";
    return ostream;
  }
};

std::string getWeekday(const std::string& dateStr) {
  std::tm timeinfo = {};
  std::istringstream ss(dateStr);
  ss >> std::get_time(&timeinfo, "%Y-%m-%d");
  std::mktime(&timeinfo);
  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%A", &timeinfo);
  return buffer;
}

std::string getMonthAndDay(const std::string& dateStr) {
  std::tm time = {};
  std::istringstream iss(dateStr);
  iss >> std::get_time(&time, "%Y-%m-%d");
  char buffer[12];
  std::strftime(buffer, 12, "%B %dth", &time);
  return buffer;
}

struct WMO {
  WMO() {
    interpretation_codes[0] = "Clear sky";
    interpretation_codes[1] = "Mainly clear";
    interpretation_codes[2] = "Partly cloudy";
    interpretation_codes[3] = "Overcast";
    interpretation_codes[45] = "Fog and depositing";
    interpretation_codes[48] = "Rime fog";
    interpretation_codes[51] = "Drizzle (light)";
    interpretation_codes[53] = "Drizzle (moderate)";
    interpretation_codes[55] = "Drizzle (dense intensity)";
    interpretation_codes[56] = "Freezing Drizzle (light)";
    interpretation_codes[57] = "Freezing Drizzle (dense intensity)";
    interpretation_codes[61] = "Rain (slight)";
    interpretation_codes[63] = "Rain (moderate)";
    interpretation_codes[65] = "Rain (heavy intensity)";
    interpretation_codes[66] = "Freezing Rain (light)";
    interpretation_codes[67] = "Freezing Rain (heavy intensity)";
    interpretation_codes[71] = "Snow fall (slight)";
    interpretation_codes[73] = "Snow fall (moderate)";
    interpretation_codes[75] = "Snow fall (heavy intensity)";
    interpretation_codes[77] = "Snow grains";
    interpretation_codes[80] = "Rain showers (slight)";
    interpretation_codes[81] = "Rain showers (moderate)";
    interpretation_codes[82] = "Rain showers (violent)";
    interpretation_codes[85] = "Snow showers (slight)";
    interpretation_codes[86] = "Snow showers (heavy)";
    interpretation_codes[95] = "Thunderstorm (slight or moderate)";
    interpretation_codes[96] = "Thunderstorm (slight)";
    interpretation_codes[99] = "Thunderstorm (heavy hail)";
  }
  std::map<int, std::string> interpretation_codes;
};