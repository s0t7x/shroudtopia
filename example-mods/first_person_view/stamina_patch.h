#pragma once
#include "camera_patch.h"
#include <memory_utils.h>

namespace FirstPerson
{
    constexpr std::size_t StaminaLayoutSite = 0xAA6244;
    constexpr std::size_t UiContextPointer = 0x273F588;
    constexpr std::size_t NativeWidgetScope = 0x9437E0;
    constexpr std::array<std::uint8_t, 8> StaminaLayoutInstructions = {
        0x0F,0x10,0x30, 0xE8,0x94,0xD5,0xE9,0xFF
    };

    bool MatchesStaminaLayout(const std::uint8_t* image, std::size_t size)
    {
        const std::array<std::uint8_t, 16> renderer = {
            0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x55,0x57,0x41,0x54,0x41
        };
        const std::array<std::uint8_t, 7> context = { 0x48,0x8B,0x0D,0xF5,0xC1,0xB8,0x01 };
        const std::array<std::uint8_t, 9> canvas = { 0x48,0x8B,0x41,0x10,0x48,0x83,0xC0,0x20,0xC3 };
        const std::array<std::uint8_t,5> layout = {0xE8,0x15,0x6A,0x13,0x00};
        const std::array<std::uint8_t,5> position = {0xE8,0x9A,0x62,0x4D,0x00};
        return size == ClientImageSize &&
            Matches(image,size,0xAA6236,layout) && Matches(image,size,0xAA6281,position) &&
            Matches(image,size,StaminaLayoutSite,StaminaLayoutInstructions) &&
            Matches(image,size,0xBB3260,renderer) && Matches(image,size,0xBB338C,context) &&
            Matches(image,size,0x952610,canvas);
    }

    struct StaminaStub
    {
        std::vector<std::uint8_t> bytes;
        std::size_t enabled = 0;
        std::size_t returnDisplacement = 0;
        std::size_t callDisplacement = 0;
    };

    StaminaStub BuildStaminaStub(uintptr_t uiContextPointer)
    {
        // Anchor the shared stamina layout before the native widget scope is
        // created. Background, rings, buff labels and clipping now consume the
        // same rectangle. The circle renderer itself remains entirely native.
        // Preserve live registers and flags, then replay the original scope call
        // with exactly its original stack and arguments.
        StaminaStub stub;
        auto& b = stub.bytes;
        auto emit = [&](std::initializer_list<std::uint8_t> bytes) { b.insert(b.end(),bytes); };
        struct Reference { std::size_t field, next; unsigned data; };
        std::vector<Reference> references;
        std::vector<std::size_t> branches;
        auto rip = [&](std::initializer_list<std::uint8_t> opcode, unsigned data,
            std::initializer_list<std::uint8_t> suffix = {}) {
            emit(opcode); const auto field = b.size(); emit({0,0,0,0}); emit(suffix);
            references.push_back({field,b.size(),data});
        };
        auto skip = [&](std::uint8_t condition) {
            emit({0x0F,condition}); branches.push_back(b.size()); emit({0,0,0,0});
        };
        emit({0x0F,0x10,0x30});                    // original movups xmm6,[rax]
        emit({0x9C,0x52,0x48,0x83,0xEC,0x20});   // preserve flags, RDX and XMM0/1
        emit({0x0F,0x11,0x04,0x24,0x0F,0x11,0x4C,0x24,0x10});
        rip({0x83,0x3D},0,{0}); skip(0x84);       // disabled: original rectangle
        emit({0x48,0xBA});
        const auto pointerOffset = b.size(); b.resize(b.size()+8);
        std::memcpy(b.data()+pointerOffset,&uiContextPointer,8);
        emit({0x48,0x8B,0x12,0x48,0x85,0xD2}); skip(0x84);
        emit({0x48,0x8B,0x52,0x10,0x48,0x85,0xD2}); skip(0x84);
        emit({0xF3,0x0F,0x7E,0x42,0x20});        // HUD canvas width/height
        auto reject = [&](unsigned limit, std::uint8_t comparison) {
            rip({0x0F,0xC2,0x0D},limit,{comparison}); // compare XMM1 with limit
            emit({0x0F,0x50,0xD1,0xF6,0xC2,0x03}); skip(0x85);
        };
        emit({0x0F,0x28,0xC8}); reject(1,2);     // canvas must be positive
        emit({0x0F,0x28,0xC8}); reject(2,6);     // bounded and finite
        emit({0x0F,0x28,0xCE,0x0F,0xC6,0xC9,0xEE}); reject(1,1);
        emit({0x0F,0x28,0xCE,0x0F,0xC6,0xC9,0xEE}); reject(2,6);
        rip({0x0F,0x59,0x05},3);                // desired centre: 45% x, 50% y
        emit({0x0F,0x28,0xCE,0x0F,0xC6,0xC9,0xEE});
        rip({0x0F,0x59,0x0D},4);                // half native rectangle size
        emit({0x0F,0x5C,0xC1,0x0F,0xC6,0xC6,0xE4,0x0F,0x28,0xF0});
        const auto restore = b.size();
        emit({0x0F,0x10,0x04,0x24,0x0F,0x10,0x4C,0x24,0x10});
        emit({0x48,0x83,0xC4,0x20,0x5A,0x9D,0xE8});
        stub.callDisplacement = b.size(); emit({0,0,0,0});
        emit({0xE9});
        stub.returnDisplacement = b.size(); emit({0,0,0,0});
        while (b.size()%16) emit({0x90});
        std::size_t data[5] = {};
        const float constants[5][4] = {
            {0,0,0,0}, {0,0,0,0},
            {16384,16384,16384,16384}, {0.45f,0.50f,0,0}, {0.5f,0.5f,0.5f,0.5f}
        };
        for (unsigned i=0;i<5;++i) {
            data[i]=b.size(); b.resize(b.size()+16);
            std::memcpy(b.data()+data[i],constants[i],16);
        }
        stub.enabled=data[0];
        for (const auto& reference: references) {
            const auto delta=static_cast<std::int32_t>(data[reference.data]-reference.next);
            std::memcpy(b.data()+reference.field,&delta,4);
        }
        for (const auto field: branches) {
            const auto delta=static_cast<std::int32_t>(restore-(field+4));
            std::memcpy(b.data()+field,&delta,4);
        }
        return stub;
    }

