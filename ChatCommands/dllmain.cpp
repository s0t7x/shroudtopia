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

using ChatCallback = std::function<void(const std::string&)>;
std::unordered_map<std::string, ChatCallback> callbackMap;

// Function prototype for the original chat message function
using OnChatMessageFunc = void(__fastcall*)(void* rcx, void* rdx, uint8_t r8b, void* r9, void* rsi, void* rdi);

// Global pointer to store the original chat message function
OnChatMessageFunc originalOnChatMessage = nullptr;

// Our detour function
void __fastcall DetourOnChatMessage(void* rcx, void* rdx, uint8_t r8b, void* r9, void* rsi, void* rdi) {
    volatile size_t messageSize = *(size_t*)((uintptr_t)r9 + 0x08);
    if (r9 && messageSize && messageSize <= 256) {
        volatile char* ptr = (volatile char*)((uintptr_t)r9 + 0x140); // Volatile to prevent optimization
        if (ptr != nullptr) {
            //std::string localMessage = "msg";
            std::cout << "localMessage" << std::endl;
        }
    }

    // Call the original function if it's not a command
    return originalOnChatMessage(rcx, rdx, r8b, r9, rsi, rdi);
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

public:
    void Load(ModContext* modContext)
    {
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        chatFunctionAddress = baseAddress + 0x6C3030;

        RegisterCommand("test", MyTestCommand); //Example register of a command
    }

    void Unload(ModContext* modContext)
    {
        if (originalOnChatMessage) {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&(PVOID&)originalOnChatMessage, DetourOnChatMessage);
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

            // Cast the address to the function pointer type
            originalOnChatMessage = (OnChatMessageFunc)(chatFunctionAddress);

            // Create the detour
            DetourAttach(&(PVOID&)originalOnChatMessage, DetourOnChatMessage);

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