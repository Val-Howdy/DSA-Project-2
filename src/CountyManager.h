//
// Created by Val Wehnau on 06/03/2026.
//

#ifndef COUNTRYMANAGER_H
#define COUNTRYMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

using std::string;
using std::vector;
using std::map;

class CountyManager {

    public:
    // stores the covid data for each week
    struct Week
    {
        int _cases;
        int _deaths;
        int _cumulative_cases;
        int _cumulative_deaths;

        Week(int cases, int deaths, int cumulative_cases, int cumulative_deaths);
    };
    // represents each county, storing data and the weeks vector.
    struct County
    {
        string _name;
        string _state;
        int _population = -1; // -1 represents un assinged population
        vector<Week> _weeks; //list of weeks in order, indexed by weeks since 01/22/2020
        County(string &name, string &state);
        void addData(std::stringstream &ss); // parse a CSV line and adds it to the week vector
    };
    private:
    // county map. Key: fips code (5 digit string), Value: County obj
    map<string, County> _counties;

    public:

    /**
     * Constructor: initializesad the manager by loing both covid and Population CSVs.
     * @param covidDataPath path to the covid data CSV
     * @param populationDataPath path to the population data CSV
     */
    CountyManager(string covidDataPath = "data/Covid_data.csv",string populationDataPath = "data/Population_data.csv");

    /**
    * parses a single row from the covid data CSV
    * if it has a novel fips code it creates a new County, then it calls addData with the remainder of the CSV line
    * if the county already exists it calls addData with the remainder of the CSV line
    */
    void load(std::stringstream &ss);

    /**
    * helper to read a CSV cell and strip quotes. e.g "01005" into 01005
    */
    static std::string cleanGetline(std::stringstream& ss);

    /**
     * formats and prints all data at a given fips code to the console.
     */
    void printCountyData(const std::string& fips) const;

    /**
     * reads the population CSV and updates the _population field for existing counties.
     */
    void loadPopulation(string &filename);

    /**
     * overloaded subscript operator to access a County object by its fips code.
    * @param code The 5-digit fips code string.
    * @return A copy of the county object.
    * @throws std::out_of_range if the fips code is not found.
    */
    County operator[](string fips) const;


    /**
    * Checks if a county exists in the map based on its fips code.
    * @param fips The 5-digit fips string to search for.
    * @return true if the fips exists, false otherwise.
    */
    bool hasCounty(const std::string& fips) const;




    using const_iterator = map<string, County>::const_iterator;
    const_iterator begin() const { return _counties.begin(); }
    const_iterator end() const { return _counties.end(); }
};



#endif //COUNTRYMANAGER_H
