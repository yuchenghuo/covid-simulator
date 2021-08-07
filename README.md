# Covid-19 Simulator

A CLI simulator that fetches live data about active cases and vaccination records in each country at the begin of the execution, combining with pre-existing data on population density, elder ratio, number of ECMOs, hospital and ICU capacity, and mask enforcement about each country, to establish a sophisticated model on predicting Covid-19 trends in the future.

# How to Use

On the command line:
```bash
$ git clone https://github.com/SuperStarOrange/covid-simulator.git
$ g++ -std=c++1z simulator.cpp -o simulator
$ ./simulator -t (the number of days) -f (reporting frequency)
```

Example:
```bash
$ ./simulator -t 120 -f 30
```
The program will report predicted trends about the pandemic over the next 120 days by every 30 days
