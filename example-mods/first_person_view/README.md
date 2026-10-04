# First-person view

Restores the first-person camera on Steam client data revision 1076226. The
original camera signature is absent on this client, and the old activation path
could dereference an uninitialized hook when no match was found.

The mod now checks the executable layout before installing its camera hook.
Unsupported layouts log a message and remain inactive. Older clients retain the
original hook when its signature occurs exactly once in executable code.

## Configuration

In `shroudtopia.json`:

```json
{
  "mods": {
    "FirstPersonView": {
      "active": true,
      "camera_distance": -0.15
    }
  }
}
```

On the supported current client, `camera_distance` is measured in metres from the
native camera pivot. The default is `-0.15`. Increasing the value moves the camera
backward; `0.0` reaches the pivot. Values from `-0.5` to `0.5` are accepted. Moving
back can reveal the character's head.

Shroudtopia applies saved config changes on its update loop. Distance changes
update an aligned data value atomically, without rewriting hook instructions.
Non-finite or out-of-range values retain the previous distance. Removing the
setting restores the default; loader-handled missing or wrong-type values also
use that default. The legacy hook retains its original fixed camera behaviour.
Replacing the DLL requires a game restart.

## Camera hook

The current-client path verifies PE timestamp 1782724296, image size 0x2DA7000,
the named `player_camera_transform` registry descriptor, the original call and
callee entry, and the camera-distance field read.

The local-player camera system at RVA 0x2662A0 calls the native transform at
0xCB9C10 from 0x2663D3. At that call, RDX points to `LocalPlayerCamera` and RCX to
the output transform. The wrapper sets the horizontal offset at +0xED4 to zero
and the distance at +0xEF0 to the configured value, then calls the native
calculation. Pivot height, FOV and orientation stay under the game's control.

The minimap hooks the system entry. This mod patches the separate interior call,
so the two hooks can coexist. An already modified call site is rejected.

## Build and verification

Build with Visual Studio 2022 C++ tools and a Windows SDK. From the repository root
in a developer PowerShell session:

```powershell
MSBuild example-mods\first_person_view\first_person_view.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /p:SolutionDir="$PWD\"
MSBuild example-mods\first_person_view\tests\camera_test.vcxproj /p:Configuration=Release /p:Platform=x64
.\example-mods\first_person_view\build\tests\camera_test.exe
```

Release x64 builds and all 32 focused checks passed with MSVC v143 and Windows
SDK 10.0.26100.0. The checks execute the detour in an isolated process with
synthetic camera data. They cover layout rejection, minimap hook coexistence,
camera-field preservation, original-call restoration, missing-hook lifecycle,
config validation and live distance changes. A concurrency check runs 100,000
camera calls while changing distance 20,000 times.

First-person gameplay and live distance adjustment have also been confirmed on
the tested Steam client. Synthetic tests do not establish graphics-driver or
long-session stability. HUD placement is unchanged: a player has reported the
stamina gauge appearing near the top-left in first person.
