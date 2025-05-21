# Function to convert a string to snake_case
function ConvertTo-SnakeCase {
    param([string]$text)
    # Convert PascalCase and camelCase to snake_case
    $text = [regex]::Replace($text, '([a-z])([A-Z])', '$1_$2')
    $text = [regex]::Replace($text, '([A-Z])([A-Z][a-z])', '$1_$2')
    return $text.ToLower()
}

# Get all cpp and hpp files
$files = Get-ChildItem -Path "src", "include" -Recurse -Include "*.cpp", "*.hpp"

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    
    # Find all include statements
    $pattern = '#include\s+["<]([^">]+)[">]'
    $matches = [regex]::Matches($content, $pattern)
    
    foreach ($match in $matches) {
        $oldPath = $match.Groups[1].Value
        $newPath = ConvertTo-SnakeCase $oldPath
        
        if ($oldPath -ne $newPath) {
            $content = $content.Replace($oldPath, $newPath)
        }
    }
    
    # Write the updated content back to the file
    Set-Content -Path $file.FullName -Value $content
} 