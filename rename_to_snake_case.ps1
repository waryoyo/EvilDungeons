# Function to convert a string to snake_case
function ConvertTo-SnakeCase {
    param([string]$text)
    # Convert PascalCase and camelCase to snake_case
    $text = [regex]::Replace($text, '([a-z])([A-Z])', '$1_$2')
    $text = [regex]::Replace($text, '([A-Z])([A-Z][a-z])', '$1_$2')
    return $text.ToLower()
}

# Function to rename a file or directory to snake_case
function Rename-ToSnakeCase {
    param([string]$path)
    
    $parent = Split-Path -Parent $path
    $name = Split-Path -Leaf $path
    $newName = ConvertTo-SnakeCase $name
    
    if ($name -ne $newName) {
        $newPath = Join-Path $parent $newName
        Write-Host "Renaming: $path -> $newPath"
        Rename-Item -Path $path -NewName $newName
    }
}

# Get all files and directories in src and include
$items = Get-ChildItem -Path "src", "include" -Recurse | Sort-Object -Property FullName -Descending

# First rename all files
foreach ($item in $items) {
    if ($item.PSIsContainer -eq $false) {
        Rename-ToSnakeCase $item.FullName
    }
}

# Then rename all directories
foreach ($item in $items) {
    if ($item.PSIsContainer -eq $true) {
        Rename-ToSnakeCase $item.FullName
    }
} 