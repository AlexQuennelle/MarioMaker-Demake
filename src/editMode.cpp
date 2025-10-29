#include "assetmanager.h"
#include "gamemode.h"

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

EditMode::EditMode(Level& lvl, asset_ptr& am) : GamemodeInstance(lvl, am)
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
	float cellSize{GetScreenWidth() / 24.0f};
	this->lvlMousePos = {(GetMousePosition() / cellSize) -
						 this->camera.offset / 16.0f};

	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
	{
		this->camera.offset.x += 1.0f;
	}
	else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
	{
		this->camera.offset.x -= 1.0f;
	}

	this->selectedTile = {
		.x = static_cast<int>(lvlMousePos.x),
		.y = static_cast<int>(lvlMousePos.y),
	};
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
	float cellSize{GetScreenWidth() / 24.0f};
	DrawRectangle(
		(this->selectedTile.x + (this->camera.offset.x / 16.0f)) * cellSize,
		(this->selectedTile.y + (this->camera.offset.y / 16.0f)) * cellSize,
		cellSize, cellSize, GREEN);
	// ImGui demo
	bool open = true;
	ImGuiWindowFlags flags{ImGuiWindowFlags_NoSavedSettings |
						   ImGuiWindowFlags_AlwaysAutoResize};
	if (ImGui::Begin("Debug Info", &open, flags))
	{
		ImGui::InputFloat2("Camera offset", &this->camera.offset.x);
		ImGui::InputFloat2("Mouse position in level", &lvlMousePos.x);
		ImGui::InputInt2("Hovered cell", &this->selectedTile.x);
	}
	ImGui::End();
}
