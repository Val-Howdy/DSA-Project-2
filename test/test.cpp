#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include "CountyManager.h"
#include <filesystem>

using namespace std;




TEST_CASE("CountyManager: Output methods", "[Output methods]") {
	CountyManager manager("data/test_Covid_data.csv", "data/test_Population_data.csv");

	SECTION("hasCounty")
	{
		REQUIRE(manager.hasCounty("01021") == true);
		REQUIRE(manager.hasCounty("99999") == false); // Non-existent code
	}
	SECTION("Operator[]")
	{
		auto county = manager["01021"];
		REQUIRE_FALSE(county._name.empty());
		REQUIRE_THROWS_AS(manager["99999"], std::out_of_range);
	}

}


//this is a general test for any test that wants to look at all of the data
TEST_CASE("full data tests", "[Data Import]")
{
	CountyManager manager("data/Covid_data.csv", "data/Population_data.csv");

	SECTION("misc")
	{
		for (auto const& pair : manager)
		{
			INFO("Checking FIPS: " << pair.first << " (" << pair.second._name << ")");
			REQUIRE(pair.second._weeks.size() > 0);
			REQUIRE(pair.second._weeks[0]._cases >= 0);
			REQUIRE(pair.second._weeks[0]._cumulative_cases >= pair.second._weeks[0]._cases);
		}
	}

	SECTION("full data population test")
	{
		for (auto const& pair : manager) {
			INFO("Checking FIPS: " << pair.first << " (" << pair.second._name << ")");
			REQUIRE(pair.second._population > 0);
		}
	}

}

TEST_CASE("CountyManager: Helper methods", "[Helper Methods]") {
	SECTION("cleanGetline") {
		std::stringstream ss("\"01005\",\"Barbour County\",\"AL\"");

		std::string cell1 = CountyManager::cleanGetline(ss);
		std::string cell2 = CountyManager::cleanGetline(ss);

		REQUIRE(cell1 == "01005");
		REQUIRE(cell2 == "Barbour County");
	}
}