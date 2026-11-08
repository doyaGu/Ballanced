param(
    [Parameter(Mandatory = $true)][ValidateSet('x86', 'x64', 'arm64')][string]$Architecture,
    [Parameter(Mandatory = $true)][string]$DestinationDirectory
)

$ErrorActionPreference = 'Stop'
$destination = (New-Item -ItemType Directory -Force -Path $DestinationDirectory).FullName
$packagePath = Join-Path ([IO.Path]::GetTempPath()) 'Ballanced-D3D12-1.620.1.nupkg'
Invoke-WebRequest -Uri 'https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.d3d12/1.620.1/microsoft.direct3d.d3d12.1.620.1.nupkg' -OutFile $packagePath
if ((Get-FileHash -LiteralPath $packagePath -Algorithm SHA256).Hash -ne 'A8A494C3E5F687D9BE0A465EC8248DA70781AB689D888DB8806B600422722BE6') {
    throw 'Microsoft Direct3D 12 package checksum mismatch'
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$package = [IO.Compression.ZipFile]::OpenRead($packagePath)
try {
    $packageArchitecture = if ($Architecture -eq 'x86') { 'win32' } else { $Architecture }
    foreach ($name in @('D3D12Core.dll', 'd3d12SDKLayers.dll', 'd3dconfig.exe')) {
        [IO.Compression.ZipFileExtensions]::ExtractToFile($package.GetEntry("build/native/bin/$packageArchitecture/$name"), (Join-Path $destination $name), $true)
    }
    [IO.Compression.ZipFileExtensions]::ExtractToFile($package.GetEntry('LICENSE.txt'), (Join-Path $destination 'D3D12-LICENSE.txt'), $true)
    Write-Host "Microsoft Direct3D 12 Agility SDK 1.620.1 ($Architecture)"
} finally { $package.Dispose() }
