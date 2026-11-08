param(
    [Parameter(Mandatory = $true)][ValidateSet('x86', 'x64', 'arm64')][string]$Architecture,
    [Parameter(Mandatory = $true)][string]$DestinationDirectory
)

$ErrorActionPreference = 'Stop'
$destination = (New-Item -ItemType Directory -Force -Path $DestinationDirectory).FullName
$packagePath = Join-Path ([IO.Path]::GetTempPath()) 'Ballanced-WARP-1.0.21.nupkg'
Invoke-WebRequest -Uri 'https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.warp/1.0.21/microsoft.direct3d.warp.1.0.21.nupkg' -OutFile $packagePath
$expectedHash = 'EA44B77EB30EEC14427193E20F40CDB2E4C31ED11AB39CB880538FDF4BAC2681'
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
    Write-Host "Microsoft WARP 1.0.21 ($Architecture), file version $((Get-Item -LiteralPath $warpPath).VersionInfo.FileVersion)"
} finally { $package.Dispose() }
