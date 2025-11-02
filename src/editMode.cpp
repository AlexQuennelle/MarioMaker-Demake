#include "assetmanager.h"
#include "gamemode.h"
#include "tile.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include <string>
#if !defined(PLATFORM_WEB)
#include <nfd.hpp>
#endif

EditMode::EditMode(Level& lvl, asset_ptr& am, const ImGuiIO& imgui)
	: GamemodeInstance(lvl, am), imGuiIO(imgui)
{
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
			this->level.SetTileAtEditor(this->brush.ID, this->selectedTile);
		}
		else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ||
				 (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && MouseMoved))
		{
			this->level.SetTileAtEditor(TileID::air, this->selectedTile);
		}

		float cameraSpeed{2.0f};
		if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		{
			if (this->camera.offset.x < 0.0f)
				this->camera.offset.x += cameraSpeed;
		}
		else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		{
			if (this->camera.offset.x >
				-((this->level.GetLength() * 16.0f) - 384))
				this->camera.offset.x -= cameraSpeed;
		}
		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		{
			if (this->camera.offset.y < 0.0f)
				this->camera.offset.y += cameraSpeed;
		}
		else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		{
			if (this->camera.offset.y >
				-((this->level.GetHeight() * 16.0f) - 216))
				this->camera.offset.y -= cameraSpeed;
		}
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
		.x = -this->camera.offset.x / 16.0f,
		.y = -this->camera.offset.y / 16.0f,
	};
	Vector2Int lvlDims{
		.x = this->level.GetLength(),
		.y = this->level.GetHeight(),
	};

	std::array<char, 256> nameBuf{};
	strcpy(nameBuf.data(), this->level.GetName().c_str());

	ImGuiWindowFlags flags{ImGuiWindowFlags_NoSavedSettings |
						   ImGuiWindowFlags_AlwaysAutoResize |
						   ImGuiWindowFlags_NoCollapse};
	if (ImGui::Begin("Menu", nullptr, flags))
	{
		this->DrawButtons();
		this->DrawPallette();

		ImGui::Separator();
		ImGui::Text("CameraPosition:");
		ImGui::SameLine();
		ImGui::SliderFloat("##CamOffsetX", &camOffset.x, 0.0f,
						   this->level.GetLength() - 24.0f);
		ImGui::SameLine();
		ImGui::SliderFloat("##CamOffsetY", &camOffset.y, 0.0f,
						   this->level.GetHeight() - 14.0f);
		// Debug info
		//ImGui::InputFloat2("Mouse position in level", &lvlMousePos.x);
		//ImGui::InputInt2("Hovered cell", &this->selectedTile.x);

		ImGui::Separator();
		ImGuiTreeNodeFlags lvlInfo{ImGuiTreeNodeFlags_DefaultOpen};
		if (ImGui::CollapsingHeader("LevelInfo", nullptr, lvlInfo))
		{
			ImGui::Text("Level Name");
			ImGui::InputText("##LevelName", nameBuf.data(), 255);

			ImGui::Separator();

			ImGui::Text("Level Size");
			ImGui::Text("Width: ");
			ImGui::SameLine();
			ImGui::DragInt("##lvlDimsX", &lvlDims.x, 0.05f, 24, 500);
			ImGui::SameLine();
			if (ImGui::Button("+##lvlX"))
				lvlDims.x++;
			ImGui::SameLine();
			if (ImGui::Button("-##lvlX"))
				lvlDims.x--;
			ImGui::Text("Height:");
			ImGui::SameLine();
			ImGui::DragInt("##lvlDimsY", &lvlDims.y, 0.05f, 14, 32);
			ImGui::SameLine();
			if (ImGui::Button("+##lvlY"))
				lvlDims.y++;
			ImGui::SameLine();
			if (ImGui::Button("-##lvlY"))
				lvlDims.y--;
		}
	}
	ImGui::End();

	std::string newName{nameBuf.data()};
	if (newName != this->level.GetName())
	{
		this->level.SetName(newName);
	}

	lvlDims.x = (lvlDims.x >= 24) ? lvlDims.x : this->level.GetLength();
	lvlDims.y = (lvlDims.y >= 14) ? lvlDims.y : this->level.GetHeight();
	if ((this->level.GetLength() != lvlDims.x) ||
		(this->level.GetHeight() != lvlDims.y))
	{
		this->level.SetLevelSize(lvlDims.x, lvlDims.y);
		// FIX: Potential problem here with texture loading
		UnloadTexture(this->tex.texture);
		UnloadRenderTexture(this->tex);
		this->tex = LoadRenderTexture(this->level.GetLength() * 16,
									  this->level.GetHeight() * 16);
		this->level.DrawGrid(this->tex);
		this->camera.offset = {
			.x = -camOffset.x * 16.0f,
			.y = -camOffset.y * 16.0f,
		};
	}
	else
	{
		this->camera.offset = {
			.x = -camOffset.x * 16.0f,
			.y = -camOffset.y * 16.0f,
		};
	}
}

void EditMode::DrawButtons()
{
	if (ImGui::Button("Save"))
	{
		this->SaveLevel();
	}
#if !defined(PLATFORM_WEB)
	ImGui::SameLine();
	if (ImGui::Button("Save As"))
	{
		this->SaveLevelAs();
	}
#endif
}
void EditMode::DrawPallette()
{
	const char* prev{
		this->tileNames[static_cast<uint8_t>(this->brush.ID) - 1].data()};
	if (ImGui::BeginCombo("Tile", prev))
	{
		for (int i{0}; i < this->tileNames.size(); i++)
		{
			bool selected{prev == this->tileNames[i].data()};
			if (ImGui::Selectable(this->tileNames[i].c_str(), selected))
			{
				prev = this->tileNames[static_cast<uint8_t>(this->brush.ID) - 1]
						   .data();
				this->brush = {.ID = static_cast<TileID>(i + 1), .flags = 0};
			}
			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void EditMode::SaveLevel()
{
	std::ofstream outFile;
	if (this->level.HasFilepath())
	{
		outFile = std::ofstream(this->level.GetFilepath(),
								std::ios::out | std::ios::binary);
	}
	else
	{
#if !defined(PLATFORM_WEB)
		NFD::Guard nfdGuard;
		NFD::UniquePath outPath;
		nfdfilteritem_t filter{"Level Files", "lvl"};
		nfdresult_t result{NFD::SaveDialog(outPath, &filter, 1, RESOURCES_PATH,
										   "MyLevel.lvl")};
		if (result == NFD_OKAY)
		{
			this->level.SetFilepath(outPath.get());
			outFile =
				std::ofstream(outPath.get(), std::ios::out | std::ios::binary);
		}
		else if (result == NFD_ERROR)
		{
			std::cout << NFD_GetError() << '\n';
			return;
		}
		else if (result == NFD_CANCEL)
		{
			std::cout << "Save cancelled.\n";
			return;
		}
#endif
	}

	if (outFile.is_open())
	{
		const vector<byte> data{this->level.Serialize()};

		outFile.write(reinterpret_cast<const char*>(data.data()), data.size());

		outFile.close();
	}
	else
	{
		SetTextColor(ERROR);
		std::cerr << "ERROR: could not open file." << '\n';
		ClearStyles();
	}
}
#if !defined(PLATFORM_WEB)
void EditMode::SaveLevelAs()
{
	this->level.SetFilepath("");
	this->SaveLevel();
}
#endif
