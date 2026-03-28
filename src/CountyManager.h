//
// Created by Val Wehnau on 06/03/2026.
//

#ifndef COUNTRYMANAGER_H
#define COUNTRYMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <chrono>


using std::string;
using std::vector;
using std::unordered_map;

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
    unordered_map<string, County> _counties;

    string _lastFips;
    County* _lastCounty = nullptr;

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
    County& operator[](string fips);


    /**
    * Checks if a county exists in the map based on its fips code.
    * @param fips The 5-digit fips string to search for.
    * @return true if the fips exists, false otherwise.
    */
    bool hasCounty(const std::string& fips) const;




    using const_iterator = unordered_map<string, County>::const_iterator;
    const_iterator begin() const { return _counties.begin(); }
    const_iterator end() const { return _counties.end(); }



    static int daysSinceEpoch(int y, int m, int d);

    /**
     * converts a calendar date into a week index (starting 01/22/2020).
     * returns the floor.
     * @return index 0-172 or -1 if out of range.
     */
    static int getClosestWeekBefore(int year, int month, int day);

    /**
     * converts a calendar date into a week index (starting 01/22/2020).
     * returns the ceiling.
     * @return index 0-172 or -1 if out of range.
     */
    static int getClosestWeekAfter(int year, int month, int day);


    /**
     * populates a vector with cumulative data for every county across a date range.
     * @param start_year, start_month, start_day beginning of the range.
     * @param end_year, end_month, end_day end of the range.
     * @param outputvector stores tuples of {cases_per_capita, County*, dummy_index}
     * @return true if the date range is valid and data was processed, false otherwise.
     */
    bool getFormatedDataCounty(int start_year,int start_month, int start_day, int end_year, int end_month, int end_day,vector<std::tuple<float,CountyManager::County*,int>> &outputvector);

    /**
    * populates a vector with individual weekly data points for every county in a range.
    * @param start_year, start_month, start_day beginning of the range.
    * @param end_year, end_month, end_day end of the range.
    * @param outputvector stores tuples of {cases_per_capita, County*, week_index}
    * @return true if the date range is valid and data was processed, false otherwise.
    */
    bool getFormatedDataWeek(int start_year, int start_month, int start_day,
    int end_year, int end_month, int end_day, vector<std::tuple<float, County*,int>>& outputvector);

    /**
     * returns the full name of the county ("County Name, State")
     * given its 5-digit fips code.
     * @param fips The 5-digit fips string.
     * @return a string containing "Name, State" or "" if not found.
     */
    string getFullCountyName(const std::string& fips) const;


    /**
     * searches for a county by name and state and returns its fips code.
     * @param name The name of the county ("Barbour County").
     * @param state The name of the state ("AL").
     * @return The 5-digit fips string, or an empty string if no match is found.
     */
    string getFipsByName(const std::string& name, const std::string& state) const;

    /**
    * calculates total cases for a county between two dates.
    * Returns -1 if dates are out of bounds.
    */
    static int getTotalCasesByDate(const County& county, int start_y, int start_m, int start_d, int end_y, int end_m, int end_d);

    /**
    * sums cases in the [start_index, end_index] range (inclusive).
    */
    static int getTotalCasesByIndex(const County& county, int start_index, int end_index);


    /**
     * Converts a week index back into a YYYY-MM-DD string.
     * @param index weeks since 01/22/2020.
     * @return "YYYY-MM-DD"
     */
    static string getDateFromIndex(int index);
};



#endif //COUNTRYMANAGER_H
