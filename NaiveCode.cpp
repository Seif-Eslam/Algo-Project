#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <windows.h> 

using namespace std;

vector<long long> naive_search_file(const string& path, const vector<unsigned char>& pattern) {
    vector<long long> matches;
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Could not open file: " + path);

    size_t m = pattern.size();
    size_t chunk_size = 1 << 20; 
    vector<unsigned char> chunk(chunk_size), prev_tail;
    long long global_offset = 0;

    while (file) {
        file.read(reinterpret_cast<char*>(chunk.data()), chunk_size);
        size_t bytes_read = file.gcount();
        if (bytes_read == 0) break;

        vector<unsigned char> buffer = prev_tail;
        buffer.insert(buffer.end(), chunk.begin(), chunk.begin() + bytes_read);

        for (size_t i = 0; i + m <= buffer.size(); i++) {
            if (memcmp(&buffer[i], pattern.data(), m) == 0) {
                matches.push_back(global_offset - prev_tail.size() + i);
            }
        }

        if (m > 1 && buffer.size() >= m - 1)
            prev_tail.assign(buffer.end() - (m - 1), buffer.end());
        else prev_tail.clear();

        global_offset += bytes_read;
    }
    return matches;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    string filePath, patternStr;

    cout << "========================================" << endl;
    cout << "       Naive Search             " << endl;
    cout << "========================================" << endl;

    cout << "1. Enter File Path: ";
    getline(cin, filePath);

    cout << "2. Enter Search Pattern: ";
    getline(cin, patternStr);

    if (patternStr.empty()) {
        cerr << "Error: Pattern cannot be empty!" << endl;
        return 1;
    }

    vector<unsigned char> pattern(patternStr.begin(), patternStr.end());

    try {
        cout << "\n--- Running Naive Search ---" << endl;
        auto start = chrono::high_resolution_clock::now();

        auto results = naive_search_file(filePath, pattern);

        auto end = chrono::high_resolution_clock::now();

        cout << "Matches found: " << results.size() << endl;
        cout << "Time taken: " << chrono::duration<double, milli>(end - start).count() << " ms" << endl;
    }
    catch (const exception& e) {
        cerr << "\nRuntime Error: " << e.what() << endl;
    }

    cout << "\nPress Enter to exit..." << endl;
    cin.get();

    return 0;
}