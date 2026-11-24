#include <unordered_map>
#include <filesystem>
#include <windows.h>
#include <string>
#include <iostream>

static std::string WStringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return {};
    }
    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), static_cast<int>(wstr.size()),
        nullptr, 0, nullptr, nullptr
    );
    if (size_needed <= 0) {
        return {};
    }
    std::string utf8(size_needed, '\0');
    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), static_cast<int>(wstr.size()),
        utf8.data(), size_needed, nullptr, nullptr
    );
    return utf8;
}

std::unordered_map<std::string, int> getAllFiles(std::filesystem::path& filepath) {
    std::unordered_map<std::string, int> directories;
    for (const auto& entry : std::filesystem::directory_iterator(filepath)) {
        std::string path_str = WStringToUtf8(entry.path().wstring());
        if (entry.is_directory()) {
            directories[path_str] = 0; 
        }
        if (entry.is_regular_file()) {
            directories[path_str] = 1; 
        }
    }
    return directories;
}