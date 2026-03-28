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
		int week;

		DataResultStruct() {
			rank = -1;
			state = "Dummy";
			county = "Dummy";
			totalCases = -1;
			casesPerCapita = -1.0f;
			week = -1;
		}
	};

	float runtime;
	std::vector<DataResultStruct> results;
};

int partitionQuickselect(vector<tuple<float, CountyManager::County*,int>>& data, int left, int right) {
	float pivot = get<0>(data[right]);
	int i = left;

	for (int j = left; j < right; j++) {
		if (get<0>(data[j]) > pivot) {
			swap(data[i], data[j]);
			i++;
		}
	}

	swap(data[i], data[right]);
	return i;
}

void quickselect(vector<tuple<float, CountyManager::County*,int>>& data, int left, int right, int kIndex) {
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

void topKQuickselect(std::vector<tuple<float, CountyManager::County*,int>>& data, int k)
{
	if (data.empty() || k <= 0) {
		return;
	}

	if (k > static_cast<int>(data.size())) {
		k = static_cast<int>(data.size());
	}

	quickselect(data, 0, static_cast<int>(data.size()) - 1, k - 1);

	sort(data.begin(), data.begin() + k,
		[](const tuple<float, CountyManager::County*,int>& a,
		   const tuple<float, CountyManager::County*,int>& b) {
			return get<0>(a) > get<0>(b);
		});
}

void heapifyDown(vector<tuple<float, CountyManager::County*,int>>& data, int heapSize, int index) {
	while (true) {
		int largest = index;
		int left = 2 * index + 1;
		int right = 2 * index + 2;

		if (left < heapSize && get<0>(data[left]) > get<0>(data[largest])) {
			largest = left;
		}

		if (right < heapSize && get<0>(data[right]) > get<0>(data[largest])) {
			largest = right;
		}

		if (largest == index) {
			break;
		}

		swap(data[index], data[largest]);
		index = largest;
	}
}

void buildMaxHeap(vector<tuple<float, CountyManager::County*,int>>& data) {
	for (int i = static_cast<int>(data.size()) / 2 - 1; i >= 0; i--) {
		heapifyDown(data, static_cast<int>(data.size()), i);
	}
}

void topKHeapInPlace(vector<tuple<float, CountyManager::County*,int>>& data, int k)
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

void printSeparator(bool showWeeks) {
	int totalWidth = 89 + (showWeeks ? 14 : 0);
	std::cout << std::string(totalWidth, '-') << std::endl;
}

void printHeader(float runtime, const std::string& method, const std::string& startDate, const std::string& endDate, int k, bool showWeeks) {
	int totalWidth = 86 + (showWeeks ? 14 : 0);
	printSeparator(showWeeks);
	std::cout << "| Method: " << std::left << std::setw(totalWidth-8) << method << "|" << std::endl;
	printSeparator(showWeeks);
	std::cout << "| Start date: " << std::left << std::setw(totalWidth-12) << startDate << "|" << std::endl;
	printSeparator(showWeeks);
	std::cout << "| End date: " << std::left << std::setw(totalWidth-10) << endDate << "|" << std::endl;
	printSeparator(showWeeks);
	std::cout << "| k: " << std::left << std::setw(totalWidth-3) << k << "|" << std::endl;
	printSeparator(showWeeks);
	std::string runtimeStr = std::to_string(runtime) + " ms";
	std::cout << "| Runtime: " << std::setw(totalWidth-9) << std::left << runtimeStr << "|" << std::endl;
	printSeparator(showWeeks);
	std::cout << "| " << std::left << std::setw(5)  << "Rank"
	          << "| " << std::setw(6)  << "State"
	          << "| " << std::setw(34) << "County";
	if (showWeeks) {
		std::cout << "| " << std::setw(12) << "Week Of";
	}
	std::cout << "| " << std::setw(16) << "Total New Cases"
	          << "| " << std::setw(17) << "Cases per Capita"
	          << "|" << std::endl;
	printSeparator(showWeeks);
}

void printResults(const std::vector<MethodResultStruct::DataResultStruct>& results,
                  const float runtime,
                  const std::string& method,
                  const std::string& startDate,
                  const std::string& endDate,
                  int k,
                  bool showWeeks) {

	printHeader(runtime, method, startDate, endDate, k, showWeeks);

	for (const auto& r : results) {
		std::cout << "| " << std::left << std::setw(5)  << r.rank
		          << "| " << std::setw(6)  << r.state
		          << "| " << std::setw(34) << r.county;
		if (showWeeks) {
			cout << "| " << setw(12) << CountyManager::getDateFromIndex(r.week);
		}
		std::cout << "| " << std::setw(16) << r.totalCases
		          << "| " << std::setw(17) << std::fixed << std::setprecision(6) << r.casesPerCapita
		          << "|" << std::endl;
	}

	printSeparator(showWeeks);
}

MethodResultStruct buildResultVector(vector<tuple<float, CountyManager::County*,int>>& data, int k, const string& method) {
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
			row.state = get<1>(data[i])->_state;
			row.county = get<1>(data[i])->_name;
			row.casesPerCapita = get<0>(data[i]);
			row.week= get<2>(data[i]);
			row.totalCases = static_cast<int>(get<0>(data[i]) * get<1>(data[i])->_population + 0.5f);
			output.results.push_back(row);

		}
	}
	else {
		int n = static_cast<int>(data.size());
		for (int i = 0; i < k; i++) {
			MethodResultStruct::DataResultStruct row;
			row.rank = i + 1;
			row.state = get<1>(data[n - k + i])->_state;
			row.county = get<1>(data[n - k + i])->_name;
			row.casesPerCapita = get<0>(data[n - k + i]);
			row.week= get<2>(data[i]);
			row.totalCases = static_cast<int>(get<0>(data[n - k + i]) * get<1>(data[n - k + i])->_population + 0.5f);
			output.results.push_back(row);
		}
	}

	return output;
}

int main(int argc, char* argv[]) {
	if (argc != 6) {
		std::cerr << "Usage: " << argv[0] << " <method> <start_date> <end_date> <k> <mode>" << std::endl;
		std::cerr << "Methods: Quickselect or Heap" << std::endl;
		std::cerr << "Date format: YYYY-MM-DD" << std::endl;
		std::cerr << "Valid k is int greater than 0" << std::endl;
		std::cerr << "Mode: Weeks or Counties " << std::endl;
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

	bool showWeeks = false;
	if (argc == 6) {
		string mode = argv[5];
		if (mode == "Weeks") {
			showWeeks = true;
		}
	}

	CountyManager manager("data/Covid_data.csv", "data/Population_data.csv");
	vector<tuple<float, CountyManager::County*,int>> data;

	bool success = false;
	if (showWeeks) {
		success = manager.getFormatedDataWeek(
			startDateYear, startDateMonth, startDateDay,
			endDateYear, endDateMonth, endDateDay, data);
	} else {
		success = manager.getFormatedDataCounty(
			startDateYear, startDateMonth, startDateDay,
			endDateYear, endDateMonth, endDateDay, data);
	}

	if (!success) {
		std::cerr << "Error: Invalid date range or date outside dataset bounds" << std::endl;
		return 1;
	}

	if (data.empty()) {
		std::cerr << "Error: No data available for selected range" << std::endl;
		return 1;
	}

	MethodResultStruct results = buildResultVector(data, k, method);
	printResults(results.results, results.runtime, method, startDate, endDate, k,showWeeks);

	return 0;
}
