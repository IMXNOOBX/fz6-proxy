# 📍 Forza RX 570/580 FH201/FH205 Fix

This is a fork of [Forza-Horizon-6-RX-580-FH201-FH205-Fix](https://github.com/Megadroidgames/Forza-Horizon-6-RX-580-FH201-FH205-Fix) with the compatibility for RX 570 GPU cards. Its an experimental fix for **FH201** or **FH205** errors when launching the game. Im no expert in the field, i suggest checking the original project for more information.

## 🕹️ Instructions

> This are the steps that best worked for me, this is an experimental fix, because microslop doesnt know how to properly develop games fro their own plataform.

1. Verify you have the following version of amd drivers: [`amd-software-adrenalin-edition-23.10.01.14-win10-win11-work-graphs (direct download)`](https://drivers.amd.com/drivers/amd-software-adrenalin-edition-23.10.01.14-win10-win11-work-graphs.exe).
    1.1. If you have any issues installing this version, i suggest selecting `minimal install` or using [Display Driver Unintaller](https://github.com/Wagnard/display-drivers-uninstaller)

2. Download or Compile (preferred) the file: `d3d12.dll`.
3. Move the file to your root game directory (ex: `...\WindowsApps\Microsoft.ForteBaseGame_3.360.259.0_x64__xxxxxxxxxxx`) (*)
4. Run the game as usual

(*) *Windows protects the `WindowsApps` folder setting `TrustedInstaller` as owner, so you will have to change permissions to your user for you to access the folder*

## ❗More info

More info on [The Original Documentation (EN)](./README.en.md) or [The Original Documentation (PT)](./README.pt.md)