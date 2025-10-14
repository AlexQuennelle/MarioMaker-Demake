#pragma once

#include "imgui.h"

/**
 * Class representing an instance of the program and encapsulating global state.
 */
class Program
{
	public:
	Program();

	/**
	 * The program's main update loop. Should be called  from a loop in main().
	 */
	void Update();

	private:
	ImGuiIO& imguiIO;
};
