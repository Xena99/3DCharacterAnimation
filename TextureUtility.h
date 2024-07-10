#pragma once

#include <string>

// Function declarations
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
unsigned int loadDefaultTexture(unsigned int width = 1, unsigned int height = 1);