    class StaminaHudHook
    {
        Mem::Detour* hook = nullptr;
        std::size_t enabledOffset = 0;
    public:
        ~StaminaHudHook() { Release(); }
        void SetEnabled(bool enabled)
        {
            if (hook && hook->active)
                InterlockedExchange(reinterpret_cast<volatile LONG*>(hook->shellcode->data->address+enabledOffset),enabled?1:0);
        }
        bool Prepare(const std::uint8_t* image)
        {
            if (hook) return true;
            // Load calls this only after the camera's PE/version gate succeeds.
            if (!MatchesStaminaLayout(image,ClientImageSize)) return false;
            const auto base=reinterpret_cast<uintptr_t>(image);
            auto code=BuildStaminaStub(base+UiContextPointer);
            const auto site=base+StaminaLayoutSite;
            hook=new Mem::Detour(site,code.bytes.data(),code.bytes.size(),false,3);
            const auto address=hook->shellcode->data->address;
            std::int32_t back=0,entry=0,call=0;
            if (!address || !RelativeDisplacement(address+code.returnDisplacement+4,site+8,back) ||
                !RelativeDisplacement(site+5,address,entry) ||
                !RelativeDisplacement(address+code.callDisplacement+4,base+NativeWidgetScope,call)) { Release(); return false; }
            enabledOffset=code.enabled;
            hook->shellcode->updateValue<std::int32_t>(code.callDisplacement,call);
            hook->shellcode->updateValue<std::int32_t>(code.returnDisplacement,back);
            if (!hook->activate()) { Release(); return false; }
            FlushInstructionCache(GetCurrentProcess(),nullptr,0);
            return true;
        }
        void Release()
        {
            if (!hook) return;
            SetEnabled(false);
            hook->deactivate();
            FlushInstructionCache(GetCurrentProcess(),nullptr,0);
            const auto allocation=hook->shellcode->data->address;
            delete hook->patch; delete hook->shellcode; delete hook; hook=nullptr;
            if (allocation) VirtualFree(reinterpret_cast<void*>(allocation),0,MEM_RELEASE);
        }
    };
}
