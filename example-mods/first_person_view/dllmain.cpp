#include "pch.h"
#include <vector>
#include <shroudtopia.h>
#include <memory_utils.h>
#include "camera_patch.h"

ModMetaData metaData = {
    "FirstPersonView",
    "Enables simple but immersive 1st Person view. (Client only)",
    "1.1.0",
    "s0T7x",
    "0.1.1",
    true,
    false
};

namespace FirstPerson
{
    float ReadCameraDistance(ModContext* context, float previous)
    {
        if (context == nullptr || !context->config.GetFloat)
            return previous;
        try
        {
            const float value = context->config.GetFloat("FirstPersonView", "camera_distance", FirstPersonDistance);
            return ValidCameraDistance(value) ? value : previous;
        }
        catch (...)
        {
            return previous;
        }
    }

    void SetCameraDistance(Mem::Detour& hook, float distance)
    {
        // Keep the saved bytes current for activation/re-activation. Only the
        // aligned data word is changed once the stub has been injected.
        hook.shellcode->updateValue<float>(StubDistance, distance);
        if (hook.shellcode->injected)
        {
            LONG bits = 0;
            static_assert(sizeof(bits) == sizeof(distance), "distance must be 32 bits");
            std::memcpy(&bits, &distance, sizeof(bits));
            InterlockedExchange(reinterpret_cast<volatile LONG*>(
                hook.shellcode->data->address + StubDistance), bits);
        }
    }
}

class FirstPersonMod : public Mod
{
    Mem::Detour* mod = nullptr;
    bool unavailableLogged = false;
    bool configurableDistance = false;
    float cameraDistance = FirstPerson::FirstPersonDistance;

    void RefreshCameraDistance(ModContext* context)
    {
        if (!configurableDistance || mod == nullptr)
            return;
        const float value = FirstPerson::ReadCameraDistance(context, cameraDistance);
        if (value == cameraDistance)
            return;
        FirstPerson::SetCameraDistance(*mod, value);
        cameraDistance = value;
        const std::string message = "[FirstPerson] camera_distance=" + std::to_string(value) + " m";
        context->Log(message.c_str());
    }

    void ReleaseHook()
    {
        if (mod == nullptr)
            return;
        mod->deactivate();
        const auto allocation = mod->shellcode->data->address;
        delete mod->patch;
        delete mod->shellcode;
        delete mod;
        mod = nullptr;
        configurableDistance = false;
        cameraDistance = FirstPerson::FirstPersonDistance;
        if (allocation != 0)
            VirtualFree(reinterpret_cast<void*>(allocation), 0, MEM_RELEASE);
        active = false;
    }

public:
    ~FirstPersonMod() override { ReleaseHook(); }

    void Load(ModContext* modContext) override
    {
        if (!modContext->game.isClient || mod != nullptr)
            return;
        const auto* image = reinterpret_cast<const std::uint8_t*>(GetModuleHandle(nullptr));
        const auto target = FirstPerson::FindTarget(image);
        if (target.kind == FirstPerson::HookKind::Unsupported)
        {
            modContext->Log("[FirstPerson] unsupported camera layout; no patch installed");
            return;
        }

        if (target.kind == FirstPerson::HookKind::Client1076226)
        {
            auto code = FirstPerson::CameraStub();
            mod = new Mem::Detour(target.address, code.data(), code.size(), false, 0);
            const auto stub = mod->shellcode->data->address;
            std::int32_t callDelta = 0, returnDelta = 0, entryDelta = 0;
            if (stub == 0 ||
                !FirstPerson::RelativeDisplacement(stub + FirstPerson::StubCallDisplacement + 4,
                    reinterpret_cast<uintptr_t>(image) + FirstPerson::CameraTransformFunction, callDelta) ||
                !FirstPerson::RelativeDisplacement(stub + FirstPerson::StubReturnDisplacement + 4, target.address + 5, returnDelta) ||
                !FirstPerson::RelativeDisplacement(target.address + 5, stub, entryDelta))
            {
                ReleaseHook();
                modContext->Log("[FirstPerson] unable to allocate a reachable camera hook");
                return;
            }
            mod->shellcode->updateValue<std::int32_t>(FirstPerson::StubCallDisplacement, callDelta);
            mod->shellcode->updateValue<std::int32_t>(FirstPerson::StubReturnDisplacement, returnDelta);
            configurableDistance = true;
            RefreshCameraDistance(modContext);
            modContext->Log("[FirstPerson] camera hook prepared for client 1076226; minimap entry preserved");
        }
        else
        {
            std::uint8_t code[] = {
                0xF3, 0x0F, 0x11, 0x9F, 0xC4, 0x06, 0x00, 0x00,
                0xC7, 0x87, 0xC4, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xC7, 0x87, 0xC8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x80, 0xBF,
                0xE9, 0x00, 0x00, 0x00, 0x00
            };
            mod = new Mem::Detour(target.address, code, sizeof(code), false, 3);
            const auto stub = mod->shellcode->data->address;
            std::int32_t returnDelta = 0, entryDelta = 0;
            if (stub == 0 ||
                !FirstPerson::RelativeDisplacement(stub + sizeof(code), target.address + 8, returnDelta) ||
                !FirstPerson::RelativeDisplacement(target.address + 5, stub, entryDelta))
            {
                ReleaseHook();
                modContext->Log("[FirstPerson] unable to allocate a reachable legacy hook");
                return;
            }
            mod->shellcode->updateValue<std::int32_t>(sizeof(code) - 4, returnDelta);
            modContext->Log("[FirstPerson] unique legacy camera hook prepared");
        }
    }

    void Unload(ModContext*) override
    {
        ReleaseHook();
        unavailableLogged = false;
        loaded = false;
    }

    void Activate(ModContext* modContext) override
    {
        if (mod == nullptr)
        {
            if (!unavailableLogged)
            {
                modContext->Log("[FirstPerson] activation skipped: compatible camera hook unavailable");
                unavailableLogged = true;
            }
            return;
        }
        if (active)
            return;
        RefreshCameraDistance(modContext);
        active = mod->activate();
        if (active)
        {
            FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
            modContext->Log("[FirstPerson] activated");
        }
        else
            modContext->Log("[FirstPerson] camera hook activation failed");
    }

    void Deactivate(ModContext* modContext) override
    {
        if (mod != nullptr)
        {
            mod->deactivate();
            FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
        }
        active = false;
        modContext->Log("[FirstPerson] deactivated");
    }

    void Update(ModContext* context) override { RefreshCameraDistance(context); }
    ModMetaData GetMetaData() override { return metaData; }
};

extern "C" __declspec(dllexport) Mod* CreateModInstance()
{
    return new FirstPersonMod();
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID)
{
    return TRUE;
}
