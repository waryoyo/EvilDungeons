#pragma once

struct TerrainSettings {
    float noiseFrequency = 0.0015f;
    float mountainFrequency = 0.0045f;
    int maxTerrainHeight = 80;
    int waterLevel = 20;
    int horizontalRadius = 4;  // Chunk render distance
    
};

inline TerrainSettings gTerrainSettings;
