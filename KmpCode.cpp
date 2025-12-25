#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <windows.h> 

using namespace std;


vector<int> compute_lps(const vector<unsigned char>& pat) {
    int m = pat.size();
    if (m == 0) return {};
    vector<int> lps(m, 0);
    int len = 0;
    for (int i = 1; i < m; ) {
        if (pat[i] == pat[len]) { lps[i++] = ++len; }
        else if (len != 0) { len = lps[len - 1]; }
        else { lps[i++] = 0; }
    }
    return lps;
}


vector<long long> kmp_search_file(const string& path, const vector<unsigned char>& pattern) {
    vector<long long> matches;
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Could not open file: " + path);

    vector<int> lps = compute_lps(pattern);
    size_t m = pattern.size();

   
    size_t chunk_size = 1 << 20; 
    vector<unsigned char> chunk(chunk_size);
    int j = 0;
    long long global_offset = 0;

    while (file) {
        file.read(reinterpret_cast<char*>(chunk.data()), chunk_size);
        size_t bytes_read = file.gcount();
        if (bytes_read == 0) break;

        for (size_t i = 0; i < bytes_read; i++) {
            while (j > 0 && chunk[i] != pattern[j]) j = lps[j - 1];
            if (chunk[i] == pattern[j]) {
                j++;
                if (j == (int)m) {
                    matches.push_back(global_offset + i - m + 1);
                    j = lps[j - 1];
                }
            }
        }
        global_offset += bytes_read;
    }
    return matches;
}

int main() {
    
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    string filePath, patternStr;

    cout << "========================================" << endl;
    cout << "        KMP Search             " << endl;
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
        cout << "\n--- Running KMP Search ---" << endl;
        auto start = chrono::high_resolution_clock::now();

        auto results = kmp_search_file(filePath, pattern);

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