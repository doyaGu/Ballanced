param(
    [ValidateRange(1, 10)]
    [int]$Attempts = 3,

    [ValidateRange(1, 16)]
    [int]$Jobs = 4
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

git submodule sync --recursive
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

for ($attempt = 1; $attempt -le $Attempts; $attempt++) {
    git -c protocol.version=2 submodule update --init --force --recursive --jobs $Jobs
    $updateExitCode = $LASTEXITCODE
    if ($updateExitCode -eq 0) {
        exit 0
    }

    if ($attempt -lt $Attempts) {
        $delaySeconds = [Math]::Pow(2, $attempt)
        Write-Host "Submodule checkout attempt $attempt failed; retrying in $delaySeconds seconds."
        Start-Sleep -Seconds $delaySeconds
    }
}

exit $updateExitCode
