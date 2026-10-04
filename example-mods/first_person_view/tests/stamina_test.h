#pragma once
#include <cstdio>
#include <cstdlib>

namespace {
    int invalidHudLogCalls = 0;
    void CaptureInvalidHudLog(const wchar_t*,const wchar_t*,const wchar_t*,unsigned,uintptr_t)
    {
        ++invalidHudLogCalls;
    }
}

void TestStaminaStartupLog()
{
    // ModContext_Log forwards messages as printf format strings in released
    // Shroudtopia. Exercise that boundary, rather than using a no-op logger.
    ModContext context{};
    bool formattedCorrectly = true;
    context.Log = [&](const char* message) {
        const std::string prefixed = std::string("(FirstPersonView) ") + message;
        char buffer[1024] = {};
        const int result = std::snprintf(buffer, sizeof(buffer), prefixed.c_str());
        formattedCorrectly &= result >= 0 && prefixed == buffer;
    };
    invalidHudLogCalls = 0;
    const auto previous = _set_thread_local_invalid_parameter_handler(CaptureInvalidHudLog);
    FirstPerson::ReportStaminaHudStatus(&context, true);
    FirstPerson::ReportStaminaHudStatus(&context, false);
    _set_thread_local_invalid_parameter_handler(previous);
    Check(formattedCorrectly && invalidHudLogCalls == 0,
        "HUD startup messages work with Shroudtopia's formatting logger");
}


