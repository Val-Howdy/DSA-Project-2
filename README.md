<H1> Covid Data Processor </h1>
The Covid Data Processor processes a large dataset of weekly COVID-19 cases and deaths alongside 
a dataset of county population data. Its primary goal is to find the top K areas
(either specific counties or specific weeks) with the highest new cases per capita.

To handle the processing thousands
of data points, the project implements and compares two selection algorithms:
1. Quickselect
2. Binary Max-Heap

## Features

* #### two Analysis Modes
  * Counties: Aggregates data over a date range to find the counties with the highest new cases per capita during the date range. 
  * Weeks:  breaks the date range down into individual weeks. then evaluates every "county-week"  pair independtly to find specific moments in time and specific locations where covid cases per capita were highest. 

## Arguments 
|  Argument |  Options | Description  |
|---|---|---|
| method  | Quickselect or Heap  | The algorithm used   |
|  start_date | YYYY-MM-DD  | Start of the analysis period  |
| end_date  | YYYY-MM-DD  |  End of the analysis period |
|  k | Integer > 0  |  Number of top results to return |
|  mode | Counties or Week  |  Analysis Mode |

## Getting Started
### Prerequisites
* C++20 compiler: GCC, Clang, MSVC, MinGW
* OS: Linux, macOS, or Windows

## Example Build and Run

### Linux/macOS
```bash
git clone https://github.com/Val-Howdy/DSA-Project-2.git
cd DSA-Project-2
g++ -std=c++20 src/main.cpp src/CountyManager.cpp -o main
./main Quickselect 2021-01-01 2022-01-01 10 Weeks
```

### Windows
```bash
git clone https://github.com/Val-Howdy/DSA-Project-2.git
cd DSA-Project-2
g++ -std=c++20 src/main.cpp src/CountyManager.cpp -o main.exe
main.exe Quickselect 2021-01-01 2022-01-01 10 Weeks
```
