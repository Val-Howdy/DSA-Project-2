#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include "CountyManager.h"
#include <catch2/catch_approx.hpp>

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
	SECTION("getFullCountyName") {

		string fullName = manager.getFullCountyName("01021");


		REQUIRE_FALSE(fullName.empty());
		REQUIRE(fullName.find(", ") != string::npos);
		REQUIRE(fullName == "Chilton County, AL");


		REQUIRE(manager.getFullCountyName("99999") == "");
	}
	SECTION("getFipsByName")
	{
		REQUIRE(manager.getFipsByName("Chilton County", "AL")=="01021");


		if (manager.hasCounty("01021")) {
			string name = manager["01021"]._name;
			string state = manager["01021"]._state;
			REQUIRE(manager.getFipsByName(name, state) == "01021");
		}


		REQUIRE(manager.getFipsByName("NonExistent", "ZZ") == "");


		REQUIRE(manager.getFipsByName("Chilton", "AL") == "");
	}

}


//this is a general test for any test that wants to look at all of the data
TEST_CASE("full data import tests", "[Data Import]")
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
	SECTION("daysSinceEpoch")
	{
		int day1 = CountyManager::daysSinceEpoch(2020, 1, 1);
		int day2 = CountyManager::daysSinceEpoch(2020, 1, 2);
		REQUIRE(day2 - day1 == 1);

		// Test Leap Year
		int feb28 = CountyManager::daysSinceEpoch(2020, 2, 28);
		int march1 = CountyManager::daysSinceEpoch(2020, 3, 1);
		REQUIRE(march1 - feb28 == 2);

		// Test non-Leap Year
		int feb28_21 = CountyManager::daysSinceEpoch(2021, 2, 28);
		int march1_21 = CountyManager::daysSinceEpoch(2021, 3, 1);
		REQUIRE(march1_21 - feb28_21 == 1);
	}
	SECTION("getClosestWeekBefore")
	{
		// Exact start date
		REQUIRE(CountyManager::getClosestWeekBefore(2020, 1, 22)== 0);
		// One day before start
		REQUIRE(CountyManager::getClosestWeekBefore(2020, 1, 21) == -1);
		// 6 days after start
		REQUIRE(CountyManager::getClosestWeekBefore(2020, 1, 28)== 0);
		// 7 days after start
		REQUIRE(CountyManager::getClosestWeekBefore(2020, 1, 29)== 1);
		// future (cap at 172)
		REQUIRE(CountyManager::getClosestWeekBefore(2030, 1, 1)== -1);
	}
	SECTION("getClosestWeekAfter")
	{
		// Exact start date
		REQUIRE(CountyManager::getClosestWeekAfter(2020, 1, 22)== 0);
		// One day after start
		REQUIRE(CountyManager::getClosestWeekAfter(2020, 1, 23)== 1);
		// Exactly at the end of the data
		REQUIRE(CountyManager::getClosestWeekAfter(2023, 5, 10)== 172);
		// Past the end of the data
		REQUIRE(CountyManager::getClosestWeekAfter(2023, 5, 11)== -1);
	}

	SECTION("getDateFromIndex")
	{
		REQUIRE(CountyManager::getDateFromIndex(0) == "2020-01-22");
		REQUIRE(CountyManager::getDateFromIndex(1) == "2020-01-29");
		//leap year
		REQUIRE(CountyManager::getDateFromIndex(10) == "2020-04-01");
		REQUIRE(CountyManager::getDateFromIndex(52) == "2021-01-20");
	}
}


TEST_CASE("CountyManager: getFormatedData error handling", "[Data Export]") {
	CountyManager manager("data/test_Covid_data.csv", "data/test_Population_data.csv");
	vector<tuple<float, CountyManager::County*,int>> output;

	SECTION("Invalid Range: End before Start") {
		// Start: 2021-01-01, End: 2020-01-01
		bool success = manager.getFormatedDataCounty(2021, 1, 1, 2020, 1, 1, output);
		REQUIRE_FALSE(success);
	}

	SECTION("Out of Bounds: Date before dataset start") {
		// Dataset starts 2020-01-22
		bool success = manager.getFormatedDataCounty(2019, 12, 31, 2020, 1, 1, output);
		REQUIRE_FALSE(success);
	}

	SECTION("Out of Bounds: Date after dataset end") {
		// Dataset ends 2023-05-10
		bool success = manager.getFormatedDataCounty(2023, 6, 1, 2023, 7, 1, output);
		REQUIRE_FALSE(success);
	}
}

TEST_CASE("Full data Export tests", "[Data Export]") {
	CountyManager manager("data/Covid_data.csv", "data/Population_data.csv");
	vector<tuple<float, CountyManager::County*,int>> output;

	// First two weeks of the dataset (Jan 22, 2020 to Feb 5, 2020)
	bool success = manager.getFormatedDataCounty(2020, 1, 22, 2020, 2, 5, output);

	REQUIRE(success);
	REQUIRE_FALSE(output.empty());

	SECTION("verify specific County") {
		string targetFips = "01021";
		auto it = find_if(output.begin(), output.end(),
			[&](tuple<float, CountyManager::County*,int>& p) { return get<1>(p)== &manager[targetFips]; });

		if (it != output.end()) {
			auto county = manager[targetFips];
			int expected_cases = county._weeks[0]._cases + county._weeks[1]._cases + county._weeks[2]._cases;
			float expected_per_capita = (float)expected_cases / county._population;

			REQUIRE(get<0>(*it) == Catch::Approx(expected_per_capita));
		}
	}

	SECTION("getFormatedDataWeek") {
		vector<tuple<float, CountyManager::County*, int>> weekOutput;
		bool success = manager.getFormatedDataWeek(2020, 1, 22, 2020, 2, 5, weekOutput);

		REQUIRE(success);
		REQUIRE_FALSE(weekOutput.empty());

		string targetFips = "01021";
		auto& targetCounty = manager[targetFips];


		vector<tuple<float, CountyManager::County*, int>> targetCountyResults;
		for (const auto& item : weekOutput) {
			if (std::get<1>(item) == &targetCounty) {
				targetCountyResults.push_back(item);
			}
		}


		REQUIRE(targetCountyResults.size() == 3);


		auto [perCapita, countyPtr, weekindex] = targetCountyResults[1];

		int actualCases = targetCounty._weeks[1]._cases;
		float expectedPerCapita = static_cast<float>(actualCases) / targetCounty._population;

		REQUIRE(countyPtr == &targetCounty);
		REQUIRE(weekindex == 1);
		REQUIRE(perCapita == Catch::Approx(expectedPerCapita));
	}
}


