# Forza RX 580 FH201/FH205 Fix

Experimental fix for **AMD RX 580 / Polaris** GPUs that encounter **FH201** or **FH205** errors when launching the game.

Created by **João Lucas**.

YouTube channel: [https://www.youtube.com/@MEGADROIDGAMESS](https://www.youtube.com/@MEGADROIDGAMESS)

---

## Before you start

This fix is intended for users who installed the **AMD Agility SDK / Work Graphs driver** and are still stuck on the **FH201** error.

Tested driver version:

```text
amd-software-adrenalin-edition-23.10.01.14-win10-win11-work-graphs
```

Important:

* This fix is experimental.
* It may not work on all GPUs or game versions.
* It does not include any game files.
* It does not remove DRM.
* Use at your own risk.

---

## Installation

1. Download this repository.

2. Open the folder:

```text
bin
```

3. Copy the file:

```text
d3d12.dll
```

4. Paste it into the main game directory (where the executable is located).

Example:

```text
...\Forza Horizon 6\d3d12.dll
```

The correct folder is the one containing the game executable and multiple `.dll` files.

5. Launch the game normally.

---

## How to verify it is working

If you previously saw:

```text
FH201
FH205
```

and the game now launches, the fix is being loaded correctly.

The fix also creates a log file in the game directory:

```text
ForzaFix_RX580.log
```

If this file appears, it means the DLL was loaded successfully.

---

## If FH205 still appears

The **FH205** error usually means the driver is not reporting support for **Enhanced Barriers**.

In this case:

* Install the AMD Agility SDK / Work Graphs driver
* Restart your PC

Then run:

```text
tools\D3D12Caps.exe
```

Look for this line:

```text
OPTIONS12: OK EnhancedBarriersSupported=TRUE
```

If it shows `FALSE`, your driver does not have the required support enabled.

---

## If you get FHE01 or the game crashes

Try:

* Verify the game files integrity
* Remove other mods/fixes
* Remove `dxgi.dll`, ReShade, OptiScaler, or any other third-party DLLs from the game folder
* Test with a clean game installation and only this `d3d12.dll`

---

## How to uninstall the fix

Delete this file from the game folder:

```text
d3d12.dll
```

Done. The game will revert to the default Windows DirectX runtime.

---

## For developers

The source code is located in:

```text
src
```

To build it, install:

* Visual Studio 2022 Build Tools
* Desktop development with C++
* Windows SDK

Then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\build_proxy.ps1
```

The compiled DLL will be generated at:

```text
proxy_build\d3d12.dll
```

---

## What this fix does

It creates a proxy `d3d12.dll`.

This DLL intercepts certain DirectX 12 calls and forces the game to detect **Feature Level 12_1**, helping bypass the **FH201** error.

Support for **Enhanced Barriers** must come from the driver. That is why the AMD Agility SDK / Work Graphs driver is required.
