#pragma once

struct TerrainSettings {
    float noiseFrequency = 0.0015f;
    float mountainFrequency = 0.0045f;
    int maxTerrainHeight = 80;
    int waterLevel = 20;
};

inline TerrainSettings gTerrainSettings;
