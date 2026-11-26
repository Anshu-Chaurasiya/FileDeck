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
#include <functional>

std::filesystem::path downloads_folder_location;
std::filesystem::path documents_folder_location;
std::filesystem::path desktop_folder_location;

std::filesystem::path selected_dir;
std::filesystem::path loaded_dir;
std::unordered_map<std::string, int> cached_files;

struct FileEntry {
    std::string filepath;
    int type;
};

enum selected_location {
    Downloads,
    Documents,
    Desktop,
};

inline static void ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 5.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border]               = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Button]               = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_Header]               = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_Separator]            = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TableBorderLight]     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TableRowBg]           = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt]        = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
}

std::vector<FileEntry> cached_entries;

bool current_center_window = false;
inline static void CreateCenterWindow(const char* label, std::function<void()> func) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = ImVec2(
        viewport->Pos.x + viewport->Size.x * 0.5f,
        viewport->Pos.y + viewport->Size.y * 0.5f
    );

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 300.0f), ImGuiCond_Appearing);

    if (!current_center_window) {
        if (ImGui::Begin(label, nullptr, flags)) {
            func();
        }
        ImGui::End();
    }
    else {
        std::cout << "Another Window is already Opened\n";
    }
}

inline static void FileView() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(5.0f, 138.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(284.0f, 649.0f), ImGuiCond_Always);
    ImGui::Begin("Files View", nullptr, flags);
    if (ImGui::Button("Downloads")) {
        if (!downloads_folder_location.empty()) {
            selected_dir = downloads_folder_location;
        }
    }
    if (ImGui::Button("Desktop")) {
        if (!desktop_folder_location.empty()) {
            selected_dir = desktop_folder_location;
        }
    }
    if (ImGui::Button("Documents")) {
        if (!documents_folder_location.empty()) {
            selected_dir = documents_folder_location;   
        }
    }
    ImGui::End();
}

selected_location location;
std::string label;
bool locations_pressed;
char needed_folder[512] = "";
std::string required_string;
inline static void Shortcuts() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(2.0f, 5.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1531.0f, 131.0f), ImGuiCond_Always);
    ImGui::Begin("FileDeck", nullptr, flags);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("Configure Locations")) {
                if (ImGui::MenuItem("Downloads Location")) {
                    location = Downloads;
                    label = "Configure Downloads Location";
                    locations_pressed = true;
                    current_center_window = false;
                }
                if (ImGui::MenuItem("Desktop Location")) {
                    location = Desktop;
                    label = "Configure Desktop Location";
                    locations_pressed = true;
                    current_center_window = false;
                }
                if (ImGui::MenuItem("Documents Location")) {
                    location = Documents;
                    label = "Configure Documents Location";
                    locations_pressed = true;
                    current_center_window = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (locations_pressed) {
        CreateCenterWindow(label.c_str(), [=] () {
             ImGui::InputText("##NOTHING", needed_folder, IM_COUNTOF(needed_folder));
             if (ImGui::Button("Done")) {
                if (location == Downloads) {
                    downloads_folder_location = needed_folder;
                    locations_pressed = false;
                    current_center_window = true;
                    needed_folder[0] = '\0';
                }
                else if (location == Documents) {
                    documents_folder_location = needed_folder;
                    locations_pressed = false;
                    current_center_window = true;
                    needed_folder[0] = '\0';
                }
                else if (location == Desktop) {
                    desktop_folder_location = needed_folder;
                    locations_pressed = false;
                    current_center_window = true;
                    needed_folder[0] = '\0';
                }
            }
        });
    }
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
            ImGui::Text("No files or directories found.");
        } else {
            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(cached_entries.size()));

            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    const auto& entry = cached_entries[i];
                    std::filesystem::path name = (std::filesystem::path) entry.filepath;
                    name = name.filename();
                    std::string line = (entry.type == 0)
                        ? std::format("Directory -> {}", name.string())
                        : std::format("File -> {}", name.string());
                    ImGui::GetStyle().ButtonTextAlign = ImVec2(0.0f, 0.8f);
                    if (ImGui::Button(line.c_str(), ImVec2(950.0f, 20.0f))) {
                        if (entry.type == 1) { continue; }
                        selected_dir = loaded_dir.string() + "/\\" + name.string();
                    }
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
    static bool styled = false;
    if (!styled) {
        ApplyStyle();
        styled = true;
    }
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