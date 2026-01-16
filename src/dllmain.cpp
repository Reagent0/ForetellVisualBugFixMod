#include "ForetellVisualBugFixMod.hpp"

extern "C"
{
    __declspec(dllexport) RC::CppUserModBase* start_mod()
    {
        return new ForetellVisualBugFixMod::ForetellVisualBugFixMod();
    }

    __declspec(dllexport) void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}