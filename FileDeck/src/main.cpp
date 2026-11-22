#include <iostream>
#include <d3d11.h>
#include <tchar.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>
#include <gui.h>

void MainGui() {
    ImGui::Begin("Window");
    ImGui::End();
}

int main() {
    RunGUI(MainGui);
}