void TestStaminaHud(std::vector<std::uint8_t>& image)
{
    TestStaminaStartupLog();
    using namespace FirstPerson;
    const std::uint8_t renderer[] = {0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x55,0x57,0x41,0x54,0x41};
    const std::uint8_t context[] = {0x48,0x8B,0x0D,0xF5,0xC1,0xB8,0x01};
    const std::uint8_t canvas[] = {0x48,0x8B,0x41,0x10,0x48,0x83,0xC0,0x20,0xC3};
    std::memcpy(image.data()+StaminaLayoutSite,StaminaLayoutInstructions.data(),8);
    std::memcpy(image.data()+0xBB3260,renderer,sizeof(renderer));
    std::memcpy(image.data()+0xBB338C,context,sizeof(context));
    std::memcpy(image.data()+0x952610,canvas,sizeof(canvas));
    const std::uint8_t layout[]={0xE8,0x15,0x6A,0x13,0};
    const std::uint8_t position[]={0xE8,0x9A,0x62,0x4D,0};
    std::memcpy(image.data()+0xAA6236,layout,5);
    std::memcpy(image.data()+0xAA6281,position,5);
    Check(MatchesStaminaLayout(image.data(),image.size()),"accept verified shared stamina layout");
    image[StaminaLayoutSite]=0xE9;
    Check(!MatchesStaminaLayout(image.data(),image.size()),"reject existing stamina hook");
    image[StaminaLayoutSite]=0x0F;
    image[0x952610+3]=0x18;
    Check(!MatchesStaminaLayout(image.data(),image.size()),"reject changed HUD canvas layout");
    image[0x952610+3]=0x10;
    Check(!MatchesStaminaLayout(image.data(),0x1000),"reject truncated HUD image");

    auto* executable=static_cast<std::uint8_t*>(VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    Check(executable!=nullptr,"allocate isolated stamina layout harness");
    std::vector<std::uint8_t> caller = {
        0x48,0x83,0xEC,0x28, 0x0F,0x11,0x74,0x24,0x10,
        0x41,0x0F,0x10,0x00, 0x41,0x0F,0x10,0x48,0x10,
        0x48,0x89,0xC8, 0x48,0x39,0xC9
    };
    const auto site=caller.size();
    caller.insert(caller.end(),StaminaLayoutInstructions.begin(),StaminaLayoutInstructions.end());
    const std::int32_t scopeDelta=static_cast<std::int32_t>(0x300-caller.size());
    std::memcpy(caller.data()+site+4,&scopeDelta,4);
    const std::uint8_t finish[] = {
        0x0F,0x11,0x32, 0x0F,0x11,0x42,0x10, 0x0F,0x11,0x4A,0x20,
        0x48,0x89,0x42,0x40, 0x9C,0x58,0x48,0x89,0x42,0x30,
        0x0F,0x10,0x74,0x24,0x10, 0x48,0x83,0xC4,0x28,0xC3
    };
    caller.insert(caller.end(),std::begin(finish),std::end(finish));
    std::memcpy(executable,caller.data(),caller.size());
    // Original scope call must execute exactly once with its incoming arguments.
    // It consumes XMM6 as the common rectangle before background/ring rendering.
    const std::uint8_t nativeScope[]={0x9C,0x49,0xFF,0x01,0x9D,0xC3};
    std::memcpy(executable+0x300,nativeScope,sizeof(nativeScope));
    std::uint64_t scopes=0;
    FlushInstructionCache(GetCurrentProcess(),executable,0x1000);
    std::array<std::uint8_t,64> viewport{}, gui{};
    const auto viewportPointer=reinterpret_cast<uintptr_t>(viewport.data());
    uintptr_t guiPointer=reinterpret_cast<uintptr_t>(gui.data());
    std::memcpy(gui.data()+0x10,&viewportPointer,8);
    auto size=[&](float width,float height) {
        std::memcpy(viewport.data()+0x20,&width,4); std::memcpy(viewport.data()+0x24,&height,4);
    };
    size(1920,1080);
    float rectangle[4]={20,40,80,100};
    const float original[4]={20,40,80,100};
    const float registers[8]={1,2,3,4,5,6,7,8};
    struct Observed { float rectangle[4],xmm0[4],xmm1[4]; std::uint64_t flags; float scale; std::uint32_t padding; uintptr_t originalPointer; } observed{};
    static_assert(offsetof(Observed,originalPointer)==0x40,"harness layout");
    const auto draw=reinterpret_cast<void(*)(const float*,Observed*,const float*,std::uint64_t*)>(executable);
    draw(rectangle,&observed,registers,&scopes);
    Check(std::memcmp(observed.rectangle,rectangle,16)==0,"harness reproduces original stamina rectangle");

    auto code=BuildStaminaStub(reinterpret_cast<uintptr_t>(&guiPointer));
    Mem::Detour hook(reinterpret_cast<uintptr_t>(executable+site),code.bytes.data(),code.bytes.size(),false,3);
    const auto stub=hook.shellcode->data->address;
    std::int32_t back=0,entry=0,call=0;
    Check(stub && RelativeDisplacement(stub+code.returnDisplacement+4,reinterpret_cast<uintptr_t>(executable+site+8),back) &&
        RelativeDisplacement(reinterpret_cast<uintptr_t>(executable+site+5),stub,entry) &&
        RelativeDisplacement(stub+code.callDisplacement+4,reinterpret_cast<uintptr_t>(executable+0x300),call),"allocate reachable stamina detour");
    hook.shellcode->updateValue<std::int32_t>(code.returnDisplacement,back);
    hook.shellcode->updateValue<std::int32_t>(code.callDisplacement,call);
    Check(hook.activate(),"install real stamina detour in isolated harness");
    FlushInstructionCache(GetCurrentProcess(),nullptr,0);
    auto enabled=[&](bool value) {InterlockedExchange(reinterpret_cast<volatile LONG*>(stub+code.enabled),value?1:0);};
    auto native=[&]() {draw(rectangle,&observed,registers,&scopes);return std::memcmp(observed.rectangle,rectangle,16)==0;};
    auto anchored=[&](float width,float height) {
        draw(rectangle,&observed,registers,&scopes);
        return std::abs(observed.rectangle[0]+rectangle[2]*0.5f-width*0.45f)<0.001f &&
            std::abs(observed.rectangle[1]+rectangle[3]*0.5f-height*0.5f)<0.001f &&
            observed.rectangle[2]==rectangle[2] && observed.rectangle[3]==rectangle[3];
    };
    Check(native(),"third-person mode retains exact native stamina position");
    enabled(true);
    auto previousScopes=scopes;
    Check(anchored(1920,1080),"first person anchors the shared widget before background and circle layout");
    Check(scopes==previousScopes+1,"anchor preserves the original widget scope call exactly once");
    Check(std::memcmp(observed.xmm0,registers,32)==0 && (observed.flags&0x8D5)==0x44 &&
        observed.originalPointer==reinterpret_cast<uintptr_t>(rectangle),
        "interior hook preserves temporary registers, flags and native pointer");
    Check(std::memcmp(rectangle,original,16)==0,"anchoring does not write to the native widget");
    const float sizes[][2]={{2580,1080},{2560,1440},{3840,2160},{800,600}};
    bool resolutions=true;
    for (auto& dimensions:sizes) {size(dimensions[0],dimensions[1]);resolutions&=anchored(dimensions[0],dimensions[1]);}
    Check(resolutions,"anchor follows HUD canvas size including ultrawide and UI scaling");
    size(1920,1080);
    rectangle[0]=-10000; rectangle[1]=10000;
    Check(anchored(1920,1080),"projected character movement does not move anchored gauge");
    rectangle[2]=180; rectangle[3]=210;
    Check(anchored(1920,1080),"shared layout stays centered as native widget dimensions change");
    std::memcpy(rectangle,original,16);
    enabled(false);Check(native(),"switching back restores native placement immediately");enabled(true);
    guiPointer=0;Check(native(),"absent UI context keeps native placement");guiPointer=reinterpret_cast<uintptr_t>(gui.data());
    uintptr_t missing=0;std::memcpy(gui.data()+0x10,&missing,8);
    Check(native(),"absent canvas keeps native placement");std::memcpy(gui.data()+0x10,&viewportPointer,8);
    bool invalid=true;
    for (float width:{0.0f,-1.0f,20000.0f,(std::numeric_limits<float>::quiet_NaN)(),(std::numeric_limits<float>::infinity)()}) {
        size(width,1080);invalid&=native();size(1920,width);invalid&=native();
    }
    Check(invalid,"invalid or minimized canvas cannot produce a broken HUD anchor");size(1920,1080);
    rectangle[2]=(std::numeric_limits<float>::quiet_NaN)();Check(native(),"invalid native dimensions are left untouched");std::memcpy(rectangle,original,16);
    std::vector<std::uint8_t> instructions(reinterpret_cast<std::uint8_t*>(stub),reinterpret_cast<std::uint8_t*>(stub+code.enabled));
    std::atomic<bool> bad{false};
    std::thread rendering([&] {
        for (int i=0;i<100000;++i) {
            draw(rectangle,&observed,registers,&scopes);
            const bool fixed=std::abs(observed.rectangle[0]-824)<0.001f && observed.rectangle[1]==490;
            const bool normal=std::memcmp(observed.rectangle,rectangle,16)==0;
            if (!fixed && !normal) bad.store(true);
        }
    });
    for (int i=0;i<20000;++i) enabled(i%2!=0);
    rendering.join();
    Check(!bad.load(),"100000 stamina draws tolerate concurrent camera-mode toggles");
    Check(std::memcmp(instructions.data(),reinterpret_cast<void*>(stub),instructions.size())==0,"camera-mode toggles never rewrite HUD instructions");
    hook.deactivate();FlushInstructionCache(GetCurrentProcess(),nullptr,0);
    Check(native(),"removing stamina hook restores original layout instructions");
    delete hook.patch;delete hook.shellcode;
    VirtualFree(reinterpret_cast<void*>(stub),0,MEM_RELEASE);VirtualFree(executable,0,MEM_RELEASE);
}
