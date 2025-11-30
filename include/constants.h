#pragma once

#include <cstdint>

// #define PLATFORM_WEB
#if defined(PLATFORM_WEB)
constexpr uint32_t SCREEN_WIDTH = 768;
constexpr uint32_t SCREEN_HEIGHT = 432;
constexpr uint32_t EDIT_PANEL_HEIGHT = 240;
#else
constexpr uint32_t SCREEN_WIDTH = 1600;
constexpr uint32_t SCREEN_HEIGHT = 900;
#endif
constexpr uint32_t DEFAULT_LEVEL_LENGTH = 100;
constexpr uint32_t DEFAULT_LEVEL_HEIGHT = 14;
