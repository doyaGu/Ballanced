param(
    [Parameter(Mandatory = $true)][ValidateSet('x86', 'x64', 'arm64')][string]$Architecture,
    [Parameter(Mandatory = $true)][string]$DestinationDirectory,
    [ValidateSet('1.0.20', '1.0.22')][string]$Version = '1.0.22'
)

$ErrorActionPreference = 'Stop'
$destination = (New-Item -ItemType Directory -Force -Path $DestinationDirectory).FullName
$packagePath = Join-Path ([IO.Path]::GetTempPath()) "Ballanced-WARP-$Version.nupkg"
Invoke-WebRequest -Uri "https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.warp/$Version/microsoft.direct3d.warp.$Version.nupkg" -OutFile $packagePath
$expectedHash = @{
    '1.0.20' = 'E5FE5DE661CE98B58EF9CFB736E73C0A7A2623D3BBF5F14839B2D55566D87E40'
    '1.0.22' = 'AFC058F4D1066EDC4231A38F3AB3592B1930F7287D4AC700E6A056E0874BB0BE'
}[$Version]
if ((Get-FileHash -LiteralPath $packagePath -Algorithm SHA256).Hash -ne $expectedHash) {
    throw 'Microsoft WARP package checksum mismatch'
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$package = [IO.Compression.ZipFile]::OpenRead($packagePath)
try {
    $packageArchitecture = if ($Architecture -eq 'x86') { 'win32' } else { $Architecture }
    $warpPath = Join-Path $destination 'd3d10warp.dll'
    [IO.Compression.ZipFileExtensions]::ExtractToFile($package.GetEntry("build/native/bin/$packageArchitecture/d3d10warp.dll"), $warpPath, $true)
    [IO.Compression.ZipFileExtensions]::ExtractToFile($package.GetEntry('LICENSE.TXT'), (Join-Path $destination 'WARP-LICENSE.txt'), $true)
    Write-Host "Microsoft WARP $Version ($Architecture), file version $((Get-Item -LiteralPath $warpPath).VersionInfo.FileVersion)"
} finally { $package.Dispose() }
