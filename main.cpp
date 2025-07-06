# Entropyscannerv2

**Author:** Cadell Richard Anderson  
**License:** Custom License: Entropyscannerv2 Attribution License (QAL) v1.0
**Version:** 0.2  
**Date:** July 2025

#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <filesystem>
#include <locale>
#include <codecvt> // Still needed for the deprecated std::wstring_convert or for other locale related things. We will replace its specific use.

// Constants
constexpr size_t BLOCK_SIZE = 4096;

// Calculate Shannon entropy
double calculateEntropy(const std::vector<unsigned char>& data) {
    std::unordered_map<unsigned char, size_t> freq;
    for (unsigned char byte : data) freq[byte]++;

    double entropy = 0.0;
    for (const auto& pair : freq) {
        double p = static_cast<double>(pair.second) / data.size();
        entropy -= p * std::log2(p);
    }
    return entropy;
}

enum class FourValuedBoolean {
    False,    // F: Definitely False
    True,     // T: Definitely True
    Neither,  // N: Neither True nor False (No Information)
    Both      // B: Both True and False (Contradictory/Conflicting)
};

std::string FourValuedBooleanToString(FourValuedBoolean val) {
    switch (val) {
    case FourValuedBoolean::False: return "False";
    case FourValuedBoolean::True: return "True";
    case FourValuedBoolean::Neither: return "Neither (No Info)";
    case FourValuedBoolean::Both: return "Both (Contradictory)";
    default: return "Invalid";
    }
}

// Overload operator<< for std::ostream (for std::cout)
std::ostream& operator<<(std::ostream& os, FourValuedBoolean val) {
    return os << FourValuedBooleanToString(val);
}

// Helper function to convert std::string to std::wstring using Windows API
std::wstring StringToWString(const std::string& s) {
    int len;
    // +1 for null terminator for MultiByteToWideChar
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0);
    std::wstring buf(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, &buf[0], len);
    return buf;
}

// Overload operator<< for std::wostream (for std::wcout)
std::wostream& operator<<(std::wostream& os, FourValuedBoolean val) {
    // Convert std::string from FourValuedBooleanToString to std::wstring using Windows API
    std::string s = FourValuedBooleanToString(val);
    return os << StringToWString(s);
}

FourValuedBoolean operator!(FourValuedBoolean val) {
    if (val == FourValuedBoolean::True) return FourValuedBoolean::False;
    if (val == FourValuedBoolean::False) return FourValuedBoolean::True;
    return val;
}

FourValuedBoolean operator&&(FourValuedBoolean a, FourValuedBoolean b) {
    // Belnap's AND (Meet operator)
    if (a == FourValuedBoolean::False || b == FourValuedBoolean::False) return FourValuedBoolean::False;
    if (a == FourValuedBoolean::True && b == FourValuedBoolean::True) return FourValuedBoolean::True;
    if (a == FourValuedBoolean::Neither || b == FourValuedBoolean::Neither) {
        if ((a == FourValuedBoolean::Neither && b == FourValuedBoolean::True) || (a == FourValuedBoolean::True && b == FourValuedBoolean::Neither)) return FourValuedBoolean::Neither;
        if ((a == FourValuedBoolean::Neither && b == FourValuedBoolean::Both) || (a == FourValuedBoolean::Both && b == FourValuedBoolean::Neither)) return FourValuedBoolean::Neither;
        if (a == FourValuedBoolean::Neither && b == FourValuedBoolean::Neither) return FourValuedBoolean::Neither;
    }
    if (a == FourValuedBoolean::Both || b == FourValuedBoolean::Both) {
        if ((a == FourValuedBoolean::Both && b == FourValuedBoolean::True) || (a == FourValuedBoolean::True && b == FourValuedBoolean::Both)) return FourValuedBoolean::Both;
        if (a == FourValuedBoolean::Both && b == FourValuedBoolean::Both) return FourValuedBoolean::Both;
    }
    return FourValuedBoolean::False;
}

