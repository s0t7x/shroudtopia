#include "pch.h"
#include <shroudtopia.h>
#include <memory_utils.h>
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>  // For stringstream

#include <detours.h> // Needed for function hooking


ModMetaData metaData = {
    "ChatCommands",
    ".",
    "1.0",
    "s0T7x",
    "0.0.3",
    false,
    true,
};

std::string uint64_to_string(uint64_t value) {
    std::string result;
    result.clear();
    result.reserve(20); // max. 20 digits possible
    uint64_t q = value;
    do {
        result += "0123456789"[q % 10];
        q /= 10;
    } while (q);
    std::reverse(result.begin(), result.end());
    return result;
}

using ChatCallback = std::function<void(const std::string&)>;
std::unordered_map<std::string, ChatCallback> callbackMap;

// Function prototype for the original function
using OnChatMessageFunc = void*(__fastcall*)(void* rcx, long long rdx, void* r8, void* r9, void* rsi, void* rdi, void* r11, void* r12, void* r13);

// void FUN_1407b1a00(longlong param_1, undefined8 * param_2)
using OnNetworkPacket = void(__fastcall*)(long long param_1, BYTE* param_2);

// GetPlayerIdBySteamId???
using GetPlayerIdBySteamId = USHORT(__fastcall*)(long long param_1);


// test
using _FUN_140339880 = long long(__fastcall*)(long long param_1, unsigned long long param_2);

// Global pointer to store the original function
OnChatMessageFunc originalOnChatMessage = nullptr;
OnNetworkPacket originalOnNetworkPacket = nullptr;

GetPlayerIdBySteamId GetPlayerIdBySteamIdFunc = nullptr;
_FUN_140339880 FUN_140339880 = nullptr;

void* __fastcall DetourOnChatMessage(void* rcx, long long rdx, void* r8, unsigned long long * r9, void* rsi, void* rdi, void* r11, void* r12, void* r13) {
    long long someVal = *(long long*)(rdx + 8);
    std::cout << "someVal: " << someVal << std::endl;
    
    volatile size_t messageSize = *(size_t*)((uintptr_t)r9 + 0x08);
    if (r9 && messageSize > 1 && messageSize <= 256) {
        volatile char* ptr = (volatile char*)((uintptr_t)r9 + 0x140); // Volatile to prevent optimization
        if (ptr != nullptr) {
            std::string localMessage((char*)ptr, messageSize);
            std::cout << "ChatMessage: " << localMessage << std::endl;
            if (localMessage[0] == '/') {
                std::string cmd = localMessage.substr(1);
                auto callback = callbackMap.find(cmd);
                if (callback != callbackMap.end()) {
                    callback->second(cmd);
                    *(char*)(rcx) = 0x12;
                    return rcx;
                }
            }
        }
    }

    // Call the original function if it's not a command
    return originalOnChatMessage(rcx, rdx, r8, r9, rsi, rdi, r11, r12, r13);
}

void __fastcall DetourOnNetworkPacket(long long param_1, BYTE* param_2) {
    
    // Session connect packet has magic byte 0x0E
    if (*param_2 == 0x0E) {
        if (*(long long*)(param_1 + 0xf98) != 0) {
            uint64_t steamId = *(uintptr_t*)(param_2 + 8);
            if (steamId != 76561198018221526) {
                // prevent peer from joining by invalidating steamId
                std::cout << "Peer (" << steamId << ") tried to connect but is not whitelisted!" << std::endl;
                *(uintptr_t*)(param_2 + 8) = 0;
            }
        }
    }

    originalOnNetworkPacket(param_1, param_2);

    uintptr_t lVar14 = (param_1 + 0xf90);
    std::cout << std::hex << "lVar14: 0x" << lVar14 << std::endl;

    USHORT playerId = GetPlayerIdBySteamIdFunc(lVar14);
    std::cout << std::dec << "playerId: " << playerId << std::endl;

    lVar14 = lVar14 + playerId * 0x120;
    std::cout << std::hex << "lVar14: 0x" << lVar14 << std::endl;
    std::cout << std::dec << "lVar14+0x72: " << *(uintptr_t*)(lVar14 + 0x72);
}


