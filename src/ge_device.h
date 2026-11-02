// ge - ReXGlue Recompiled Project
//
// What the game is running on, for the handheld preset and the log header:
// the machine's make and model from the firmware (SMBIOS, as Windows and Wine
// both publish it in the registry) and whether this is Wine or Proton rather
// than Windows. Plain Win32 and C++17 so the launcher (packaging/launcher)
// builds it too; nothing here touches the SDK.

#pragma once

#include <string>

namespace ge {

struct DeviceInfo {
  // A handheld PC this port has defaults for: Lenovo Legion Go (and Go S,
  // Go 2), Valve Steam Deck, ASUS ROG Ally. name is its short name then
  // ("Legion Go"); empty otherwise.
  bool handheld = false;
  std::string name;
  // The firmware's own strings, for the log ("" when not published).
  std::string manufacturer;  // SystemManufacturer, e.g. "LENOVO"
  std::string product;       // SystemProductName, e.g. "83E1"
  std::string version;       // SystemVersion, e.g. "Legion Go 8APU1"
  // Running under Wine or Proton (SteamOS, Bazzite, any Linux): ntdll exports
  // wine_get_version there and never on Windows.
  bool wine = false;
};

// Reads the firmware strings and Wine check once; later calls return the same.
const DeviceInfo& DetectDevice();

}  // namespace ge
