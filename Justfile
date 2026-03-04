set windows-shell := ["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command"]

default:
    @just --list

[windows]
build:
    @echo "{{BOLD + ITALIC + BLUE}}Launching project{{NORMAL}}"
    @& 'C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' (Resolve-Path ".\src\Apesack_Pickleballs.uproject").Path
