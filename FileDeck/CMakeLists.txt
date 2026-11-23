cmake_minimum_required(VERSION 4.0)
project(FileDeck)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable(FileDeck
    src/main.cpp
    src/files/files.cpp
    res/ImGui/gui.cpp
    res/ImGui/imgui.cpp
    res/ImGui/imgui_draw.cpp
    res/ImGui/imgui_tables.cpp
    res/ImGui/imgui_widgets.cpp
    res/ImGui/imgui_impl_win32.cpp
    res/ImGui/imgui_impl_dx11.cpp
)

target_include_directories(
    FileDeck PRIVATE res/ImGui
    FileDeck PRIVATE src/files)

target_link_libraries(FileDeck PRIVATE d3d11)