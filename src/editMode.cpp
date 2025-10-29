#include "assetmanager.h"
#include "gamemode.h"
#include "tile.h"

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

EditMode::EditMode(Level& lvl, asset_ptr& am, const ImGuiIO& imgui)
	: GamemodeInstance(lvl, am), imGuiIO(imgui)
{
	// Initialize mode
	this->camera = Camera2D{0};
	this->camera.target = {.x = 0.0f, .y = 0.0f};
	this->camera.offset = {
		.x = 0.0f,
		.y = 216 - (this->level.GetHeight() * 16.0f),
	};
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

	Vector2Int newSelection{
		.x = static_cast<int>(lvlMousePos.x),
		.y = static_cast<int>(lvlMousePos.y),
	};
	bool MouseMoved{newSelection != this->selectedTile};
	this->selectedTile = newSelection;

	if (!this->imGuiIO.WantCaptureMouse)
	{
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
			(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && MouseMoved))
		{
			this->level.SetTileAtEditor(TileID::ground, this->selectedTile);
		}
		else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ||
				 (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && MouseMoved))
		{
			this->level.SetTileAtEditor(TileID::air, this->selectedTile);
		}
	}

	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
	{
		if (this->camera.offset.x < 0.0f)
			this->camera.offset.x += 1.0f;
	}
	else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
	{
		if (this->camera.offset.x > -((this->level.GetLength() * 16.0f) - 384))
			this->camera.offset.x -= 1.0f;
	}
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
	if (!this->imGuiIO.WantCaptureMouse)
	{
		float cellSize{GetScreenWidth() / 24.0f};
		DrawRectangle(
			(this->selectedTile.x + (this->camera.offset.x / 16.0f)) * cellSize,
			(this->selectedTile.y + (this->camera.offset.y / 16.0f)) * cellSize,
			cellSize, cellSize, Fade(WHITE, 0.4f));
	}

	Vector2 camOffset = {
		.x = -this->camera.offset.x,
		.y = this->camera.offset.y,
	};

	Vector2Int lvlDims{
		.x = this->level.GetLength(),
		.y = this->level.GetHeight(),
	};

	bool open = true;
	ImGuiWindowFlags flags{ImGuiWindowFlags_NoSavedSettings |
						   ImGuiWindowFlags_AlwaysAutoResize};
	if (ImGui::Begin("Debug Info", &open, flags))
	{
		ImGui::InputFloat2("Camera offset", &camOffset.x);
		ImGui::InputFloat2("Mouse position in level", &lvlMousePos.x);
		ImGui::InputInt2("Hovered cell", &this->selectedTile.x);
		ImGui::Text(" ");
		//
		//ImGui::InputInt2("Level Size", &lvlDims.x);
		if (ImGui::BeginTable("Level size", 2,
							  ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableSetupColumn("Width");
			ImGui::TableSetupColumn("Height");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::SliderInt(" ", &lvlDims.x, 24, 500);
			//ImGui::SliderInt2(" ", &lvlDims.x, 24, 500);
			ImGui::TableSetColumnIndex(1);
			ImGui::SliderInt("#01", &lvlDims.y, 14, 32);
		}
		ImGui::EndTable();
	}
	ImGui::End();

	//if (IsKeyPressed(KEY_ENTER))
	if ((this->level.GetLength() != lvlDims.x) ||
		(this->level.GetHeight() != lvlDims.y))
	{
		this->level.SetLevelSize(lvlDims.x, lvlDims.y);
		this->level.DrawGrid(this->tex);
	}

	this->camera.offset = {.x = -camOffset.x, .y = camOffset.y};
}
