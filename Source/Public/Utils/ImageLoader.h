#pragma once
#include "Core/Core.h"

uint8* load_image_from_memory(const uint8* buffer, size_t size, int32& width, int32& height, int32& channels);
void free_image(uint8* image);