FourValuedBoolean operator||(FourValuedBoolean a, FourValuedBoolean b) {
    // Belnap's OR (Join operator)
    if (a == FourValuedBoolean::True || b == FourValuedBoolean::True) return FourValuedBoolean::True;
    if (a == FourValuedBoolean::False && b == FourValuedBoolean::False) return FourValuedBoolean::False;
    if (a == FourValuedBoolean::Neither || b == FourValuedBoolean::Neither) {
        if ((a == FourValuedBoolean::Neither && b == FourValuedBoolean::False) || (a == FourValuedBoolean::False && b == FourValuedBoolean::Neither)) return FourValuedBoolean::Neither;
        if (a == FourValuedBoolean::Neither && b == FourValuedBoolean::Neither) return FourValuedBoolean::Neither;
        if ((a == FourValuedBoolean::Neither && b == FourValuedBoolean::Both) || (a == FourValuedBoolean::Both && b == FourValuedBoolean::Neither)) return FourValuedBoolean::Both;
    }
    if (a == FourValuedBoolean::Both || b == FourValuedBoolean::Both) {
        if ((a == FourValuedBoolean::Both && b == FourValuedBoolean::False) || (a == FourValuedBoolean::False && b == FourValuedBoolean::Both)) return FourValuedBoolean::Both;
        if (a == FourValuedBoolean::Both && b == FourValuedBoolean::Both) return FourValuedBoolean::Both;
    }
    return FourValuedBoolean::True;
}

bool operator==(FourValuedBoolean a, FourValuedBoolean b) {
    return static_cast<int>(a) == static_cast<int>(b);
}

bool operator!=(FourValuedBoolean a, FourValuedBoolean b) {
    return !(a == b);
}

// Enhanced analyzeFileEntropy to return FourValuedBoolean
FourValuedBoolean analyzeFileEntropy(const std::wstring& filePath) {
    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, filePath.c_str(), L"rb");
    if (err != 0 || !fp) {
        std::wcerr << L"[-] Cannot open: " << filePath << L"\n";
        return FourValuedBoolean::Neither; // Cannot open file, so no information
    }

    std::wcout << L"\n=== Entropy Analysis: " << filePath << L" ===\n";

    size_t blockIndex = 0;
    double totalEntropy = 0.0, minEntropy = 9.0, maxEntropy = 0.0;
    size_t blockCount = 0;

    std::vector<unsigned char> buffer(BLOCK_SIZE);
    while (true) {
        size_t bytesRead = fread(buffer.data(), 1, BLOCK_SIZE, fp);
        if (bytesRead == 0) break;

        buffer.resize(bytesRead);
        double entropy = calculateEntropy(buffer);
        totalEntropy += entropy;
        minEntropy = std::min(minEntropy, entropy);
        maxEntropy = std::max(maxEntropy, entropy);

        std::wcout << L"Block " << std::setw(4) << blockIndex++ << L" | Entropy: "
            << std::fixed << std::setprecision(4) << entropy << L" | ";

        if (entropy < 6.5)      std::wcout << L"[NORMAL]       ";
        else if (entropy < 7.5)  std::wcout << L"[SUSPICIOUS] ";
        else                    std::wcout << L"[CRITICAL!]  ";

        for (int i = 0; i < static_cast<int>(entropy * 2); ++i)
            std::wcout << L"|";
        std::wcout << L"\n";

        buffer.resize(BLOCK_SIZE);
        blockCount++;
    }

    fclose(fp);

    if (blockCount == 0) {
        std::wcerr << L"[-] No data read from file.\n";
        return FourValuedBoolean::Neither; // No data, no information
    }

    double avgEntropy = totalEntropy / blockCount;

    std::wcout << L"\n[Summary]\n";
    std::wcout << L"  Total Blocks:    " << blockCount << L"\n";
    std::wcout << L"  Average Entropy: " << std::fixed << std::setprecision(4) << avgEntropy << L"\n";
    std::wcout << L"  Min Entropy:     " << minEntropy << L"\n";
    std::wcout << L"  Max Entropy:     " << maxEntropy << L"\n";

    // Determine the FourValuedBoolean result based on entropy
    if (avgEntropy > 7.5 || maxEntropy > 7.8) {
        std::wcout << L"[!] HIGH ENTROPY DETECTED. -> True (Malicious/Critical)\n";
        return FourValuedBoolean::True; // Clearly indicative of high entropy
    }
    else if (avgEntropy >= 6.5) {
        std::wcout << L"[!] Suspicious entropy range. -> Neither (Indeterminate)\n";
        return FourValuedBoolean::Neither;
    }
    else {
        std::wcout << L"[!] Normal entropy. -> False (Benign)\n";
        return FourValuedBoolean::False;
    }
}

