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

    County* currentCounty = nullptr;
    if (fips == _lastFips && _lastCounty != nullptr) {
        cleanGetline(ss);
        cleanGetline(ss);
        currentCounty = _lastCounty;
    } else
    {
        std::string name = cleanGetline(ss);
        std::string state = cleanGetline(ss);
        auto [it, inserted] = _counties.try_emplace(fips, name, state);
        currentCounty = &it->second;
        _lastFips = fips;
        _lastCounty = currentCounty;
    }
    currentCounty->addData(ss);
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


CountyManager::County& CountyManager::operator[](string fips)
{
    return _counties.at(fips);
}

bool CountyManager::hasCounty(const std::string& fips) const
{
    return _counties.find(fips) != _counties.end();
}

/**
 * helper function to calculate total days since year 0 while handling leap years.
 */
int CountyManager::daysSinceEpoch(int y, int m, int d)
{
    if (m < 3) {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + d - 306;
}

int CountyManager::getClosestWeekBefore(int year, int month, int day)
{
    int startDays = daysSinceEpoch(2020, 1, 22);
    int endDays   = daysSinceEpoch(2023, 5, 10);
    int targetDays = daysSinceEpoch(year, month, day);

    if (targetDays < startDays || targetDays > endDays) {
        return -1;
    }

    int d = targetDays - startDays;
    return d / 7;
}

int CountyManager::getClosestWeekAfter(int year, int month, int day)
{
    int startDays = daysSinceEpoch(2020, 1, 22);
    int endDays   = daysSinceEpoch(2023, 5, 10);
    int targetDays = daysSinceEpoch(year, month, day);

    if (targetDays < startDays || targetDays > endDays) {
        return -1;
    }

    int d = targetDays - startDays;
    int index = (d + 6) / 7;

    if (index > 172) {
        return 172;
    }

    return index;
}

bool CountyManager::getFormatedDataCounty(int start_year, int start_month, int start_day,
    int end_year, int end_month, int end_day, vector<std::pair<float, County*>>& outputvector)
{
    for (auto & pair : _counties)
    {
        County& county = pair.second;
        int total_cases = getTotalCasesByDate(county, start_year, start_month, start_day, end_year, end_month, end_day);
        if (total_cases == -1) {
            return false;
        }
        float cases_per_capita = 0.0f;
        if (county._population > 0) {
            cases_per_capita = static_cast<float>(total_cases) / county._population;
        }

        outputvector.emplace_back(cases_per_capita, &pair.second);
    }
    return true;

}

bool CountyManager::getFormatedDataWeek(int start_year, int start_month, int start_day,
    int end_year, int end_month, int end_day, vector<std::tuple<float, County*,int>>& outputvector)
{
    int start_index = getClosestWeekBefore(start_year, start_month, start_day);
    if (start_index == -1) return false;
    int end_index = getClosestWeekAfter(end_year, end_month, end_day);
    if (end_index == -1) return false;
    if (end_index < start_index) return false;

    for (auto & pair : _counties)
    {
        County& county = pair.second;
        for (int i = start_index; i <= end_index; ++i)
        {
            float cases_per_capita = static_cast<float>(county._weeks[i]._cases) / county._population;
            outputvector.emplace_back(cases_per_capita, &county, i);
        }
    }
    return true;

}

string CountyManager::getFullCountyName(const string& fips) const
{
    auto it = _counties.find(fips);

    if (it != _counties.end()) {
        return it->second._name + ", " + it->second._state;
    }
    return "";

}

string CountyManager::getFipsByName(const string& name, const string& state) const
{
    for (auto it = _counties.begin(); it != _counties.end(); ++it)
    {
        if (it->second._name == name && it->second._state == state)
        {
            return it->first;
        }
    }
    return "";
}

int CountyManager::getTotalCasesByDate(const County& county, int start_y, int start_m, int start_d, int end_y,
    int end_m, int end_d)
{
    int start = getClosestWeekBefore(start_y, start_m, start_d);
    int end = getClosestWeekAfter(end_y, end_m, end_d);

    if (start == -1 || end == -1 || end < start) {
        return -1;
    }

    return getTotalCasesByIndex(county, start, end);
}


int CountyManager::getTotalCasesByIndex(const County& county, int start_index, int end_index)
{
    int total = 0;

    for (int i = start_index; i <= end_index; ++i) {
        total += county._weeks[i]._cases;
    }

    return total;
}

string CountyManager::getDateFromIndex(int index)
{
    using namespace std::chrono;

    year_month_day startDate{year{2020}, month{1}, day{22}};

    sys_days calculatedDate = sys_days{startDate} + days{index * 7};

    year_month_day targetDate{calculatedDate};

    return std::format("{:%F}", targetDate);
}






