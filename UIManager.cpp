#include "UIManager.h"
#include "Constants.h"
#include "GameState.h"
#include "Game.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

UIManager::UIManager(GLFWwindow* window, Game* game) : 
    window(window),
    game(game),
    alpha(0.01f),
    avg_fps(60.0f) 
{
    const char* glsl_version = "#version 330";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    regularFont = io.Fonts->AddFontFromFileTTF("Minecraftia.ttf", 36.0f);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::render_overlay() {
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
    ImGui::PushFont(regularFont);
    ImGui::Text("Minecraft OpenGL");
    ImGui::Text("Moosa Saghir");
    ImGui::Text("FPS: %.0f", avg_fps);
    ImGui::PopFont();
    ImGui::End();
}

void UIManager::render_pause_screen() {

    ImVec2 button_size = ImVec2(300, 50);
    float button_spacing = 20.0f;

    ImVec2 window_size = ImVec2(400, 3*button_size.y + 4*button_spacing);
    ImVec2 window_pos = ImVec2(
        (SCREEN_WIDTH - window_size.x) * 0.5f,
        (SCREEN_HEIGHT - window_size.y) * 0.5f
    );
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size);
    ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushFont(regularFont);

    ImGui::SetCursorPos(ImVec2(
        (window_size.x - button_size.x) * 0.5f,
        button_spacing
    ));
    if (ImGui::Button("Return to game", button_size)) {
        trans_to_game();
    }

    ImGui::SetCursorPos(ImVec2(
        (window_size.x - button_size.x) * 0.5f,
        button_size.y + 2*button_spacing
    ));
    if (ImGui::Button("Exit", button_size)) {
        glfwSetWindowShouldClose(window, true);
    }

    ImGui::SetCursorPos(ImVec2(
        (window_size.x - button_size.x) * 0.5f,
        2 * button_size.y + 3 * button_spacing
    ));
    ImGui::Text("FOV: ");
    ImGui::SameLine();
    ImGui::PushItemWidth(button_size.x * 0.8);
    ImGui::SliderFloat("##FOV", &game->camera.FOV_Y, MIN_FOV_Y, MAX_FOV_Y, "%.0f");
    ImGui::PopItemWidth();
    game->camera.FOV_X = Camera::get_fov_x_deg(game->camera.FOV_Y);

    ImGui::PopFont();
    ImGui::End();
}

void UIManager::trans_to_game() {
    game->gameState = InGame;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    game->lastMousePosX = SCREEN_WIDTH / 2;
    game->lastMousePosY = SCREEN_HEIGHT / 2;
    game->physics.lastTime = glfwGetTime();

    glfwSetCursorPosCallback(window, Game::game_mouse_callback);
}

void UIManager::trans_to_settings() {
    game->gameState = Settings;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, nullptr);
}

void UIManager::draw() {
    update_avg_fps();

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    ImGui_ImplGlfw_CursorPosCallback(window, mouseX, mouseY);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    switch (game->gameState) {
    case InGame:
        render_overlay();
        break;
    case Settings:
        render_pause_screen();
        break;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::update_avg_fps() {
    float currFPS = ImGui::GetIO().Framerate;
    avg_fps = alpha * currFPS + (1 - alpha) * avg_fps;
}