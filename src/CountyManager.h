//
// Created by Val Wehnau on 06/03/2026.
//

#ifndef COUNTRYMANAGER_H
#define COUNTRYMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>

using std::string;
using std::vector;
using std::map;

class CountyManager {

    public:
    struct Week
    {
        int _cases;
        int _deaths;
        float _rate;
    };
    struct Country
    {
        string _name;
        string _state;
        int _population;
        vector<Week> _weeks;
    };
    private:




};



#endif //COUNTRYMANAGER_H
