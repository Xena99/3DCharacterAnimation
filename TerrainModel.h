#pragma once
#include <vector>
#include <algorithm>  // For std::min/max
#include <cmath>       // For std::floor

class TerrainModel : public Model {
public:
    std::vector<std::vector<float>> heightMap;
    float minX, maxX, minZ, maxZ;
    float gridSpacingX, gridSpacingZ;

    TerrainModel(const string& path, bool gamma = false) : Model(path, false, gamma) {
        generateHeightMap();
    }

private:
    void generateHeightMap() {
        // Assuming terrain is a single mesh in the model
        if (meshes.empty()) return;

        const auto& mesh = meshes[0];
        float minHeight = std::numeric_limits<float>::max();
        float maxHeight = std::numeric_limits<float>::min();

        // Determine the bounds and grid spacing based on vertices
        minX = maxX = mesh.vertices[0].Position.x;
        minZ = maxZ = mesh.vertices[0].Position.z;

        for (const auto& vertex : mesh.vertices) {
            minX = std::min(minX, vertex.Position.x);
            maxX = std::max(maxX, vertex.Position.x);
            minZ = std::min(minZ, vertex.Position.z);
            maxZ = std::max(maxZ, vertex.Position.z);
            minHeight = std::min(minHeight, vertex.Position.y);
            maxHeight = std::max(maxHeight, vertex.Position.y);
        }

        // Assuming a fixed grid size for simplicity
        int gridWidth = 100;
        int gridDepth = 100;
        gridSpacingX = (maxX - minX) / (gridWidth - 1);
        gridSpacingZ = (maxZ - minZ) / (gridDepth - 1);

        heightMap.resize(gridWidth, std::vector<float>(gridDepth, minHeight));

        // Fill the heightMap with the maximum heights at each grid position
        for (const auto& vertex : mesh.vertices) {
            int gridX = static_cast<int>((vertex.Position.x - minX) / gridSpacingX);
            int gridZ = static_cast<int>((vertex.Position.z - minZ) / gridSpacingZ);
            heightMap[gridX][gridZ] = std::max(heightMap[gridX][gridZ], vertex.Position.y);
        }
    }

public:
    float getHeight(float x, float z) {
        // Clamp x, z within the bounds
        x = std::max(minX, std::min(maxX, x));
        z = std::max(minZ, std::min(maxZ, z));

        // Calculate grid coordinates
        int gridX = static_cast<int>((x - minX) / gridSpacingX);
        int gridZ = static_cast<int>((z - minZ) / gridSpacingZ);

        // Simple point sample
        return heightMap[gridX][gridZ];
    }
};
