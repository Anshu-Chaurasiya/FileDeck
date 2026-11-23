#include <iostream>
#include <d3d11.h>
#include <tchar.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>
#include <gui.h>
#include <files.h>
#include <unordered_map>
#include <filesystem>
#include <format>

std::filesystem::path selected_dir;
std::filesystem::path loaded_dir;
std::unordered_map<std::string, int> cached_files;

struct FileEntry {
    std::string filepath;
    int type;
};

std::vector<FileEntry> cached_entries;

inline static void FileView() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(5.0f, 138.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(284.0f, 649.0f), ImGuiCond_Always);
    ImGui::Begin("Files View", nullptr, flags);
    if (ImGui::Button("Downloads")) {
        selected_dir = "C:\\Users\\suraj chaurasiya\\Downloads";
    }
    ImGui::End();
}

inline static void Shortcuts() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(2.0f, 5.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1531.0f, 131.0f), ImGuiCond_Always);
    ImGui::Begin("Shortcuts", nullptr, flags);
    
    ImGui::End();
}

inline static void MainPanel() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(291.0f, 138.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(976.0f, 649.0f), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, flags);
    if (!selected_dir.empty()) {
        if (selected_dir != loaded_dir) {
            cached_entries.clear();
            auto map = getAllFiles(selected_dir);
            cached_entries.reserve(map.size());
            for (const auto& [path, type] : map) {
                cached_entries.push_back({ path, type });
            }
            loaded_dir = selected_dir;
        }

        if (cached_entries.empty()) {
            ImGui::TextUnformatted("No files or directories found.");
        } else {
            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(cached_entries.size()));

            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    const auto& entry = cached_entries[i];
                    std::string line = (entry.type == 0)
                        ? std::format("Directory -> FilePath: {}", entry.filepath)
                        : std::format("File -> FilePath: {}", entry.filepath);
                    ImGui::TextUnformatted(line.c_str());
                }
            }
            clipper.End();
        }
    }
    ImGui::End();
}

inline static void Properties() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(1269.0f, 138.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(262.0f, 649.0f), ImGuiCond_Always);
    ImGui::Begin("Properties", nullptr, flags);
    
    ImGui::End();
}

void MainGui() {
    FileView();
    Shortcuts();
    MainPanel();
    Properties();
}

int main() {
    try {
        RunGUI(MainGui);
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Unhandled std::exception: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown non-standard exception caught.\n";
        return 1;
    }
}