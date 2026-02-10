Get-ChildItem -Path . -Recurse -Include *.cpp, *.h, *.hpp | 
ForEach-Object {
    # Read file content and replace CRLF (\r\n) with LF (\n), then reapply Windows line endings
    $content = (Get-Content $_.FullName -Raw) -replace "`r`n", "`n"  # Normalize line endings
    $content = $content -replace "`n", "`r`n"  # Convert back to Windows-style CRLF

    # Write content back with UTF-8 encoding (without BOM)
    [System.IO.File]::WriteAllLines($_.FullName, $content, [System.Text.Encoding]::UTF8)
}