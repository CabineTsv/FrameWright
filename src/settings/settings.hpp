#pragma once

#include <Geode/Geode.hpp>

class Bot;

class Settings {
  public:
    static void applyDefaults();
    static void loadRuntimeState(Bot& bot);
};
