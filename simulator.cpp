#include <vector>
#include <iomanip>
#include <iostream>
#include <string>
#include <math.h>
#include <getopt.h>
#include <map>
#include "center.h"
#include "csvstream.h"
#include "fetch.c"

using namespace std;

class simulator {
private:
  int num_countries = 10, frequency = 30, time = 50, current_time = 1, total_infections = 0, total_deaths = 0;
  vector<int> active_cases;
  vector<int> new_deaths;
  vector<int> cases_last_cycle;
  vector<float> vaccinated_ratio;
  const vector<float> r0s = {0.8, 1.5, 1.2, 0.84, 1.3, 0.93, 1.3, 1.1, 1, 1.1};
  const vector<float> hospital_availability = {4.4, 0.53, 2.8, 1.2, 0.6, 0.8, 2.3, 0.8, 8.05, 1.38};
  const vector<int> num_ECMOs = {400, 0, 264, 0, 0, 21, 0, 0, 141, 0};
  const vector<int> population_densities = {153, 464, 36, 151, 287, 226, 25, 265, 9, 66};
  const vector<float> elder_ratios = {0.119, 0.061, 0.16, 0.056, 0.043, 0.085, 0.027, 0.051, 0.146, 0.072};
  const vector<float> mask_enforcements = {0.25, 0.75, 0.5, 0.75, 0.75, 0.5, 0.75, 0.25, 0.5, 0.5};
  const vector<string> country_names = {"China", "India", "USA", "Indonesia", "Pakistan", "Brazil", "Nigeria", "Bangladesh", "Russia", "Mexico"};

  void get_active_cases() {
    system("python scraper.py");
    csvstream csvin("active_cases.csv");
    map<string, string> row;
    while (csvin >> row) {
      string country_name = row["Country"];
      auto it = find(country_names.begin(), country_names.end(), country_name);
      if (it != country_names.end()) {
        string number = "";
        for (char chara : row["ActiveCases"])
          if (chara != ',') number += chara;
        active_cases[it - country_names.begin()] = stoi(number);
      }
    }
  }

  void get_vaccination() {
    long long int tp = 0;
    int n = fetchData(&tp);
    struct Data* datas = (struct Data*) *((long long int*)tp);
    vaccinated_ratio[0] = atof(datas[0].per) / 200;
    vaccinated_ratio[1] = atof(datas[1].per) / 200;
    vaccinated_ratio[2] = atof(datas[2].per) / 200;
    vaccinated_ratio[3] = atof(datas[9].per) / 200;
    vaccinated_ratio[4] = atof(datas[15].per) / 200;
    vaccinated_ratio[5] = atof(datas[3].per) / 200;
    vaccinated_ratio[6] = atof(datas[68].per) / 200;
    vaccinated_ratio[7] = atof(datas[30].per) / 200;
    vaccinated_ratio[8] = atof(datas[12].per) / 200;
    vaccinated_ratio[9] = atof(datas[11].per) / 200;
  }

  void forward_day() {
    calcCures();
    calcDeaths();
    calcInfections();
    current_time++;
  }

  void calcCures() {
    for (int i = 0; i < num_countries; ++i) {
      active_cases[i] -= (active_cases[i] * hospital_availability[i] / 10 / 21 + active_cases[i] * (1 - hospital_availability[i] / 10) / 42) / 3;
    }
  }

  void calcInfections() {
    for (int i = 0; i < num_countries; ++i) {
      int new_cases = (active_cases[i] * r0s[i] * sqrt(sqrt(population_densities[i])) / 420 / mask_enforcements[i] / sqrt(vaccinated_ratio[i])) / 3;
      total_infections += new_cases;
      cases_last_cycle[i] += new_cases;
      active_cases[i] += new_cases;
    }
  }

  void calcDeaths() {
    for (int i = 0; i < num_countries; ++i) {
      int num_severe_cases = elder_ratios[i] * active_cases[i] * vaccinated_ratio[i] * 0.07 + elder_ratios[i] * active_cases[i] * (1 - vaccinated_ratio[i]) * 0.2;
      int num_deaths;
      if (num_severe_cases - num_ECMOs[i] > 0) {
        num_deaths = ((num_severe_cases - num_ECMOs[i]) * 0.3433 + num_ECMOs[i] * 0.04460) / 30;
      } else {
        num_deaths = num_ECMOs[i] * 0.0760 / 30;
      }
      total_deaths += num_deaths;
      active_cases[i] -= num_deaths;
      new_deaths[i] += num_deaths;
    }
  }

  void start_output() {
    int total_cases = 0;
    for (int i = 0; i < num_countries; ++i)
      total_cases += active_cases[i];

    cout << "At the start, there is a total of " << total_cases << " active Covid-19 cases.\n"
         << "Below is a summary of the active cases listed by countries:\n";
    cout << "______________________________\n";
    cout << "|   COUNTRY   | ACTIVE CASES |\n";
    for (int i = 0; i < num_countries; ++i) {
      cout << "|" << setw(13) << centered(country_names[i])
      << "|" << setw(14) << centered(to_string(active_cases[i])) << "|\n";
    }
    cout << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n";
    cout << "Now, we will begin a simulation of Covid-19 spread over the top " << num_countries << " populated countries\n"
         << "with their most up-to-date Covid-19 prevention measures and vaccination record over " << time << " days.\n\n";
    cout << "----------------------------------- Simulation Started ----------------------------------\n";
  }

  void summary_output() {
    cout << "After " << current_time << " days has elapsed, we estimate a total of " << total_infections << " new infections and "
         << total_deaths << " new deaths.\nBelow is a summary of new infections and deaths listed by countries:\n";
    cout << "_____________________________________________________________\n";
    cout << "|   COUNTRY   | ACTIVE CASES | " << frequency << " DAYS TOTAL | TOTAL DEATHS |\n";
    for (int i = 0; i < num_countries; ++i) {
      cout << "|" << setw(13) << centered(country_names[i])
           << "|" << setw(14) << centered(to_string(active_cases[i]))
           << "|" << setw(15) << centered(to_string(cases_last_cycle[i]))
           << "|" << setw(14) << centered(to_string(new_deaths[i])) << "|\n";
    }
    cout << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n";
  }

public:
  void get_options(int argc, char **argv) {
    int option_index = 0, option;

    struct option LongOpts[] ={
            { "time"      , required_argument , nullptr, 't'},
            { "frequency" , required_argument , nullptr, 'f'},
            { nullptr     , 0        , nullptr , '\0' }};

    while ((option = getopt_long(argc, argv, "t:f:", LongOpts, &option_index)) != -1) {
      switch (option) {
        case 't':
          time = atoi(optarg);
          break;
        case 'f':
          frequency = atoi(optarg);
          break;
        default:
          break;
      } // end switch statement
    } // end while loop
  } // end get_options()

  void simulate() {
    new_deaths.resize(num_countries);
    cases_last_cycle.resize(num_countries);
    active_cases.resize(num_countries);
    vaccinated_ratio.resize(num_countries);
    get_active_cases();
    get_vaccination();

    start_output();
    while (current_time < time) {
      forward_day();
      if (current_time % frequency == 0) {
        summary_output();
        if (current_time != time) cases_last_cycle = vector<int>(10);
      }
    }
    cout << "----------------------------------- Simulation Finished ----------------------------------\n";
    summary_output();
  }
};

int main(int argc, char** argv) {
  iostream::sync_with_stdio(false);
  simulator covid;
  covid.get_options(argc, argv);
  covid.simulate();
}