long long __fastcall DetourFUN_140339880(long long param_1, unsigned long long param_2) {
    // ANNAHME: param_1 player struct irgendwie
    // param_2 0x40 + chat message size, warum auch immer 0x40 🤷‍
    Mem::hex_dump((void*)param_1, 8);
    Mem::hex_dump(&param_2, 8);

    return FUN_140339880(param_1, param_2);
}
// Register a chat command
void RegisterCommand(const std::string& command, ChatCallback callback) {
    callbackMap[command] = callback;
}

void MyTestCommand(const std::string& command) {
    std::cout << "We did make a chat command!! NICE /" << command << "!" << std::endl;
}

class ChatCommands : public Mod
{
    // Store the address of the chat function.  No longer need Patch object.
    uintptr_t chatFunctionAddress = 0;
    uintptr_t baseAddress = 0;

public:
    void Load(ModContext* modContext)
    {
        baseAddress = (uintptr_t)GetModuleHandle(NULL);
        chatFunctionAddress = baseAddress + 0x6C9080;

        GetPlayerIdBySteamIdFunc = (GetPlayerIdBySteamId)(baseAddress + 0x7adc60);
        FUN_140339880 = (_FUN_140339880)(baseAddress + 0x339880);

        RegisterCommand("test", MyTestCommand); //Example register of a command
    }

    void Unload(ModContext* modContext)
    {
        if (originalOnChatMessage) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&(PVOID&)originalOnChatMessage, DetourOnChatMessage);
            DetourDetach(&(PVOID&)originalOnNetworkPacket, DetourOnNetworkPacket);
            DetourDetach(&(PVOID&)FUN_140339880, DetourFUN_140339880);
            DetourTransactionCommit();
            originalOnChatMessage = nullptr;  // Reset the pointer
        }
    }

    void Activate(ModContext* modContext) {
        if (chatFunctionAddress)
        {
            // DetourCreate/DetourAttach require a transaction, so begin one
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());

            originalOnChatMessage = (OnChatMessageFunc)(chatFunctionAddress);
            DetourAttach(&(PVOID&)originalOnChatMessage, DetourOnChatMessage);


            originalOnNetworkPacket = (OnNetworkPacket)(baseAddress + 0x7B8870);
            DetourAttach(&(PVOID&)originalOnNetworkPacket, DetourOnNetworkPacket);

            DetourAttach(&(PVOID&)FUN_140339880, DetourFUN_140339880);

            // Commit the transaction
            LONG error = DetourTransactionCommit();

            if (error != NO_ERROR) {
                std::stringstream ss;
                ss << "Detour failed with error code: " << error;
                OutputDebugStringA(ss.str().c_str()); // Log error

                //Potentially crash the game intentionally, as it may be in an unrecoverable state.
                //throw std::runtime_error(ss.str());
            }

        }
        else
        {
            OutputDebugStringA("Failed to find chat function address.\n");
            //Log::Get()->Print(Color::Red, "Failed to find chat function address."); // Log error
        }
        modContext->Log(std::string().append(metaData.name).append(" activated").c_str());
        active = true;
    }

    void Deactivate(ModContext* modContext) {
        if (originalOnChatMessage) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&(PVOID&)originalOnChatMessage, DetourOnChatMessage);
            DetourDetach(&(PVOID&)originalOnNetworkPacket, DetourOnNetworkPacket);
            DetourDetach(&(PVOID&)FUN_140339880, DetourFUN_140339880);
            DetourTransactionCommit();
            originalOnChatMessage = nullptr;  // Reset the pointer
        }
        modContext->Log(std::string().append(metaData.name).append(" deactivated").c_str());
        active = false;
    }

    void Update(ModContext* modContext) { }

    ModMetaData GetMetaData() {
        return metaData;
    }
};


extern "C" __declspec(dllexport) Mod* CreateModInstance() {
    return new ChatCommands();
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DetourRestoreAfterWith();
    }
    return TRUE;
}