// ge - ReXGlue Recompiled Project
//
// Device detection (see ge_device.h). The firmware strings come from
// HKLM\HARDWARE\DESCRIPTION\System\BIOS, which Windows fills from SMBIOS and
// Wine fills from the same tables (wineboot), so the Legion Go reads the same
// under Windows and under Proton on Bazzite or SteamOS.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cctype>
#include <cstring>
#include <string>

#include "ge_device.h"

namespace ge {
namespace {

std::string ReadBiosString(const wchar_t* value_name) {
  wchar_t wide[256] = {};
  DWORD size = sizeof(wide);
  if (RegGetValueW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", value_name,
                   RRF_RT_REG_SZ, nullptr, wide, &size) != ERROR_SUCCESS) {
    return std::string();
  }
  char utf8[512] = {};
  WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, sizeof(utf8) - 1, nullptr, nullptr);
  std::string out(utf8);
  while (!out.empty() && (out.back() == ' ' || out.back() == '\t')) out.pop_back();
  return out;
}

std::string Lower(std::string s) {
  for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

bool Contains(const std::string& haystack, const char* needle) {
  return haystack.find(needle) != std::string::npos;
}

bool StartsWith(const std::string& s, const char* prefix) {
  return s.compare(0, std::strlen(prefix), prefix) == 0;
}

// The short name of a known handheld, or "" for anything else. The model
// line Lenovo writes into SystemVersion ("Legion Go 8APU1", "Legion Go S
// 8ARP1") is the reliable part; the machine-type codes in SystemProductName
// are a fallback for firmware that leaves the version blank.
std::string HandheldName(const DeviceInfo& d) {
  const std::string maker = Lower(d.manufacturer);
  const std::string product = Lower(d.product);
  const std::string version = Lower(d.version);
  if (Contains(maker, "lenovo")) {
    if (Contains(version, "legion go 2")) return "Legion Go 2";
    if (Contains(version, "legion go s")) return "Legion Go S";
    if (Contains(version, "legion go")) return "Legion Go";
    if (product == "83e1") return "Legion Go";
    if (product == "83l3" || product == "83n6") return "Legion Go S";
    return std::string();
  }
  if (Contains(maker, "valve")) {
    // Jupiter is the LCD Steam Deck, Galileo the OLED.
    if (Contains(product, "jupiter") || Contains(product, "galileo")) return "Steam Deck";
    return std::string();
  }
  if (Contains(maker, "asus")) {
    // RC71L is the ROG Ally, RC72LA the Ally X.
    if (StartsWith(product, "rc71") || StartsWith(product, "rc72")) return "ROG Ally";
    return std::string();
  }
  return std::string();
}

bool RunningUnderWine() {
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  return ntdll && GetProcAddress(ntdll, "wine_get_version") != nullptr;
}

}  // namespace

const DeviceInfo& DetectDevice() {
  static const DeviceInfo info = [] {
    DeviceInfo d;
    d.manufacturer = ReadBiosString(L"SystemManufacturer");
    d.product = ReadBiosString(L"SystemProductName");
    d.version = ReadBiosString(L"SystemVersion");
    d.name = HandheldName(d);
    d.handheld = !d.name.empty();
    d.wine = RunningUnderWine();
    return d;
  }();
  return info;
}

}  // namespace ge
