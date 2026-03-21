#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include "CountyManager.h"

struct DummyMethodResultStruct {
	struct DummyDataResultStruct {
		int rank;
		std::string state;
		std::string county;
		int totalCases;
		int casesPerCapita;

		DummyDataResultStruct() {
			rank = -1;
			state = "Dummy";
			county = "Dummy";
			totalCases = -1;
			casesPerCapita = -1;
		}
	};

	float runtime;
	std::vector<DummyDataResultStruct> results;
};

void topKQuickselect(std::vector<std::pair<float, CountyManager::County*>>& data, int k)
{

}

void topKHeapInPlace(vector<std::pair<float, CountyManager::County*>>& data, int k)
{

}

bool isValidDate(int year, int month, int day){
	auto ymd = std::chrono::year_month_day(std::chrono::year(year), std::chrono::month(month), std::chrono::day(day));

	return ymd.ok();
}

void printSeparator() {
	std::cout << std::string(75, '-') << std::endl;
}

void printHeader(float runtime, const std::string& method, const std::string& startDate, const std::string& endDate, int k) {
	printSeparator();
	std::cout << "| Method: " << std::left << std::setw(64) << method << "|" << std::endl;
	printSeparator();
	std::cout << "| Start date: " << std::left << std::setw(60) << startDate << "|" << std::endl;
	printSeparator();
	std::cout << "| End date: " << std::left << std::setw(62) << endDate << "|" << std::endl;
	printSeparator();
	std::cout << "| k: " << std::left << std::setw(69) << k << "|" << std::endl;
	printSeparator();
	std::cout << "| Runtime: " << std::fixed << std::setprecision(4) << std::setw(63) << std::left << runtime << "|" << std::endl;
	printSeparator();
	std::cout << "| " << std::left << std::setw(5)  << "Rank" << "| " << std::setw(6)  << "State" << "| " << std::setw(20) << "County" << "| " << std::setw(16) << "Total New Cases" << "| " << std::setw(17) << "Cases per Capita" << "|" << std::endl;
	printSeparator();
}

void printResults(const std::vector<DummyMethodResultStruct::DummyDataResultStruct>& results, const float runtime, const std::string& method, const std::string& startDate, const std::string& endDate, int k) {
	printHeader(runtime, method, startDate, endDate, k);
	for (const auto& r : results) {
		std::cout << "| " << std::left << std::setw(5)  << r.rank << "| " << std::setw(6)  << r.state << "| " << std::setw(20) << r.county << "| " << std::setw(16) << r.totalCases << "| " << std::setw(17) << std::fixed << std::setprecision(6) << r.casesPerCapita << "|" << std::endl;
	}
	printSeparator();
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <method> <start_date> <end_date> <k>" << std::endl;
		std::cerr << "Methods: Quickselect or Heap" << std::endl;
		std::cerr << "Date format: YYYY-MM-DD" << std::endl;
		std::cerr << "Valid k is int greater than 0" << std::endl;
	} else {
		// Variables
		std::string method = argv[1];
		std::string startDate = argv[2];
		std::string endDate = argv[3];
		int startDateYear;
		int startDateMonth;
		int startDateDay;
		int endDateYear;
		int endDateMonth;
		int endDateDay;
		int k;

		// Input validation for method, dates, and k
		if (method != "Quickselect" && method != "Heap") {
			std::cerr << "Error: Invalid method" << std::endl;
		}
		try {
			if (startDate.size() == 10 && endDate.size() == 10 && startDate[4] == '-' && endDate[4] == '-' && startDate[7] == '-' && endDate[7] == '-') {
				startDateYear = std::stoi(startDate.substr(0, 4));
				startDateMonth = std::stoi(startDate.substr(5, 2));
				startDateDay = std::stoi(startDate.substr(8, 2));
				endDateYear = std::stoi(endDate.substr(0, 4));
				endDateMonth = std::stoi(endDate.substr(5, 2));
				endDateDay = std::stoi(endDate.substr(8, 2));

				if (!(isValidDate(startDateYear, startDateMonth, startDateDay) && isValidDate(endDateYear, endDateMonth, endDateDay))) {
					throw std::invalid_argument("Invalid date");
				}
			} else {
				throw std::invalid_argument("Invalid start/end date");
			}
		} catch (std::invalid_argument& e) {
			std::cerr << "Error: Invalid date format" << std::endl;
		}
		try {
			k = std::stoi(argv[4]);
			if (k < 1) {
				throw std::invalid_argument("k less than 1");
			}
		} catch (std::invalid_argument& e) {
			std::cerr << "Error: Invalid k" << std::endl;
		}


		if (method == "Quickselect") {
			// Call to quickselect function
		} else if (method == "Heap") {
			// Call to heap function
		}
		// Above will populate results when finished but dummy data for now below
		DummyMethodResultStruct dummyMethodResults;
		dummyMethodResults.runtime = -1;
		for (int i = 0; i < k; i++) {
			dummyMethodResults.results.emplace_back();
		}

		printResults(dummyMethodResults.results, dummyMethodResults.runtime, method, startDate, endDate, k);
	}

	return 0;
}
