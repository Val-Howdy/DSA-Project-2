//
// Created by Val Wehnau on 06/03/2026.
//

#include "CountyManager.h"


// Week Constructor: simple assignment of data
CountyManager::Week::Week(int cases, int deaths, int cumulative_cases, int cumulative_deaths)
{
    _cases = cases;
    _deaths = deaths;
    _cumulative_cases = cumulative_cases;
    _cumulative_deaths = cumulative_deaths;
}
// County Constructor: initializes basic county metadata; population is set later by loadPopulation
CountyManager::County::County(string &name, string &state)
{
    _name = name;
    _state = state;
}

/**
 * Primary Constructor
 * 1. opens the main COVID data file.
 * 2. skips the header row.
 * 3. iterates through lines, converting each to a stringstream for the load() function.
 * 4. triggers the  population data load.
 */
CountyManager::CountyManager(string covidDataPath, string populationDataPath)
{
    std::ifstream file(covidDataPath);
    std::string line;
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
    }
    getline(file,line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        load(ss);
    }
    loadPopulation(populationDataPath);
}

/**
 * parses specific columns from a line of COVID data.
 * Skips state fips code and date and stores the remaining data in a week that is emplaced in the weeks vector .
 */
void CountyManager::County::addData(std::stringstream& ss)
{
    cleanGetline(ss); // skip state_fips
    cleanGetline(ss); // skip date

    std::string CC = cleanGetline(ss); // cumulative_cases
    std::string CD = cleanGetline(ss); // cumulative_deaths
    std::string C  = cleanGetline(ss); // New cases
    std::string D  = cleanGetline(ss); // New deaths

    _weeks.emplace_back(Week(stoi(C),stoi(D),stoi(CC),stoi(CD)));

}

/**
 * handles logic for loading into the map of counties.
 * searches the map for a fips code
 * if a fips code is not found, it creates a new County, adds it to the map and executes addData
 * otherwise, it just executes addData to add the data to the found fips code .
 */
void CountyManager::load(std::stringstream &ss)
{
    std::string fips = cleanGetline(ss);
    std::string name = cleanGetline(ss);
    std::string state = cleanGetline(ss);
    if (_counties.find(fips) != _counties.end())
    {
        //read data
        _counties.at(fips).addData(ss);

    } else
    {
        _counties.emplace(fips,County(name,state));
        _counties.at(fips).addData(ss);
    }
}

/**
 * wraps std::getline to also remove double quotes from fields
 */
std::string CountyManager::cleanGetline(std::stringstream& ss)
{
    std::string cell;
    if (std::getline(ss, cell, ',')) {
        cell.erase(std::remove(cell.begin(), cell.end(), '\"'), cell.end());
    }
    return cell;
}

/**
 * prints a formatted table of a county's data.
 */
void CountyManager::printCountyData(const std::string& fips) const
{
    auto it = _counties.find(fips);


    if (it == _counties.end()) {
        std::cout << "County with FIPS " << fips << " not found." << std::endl;
        return;
    }

    const County& county = it->second;

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << std::left << std::setw(15) << "COUNTY:" << county._name << ", " << county._state << std::endl;
    std::cout << std::left << std::setw(15) << "FIPS:" << fips << std::endl;
    std::cout << std::left << std::setw(15) << "POPULATION:" << county._population << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;

    const int w_wk = 4;
    const int w_val = 12;


    std::cout << std::left << std::setw(w_wk)  << "Wk"    << " | "
              << std::setw(w_val) << "New Cases"  << " | "
              << std::setw(w_val) << "New Deaths" << " | "
              << std::setw(w_val) << "Cum. Cases" << " | "
              << std::setw(w_val) << "Cum. Deaths" << std::endl;

    std::cout << std::string(w_wk + (w_val * 4) + 12, '-') << std::endl;


    for (size_t i = 0; i < county._weeks.size(); ++i) {
        const Week& w = county._weeks[i];

        std::cout << std::left  << std::setw(w_wk)  << (i + 1) << " | "
                  << std::right << std::setw(w_val) << w._cases << " | "
                  << std::setw(w_val) << w._deaths << " | "
                  << std::setw(w_val) << w._cumulative_cases << " | "
                  << std::setw(w_val) << w._cumulative_deaths << std::endl;
    }
}

/**
 * opens population CSV and updates the _population field for all matching counties in the map.
 */
void CountyManager::loadPopulation(string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open population file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string fips, population;

        if (std::getline(ss, fips, ',') && std::getline(ss, population, ',')) {

            auto it = _counties.find(fips);
            if (it != _counties.end()) {
                it->second._population = std::stoi(population);
            }
        }
    }
}

