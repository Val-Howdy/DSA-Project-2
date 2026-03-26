#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "CountyManager.h"

using namespace std;

struct MethodResultStruct {
	struct DataResultStruct {
		int rank;
		std::string state;
		std::string county;
		int totalCases;
		float casesPerCapita;

		DataResultStruct() {
			rank = -1;
			state = "Dummy";
			county = "Dummy";
			totalCases = -1;
			casesPerCapita = -1.0f;
		}
	};

	float runtime;
	std::vector<DataResultStruct> results;
};

int partitionQuickselect(vector<pair<float, CountyManager::County*>>& data, int left, int right) {
	float pivot = data[right].first;
	int i = left;

	for (int j = left; j < right; j++) {
		if (data[j].first > pivot) {
			swap(data[i], data[j]);
			i++;
		}
	}

	swap(data[i], data[right]);
	return i;
}

void quickselect(vector<pair<float, CountyManager::County*>>& data, int left, int right, int kIndex) {
	while (left < right) {
		int pivotIndex = partitionQuickselect(data, left, right);

		if (pivotIndex == kIndex) {
			return;
		}
		else if (pivotIndex < kIndex) {
			left = pivotIndex + 1;
		}
		else {
			right = pivotIndex - 1;
		}
	}
}

void topKQuickselect(std::vector<std::pair<float, CountyManager::County*>>& data, int k)
{
	if (data.empty() || k <= 0) {
		return;
	}

	if (k > static_cast<int>(data.size())) {
		k = static_cast<int>(data.size());
	}

	quickselect(data, 0, static_cast<int>(data.size()) - 1, k - 1);

	sort(data.begin(), data.begin() + k,
		[](const pair<float, CountyManager::County*>& a,
		   const pair<float, CountyManager::County*>& b) {
			return a.first > b.first;
		});
}

void heapifyDown(vector<pair<float, CountyManager::County*>>& data, int heapSize, int index) {
	while (true) {
		int largest = index;
		int left = 2 * index + 1;
		int right = 2 * index + 2;

		if (left < heapSize && data[left].first > data[largest].first) {
			largest = left;
		}

		if (right < heapSize && data[right].first > data[largest].first) {
			largest = right;
		}

		if (largest == index) {
			break;
		}

		swap(data[index], data[largest]);
		index = largest;
	}
}

void buildMaxHeap(vector<pair<float, CountyManager::County*>>& data) {
	for (int i = static_cast<int>(data.size()) / 2 - 1; i >= 0; i--) {
		heapifyDown(data, static_cast<int>(data.size()), i);
	}
}

void topKHeapInPlace(vector<std::pair<float, CountyManager::County*>>& data, int k)
{
	if (data.empty() || k <= 0) {
		return;
	}

	int n = static_cast<int>(data.size());

	if (k > n) {
		k = n;
	}

	buildMaxHeap(data);

	for (int end = n - 1; end >= n - k; end--) {
		swap(data[0], data[end]);
		heapifyDown(data, end, 0);
	}

	reverse(data.begin() + (n - k), data.end());
}

bool isValidDate(int year, int month, int day) {
	auto ymd = std::chrono::year_month_day(
		std::chrono::year(year),
		std::chrono::month(month),
		std::chrono::day(day)
	);

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
	std::cout << "| " << std::left << std::setw(5)  << "Rank"
	          << "| " << std::setw(6)  << "State"
	          << "| " << std::setw(20) << "County"
	          << "| " << std::setw(16) << "Total New Cases"
	          << "| " << std::setw(17) << "Cases per Capita"
	          << "|" << std::endl;
	printSeparator();
}

void printResults(const std::vector<MethodResultStruct::DataResultStruct>& results,
                  const float runtime,
                  const std::string& method,
                  const std::string& startDate,
                  const std::string& endDate,
                  int k) {
	printHeader(runtime, method, startDate, endDate, k);

	for (const auto& r : results) {
		std::cout << "| " << std::left << std::setw(5)  << r.rank
		          << "| " << std::setw(6)  << r.state
		          << "| " << std::setw(20) << r.county
		          << "| " << std::setw(16) << r.totalCases
		          << "| " << std::setw(17) << std::fixed << std::setprecision(6) << r.casesPerCapita
		          << "|" << std::endl;
	}

	printSeparator();
}

