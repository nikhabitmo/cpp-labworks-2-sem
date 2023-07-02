#include "cpr/cpr.h"
#include "lib/forecast_weather.h"
#include "nlohmann/json.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

using json = nlohmann::json;
using namespace ftxui;

struct City {
  std::string name;
  std::string country;
  Coordinates coordinates;
  bool is_capital;
  json weather_forecast_data;
  friend std::ostream& operator<<(std::ostream& ostream, const City& city);
};

std::ostream& operator<<(std::ostream& ostream, const City& city) {
  ostream << "Weather forecast: " << '\n';
  ostream << "City: " << city.name << " | ";
  ostream << "Country: " << city.country << " | ";
  ostream << "Is capital: " << ((city.is_capital) ? "true" : "false") << '\n';
  ostream << city.coordinates << '\n';
  return ostream;
}

void formattedOutput(const City& city, const std::string& current_time, WMO& wmo) {
  system("cls");
  size_t index_current_time = 0;
  for (auto& day : city.weather_forecast_data["daily"]["time"]) {
    std::string_view current(current_time);
    size_t index_space = current_time.find(' ');
    if (day == current.substr(0, index_space)) {
      index_current_time = std::stoi(current_time.substr(current_time.find('T') + 1, 2));
    }
  };

  auto document = vbox({
                           hbox({
                                    text(city.country) | border,
                                    center(text(city.name + " | " + ((city.is_capital) ? "Capital | " : "") + "("
                                                    + std::to_string(city.coordinates.longitude) + ","
                                                    + std::to_string(city.coordinates.latitude) + ')')) | border
                                        | flex,
                                    text("GMT | " + current_time) | border
                                }) | color(Color::Green3Bis),
                           separator(),
                           hbox({
                                    text(L"Weather") | border,
                                    separator(),
                                    vbox({
                                             text(L"General: ") | bold,
                                             text(wmo.interpretation_codes[city.weather_forecast_data["daily"]["weathercode"][0]])
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Temperature(2m) \n") | bold,
                                             text("max: "
                                                      + city.weather_forecast_data["daily"]["temperature_2m_max"][0].dump()
                                                      + city.weather_forecast_data["daily_units"]["temperature_2m_max"].dump().substr(
                                                          1,
                                                          2)),
                                             text("min: "
                                                      + city.weather_forecast_data["daily"]["temperature_2m_min"][0].dump()
                                                      + city.weather_forecast_data["daily_units"]["temperature_2m_min"].dump().substr(
                                                          1,
                                                          2)),
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Relative Humidity(2m): \n") | bold,
                                             text(
                                                 city.weather_forecast_data["hourly"]["relativehumidity_2m"][index_current_time].dump()
                                                     + city.weather_forecast_data["hourly_units"]["relativehumidity_2m"].dump()[1])
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Dewpoint(2m): \n") | bold,
                                             text(
                                                 city.weather_forecast_data["hourly"]["dewpoint_2m"][index_current_time].dump()
                                                     + city.weather_forecast_data["hourly_units"]["dewpoint_2m"].dump().substr(
                                                         1,
                                                         2))
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Wind Direction(10m): \n") | bold,
                                             text(
                                                 city.weather_forecast_data["daily"]["winddirection_10m_dominant"][0].dump()
                                                     + city.weather_forecast_data["daily_units"]["winddirection_10m_dominant"].dump().substr(
                                                         1,
                                                         2))
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Wind Speed(max, 10m): \n") | bold,
                                             text(city.weather_forecast_data["daily"]["windspeed_10m_max"][0].dump()
                                                      + city.weather_forecast_data["daily_units"]["windspeed_10m_max"].dump().substr(
                                                          1,
                                                          4))
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Precipitation \n") | bold,
                                             text("Hours: "
                                                      + city.weather_forecast_data["daily"]["precipitation_hours"][0].dump()
                                                      + city.weather_forecast_data["daily_units"]["precipitation_hours"].dump()[1]),
                                             text("Summary: "
                                                      + city.weather_forecast_data["daily"]["precipitation_sum"][0].dump()
                                                      + city.weather_forecast_data["daily_units"]["precipitation_sum"].dump().substr(
                                                          1,
                                                          2))
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Sunrise: \n") | bold,
                                             text(city.weather_forecast_data["daily"]["sunrise"][0].dump().substr(1,
                                                                                                                  city.weather_forecast_data["daily"]["sunset"][0].dump().size()
                                                                                                                      - 2))
                                         }),
                                    separator(),
                                    vbox({
                                             text(L"Sunset: \n") | bold,
                                             text(city.weather_forecast_data["daily"]["sunset"][0].dump().substr(1,
                                                                                                                 city.weather_forecast_data["daily"]["sunset"][0].dump().size()
                                                                                                                     - 2))
                                         })
                                }) | color(Color::GrayLight),
                           separator(),
                       });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  int change = 0;
  int ind = 0;
  for (auto& day : city.weather_forecast_data["daily"]["time"]) {
    int index_6am = kHoursToTimeShift;
    auto day_document = vbox({
                                 text("\n"),
                                 separator(),
                                 center(text(getMonthAndDay(day) + ", " + getWeekday(day))) | color(Color::Green3Bis)
                                     | bold,
                                 separator(),
                                 hbox({
                                          vbox({
                                                   separator(),
                                                   center(text("Morning: ")) | bold | color(Color::GrayLight),
                                                   text(wmo.interpretation_codes[city.weather_forecast_data["daily"]["weathercode"][ind]]),
                                                   text("Temperature: "
                                                            + city.weather_forecast_data["hourly"]["temperature_2m"][6
                                                                + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["temperature_2m"].dump().substr(
                                                                1,
                                                                3)),
                                                   text("Wind Speed: "
                                                            + city.weather_forecast_data["hourly"]["windspeed_10m"][
                                                                6 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["windspeed_10m"].dump().substr(
                                                                1,
                                                                4)),
                                                   text("Wind Direction: "
                                                            + city.weather_forecast_data["hourly"]["winddirection_10m"][
                                                                6 + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["winddirection_10m"].dump().substr(
                                                                1,
                                                                2)),
                                                   text("Precipitation : "
                                                            + city.weather_forecast_data["hourly"]["precipitation"][
                                                                6 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["precipitation"].dump().substr(
                                                                1,
                                                                2)),
                                                   separator(),
                                               }) | flex,
                                          separator(),
                                          vbox({
                                                   separator(),
                                                   center(text("Day: ")) | bold | color(Color::GrayLight),
                                                   text(wmo.interpretation_codes[city.weather_forecast_data["daily"]["weathercode"][ind]]),
                                                   text("Temperature: "
                                                            + city.weather_forecast_data["hourly"]["temperature_2m"][
                                                                12
                                                                    + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["temperature_2m"].dump().substr(
                                                                1,
                                                                3)),
                                                   text("Wind Speed: "
                                                            + city.weather_forecast_data["hourly"]["windspeed_10m"][
                                                                12 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["windspeed_10m"].dump().substr(
                                                                1,
                                                                4)),
                                                   text("Wind Direction: "
                                                            + city.weather_forecast_data["hourly"]["winddirection_10m"][
                                                                12 + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["winddirection_10m"].dump().substr(
                                                                1,
                                                                2)),
                                                   text("Precipitation : "
                                                            + city.weather_forecast_data["hourly"]["precipitation"][
                                                                12 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["precipitation"].dump().substr(
                                                                1,
                                                                2)),
                                                   separator(),
                                               }) | flex,
                                          separator(),
                                          vbox({
                                                   separator(),
                                                   center(text("Evening: ")) | bold | color(Color::GrayLight),
                                                   text(wmo.interpretation_codes[city.weather_forecast_data["daily"]["weathercode"][ind]]),
                                                   text("Temperature: "
                                                            + city.weather_forecast_data["hourly"]["temperature_2m"][
                                                                18
                                                                    + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["temperature_2m"].dump().substr(
                                                                1,
                                                                3)),
                                                   text("Wind Speed: "
                                                            + city.weather_forecast_data["hourly"]["windspeed_10m"][
                                                                18 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["windspeed_10m"].dump().substr(
                                                                1,
                                                                4)),
                                                   text("Wind Direction: "
                                                            + city.weather_forecast_data["hourly"]["winddirection_10m"][
                                                                18 + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["winddirection_10m"].dump().substr(
                                                                1,
                                                                2)),
                                                   text("Precipitation : "
                                                            + city.weather_forecast_data["hourly"]["precipitation"][
                                                                18 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["precipitation"].dump().substr(
                                                                1,
                                                                2)),
                                                   separator(),
                                               }) | flex,
                                          separator(),
                                          vbox({
                                                   separator(),
                                                   center(text("Night: ")) | bold | color(Color::GrayLight),
                                                   text(wmo.interpretation_codes[city.weather_forecast_data["daily"]["weathercode"][ind++]]),
                                                   text("Temperature: "
                                                            + city.weather_forecast_data["hourly"]["temperature_2m"][6
                                                                + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["temperature_2m"].dump().substr(
                                                                1,
                                                                3)),
                                                   text("Wind Speed: "
                                                            + city.weather_forecast_data["hourly"]["windspeed_10m"][
                                                                6 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["windspeed_10m"].dump().substr(
                                                                1,
                                                                4)),
                                                   text("Wind Direction: "
                                                            + city.weather_forecast_data["hourly"]["winddirection_10m"][
                                                                6 + change].dump()
                                                            + city.weather_forecast_data["hourly_units"]["winddirection_10m"].dump().substr(
                                                                1,
                                                                2)),
                                                   text("Precipitation : "
                                                            + city.weather_forecast_data["hourly"]["precipitation"][
                                                                6 + change].dump() + ' '
                                                            + city.weather_forecast_data["hourly_units"]["precipitation"].dump().substr(
                                                                1,
                                                                2)),
                                                   separator(),
                                               }) | flex,
                                          text("\n"),
                                          separator(),
                                      }),
                                 separator(),
                             });
    change += kHoursInaDay;
    auto t_screen = Screen::Create(Dimension::Full(), Dimension::Fit(day_document));
    Render(t_screen, day_document);
    t_screen.Print();
  }
}

void requestingWeatherForecast(City& city, const Days_to_Show& forecast_days) try {
  cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                             cpr::Parameters{{"latitude", std::to_string(city.coordinates.latitude)},
                                             {"longitude", std::to_string(city.coordinates.longitude)},
                                             {"daily",
                                              "weathercode,temperature_2m_max,temperature_2m_min,sunrise,sunset,precipitation_sum,"
                                              "rain_sum,snowfall_sum,precipitation_hours,windspeed_10m_max,winddirection_10m_dominant"},
                                             {"timezone", "auto"},
                                             {"forecast_days", std::to_string(forecast_days)},
                                             {"hourly",
                                              "temperature_2m,relativehumidity_2m,dewpoint_2m,precipitation,weathercode,windspeed_10m,winddirection_10m"}});
  if (r.status_code != 200) system("cls"), throw std::logic_error("Either bad request or response");
  city.weather_forecast_data = json::parse(r.text);
} catch (const std::exception& e) {
  std::cerr << e.what();
}

int main(int argc, char** argv) {

  WMO wmo;

  auto time_app_started_working = std::chrono::system_clock::now();
  auto duration = time_app_started_working.time_since_epoch();
  size_t seconds_time_app_started_working = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

  std::ifstream file("../config.json");
  json general_data = json::parse(file);
  json based_all_cities = json::array({*general_data.begin()});

  Days_to_Show forecast_days = general_data.at("forecast_days");

  int update_frequency = general_data.at("update_frequency");

  std::vector<City> all_cities;

  for (auto& city : based_all_cities[0]) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
                               cpr::Header{{"X-Api-Key", kCoordsToken}},
                               cpr::Parameters{{"name", city.at("name")}});
    json received_data = json::parse(r.text);
    City temp;
    temp.name = received_data[0].at("name");
    temp.country = received_data[0].at("country");
    temp.is_capital = received_data[0].at("is_capital");
    temp.coordinates.latitude = received_data[0].at("latitude");
    temp.coordinates.longitude = received_data[0].at("longitude");
    all_cities.push_back(temp);
  }

//  std::for_each(all_cities.begin(), all_cities.end(), [](const City& i) { std::cout << i << "\n"; });

  for (auto& city : all_cities) {
    requestingWeatherForecast(city, forecast_days);
  }

  size_t index = 0;
  size_t temp_store_to_seconds = 0;

  while (true) {

    auto current_time = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(current_time);
    std::tm* now_tm = std::localtime(&now_c);
    std::stringstream ss;
    ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    std::string current_data = ss.str();
    size_t seconds = std::mktime(now_tm);

    if (seconds - update_frequency >= seconds_time_app_started_working) {
      for (auto& city : all_cities) {
        temp_store_to_seconds = (seconds - update_frequency) - temp_store_to_seconds;
        temp_store_to_seconds += update_frequency;
        requestingWeatherForecast(city, forecast_days);
      }
    }

    char input_symbol = getch();

    if (input_symbol == kESC) {
      break;
    }

    switch (input_symbol) {
      case 'r':
        try { requestingWeatherForecast(all_cities[index], forecast_days); }
        catch (...) {
          system("cls");
          std::cerr << "Bad connection - try to reconnect, or push ESC to quit, or press 'r' to reload";
        }
        break;
      case 'n':index = (index + 1) % all_cities.size();
        break;
      case 'p':index = (index + all_cities.size() - 1) % all_cities.size();
        break;
      case '+':requestingWeatherForecast(all_cities[index], ++forecast_days);
        break;
      case '-':requestingWeatherForecast(all_cities[index], --forecast_days);
        break;
      default:break;
    }
    formattedOutput(all_cities[index], current_data, wmo);
  }

  return 0;
}