#include "gamemode.h"

#include <array>
#include <imgui.h>
#include <raylib.h>

EditMode::EditMode(Level& lvl) : GamemodeInstance(lvl)
{
	// Initialize mode
	this->camera = Camera2D{0};
	this->camera.target = {.x = 0.0f, .y = 0.0f};
	this->camera.offset = {.x = 0.0f, .y = 0.0f};
	this->camera.rotation = 0.0f;
	this->camera.zoom = 1.0f;

	this->tex = LoadRenderTexture(this->level.GetLength() * 16,
								  this->level.GetHeight() * 16);
	this->level.DrawGrid(this->tex);
}
void EditMode::Update()
{
	Vector2 lvlMousePos{GetMousePosition() + this->camera.offset};
	float cellSize{GetScreenWidth() / 24.0f};
	int cellX{static_cast<int>(lvlMousePos.x / cellSize)};
}
void EditMode::Draw()
{
	BeginMode2D(this->camera);
	this->level.Draw();
	DrawTexture(this->tex.texture, 0, 0, WHITE);
	EndMode2D();
}
void EditMode::DrawUI()
{
	Vector2 lvlMousePos{GetMousePosition() + this->camera.offset};
	float cellSize{GetScreenWidth() / 24.0f};
	std::array<int, 2> cell{static_cast<int>(lvlMousePos.x / cellSize),
							static_cast<int>(lvlMousePos.y / cellSize)};
	DrawRectangle(cell[0] * cellSize, cell[1] * cellSize, cellSize, cellSize,
				  GREEN);

	// ImGui demo
	bool open = true;
	ImGuiWindowFlags flags{ImGuiWindowFlags_NoSavedSettings |
						   ImGuiWindowFlags_AlwaysAutoResize};
	if (ImGui::Begin("Debug Info", &open, flags))
	{
		ImGui::InputFloat2("Mouse position in level", &lvlMousePos.x);
		ImGui::InputInt2("Hovered cell", cell.data());
	}
	ImGui::End();
}
