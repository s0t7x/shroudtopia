#pragma once
#include <windows.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace FirstPerson
{
    constexpr std::uint32_t ClientTimestamp = 1782724296;
    constexpr std::size_t ClientImageSize = 0x2DA7000;
    constexpr std::size_t CameraSystemDescriptor = 0x1D43630;
    constexpr std::size_t CameraSystemName = 0x1D443E0;
    constexpr std::size_t CameraSystemFunction = 0x2662A0;
    constexpr std::size_t CameraTransformCall = 0x2663D3;
    constexpr std::size_t CameraTransformFunction = 0xCB9C10;
    constexpr std::size_t ShoulderOffset = 0xED4;
    constexpr std::size_t DistanceOffset = 0xEF0;
    constexpr float FirstPersonDistance = -0.15f;
    constexpr std::size_t StubCallDisplacement = 25;
    constexpr std::size_t StubReturnDisplacement = 30;
    constexpr std::size_t StubDistance = 36;

    bool ValidCameraDistance(float distance)
    {
        return std::isfinite(distance) && distance >= -0.5f && distance <= 0.5f;
    }

    enum class HookKind { Unsupported, Legacy, Client1076226 };
    struct HookTarget
    {
        HookKind kind = HookKind::Unsupported;
        uintptr_t address = 0;
    };

    template <std::size_t N>
    bool Matches(const std::uint8_t* image, std::size_t imageSize, std::size_t rva,
        const std::array<std::uint8_t, N>& bytes)
    {
        return rva <= imageSize && N <= imageSize - rva &&
            std::memcmp(image + rva, bytes.data(), N) == 0;
    }

    HookTarget FindTarget(const std::uint8_t* image)
    {
        if (image == nullptr)
            return {};
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < sizeof(IMAGE_DOS_HEADER) || dos->e_lfanew > 0x1000)
            return {};
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
            nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            return {};
        const std::size_t imageSize = nt->OptionalHeader.SizeOfImage;
        const auto base = reinterpret_cast<uintptr_t>(image);
        if (nt->FileHeader.TimeDateStamp == ClientTimestamp && imageSize == ClientImageSize)
        {
            // The minimap hooks the camera system entry. Identify it from the
            // registry and patch a separate call site inside that same system.
            const char name[] = "player_camera_transform";
            uintptr_t descriptor[3] = {};
            std::memcpy(descriptor, image + CameraSystemDescriptor, sizeof(descriptor));
            const std::array<std::uint8_t, 5> call = { 0xE8, 0x38, 0x38, 0xA5, 0x00 };
            const std::array<std::uint8_t, 16> transformEntry = {
                0x48, 0x8B, 0xC4, 0x48, 0x89, 0x58, 0x08, 0x48,
                0x89, 0x78, 0x10, 0x55, 0x48, 0x8D, 0x68, 0xA1
            };
            const std::array<std::uint8_t, 8> distanceRead = {
                0xF3, 0x0F, 0x10, 0xA3, 0xF0, 0x0E, 0x00, 0x00
            };
            if (descriptor[0] != base + CameraSystemName || descriptor[1] != sizeof(name) - 1 ||
                descriptor[2] != base + CameraSystemFunction ||
                std::memcmp(image + CameraSystemName, name, sizeof(name)) != 0 ||
                !Matches(image, imageSize, CameraTransformCall, call) ||
                !Matches(image, imageSize, CameraTransformFunction, transformEntry) ||
                !Matches(image, imageSize, 0xCB9E21, distanceRead))
                return {};
            return { HookKind::Client1076226, base + CameraTransformCall };
        }

        // Retain the old mod's hook only for a unique match in executable code.
        const std::array<std::uint8_t, 8> pattern = { 0xF3, 0x0F, 0x11, 0x9F, 0xC4, 0x06, 0, 0 };
        uintptr_t found = 0;
        const auto* section = IMAGE_FIRST_SECTION(nt);
        for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i)
        {
            if ((section[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0)
                continue;
            const std::size_t start = section[i].VirtualAddress;
            const std::size_t length = section[i].Misc.VirtualSize;
            if (start > imageSize || length > imageSize - start || length < pattern.size())
                continue;
            for (std::size_t offset = 0; offset <= length - pattern.size(); ++offset)
            {
                if (std::memcmp(image + start + offset, pattern.data(), pattern.size()) != 0)
                    continue;
                if (found != 0)
                    return {};
                found = base + start + offset;
            }
        }
        return found ? HookTarget{ HookKind::Legacy, found } : HookTarget{};
    }

    bool RelativeDisplacement(uintptr_t nextInstruction, uintptr_t target, std::int32_t& result)
    {
        const auto delta = static_cast<std::int64_t>(target) - static_cast<std::int64_t>(nextInstruction);
        if (delta < (std::numeric_limits<std::int32_t>::min)() || delta > (std::numeric_limits<std::int32_t>::max)())
            return false;
        result = static_cast<std::int32_t>(delta);
        return true;
    }

    std::vector<std::uint8_t> CameraStub()
    {
        // At this local-player-only call site, RDX is LocalPlayerCamera and RCX
        // is the output transform. Preserve RAX and flags while loading the
        // distance from aligned data, so config reloads never rewrite executing
        // instructions. Restore the stack before calling the native calculation.
        std::vector<std::uint8_t> code = {
            0x50,                                           // push rax
            0x8B, 0x05, 0x1D, 0x00, 0x00, 0x00,           // mov eax,[rip+29] (data at 36)
            0x89, 0x82, 0xF0, 0x0E, 0x00, 0x00,           // mov [rdx+EF0],eax
            0x58,                                           // pop rax
            0xC7, 0x82, 0xD4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xE8, 0x00, 0x00, 0x00, 0x00,
            0xE9, 0x00, 0x00, 0x00, 0x00,
            0x90, 0x90,
            0x00, 0x00, 0x00, 0x00                          // aligned float data
        };
        const float distance = FirstPersonDistance;
        std::memcpy(code.data() + StubDistance, &distance, sizeof(distance));
        return code;
    }
}