// New helper function to copy files
bool copyFileToFolder(const std::wstring& sourcePath, const std::wstring& destFolderPath) {
    // Ensure the destination directory exists
    if (!std::filesystem::exists(destFolderPath)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(destFolderPath, ec)) {
            std::wcerr << L"[-] Failed to create destination directory '" << destFolderPath << L"': " << ec.message().c_str() << L"\n";
            return false;
        }
    }

    std::filesystem::path source_fs_path(sourcePath);
    std::filesystem::path dest_fs_path(destFolderPath);
    dest_fs_path /= source_fs_path.filename(); // Append source file name to destination path

    std::wcout << L"[i] Attempting to copy from:\n    " << sourcePath << L"\n    to:\n    " << dest_fs_path.c_str() << L"\n";

    // Copy the file. FALSE means it will fail if the file already exists.
    if (CopyFileW(sourcePath.c_str(), dest_fs_path.c_str(), FALSE)) {
        return true;
    }
    else {
        DWORD error = GetLastError();
        std::wcerr << L"[-] CopyFileW failed with error code: " << error << L". (Source: " << sourcePath << L", Dest: " << dest_fs_path.c_str() << L")\n";
        // Specific handling for ERROR_FILE_EXISTS
        if (error == ERROR_FILE_EXISTS) {
            std::wcerr << L"    File already exists in the destination folder.\n";
        }
        return false;
    }
}


int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::ios::sync_with_stdio(false);

    wchar_t systemDrive[MAX_PATH] = { 0 };
    DWORD len = GetEnvironmentVariableW(L"SystemDrive", systemDrive, MAX_PATH);
    if (len == 0 || len > MAX_PATH) {
        std::wcerr << L"[-] Failed to detect system drive.\n";
        return 1;
    }

    std::wstring root = std::wstring(systemDrive) + L"\\";
    // Define the destination folder for suspicious/critical files
    // IMPORTANT: Ensure this folder exists or the program has permissions to create it.
    // For testing, you might want to create C:\SuspiciousFiles manually or change this path.
    const std::wstring destinationFolder = L"C:\\Noob\\ScanSuspisciousness";

    std::wcout << L"[+] Scanning all files on system drive: " << root << L"\n";

    try {
        std::unordered_map<std::wstring, FourValuedBoolean> file_suspiciousness;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(
            root, std::filesystem::directory_options::skip_permission_denied)) {

            if (entry.is_regular_file()) {
                FourValuedBoolean file_status = analyzeFileEntropy(entry.path().wstring());
                file_suspiciousness[entry.path().wstring()] = file_status;
                std::wcout << L"Overall Entropy Status for " << entry.path().wstring() << L": " << file_status << L"\n";

                // Logic to copy suspicious or critical files
                if (file_status == FourValuedBoolean::True || file_status == FourValuedBoolean::Neither) {
                    std::wcout << L"[!] Alertive entropy detected. Attempting to copy file to quarantine folder...\n";
                    if (copyFileToFolder(entry.path().wstring(), destinationFolder)) {
                        std::wcout << L"[+] File copied successfully to: " << destinationFolder << L"\n";
                    }
                    else {
                        // Error message handled inside copyFileToFolder
                    }
                }
            }
        }

        std::wcout << L"\n--- Summary of File Entropy Statuses ---\n";
        for (const auto& pair : file_suspiciousness) {
            std::wcout << L"File: " << pair.first << L" | Status: " << pair.second << L"\n";
        }

    }
    catch (const std::exception& e) {
        std::cerr << "[!] Exception while scanning: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
