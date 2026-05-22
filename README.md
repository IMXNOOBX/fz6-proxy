# 📍 Forza RX 570/580 FH201/FH205 Fix

This is a fork of [Forza-Horizon-6-RX-580-FH201-FH205-Fix](https://github.com/Megadroidgames/Forza-Horizon-6-RX-580-FH201-FH205-Fix) with compatibility for RX 570 GPU cards. It's an experimental fix for **FH201** or **FH205** errors when launching the game. I'm no expert in the field; I suggest checking the original project for more information.

## 🕹️ Instructions

> These are the steps that worked best for me. This is an experimental fix because Microsoft doesn't know how to properly develop games for their own platform.

1. Verify you have the following version of AMD drivers: [`amd-software-adrenalin-edition-23.10.01.14-win10-win11-work-graphs (direct download)`](https://drivers.amd.com/drivers/amd-software-adrenalin-edition-23.10.01.14-win10-win11-work-graphs.exe).
   - If you have any issues installing this version, I suggest selecting `minimal install` or using [Display Driver Uninstaller](https://github.com/Wagnard/display-drivers-uninstaller).

2. Download or compile (preferred) the file: `d3d12.dll`.
3. Move the file to your root game directory (e.g., `...\WindowsApps\Microsoft.ForteBaseGame_3.360.259.0_x64__xxxxxxxxxxx`) (*)
4. Run the game as usual.

(*) *Windows protects the `WindowsApps` folder by setting `TrustedInstaller` as the owner, so you will have to change permissions to your user to access the folder.*

## ❗More Info

More info on [The Original Documentation (EN)](./README.en.md) or [The Original Documentation (PT)](./README.pt.md)