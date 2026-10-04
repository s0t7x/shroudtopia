#include "../dllmain.cpp"
#include <iostream>
#include <atomic>
#include <thread>

namespace
{
    void Check(bool ok, const char* name)
    {
        if (!ok) { std::cerr << "FAIL: " << name << '\n'; std::exit(1); }
        std::cout << "PASS: " << name << '\n';
    }

    IMAGE_NT_HEADERS64* SetImageHeaders(std::vector<std::uint8_t>& image)
    {
        auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());
        dos->e_magic = IMAGE_DOS_SIGNATURE;
        dos->e_lfanew = sizeof(IMAGE_DOS_HEADER);
        auto* nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data() + dos->e_lfanew);
        nt->Signature = IMAGE_NT_SIGNATURE;
        nt->FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
        nt->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
        nt->OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
        nt->OptionalHeader.SizeOfImage = static_cast<DWORD>(image.size());
        return nt;
    }

    void StoreFloat(std::vector<std::uint8_t>& data, std::size_t offset, float value)
    {
        std::memcpy(data.data() + offset, &value, sizeof(value));
    }
}

int main()
{
    using namespace FirstPerson;
    Check(FindTarget(nullptr).kind == HookKind::Unsupported, "reject absent image");
    std::vector<std::uint8_t> image(ClientImageSize);
    auto* nt = SetImageHeaders(image);
    nt->FileHeader.TimeDateStamp = ClientTimestamp;
    const auto base = reinterpret_cast<uintptr_t>(image.data());
    const char name[] = "player_camera_transform";
    const uintptr_t descriptor[] = { base + CameraSystemName, sizeof(name) - 1, base + CameraSystemFunction };
    std::memcpy(image.data() + CameraSystemName, name, sizeof(name));
    std::memcpy(image.data() + CameraSystemDescriptor, descriptor, sizeof(descriptor));
    const std::uint8_t call[] = { 0xE8, 0x38, 0x38, 0xA5, 0x00 };
    const std::uint8_t entry[] = { 0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x78,0x10,0x55,0x48,0x8D,0x68,0xA1 };
    const std::uint8_t distance[] = { 0xF3,0x0F,0x10,0xA3,0xF0,0x0E,0,0 };
    std::memcpy(image.data() + CameraTransformCall, call, sizeof(call));
    std::memcpy(image.data() + CameraTransformFunction, entry, sizeof(entry));
    std::memcpy(image.data() + 0xCB9E21, distance, sizeof(distance));
    Check(FindTarget(image.data()).address == base + CameraTransformCall, "resolve verified current camera call");
    image[CameraSystemFunction] = 0xE9;
    Check(FindTarget(image.data()).kind == HookKind::Client1076226, "coexist with minimap entry detour");
    image[CameraSystemName] = 'X';
    Check(FindTarget(image.data()).kind == HookKind::Unsupported, "reject wrong named system");
    image[CameraSystemName] = 'p';
    image[CameraTransformCall] = 0xE9;
    Check(FindTarget(image.data()).kind == HookKind::Unsupported, "reject already modified camera call");
    image[CameraTransformCall] = 0xE8;
    image[0xCB9E21 + 4] = 0xF4;
    Check(FindTarget(image.data()).kind == HookKind::Unsupported, "reject changed camera field layout");
    image[0xCB9E21 + 4] = 0xF0;
    nt->FileHeader.TimeDateStamp = 0;
    Check(FindTarget(image.data()).kind == HookKind::Unsupported, "reject unrecognized client without legacy hook");

    std::vector<std::uint8_t> legacy(0x1000);
    auto* legacyNt = SetImageHeaders(legacy);
    legacyNt->FileHeader.NumberOfSections = 1;
    auto* section = IMAGE_FIRST_SECTION(legacyNt);
    section->VirtualAddress = 0x500;
    section->Misc.VirtualSize = 0x100;
    section->Characteristics = IMAGE_SCN_MEM_EXECUTE;
    const std::uint8_t legacyPattern[] = { 0xF3,0x0F,0x11,0x9F,0xC4,0x06,0,0 };
    std::memcpy(legacy.data() + 0x5F8, legacyPattern, sizeof(legacyPattern));
    Check(FindTarget(legacy.data()).kind == HookKind::Legacy, "find unique legacy hook at final valid section offset");
    std::memcpy(legacy.data() + 0x500, legacyPattern, sizeof(legacyPattern));
    Check(FindTarget(legacy.data()).kind == HookKind::Unsupported, "reject ambiguous legacy signature");
    section->Characteristics = 0;
    Check(FindTarget(legacy.data()).kind == HookKind::Unsupported, "ignore legacy signature in non-executable data");

    std::int32_t displacement = 0;
    Check(RelativeDisplacement(0x10000, 0x8000, displacement) && displacement == -0x8000,
        "encode backward relative branch");
    Check(!RelativeDisplacement(0x10000, 0x80010000, displacement), "reject unreachable branch");

    ModContext distanceContext{};
    float configuredDistance = -0.05f;
    distanceContext.config.GetFloat = [&](const char* modKey, const char* key, float fallback) {
        if (std::strcmp(modKey, "FirstPersonView") != 0 || std::strcmp(key, "camera_distance") != 0)
            return fallback;
        return configuredDistance;
    };
    Check(ReadCameraDistance(&distanceContext, FirstPersonDistance) == -0.05f,
        "read scoped live camera setting from loader config");
    configuredDistance = 0.0f;
    Check(ReadCameraDistance(&distanceContext, FirstPersonDistance) == 0.0f,
        "accept pivot position as a valid distance");
    configuredDistance = 2.0f;
    Check(ReadCameraDistance(&distanceContext, -0.05f) == -0.05f,
        "retain previous distance on out-of-range setting");
    configuredDistance = (std::numeric_limits<float>::quiet_NaN)();
    Check(ReadCameraDistance(&distanceContext, -0.05f) == -0.05f,
        "retain previous distance on non-finite setting");
    distanceContext.config.GetFloat = [](const char*, const char*, float fallback) { return fallback; };
    Check(ReadCameraDistance(&distanceContext, -0.05f) == FirstPersonDistance,
        "removing the setting restores the original default");
    distanceContext.config.GetFloat = [](const char*, const char*, float) -> float { throw 1; };
    Check(ReadCameraDistance(&distanceContext, -0.05f) == -0.05f,
        "retain previous distance if config provider throws");
    distanceContext.config.GetFloat = {};
    Check(ReadCameraDistance(&distanceContext, -0.05f) == -0.05f,
        "handle missing config provider");

    // Execute the real stub and Detour in this process using a synthetic caller
    // and camera-calculation function. No game process is opened or modified.
    auto* executable = static_cast<std::uint8_t*>(VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    Check(executable != nullptr, "allocate synthetic camera code");
    const std::uint8_t caller[] = { 0x48,0x83,0xEC,0x28,0xE8,0,0,0,0,0x48,0x83,0xC4,0x28,0xC3 };
    const std::uint8_t callee[] = {
        0xF3,0x0F,0x10,0x82,0xD4,0x0E,0,0,0xF3,0x0F,0x11,0x01,
        0xF3,0x0F,0x10,0x82,0xF0,0x0E,0,0,0xF3,0x0F,0x11,0x41,0x04,
        0xF3,0x0F,0x10,0x82,0xD8,0x0E,0,0,0xF3,0x0F,0x11,0x41,0x08,0xC3
    };
    std::memcpy(executable, caller, sizeof(caller));
    std::memcpy(executable + 0x100, callee, sizeof(callee));
    const std::int32_t originalDelta = 0x100 - 9;
    std::memcpy(executable + 5, &originalDelta, sizeof(originalDelta));
    FlushInstructionCache(GetCurrentProcess(), executable, 0x1000);
    std::vector<std::uint8_t> camera(0x1000, 0xA5);
    StoreFloat(camera, ShoulderOffset, 0.6f);
    StoreFloat(camera, DistanceOffset, 4.0f);
    StoreFloat(camera, ShoulderOffset + 4, 1.5f);
    const auto originalCamera = camera;
    float observed[3] = {};
    const auto function = reinterpret_cast<void(*)(float*, void*)>(executable);
    function(observed, camera.data());
    Check(observed[0] == 0.6f && observed[1] == 4.0f && observed[2] == 1.5f,
        "synthetic original camera behavior");

    auto code = CameraStub();
    Mem::Detour hook(reinterpret_cast<uintptr_t>(executable + 4), code.data(), code.size(), false, 0);
    const auto stub = hook.shellcode->data->address;
    Check(stub != 0 && RelativeDisplacement(stub + StubCallDisplacement + 4, reinterpret_cast<uintptr_t>(executable + 0x100), displacement),
        "allocate reachable real detour");
    hook.shellcode->updateValue<std::int32_t>(StubCallDisplacement, displacement);
    Check(RelativeDisplacement(stub + StubReturnDisplacement + 4, reinterpret_cast<uintptr_t>(executable + 9), displacement),
        "resolve return to original caller");
    hook.shellcode->updateValue<std::int32_t>(StubReturnDisplacement, displacement);
    Check(hook.activate(), "activate synthetic camera detour");
    FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
    function(observed, camera.data());
    Check(observed[0] == 0.0f && observed[1] == FirstPersonDistance && observed[2] == 1.5f,
        "native calculation receives centered first-person distance and unchanged height");
    auto expectedCamera = originalCamera;
    StoreFloat(expectedCamera, ShoulderOffset, 0.0f);
    StoreFloat(expectedCamera, DistanceOffset, FirstPersonDistance);
    Check(camera == expectedCamera, "touch only the two intended camera fields");

    const std::vector<std::uint8_t> activeInstructions(
        reinterpret_cast<const std::uint8_t*>(stub), reinterpret_cast<const std::uint8_t*>(stub + StubDistance));
    configuredDistance = -0.05f;
    distanceContext.config.GetFloat = [&](const char*, const char*, float) { return configuredDistance; };
    SetCameraDistance(hook, ReadCameraDistance(&distanceContext, FirstPersonDistance));
    function(observed, camera.data());
    Check(observed[0] == 0.0f && observed[1] == -0.05f && observed[2] == 1.5f,
        "running camera picks up backward config adjustment without re-hooking");
    configuredDistance = -0.25f;
    SetCameraDistance(hook, ReadCameraDistance(&distanceContext, -0.05f));
    function(observed, camera.data());
    Check(observed[1] == -0.25f, "running camera picks up forward adjustment");
    Check(std::memcmp(activeInstructions.data(), reinterpret_cast<void*>(stub), activeInstructions.size()) == 0,
        "live config changes leave executing instructions unchanged");

    SetCameraDistance(hook, -0.05f);
    std::atomic<bool> badDistance{false};
    std::thread cameraThread([&] {
        for (int i = 0; i < 100000; ++i)
        {
            function(observed, camera.data());
            if ((observed[1] != -0.05f && observed[1] != FirstPersonDistance) || observed[2] != 1.5f)
                badDistance.store(true);
        }
    });
    for (int i = 0; i < 20000; ++i)
        SetCameraDistance(hook, i % 2 ? -0.05f : FirstPersonDistance);
    cameraThread.join();
    Check(!badDistance.load(), "100000 camera calls tolerate concurrent distance updates without torn values");

    hook.deactivate();
    FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
    camera = originalCamera;
    function(observed, camera.data());
    Check(observed[0] == 0.6f && observed[1] == 4.0f && observed[2] == 1.5f,
        "restore original call and behavior on deactivation");
    delete hook.patch;
    delete hook.shellcode;
    VirtualFree(reinterpret_cast<void*>(stub), 0, MEM_RELEASE);
    VirtualFree(executable, 0, MEM_RELEASE);

    ModContext context{};
    context.game.isClient = true;
    context.Log = [](const char*) {};
    FirstPersonMod unsupported;
    unsupported.Load(&context);
    unsupported.Activate(&context);
    unsupported.Deactivate(&context);
    unsupported.Unload(&context);
    unsupported.Unload(&context);
    Check(!unsupported.active, "missing hook survives activate, deactivate, repeated unload");
    return 0;
}