MethodResultStruct buildResultVector(vector<pair<float, CountyManager::County*>>& data, int k, const string& method) {
	MethodResultStruct output;

	if (k > static_cast<int>(data.size())) {
		k = static_cast<int>(data.size());
	}

	auto start = chrono::high_resolution_clock::now();

	if (method == "Quickselect") {
		topKQuickselect(data, k);
	}
	else {
		topKHeapInPlace(data, k);
	}

	auto stop = chrono::high_resolution_clock::now();
	output.runtime = chrono::duration<float, std::milli>(stop - start).count();

	if (method == "Quickselect") {
		for (int i = 0; i < k; i++) {
			MethodResultStruct::DataResultStruct row;
			row.rank = i + 1;
			row.state = data[i].second->_state;
			row.county = data[i].second->_name;
			row.casesPerCapita = data[i].first;
			row.totalCases = static_cast<int>(data[i].first * data[i].second->_population + 0.5f);
			output.results.push_back(row);
		}
	}
	else {
		int n = static_cast<int>(data.size());
		for (int i = 0; i < k; i++) {
			MethodResultStruct::DataResultStruct row;
			row.rank = i + 1;
			row.state = data[n - k + i].second->_state;
			row.county = data[n - k + i].second->_name;
			row.casesPerCapita = data[n - k + i].first;
			row.totalCases = static_cast<int>(data[n - k + i].first * data[n - k + i].second->_population + 0.5f);
			output.results.push_back(row);
		}
	}

	return output;
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <method> <start_date> <end_date> <k>" << std::endl;
		std::cerr << "Methods: Quickselect or Heap" << std::endl;
		std::cerr << "Date format: YYYY-MM-DD" << std::endl;
		std::cerr << "Valid k is int greater than 0" << std::endl;
		return 1;
	}

	string method = argv[1];
	string startDate = argv[2];
	string endDate = argv[3];
	int startDateYear;
	int startDateMonth;
	int startDateDay;
	int endDateYear;
	int endDateMonth;
	int endDateDay;
	int k;

	if (method != "Quickselect" && method != "Heap") {
		std::cerr << "Error: Invalid method" << std::endl;
		return 1;
	}

	try {
		if (startDate.size() == 10 && endDate.size() == 10 &&
			startDate[4] == '-' && startDate[7] == '-' &&
			endDate[4] == '-' && endDate[7] == '-') {

			startDateYear = stoi(startDate.substr(0, 4));
			startDateMonth = stoi(startDate.substr(5, 2));
			startDateDay = stoi(startDate.substr(8, 2));

			endDateYear = stoi(endDate.substr(0, 4));
			endDateMonth = stoi(endDate.substr(5, 2));
			endDateDay = stoi(endDate.substr(8, 2));

			if (!(isValidDate(startDateYear, startDateMonth, startDateDay) &&
			      isValidDate(endDateYear, endDateMonth, endDateDay))) {
				throw invalid_argument("Invalid date");
			}
		}
		else {
			throw invalid_argument("Invalid date format");
		}
	}
	catch (...) {
		std::cerr << "Error: Invalid date format" << std::endl;
		return 1;
	}

	try {
		k = stoi(argv[4]);
		if (k < 1) {
			throw invalid_argument("k less than 1");
		}
	}
	catch (...) {
		std::cerr << "Error: Invalid k" << std::endl;
		return 1;
	}

	CountyManager manager("data/Covid_data.csv", "data/Population_data.csv");
	vector<pair<float, CountyManager::County*>> data;

	bool success = manager.getFormatedData(
		startDateYear, startDateMonth, startDateDay,
		endDateYear, endDateMonth, endDateDay,
		data
	);

	if (!success) {
		std::cerr << "Error: Invalid date range or date outside dataset bounds" << std::endl;
		return 1;
	}

	if (data.empty()) {
		std::cerr << "Error: No data available for selected range" << std::endl;
		return 1;
	}

	MethodResultStruct results = buildResultVector(data, k, method);
	printResults(results.results, results.runtime, method, startDate, endDate, k);

	return 0;
}