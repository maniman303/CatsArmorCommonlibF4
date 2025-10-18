<#
.SYNOPSIS
  Create symlinks for Cats Armor files into a Fallout 4 Data folder.
  Requires admin privileges unless Developer Mode is enabled.

.PARAMETER TargetPath
  Destination Data folder, e.g. "D:\SteamLibrary\steamapps\common\Fallout 4\Data"

.EXAMPLE
  .\install-cats.ps1 -TargetPath 'D:\SteamLibrary\steamapps\common\Fallout 4\Data'
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
)

# --- Check if running as admin ---
$principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)) {
    Write-Host "This script must be run as Administrator (or with Developer Mode enabled)." -ForegroundColor Red
    Write-Host "Right-click PowerShell → 'Run as administrator' and try again."
    exit 10
}

function Is-ReparsePoint($path) {
    if (-not (Test-Path -LiteralPath $path)) { return $false }
    $it = Get-Item -LiteralPath $path -Force
    return (($it.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0)
}

# --- Resolve and validate target path safely ---
$resolved = $null
try {
    $resolved = Resolve-Path -LiteralPath $TargetPath -ErrorAction Stop
} catch {
    Write-Error "Target path does not exist or cannot be resolved: '$TargetPath'. Exiting."
    exit 1
}
if (-not $resolved -or -not $resolved.Path) {
    Write-Error "Target path resolution returned no result. Exiting."
    exit 1
}
$TargetPath = $resolved.Path.Trim()
Write-Host "Target path resolved to: $TargetPath"

# Script directory
$ScriptDir = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Definition }
Write-Host "Script directory: $ScriptDir"

# Source items
$srcMeshes = Join-Path $ScriptDir 'meshes\armor\CatsArmor'
$srcScripts = Join-Path $ScriptDir 'scripts\source\user\CATS'

# Destination items
$dstMeshes = Join-Path $TargetPath 'meshes\armor\CatsArmor'
$dstScripts = Join-Path $TargetPath 'scripts\source\user\CATS'

Write-Host "Will create symlinks:"
Write-Host "  Meshes:  $dstMeshes -> $srcMeshes"
Write-Host "  Scripts: $dstScripts -> $srcScripts"

# Validate sources
if (-not (Test-Path -LiteralPath $srcMeshes -PathType Container)) {
    Write-Error "Source meshes folder not found: $srcMeshes. Exiting."
    exit 3
}
if (-not (Test-Path -LiteralPath $srcScripts -PathType Container)) {
    Write-Error "Source scripts folder not found: $srcScripts. Exiting."
    exit 4
}

# Helper to backup or remove existing destination
function Prepare-Destination($dst) {
    if ($null -eq $dst -or $dst -eq '') { return }
    if (Test-Path -LiteralPath $dst) {
        if (Is-ReparsePoint $dst) {
            Write-Host "Removing existing symlink: $dst"
            Remove-Item -LiteralPath $dst -Force -Recurse
        }
        else {
            $bak = "$dst.bak_$(Get-Date -Format yyyyMMddHHmmss)"
            Write-Host "Renaming existing item to: $bak"
            try {
                Rename-Item -LiteralPath $dst -NewName (Split-Path -Leaf $bak) -Force
                if (-not (Test-Path -LiteralPath $bak)) {
                    Move-Item -LiteralPath $dst -Destination $bak -Force
                }
            } catch {
                Write-Warning "Failed to rename existing item. Will try to remove it instead."
                Remove-Item -LiteralPath $dst -Force -Recurse
            }
        }
    }
}

# Ensure parent folders exist
foreach ($folder in @(
    (Split-Path -Parent $dstMeshes),
    (Split-Path -Parent $dstScripts)
)) {
    if (-not [string]::IsNullOrEmpty($folder) -and -not (Test-Path -LiteralPath $folder)) {
        Write-Host "Creating folder: $folder"
        New-Item -ItemType Directory -Path $folder -Force | Out-Null
    }
}

# Prepare (backup/remove) existing items
Prepare-Destination -dst $dstMeshes
Prepare-Destination -dst $dstScripts

# Create symlinks
try {
    Write-Host "Creating directory symlink: $dstMeshes -> $srcMeshes"
    New-Item -ItemType SymbolicLink -Path $dstMeshes -Target $srcMeshes -Force | Out-Null
} catch {
    Write-Error "Failed to create directory symlink: $_"
    exit 6
}

try {
    Write-Host "Creating directory symlink: $dstScripts -> $srcScripts"
    New-Item -ItemType SymbolicLink -Path $dstScripts -Target $srcScripts -Force | Out-Null
} catch {
    Write-Error "Failed to create directory symlink: $_"
    exit 7
}

Write-Host "Symlinks created successfully." -ForegroundColor Green
exit 0
