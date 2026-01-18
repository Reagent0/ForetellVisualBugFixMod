#include "ForetellVisualBugFixMod.hpp"
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/Script.hpp>
#include <Unreal/Hooks.hpp>
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
    , hooked_funcs{}
    , hooked(false)
{
    ModName = STR("ForetellVisualBugFixMod");
    ModVersion = STR("0.1.1");
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

    hooked_funcs.fn_OnBattleDependenciesFullyLoaded = UObjectGlobals::StaticFindObject<Unreal::UFunction*>(nullptr, nullptr, Paths::Fn_OnBattleDependenciesFullyLoaded);
    if (hooked_funcs.fn_OnBattleDependenciesFullyLoaded == nullptr) {
        FVBFMOD_LOG(LogLevel::Error, STR("Cannot find function OnBattleDependenciesFullyLoaded!"));
        return;
    }
    FVBFMOD_LOG(LogLevel::Verbose, STR("OnBattleDependenciesFullyLoaded function found!"));

    Unreal::Hook::RegisterProcessInternalPostCallback([this](Unreal::UObject *context, Unreal::FFrame &stack, void *RESULT_DECL)
    {
        if (hooked_funcs.fn_OnBattleDependenciesFullyLoaded != nullptr && hooked_funcs.fn_OnBattleDependenciesFullyLoaded == stack.Node())
        {
            FVBFMOD_LOG(LogLevel::Verbose, STR("OnBattleDependenciesFullyLoaded post-hook called!"));

            if (!hooked)
            {
                hooked_funcs.fn_ApplyForetell = UObjectGlobals::StaticFindObject<Unreal::UFunction*>(nullptr, nullptr, Paths::Fn_ApplyForetell);
                if (hooked_funcs.fn_ApplyForetell == nullptr)
                {
                    FVBFMOD_LOG(LogLevel::Error, STR("Cannot find function ApplyForetell!"));
                    return;
                }
                hooked = true;
                FVBFMOD_LOG(LogLevel::Verbose, STR("ApplyForetell function found!"));
            }
        }
    });

    Unreal::Hook::RegisterProcessLocalScriptFunctionPreCallback([this](Unreal::UObject *context, Unreal::FFrame &stack, void *RESULT_DECL)
    {
        if (hooked_funcs.fn_ApplyForetell != nullptr && hooked_funcs.fn_ApplyForetell == stack.Node())
        {
            hook_ApplyForetell(context, stack, RESULT_DECL);
            return;
        }
    });
}

void hook_ApplyForetell(Unreal::UObject *context, Unreal::FFrame &stack, void *RESULT_DECL)
{
    FVBFMOD_LOG(LogLevel::Verbose, STR("ApplyForetell pre-hook called!"));

    auto node = stack.Node();
    FVBFMOD_RETIFNULL(node);

    auto prop_Target = node->GetPropertyByName(STR("Target"));
    FVBFMOD_RETIFNULL(prop_Target);

    auto val_Target = *prop_Target->ContainerPtrToValuePtr<Unreal::UObject*>(stack.Locals());
    FVBFMOD_RETIFNULL(val_Target);

    auto val_ForetellCounts = context->GetValuePtrByPropertyName<Unreal::TMap<Unreal::UObject*, Unreal::int32>>(STR("ForetellCounts"));
    FVBFMOD_RETIFNULL(val_ForetellCounts);

    auto target_foretell_ptr = val_ForetellCounts->Find(val_Target);
    auto target_foretell_count = target_foretell_ptr ? *target_foretell_ptr : 0;

    auto fn_GetCurrentMaxForetellCount = context->GetFunctionByName(STR("GetCurrentMaxForetellCount"));
    FVBFMOD_RETIFNULL(fn_GetCurrentMaxForetellCount);

    struct GetCurrentMaxForetellCount_Params
    {
        Unreal::int32 ReturnValue;
    };
    auto params_GetCurrentMaxForetellCount = GetCurrentMaxForetellCount_Params{.ReturnValue = 0};
    context->ProcessEvent(fn_GetCurrentMaxForetellCount, &params_GetCurrentMaxForetellCount);

    if (params_GetCurrentMaxForetellCount.ReturnValue <= 0)
    {
        FVBFMOD_LOG(LogLevel::Error, STR("Failed to GetCurrentMaxForetellCount(): unexpected return value"));
        return;
    }
    if (params_GetCurrentMaxForetellCount.ReturnValue < target_foretell_count)
    {
        FVBFMOD_LOG(LogLevel::Verbose, STR("Foretell cap is smaller than target's foretell, preventing ApplyForetell.."));

        auto &script = node->GetScript();
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
        stack.Code() = returninst_offset;
        return;
    }
}

} // Namespace