#pragma once

#include <string>
#include <filesystem>
#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Helpers/String.hpp>

#define FVBFMOD_LOG(level, str, ...) \
    RC::Output::send<level>( \
        RC::StringType{STR("[ForetellVisualBugFixMod] (")} + \
        RC::ensure_str(std::filesystem::path(__FILE__).filename().wstring()) + \
        RC::StringType{STR(":")} + \
        RC::ensure_str(std::to_wstring(__LINE__)) + \
        RC::StringType{STR(") ")} + \
        RC::StringType{str} + \
        STR("\n"), \
        ##__VA_ARGS__ \
    );

#define FVBFMOD_RETIFNULL(ptr) \
    do { \
        if (ptr == nullptr) { \
            FVBFMOD_LOG(RC::LogLevel::Error, STR("{} is null!"), STR(#ptr)); \
            return; \
        } \
    } while(0)

// Not needed AFAIK, but keeping it anyways for future-proofing.
namespace RC::Unreal
{
    class UnrealScriptFunctionCallableContext;
}

namespace ForetellVisualBugFixMod
{

inline constexpr auto NoOpLambda = [](auto&&...) {};

namespace Paths
{
    inline constexpr auto *Fn_OnBattleDependenciesFullyLoaded = STR("/Game/jRPGTemplate/Blueprints/Components/AC_jRPG_BattleManager.AC_jRPG_BattleManager_C:OnBattleDependenciesFullyLoaded");
    inline constexpr auto *Fn_ApplyForetell = STR("/Game/Gameplay/Battle/UniqueMechanics/Foretell/BP_UniqueMechanic_Foretell_Component.BP_UniqueMechanic_Foretell_Component_C:ApplyForetell");
}

class ForetellVisualBugFixMod : public RC::CppUserModBase
{
public:
    ForetellVisualBugFixMod();
    ~ForetellVisualBugFixMod() override;
    void on_update() override;
    void on_unreal_init() override;

private:
    std::pair<int, int> initial_hook_ids;
    struct HookedFuncs
    {
        RC::Unreal::UFunction *fn_OnBattleDependenciesFullyLoaded;
        RC::Unreal::UFunction *fn_ApplyForetell;
    } hooked_funcs;
    void init_hooks();
};

void hook_ApplyForetell(RC::Unreal::UObject *context, RC::Unreal::FFrame &stack, void *RESULT_DECL);

} // Namespace