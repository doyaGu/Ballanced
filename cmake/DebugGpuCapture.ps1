param(
    [Parameter(Mandatory = $true)][string]$BinaryDirectory,
    [Parameter(Mandatory = $true)][string]$OutputDirectory,
    [string]$Debugger,
    [switch]$FirstChance,
    [switch]$TracePresent,
    [ValidateRange(1, 10000)][int]$Frames = 5,
    [ValidatePattern('^[A-Za-z0-9_]+$')][string]$Scene = 'opaque_lit'
)

$ErrorActionPreference = 'Stop'
$binaryRoot = (Resolve-Path -LiteralPath $BinaryDirectory).Path
$outputRoot = (New-Item -ItemType Directory -Force -Path $OutputDirectory).FullName
foreach ($name in @('debugger.log', 'crash.dmp', "$Scene.png")) {
    if (Test-Path -LiteralPath (Join-Path $outputRoot $name)) {
        throw 'GPU crash diagnostics require a fresh output directory'
    }
}
$tool = Join-Path $binaryRoot 'ckre_scene_capture.exe'
if (-not (Test-Path -LiteralPath $tool -PathType Leaf)) {
    throw "Scene capture executable not found: $tool"
}

if (-not $Debugger) {
    # Extract Microsoft's signed debugger package without installing it or
    # changing the machine's postmortem debugger settings.
    $downloadRoot = (New-Item -ItemType Directory -Path (Join-Path ([IO.Path]::GetTempPath()) "Ballanced-WinDbg-$([Guid]::NewGuid())")).FullName
    $bundlePath = Join-Path $downloadRoot 'windbg.msixbundle'
    Invoke-WebRequest -Uri 'https://windbg.download.prss.microsoft.com/dbazure/prod/1-2611-4001-0/windbg.msixbundle' -OutFile $bundlePath
    $expectedHash = '158F6365F365A939429977F7BC4CA3D15F8C2A39BC3ACD2B297FC36A31D69274'
    if ((Get-FileHash -LiteralPath $bundlePath -Algorithm SHA256).Hash -ne $expectedHash) {
        throw 'Microsoft debugger package checksum mismatch'
    }
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $architecture = if ([Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq 'Arm64') { 'arm64' } else { 'x64' }
    $bundle = [IO.Compression.ZipFile]::OpenRead($bundlePath)
    try {
        $manifestReader = [IO.StreamReader]::new($bundle.GetEntry('AppxMetadata/AppxBundleManifest.xml').Open())
        try { [xml]$manifest = $manifestReader.ReadToEnd() } finally { $manifestReader.Dispose() }
        $package = @($manifest.Bundle.Packages.Package | Where-Object { $_.Type -eq 'application' -and $_.Architecture -eq $architecture })
        if ($package.Count -ne 1) { throw "Expected one $architecture debugger package" }
        $packagePath = Join-Path $downloadRoot 'windbg.msix'
        [IO.Compression.ZipFileExtensions]::ExtractToFile($bundle.GetEntry($package[0].FileName), $packagePath, $true)
    } finally { $bundle.Dispose() }
    $debuggerRoot = Join-Path $downloadRoot 'windbg'
    [IO.Compression.ZipFile]::ExtractToDirectory($packagePath, $debuggerRoot)
    $debuggerArchitecture = if ($architecture -eq 'arm64') { 'arm64' } else { 'amd64' }
    $Debugger = Join-Path $debuggerRoot "$debuggerArchitecture/cdb.exe"
    if (-not (Test-Path -LiteralPath $Debugger -PathType Leaf)) { throw 'CDB not found in the Microsoft debugger package' }
    # Keep only logs, frames and the minidump in the published diagnostics.
    Remove-Item -LiteralPath $bundlePath, $packagePath
}

$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = (Resolve-Path -LiteralPath $Debugger).Path
$start.WorkingDirectory = $outputRoot
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$exceptionFilter = if ($FirstChance) { 'sxe -c' } else { 'sxd -c2' }
$commands = "sxi ibp; sxi 0x4000001f; $exceptionFilter `"r; .exr -1; kv; lmvm d3d10warp; .dump /m /o crash.dmp; q`" av; g"
if ($TracePresent) {
    $resultRegister = if ([Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq 'Arm64') { 'x0' } else { 'eax' }
    $trace = 'sxe -c "bm /d dxgi!CDXGISwapChain::PresentImpl \"gu; r ' + $resultRegister + '; g\"; g" ld:dxgi.dll; '
    $commands = $trace + $commands
}
$symbolCache = Join-Path ([IO.Path]::GetTempPath()) 'Ballanced-DebugSymbols'
$arguments = @('-G', '-o', '-y', "$binaryRoot;srv*$symbolCache*https://msdl.microsoft.com/download/symbols", '-logo', 'debugger.log', '-c',
    $commands,
    $tool, '--render-engine-dir', $binaryRoot, '--rasterizer', 'sdlgpu',
    '--scene', $Scene, '--frames', [string]$Frames, '--out', $outputRoot)
foreach ($argument in $arguments) { $start.ArgumentList.Add($argument) }
$process = [Diagnostics.Process]::Start($start)
try {
    if (-not $process.WaitForExit(180000)) {
        $process.Kill($true)
        throw 'GPU capture under CDB exceeded three minutes'
    }
    if (Test-Path -LiteralPath (Join-Path $outputRoot 'crash.dmp')) {
        throw 'GPU capture crashed; see debugger.log and crash.dmp'
    }
    if ($process.ExitCode -ne 0 -or -not (Test-Path -LiteralPath (Join-Path $outputRoot "$Scene.png"))) {
        throw "CDB did not complete the GPU capture (exit $($process.ExitCode))"
    }
} finally { $process.Dispose() }
