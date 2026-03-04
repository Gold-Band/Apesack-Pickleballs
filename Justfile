set dotenv-load
set export

set windows-shell := ["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command"]

ue_path_win := env_var_or_default("UE_PATH", "C:\\Program Files\\Epic Games\\UE_5.4\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe")
ue_path_unix := env_var_or_default("UE_PATH", "~/Downloads/Engine/Binaries/Linux/UnrealEditor")

[private]
default:
    @just --list

# Opens the project
[windows]
open:
    @echo "{{BOLD + ITALIC + BLUE}}Launching project{{NORMAL}}"
    @& "{{ue_path_win}}" (Resolve-Path ".\src\Apesack_Pickleballs.uproject").Path

# Opens the project
[unix]
open:
	@echo "{{BOLD + ITALIC + BLUE}}Launching project{{NORMAL}}"
	"${{ue_path_unix}}" "$(shell pwd)/src/Apesack_Pickleballs.uproject"

# Deletes Binaries directory
[windows]
clean:
    @echo "{{BOLD + ITALIC + RED}}Cleaning project{{NORMAL}}"
    @if (Test-Path .\src\Binaries) { Remove-Item .\src\Binaries -Recurse -Force }

# Deletes Binaries directory
[unix]
clean:
    @echo "{{BOLD + ITALIC + RED}}Cleaning project{{NORMAL}}"
    @rm -rf ./src/Binaries
