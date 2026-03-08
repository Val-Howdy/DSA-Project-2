#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include "CountyManager.h"
#include <filesystem>

using namespace std;


TEST_CASE("verify population import", "[Data Import]") {
	CountyManager manager("data/Covid_data.csv", "data/Population_data.csv");

	for (auto const& pair : manager) {
		INFO("Checking FIPS: " << pair.first << " (" << pair.second._name << ")");
		REQUIRE(pair.second._population > 0);
	}


}


