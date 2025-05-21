# Rename directories to snake_case
$dirs = @(
    "src/engine_core",
    "src/game_engine",
    "include/engine_core",
    "include/game_engine",
    "src/engine_core/engine_utils",
    "src/engine_core/game_objects",
    "include/engine_core/engine_utils",
    "include/engine_core/game_objects",
    "include/game_engine/game_utils"
)

foreach ($dir in $dirs) {
    $newDir = $dir -replace "_", "-"
    if (Test-Path $dir) {
        Rename-Item -Path $dir -NewName $newDir
        Write-Host "Renamed $dir to $newDir"
    }
} 