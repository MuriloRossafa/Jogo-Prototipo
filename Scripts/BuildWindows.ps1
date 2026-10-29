param(
    [ValidateSet('Diagnose','Prepare','Editor','Package')][string]$Mode = 'Diagnose',
    [string]$EngineRoot = $env:UE_ENGINE_ROOT
)
$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $ProjectRoot 'OtherSide.uproject'
$LogDir = Join-Path $ProjectRoot 'Logs'
New-Item -ItemType Directory -Force -Path $LogDir | Out-Null
$Started = $false
try {
    Start-Transcript -Path (Join-Path $LogDir ((Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' + $Mode + '.log')) | Out-Null
    $Started = $true
    if (-not $EngineRoot) {
        $DefaultEngine = Join-Path $env:ProgramFiles 'Epic Games/UE_5.6'
        if (Test-Path -LiteralPath $DefaultEngine) { $EngineRoot = $DefaultEngine }
    }
    if (-not $EngineRoot) { throw 'Set UE_ENGINE_ROOT or pass -EngineRoot with the actual Unreal Engine 5.6 directory.' }
    $Version = Get-Content -LiteralPath (Join-Path $EngineRoot 'Engine/Build/Build.version') -Raw | ConvertFrom-Json
    Write-Host "Detected Unreal $($Version.MajorVersion).$($Version.MinorVersion).$($Version.PatchVersion) at $EngineRoot"
    if ($Version.MajorVersion -ne 5 -or $Version.MinorVersion -ne 6) { throw 'This initial project targets UE 5.6. Other versions require a separate compatibility check.' }
    $Editor = Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
    $EditorCmd = Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
    $Build = Join-Path $EngineRoot 'Engine/Build/BatchFiles/Build.bat'
    $Uat = Join-Path $EngineRoot 'Engine/Build/BatchFiles/RunUAT.bat'
    foreach ($File in @($Editor, $EditorCmd, $Build, $Uat)) {
        if (-not (Test-Path -LiteralPath $File)) { throw "Required engine file missing: $File" }
    }
    if ($Mode -eq 'Diagnose') {
        $Vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
        if (-not (Test-Path -LiteralPath $Vswhere)) { throw 'Visual Studio Installer not found. Use official Visual Studio 2022 installer, Game development with C++, and Windows SDK compatible with this engine.' }
        $Vs = & $Vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if (-not $Vs) { throw 'MSVC C++ tools not found. Modify Visual Studio through its official installer.' }
        Write-Host "C++ installation: $Vs"
        $SdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits/10/Include'
        if (-not (Test-Path -LiteralPath $SdkRoot)) { throw 'Windows SDK headers not found in the standard Windows Kits location.' }
        Get-ChildItem -LiteralPath $SdkRoot -Directory | Select-Object -ExpandProperty Name | Write-Host
        Write-Host 'Paths detected. Actual compiler/SDK compatibility is checked by the build; nothing was installed.'
        return
    }
    & $Build OtherSideEditor Win64 Development "-Project=$Project" -WaitMutex -NoHotReloadFromIDE
    if ($LASTEXITCODE -ne 0) { throw "Editor build failed: $LASTEXITCODE" }
    & $EditorCmd $Project -run=pythonscript "-script=$(Join-Path $PSScriptRoot 'CreateAssets.py')" -unattended -NullRHI -NoSound -stdout -FullStdOutLogOutput
    if ($LASTEXITCODE -ne 0) { throw "Asset generation failed: $LASTEXITCODE" }
    foreach ($Asset in @('Content/Maps/Case001.umap','Content/Audio/Warning.uasset')) {
        if (-not (Test-Path -LiteralPath (Join-Path $ProjectRoot $Asset))) { throw "Asset missing: $Asset" }
    }
    if ($Mode -eq 'Editor') {
        Start-Process -FilePath $Editor -ArgumentList ('"' + $Project + '"')
    } elseif ($Mode -eq 'Package') {
        $Output = Join-Path $ProjectRoot 'Builds/Windows'
        & $Uat BuildCookRun "-project=$Project" -nop4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$Output" -prereqs -utf8output
        if ($LASTEXITCODE -ne 0) { throw "Packaging failed: $LASTEXITCODE" }
        $Executables = @(Get-ChildItem -LiteralPath $Output -Recurse -Filter OtherSide.exe)
        if ($Executables.Count -eq 0) { throw 'Packaging returned success but OtherSide.exe was not found.' }
        $Executables | Select-Object -ExpandProperty FullName | Write-Host
        Write-Host 'Distribute the complete packaged directory. Runtime validation is still required.'
    }
} catch {
    Write-Host $_ -ForegroundColor Red
    exit 1
} finally {
    if ($Started) { Stop-Transcript | Out-Null }
}
