#include "ForetellVisualBugFixMod.hpp"
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/Script.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

namespace Unreal = RC::Unreal;
namespace LogLevel = RC::LogLevel;
namespace UObjectGlobals = Unreal::UObjectGlobals;

namespace ForetellVisualBugFixMod
{

ForetellVisualBugFixMod::ForetellVisualBugFixMod()
    : RC::CppUserModBase()
    , initial_hook_ids(-1, -1)
    , hooked(false)
{
    ModName = STR("ForetellVisualBugFixMod");
    ModVersion = STR("0.1.0");
    ModDescription = STR("Fix for the Foretell overflow visual bug");
    ModAuthors = STR("Reagent0");
    
    FVBFMOD_LOG(LogLevel::Verbose, STR("{} v{} loaded!"), ModName, ModVersion);
}

ForetellVisualBugFixMod::~ForetellVisualBugFixMod()
{
}

void ForetellVisualBugFixMod::on_update()
{
}

void ForetellVisualBugFixMod::on_unreal_init()
{
    initial_hook_ids = UObjectGlobals::RegisterHook(
        STR("/Script/Engine.PlayerController:ClientRestart"),
        NoOpLambda,
        [this](auto&&... args)
        {
            UObjectGlobals::UnregisterHook(STR("/Script/Engine.PlayerController:ClientRestart"), initial_hook_ids);
            init_hooks();
        },
        nullptr
    );
}

void ForetellVisualBugFixMod::init_hooks()
{
    FVBFMOD_LOG(LogLevel::Verbose, STR("Registering hooks.."));

    UObjectGlobals::RegisterHook(Paths::Fn_OnBattleDependenciesFullyLoaded, NoOpLambda, [this](auto&&... args)
    {
        FVBFMOD_LOG(LogLevel::Verbose, STR("OnBattleDependenciesFullyLoaded post-hook called!"));

        if (!hooked)
        {
            Unreal::UFunction *fn_ApplyForetell = UObjectGlobals::StaticFindObject<Unreal::UFunction*>(nullptr, nullptr, Paths::Fn_ApplyForetell);
            if (fn_ApplyForetell)
            {
                FVBFMOD_LOG(LogLevel::Verbose, STR("ApplyForetell function found! Registering hook.."));
                UObjectGlobals::RegisterHook(fn_ApplyForetell, hook_ApplyForetell, NoOpLambda, nullptr);
                hooked = true;
            }
        }
    }, nullptr);
}

void hook_ApplyForetell(Unreal::UnrealScriptFunctionCallableContext &context, void *custom_data)
{
    FVBFMOD_LOG(LogLevel::Verbose, STR("ApplyForetell pre-hook called!"));

    auto node = context.TheStack.Node();
    FVBFMOD_RETIFNULL(node);

    auto prop_Target = node->GetPropertyByName(STR("Target"));
    FVBFMOD_RETIFNULL(prop_Target);

    auto val_Target = *prop_Target->ContainerPtrToValuePtr<Unreal::UObject*>(context.TheStack.Locals());
    FVBFMOD_RETIFNULL(val_Target);

    auto val_ForetellCounts = context.Context->GetValuePtrByPropertyName<Unreal::TMap<Unreal::UObject*, Unreal::int32>>(STR("ForetellCounts"));
    FVBFMOD_RETIFNULL(val_ForetellCounts);

    auto target_foretell_ptr = val_ForetellCounts->Find(val_Target);
    auto target_foretell_count = target_foretell_ptr ? *target_foretell_ptr : 0;

    auto fn_GetCurrentMaxForetellCount = context.Context->GetFunctionByName(STR("GetCurrentMaxForetellCount"));
    FVBFMOD_RETIFNULL(fn_GetCurrentMaxForetellCount);

    struct GetCurrentMaxForetellCount_Params
    {
        Unreal::int32 ReturnValue;
    };
    auto params_GetCurrentMaxForetellCount = GetCurrentMaxForetellCount_Params{.ReturnValue = 0};
    context.Context->ProcessEvent(fn_GetCurrentMaxForetellCount, &params_GetCurrentMaxForetellCount);

    if (params_GetCurrentMaxForetellCount.ReturnValue <= 0)
    {
        FVBFMOD_LOG(LogLevel::Error, STR("Failed to GetCurrentMaxForetellCount(): unexpected return value"));
        return;
    }
    if (params_GetCurrentMaxForetellCount.ReturnValue < target_foretell_count)
    {
        FVBFMOD_LOG(LogLevel::Verbose, STR("Foretell cap is smaller than target's foretell, preventing ApplyForetell.."));

        auto &script = context.TheStack.Node()->GetScript();
        if (script.Num() < 3)
        {
            FVBFMOD_LOG(LogLevel::Error, STR("Failed to prevent original call: unexpected script size"));
            return;
        }

        auto returninst_offset = script.GetData() + script.Num() - 3;
        if (*returninst_offset != Unreal::EX_Return)
        {
            FVBFMOD_LOG(LogLevel::Error, STR("Failed to prevent original call: offset doesn't point to a EX_Return instruction"));
            return;
        }
        context.TheStack.Code() = returninst_offset;
        return;
    }
}

} // Namespace