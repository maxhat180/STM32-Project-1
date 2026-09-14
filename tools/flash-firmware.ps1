[CmdletBinding(SupportsShouldProcess)]
param(
    [Parameter()]
    [string]$ArtifactPath,

    [Parameter()]
    [string]$ProgrammerPath
)

$ErrorActionPreference = 'Stop'

function Resolve-Stm32Programmer {
    param([string]$RequestedPath)

    if ($RequestedPath) {
        return (Resolve-Path -LiteralPath $RequestedPath).Path
    }

    $installedCommand = Get-Command 'STM32_Programmer_CLI.exe' -ErrorAction SilentlyContinue
    if ($installedCommand) {
        return $installedCommand.Source
    }

    $standardPath = Join-Path ${env:ProgramFiles} `
        'STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe'
    if (Test-Path -LiteralPath $standardPath -PathType Leaf) {
        return $standardPath
    }

    throw 'STM32CubeProgrammer CLI was not found. Install it or pass -ProgrammerPath.'
}

function Find-LatestDownloadedArtifact {
    $downloadsPath = Join-Path ([Environment]::GetFolderPath('UserProfile')) 'Downloads'
    if (-not (Test-Path -LiteralPath $downloadsPath -PathType Container)) {
        throw "Downloads folder was not found at: $downloadsPath"
    }

    $artifacts = @(
        Get-ChildItem -LiteralPath $downloadsPath -File -Filter 'stm32-telemetry-*.zip' |
            Where-Object {
                $_.Name -match '^stm32-telemetry-\d{8}-\d{6}-utc(?: \(\d+\))?\.zip$'
            } |
            Sort-Object LastWriteTimeUtc -Descending
    )

    if ($artifacts.Count -eq 0) {
        throw "No stm32-telemetry-*-utc.zip artifact was found in: $downloadsPath"
    }

    return $artifacts[0].FullName
}

function Confirm-FirmwareChecksum {
    param(
        [string]$PackageDirectory,
        [string]$FirmwarePath
    )

    $checksumFiles = @(
        Get-ChildItem -LiteralPath $PackageDirectory -File -Recurse -Filter 'SHA256SUMS.txt'
    )
    if ($checksumFiles.Count -ne 1) {
        throw "Expected one SHA256SUMS.txt file, found $($checksumFiles.Count)."
    }

    $checksumLine = Get-Content -LiteralPath $checksumFiles[0].FullName |
        Where-Object { $_ -match '\s+\*?stm32_telemetry\.hex$' } |
        Select-Object -First 1

    if (-not $checksumLine -or
        $checksumLine -notmatch '^([0-9a-fA-F]{64})\s+\*?stm32_telemetry\.hex$') {
        throw 'The expected checksum for stm32_telemetry.hex is missing or malformed.'
    }

    $expectedHash = $Matches[1]
    $actualHash = (Get-FileHash -LiteralPath $FirmwarePath -Algorithm SHA256).Hash
    if (-not $actualHash.Equals($expectedHash, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Firmware checksum verification failed. Do not flash this artifact.'
    }

    Write-Host "SHA-256 verified: $actualHash"
}

$temporaryDirectory = $null

try {
    $firmwareBesideScript = Join-Path $PSScriptRoot 'stm32_telemetry.hex'

    if ($ArtifactPath) {
        $resolvedArtifactPath = (Resolve-Path -LiteralPath $ArtifactPath).Path
    }
    elseif (Test-Path -LiteralPath $firmwareBesideScript -PathType Leaf) {
        $resolvedArtifactPath = $PSScriptRoot
    }
    else {
        $resolvedArtifactPath = Find-LatestDownloadedArtifact
    }

    $artifactItem = Get-Item -LiteralPath $resolvedArtifactPath
    if ($artifactItem.PSIsContainer) {
        $packageDirectory = $artifactItem.FullName
    }
    elseif ($artifactItem.Extension -ieq '.zip') {
        $temporaryDirectory = Join-Path ([IO.Path]::GetTempPath()) `
            ("stm32-telemetry-flash-{0}" -f [Guid]::NewGuid().ToString('N'))
        New-Item -ItemType Directory -Path $temporaryDirectory | Out-Null
        Expand-Archive -LiteralPath $artifactItem.FullName -DestinationPath $temporaryDirectory
        $packageDirectory = $temporaryDirectory
    }
    else {
        throw 'ArtifactPath must refer to a timestamped ZIP artifact or extracted directory.'
    }

    $firmwareFiles = @(
        Get-ChildItem -LiteralPath $packageDirectory -File -Recurse -Filter 'stm32_telemetry.hex'
    )
    if ($firmwareFiles.Count -ne 1) {
        throw "Expected one stm32_telemetry.hex file, found $($firmwareFiles.Count)."
    }

    $firmwarePath = $firmwareFiles[0].FullName
    Confirm-FirmwareChecksum -PackageDirectory $packageDirectory -FirmwarePath $firmwarePath

    $resolvedProgrammerPath = Resolve-Stm32Programmer -RequestedPath $ProgrammerPath
    Write-Host "Artifact: $resolvedArtifactPath"
    Write-Host "Firmware: $firmwarePath"
    Write-Host 'Flashing over ST-LINK/SWD...'

    if ($PSCmdlet.ShouldProcess($firmwarePath, 'Flash, verify, and reset STM32')) {
        & $resolvedProgrammerPath -c 'port=SWD' -w $firmwarePath -v -rst
        if ($LASTEXITCODE -ne 0) {
            throw "STM32CubeProgrammer failed with exit code $LASTEXITCODE."
        }

        Write-Host 'Firmware write, verification, and reset completed successfully.'
    }
}
finally {
    if ($temporaryDirectory -and (Test-Path -LiteralPath $temporaryDirectory)) {
        $tempRoot = [IO.Path]::GetFullPath([IO.Path]::GetTempPath())
        $resolvedTemporaryDirectory = [IO.Path]::GetFullPath($temporaryDirectory)
        $temporaryLeaf = Split-Path -Leaf $resolvedTemporaryDirectory

        if ($resolvedTemporaryDirectory.StartsWith($tempRoot, [StringComparison]::OrdinalIgnoreCase) -and
            $temporaryLeaf -like 'stm32-telemetry-flash-*') {
            Remove-Item -LiteralPath $resolvedTemporaryDirectory -Recurse -Force
        }
    }
}
