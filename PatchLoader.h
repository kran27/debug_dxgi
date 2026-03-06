#pragma once
#include "console.h"
#include "functions.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

struct Patch {
  std::string name;
  uintptr_t address;
  std::vector<uint8_t> bytes;
};

struct PatchFile {
  std::string game;
  std::string version;
  DWORD timestamp = 0;
  int delay = 0;
  std::vector<Patch> patches;
};

// Helper to parse hex string tokens (e.g. "E9", "97", "03", "00", "00")
inline std::vector<uint8_t> ParseBytes(const std::string &line) {
  std::vector<uint8_t> bytes;
  std::stringstream ss(line);
  std::string token;
  while (ss >> token) {
    try {
      bytes.push_back(static_cast<uint8_t>(std::stoul(token, nullptr, 16)));
    } catch (...) {
      // ignore invalid errors for now, or log
    }
  }
  return bytes;
}

class PatchLoader {
public:
  static void ApplyPatches() {
    // Ensure patches directory exists
    if (!fs::exists("patches")) {
      fs::create_directory("patches");
      return;
    }

    const auto currentGame = get_pdb_path();
    const auto currentFallbackName = get_product_name();
    const auto currentVersion = get_exe_version();
    const auto currentTimestamp = get_pe_timestamp();

    for (const auto &entry : fs::directory_iterator("patches")) {
      if (entry.path().extension() == ".txt") {
        LoadAndApplyFile(entry.path(), currentGame, currentFallbackName,
                         currentVersion, currentTimestamp);
      }
    }
  }

private:
  static void LoadAndApplyFile(const fs::path &path,
                               const std::string &currGame,
                               const std::string &currFallback,
                               const std::string &currVer, DWORD currTime) {
    std::ifstream file(path);
    if (!file.is_open())
      return;

    PatchFile pf;
    std::string line;

    Patch currentPatch;
    bool inPatch = false;

    while (std::getline(file, line)) {
      // Trim whitespace
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n") + 1);

      if (line.empty() || line[0] == '#')
        continue;

      std::stringstream ss(line);
      std::string key;
      std::getline(ss, key, ':');

      std::string value;
      if (std::getline(ss, value)) {
        value.erase(0, value.find_first_not_of(" \t"));
      }

      if (key == "Game")
        pf.game = value;
      else if (key == "Version")
        pf.version = value;
      else if (key == "Timestamp") {
        try {
          // Check for hex prefix or default to decimal (or hex if no prefix
          // logic desired, but stoul handles 0x automatically with base 0)
          if (value.find("0x") == 0)
            pf.timestamp = std::stoul(value, nullptr, 16);
          else
            pf.timestamp = std::stoul(value);
        } catch (...) {
        }
      } else if (key == "Delay") {
        try {
          pf.delay = std::stoi(value);
        } catch (...) {
        }
      } else if (key == "Patch") {
        if (inPatch) {
          pf.patches.push_back(currentPatch);
          currentPatch = {};
        }
        currentPatch.name = value;
        inPatch = true;
      } else if (key == "Address") {
        try {
          currentPatch.address = std::stoul(value, nullptr, 16);
        } catch (...) {
        }
      } else if (key == "Bytes") {
        currentPatch.bytes = ParseBytes(value);
      }
    }
    // Push last patch
    if (inPatch) {
      pf.patches.push_back(currentPatch);
    }

    // Match logic
    bool gameMatch = false;
    if (!pf.game.empty()) {
      if (pf.game == currGame || pf.game == currFallback) {
        gameMatch = true;
      }
    } else {
      return;
    }

    if (!gameMatch)
      return;

    if (!pf.version.empty() && pf.version != currVer)
      return;
    if (pf.timestamp != 0 && pf.timestamp != currTime)
      return;

    // Apply patches
    auto apply = [pf, path]() {
      consoleQueue.push(
          "Applying matches from " + path.filename().string() + "\n", cyan);
      for (const auto &p : pf.patches) {
        if (p.address != 0 && !p.bytes.empty()) {
          apply_byte_patch(p.address, p.bytes, p.name);
        }
      }
    };

    if (pf.delay > 0) {
      std::thread([apply, delay = pf.delay]() {
        Sleep(delay);
        apply();
      }).detach();
    } else {
      apply();
    }
  }
};
