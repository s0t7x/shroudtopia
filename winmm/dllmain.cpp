#pragma once

#pragma comment(linker,"/export:CloseDriver=c:\\windows\\system32\\winmm.CloseDriver,@4")
#pragma comment(linker,"/export:DefDriverProc=c:\\windows\\system32\\winmm.DefDriverProc,@5")
#pragma comment(linker,"/export:DriverCallback=c:\\windows\\system32\\winmm.DriverCallback,@6")
#pragma comment(linker,"/export:DrvGetModuleHandle=c:\\windows\\system32\\winmm.DrvGetModuleHandle,@7")
#pragma comment(linker,"/export:GetDriverModuleHandle=c:\\windows\\system32\\winmm.GetDriverModuleHandle,@8")
#pragma comment(linker,"/export:OpenDriver=c:\\windows\\system32\\winmm.OpenDriver,@9")
#pragma comment(linker,"/export:PlaySound=c:\\windows\\system32\\winmm.PlaySound,@10")
#pragma comment(linker,"/export:PlaySoundA=c:\\windows\\system32\\winmm.PlaySoundA,@11")
#pragma comment(linker,"/export:PlaySoundW=c:\\windows\\system32\\winmm.PlaySoundW,@12")
#pragma comment(linker,"/export:SendDriverMessage=c:\\windows\\system32\\winmm.SendDriverMessage,@13")
#pragma comment(linker,"/export:WOWAppExit=c:\\windows\\system32\\winmm.WOWAppExit,@14")
#pragma comment(linker,"/export:auxGetDevCapsA=c:\\windows\\system32\\winmm.auxGetDevCapsA,@15")
#pragma comment(linker,"/export:auxGetDevCapsW=c:\\windows\\system32\\winmm.auxGetDevCapsW,@16")
#pragma comment(linker,"/export:auxGetNumDevs=c:\\windows\\system32\\winmm.auxGetNumDevs,@17")
#pragma comment(linker,"/export:auxGetVolume=c:\\windows\\system32\\winmm.auxGetVolume,@18")
#pragma comment(linker,"/export:auxOutMessage=c:\\windows\\system32\\winmm.auxOutMessage,@19")
#pragma comment(linker,"/export:auxSetVolume=c:\\windows\\system32\\winmm.auxSetVolume,@20")
#pragma comment(linker,"/export:joyConfigChanged=c:\\windows\\system32\\winmm.joyConfigChanged,@21")
#pragma comment(linker,"/export:joyGetDevCapsA=c:\\windows\\system32\\winmm.joyGetDevCapsA,@22")
#pragma comment(linker,"/export:joyGetDevCapsW=c:\\windows\\system32\\winmm.joyGetDevCapsW,@23")
#pragma comment(linker,"/export:joyGetNumDevs=c:\\windows\\system32\\winmm.joyGetNumDevs,@24")
#pragma comment(linker,"/export:joyGetPos=c:\\windows\\system32\\winmm.joyGetPos,@25")
#pragma comment(linker,"/export:joyGetPosEx=c:\\windows\\system32\\winmm.joyGetPosEx,@26")
#pragma comment(linker,"/export:joyGetThreshold=c:\\windows\\system32\\winmm.joyGetThreshold,@27")
#pragma comment(linker,"/export:joyReleaseCapture=c:\\windows\\system32\\winmm.joyReleaseCapture,@28")
#pragma comment(linker,"/export:joySetCapture=c:\\windows\\system32\\winmm.joySetCapture,@29")
#pragma comment(linker,"/export:joySetThreshold=c:\\windows\\system32\\winmm.joySetThreshold,@30")
#pragma comment(linker,"/export:mciDriverNotify=c:\\windows\\system32\\winmm.mciDriverNotify,@31")
#pragma comment(linker,"/export:mciDriverYield=c:\\windows\\system32\\winmm.mciDriverYield,@32")
#pragma comment(linker,"/export:mciExecute=c:\\windows\\system32\\winmm.mciExecute,@3")
#pragma comment(linker,"/export:mciFreeCommandResource=c:\\windows\\system32\\winmm.mciFreeCommandResource,@33")
#pragma comment(linker,"/export:mciGetCreatorTask=c:\\windows\\system32\\winmm.mciGetCreatorTask,@34")
#pragma comment(linker,"/export:mciGetDeviceIDA=c:\\windows\\system32\\winmm.mciGetDeviceIDA,@35")
#pragma comment(linker,"/export:mciGetDeviceIDFromElementIDA=c:\\windows\\system32\\winmm.mciGetDeviceIDFromElementIDA,@36")
#pragma comment(linker,"/export:mciGetDeviceIDFromElementIDW=c:\\windows\\system32\\winmm.mciGetDeviceIDFromElementIDW,@37")
#pragma comment(linker,"/export:mciGetDeviceIDW=c:\\windows\\system32\\winmm.mciGetDeviceIDW,@38")
#pragma comment(linker,"/export:mciGetDriverData=c:\\windows\\system32\\winmm.mciGetDriverData,@39")
#pragma comment(linker,"/export:mciGetErrorStringA=c:\\windows\\system32\\winmm.mciGetErrorStringA,@40")
#pragma comment(linker,"/export:mciGetErrorStringW=c:\\windows\\system32\\winmm.mciGetErrorStringW,@41")
#pragma comment(linker,"/export:mciGetYieldProc=c:\\windows\\system32\\winmm.mciGetYieldProc,@42")
#pragma comment(linker,"/export:mciLoadCommandResource=c:\\windows\\system32\\winmm.mciLoadCommandResource,@43")
#pragma comment(linker,"/export:mciSendCommandA=c:\\windows\\system32\\winmm.mciSendCommandA,@44")
#pragma comment(linker,"/export:mciSendCommandW=c:\\windows\\system32\\winmm.mciSendCommandW,@45")
#pragma comment(linker,"/export:mciSendStringA=c:\\windows\\system32\\winmm.mciSendStringA,@46")
#pragma comment(linker,"/export:mciSendStringW=c:\\windows\\system32\\winmm.mciSendStringW,@47")
#pragma comment(linker,"/export:mciSetDriverData=c:\\windows\\system32\\winmm.mciSetDriverData,@48")
#pragma comment(linker,"/export:mciSetYieldProc=c:\\windows\\system32\\winmm.mciSetYieldProc,@49")
#pragma comment(linker,"/export:midiConnect=c:\\windows\\system32\\winmm.midiConnect,@50")
#pragma comment(linker,"/export:midiDisconnect=c:\\windows\\system32\\winmm.midiDisconnect,@51")
#pragma comment(linker,"/export:midiInAddBuffer=c:\\windows\\system32\\winmm.midiInAddBuffer,@52")
#pragma comment(linker,"/export:midiInClose=c:\\windows\\system32\\winmm.midiInClose,@53")
#pragma comment(linker,"/export:midiInGetDevCapsA=c:\\windows\\system32\\winmm.midiInGetDevCapsA,@54")
#pragma comment(linker,"/export:midiInGetDevCapsW=c:\\windows\\system32\\winmm.midiInGetDevCapsW,@55")
#pragma comment(linker,"/export:midiInGetErrorTextA=c:\\windows\\system32\\winmm.midiInGetErrorTextA,@56")
#pragma comment(linker,"/export:midiInGetErrorTextW=c:\\windows\\system32\\winmm.midiInGetErrorTextW,@57")
#pragma comment(linker,"/export:midiInGetID=c:\\windows\\system32\\winmm.midiInGetID,@58")
#pragma comment(linker,"/export:midiInGetNumDevs=c:\\windows\\system32\\winmm.midiInGetNumDevs,@59")
#pragma comment(linker,"/export:midiInMessage=c:\\windows\\system32\\winmm.midiInMessage,@60")
#pragma comment(linker,"/export:midiInOpen=c:\\windows\\system32\\winmm.midiInOpen,@61")
#pragma comment(linker,"/export:midiInPrepareHeader=c:\\windows\\system32\\winmm.midiInPrepareHeader,@62")
#pragma comment(linker,"/export:midiInReset=c:\\windows\\system32\\winmm.midiInReset,@63")
#pragma comment(linker,"/export:midiInStart=c:\\windows\\system32\\winmm.midiInStart,@64")
#pragma comment(linker,"/export:midiInStop=c:\\windows\\system32\\winmm.midiInStop,@65")
#pragma comment(linker,"/export:midiInUnprepareHeader=c:\\windows\\system32\\winmm.midiInUnprepareHeader,@66")
#pragma comment(linker,"/export:midiOutCacheDrumPatches=c:\\windows\\system32\\winmm.midiOutCacheDrumPatches,@67")
#pragma comment(linker,"/export:midiOutCachePatches=c:\\windows\\system32\\winmm.midiOutCachePatches,@68")
#pragma comment(linker,"/export:midiOutClose=c:\\windows\\system32\\winmm.midiOutClose,@69")
#pragma comment(linker,"/export:midiOutGetDevCapsA=c:\\windows\\system32\\winmm.midiOutGetDevCapsA,@70")
#pragma comment(linker,"/export:midiOutGetDevCapsW=c:\\windows\\system32\\winmm.midiOutGetDevCapsW,@71")
#pragma comment(linker,"/export:midiOutGetErrorTextA=c:\\windows\\system32\\winmm.midiOutGetErrorTextA,@72")
#pragma comment(linker,"/export:midiOutGetErrorTextW=c:\\windows\\system32\\winmm.midiOutGetErrorTextW,@73")
#pragma comment(linker,"/export:midiOutGetID=c:\\windows\\system32\\winmm.midiOutGetID,@74")
#pragma comment(linker,"/export:midiOutGetNumDevs=c:\\windows\\system32\\winmm.midiOutGetNumDevs,@75")
#pragma comment(linker,"/export:midiOutGetVolume=c:\\windows\\system32\\winmm.midiOutGetVolume,@76")
#pragma comment(linker,"/export:midiOutLongMsg=c:\\windows\\system32\\winmm.midiOutLongMsg,@77")
#pragma comment(linker,"/export:midiOutMessage=c:\\windows\\system32\\winmm.midiOutMessage,@78")
#pragma comment(linker,"/export:midiOutOpen=c:\\windows\\system32\\winmm.midiOutOpen,@79")
#pragma comment(linker,"/export:midiOutPrepareHeader=c:\\windows\\system32\\winmm.midiOutPrepareHeader,@80")
#pragma comment(linker,"/export:midiOutReset=c:\\windows\\system32\\winmm.midiOutReset,@81")
#pragma comment(linker,"/export:midiOutSetVolume=c:\\windows\\system32\\winmm.midiOutSetVolume,@82")
#pragma comment(linker,"/export:midiOutShortMsg=c:\\windows\\system32\\winmm.midiOutShortMsg,@83")
#pragma comment(linker,"/export:midiOutUnprepareHeader=c:\\windows\\system32\\winmm.midiOutUnprepareHeader,@84")
#pragma comment(linker,"/export:midiStreamClose=c:\\windows\\system32\\winmm.midiStreamClose,@85")
#pragma comment(linker,"/export:midiStreamOpen=c:\\windows\\system32\\winmm.midiStreamOpen,@86")
#pragma comment(linker,"/export:midiStreamOut=c:\\windows\\system32\\winmm.midiStreamOut,@87")
#pragma comment(linker,"/export:midiStreamPause=c:\\windows\\system32\\winmm.midiStreamPause,@88")
#pragma comment(linker,"/export:midiStreamPosition=c:\\windows\\system32\\winmm.midiStreamPosition,@89")
#pragma comment(linker,"/export:midiStreamProperty=c:\\windows\\system32\\winmm.midiStreamProperty,@90")
#pragma comment(linker,"/export:midiStreamRestart=c:\\windows\\system32\\winmm.midiStreamRestart,@91")
#pragma comment(linker,"/export:midiStreamStop=c:\\windows\\system32\\winmm.midiStreamStop,@92")
#pragma comment(linker,"/export:mixerClose=c:\\windows\\system32\\winmm.mixerClose,@93")
#pragma comment(linker,"/export:mixerGetControlDetailsA=c:\\windows\\system32\\winmm.mixerGetControlDetailsA,@94")
#pragma comment(linker,"/export:mixerGetControlDetailsW=c:\\windows\\system32\\winmm.mixerGetControlDetailsW,@95")
#pragma comment(linker,"/export:mixerGetDevCapsA=c:\\windows\\system32\\winmm.mixerGetDevCapsA,@96")
#pragma comment(linker,"/export:mixerGetDevCapsW=c:\\windows\\system32\\winmm.mixerGetDevCapsW,@97")
#pragma comment(linker,"/export:mixerGetID=c:\\windows\\system32\\winmm.mixerGetID,@98")
#pragma comment(linker,"/export:mixerGetLineControlsA=c:\\windows\\system32\\winmm.mixerGetLineControlsA,@99")
#pragma comment(linker,"/export:mixerGetLineControlsW=c:\\windows\\system32\\winmm.mixerGetLineControlsW,@100")
#pragma comment(linker,"/export:mixerGetLineInfoA=c:\\windows\\system32\\winmm.mixerGetLineInfoA,@101")
#pragma comment(linker,"/export:mixerGetLineInfoW=c:\\windows\\system32\\winmm.mixerGetLineInfoW,@102")
#pragma comment(linker,"/export:mixerGetNumDevs=c:\\windows\\system32\\winmm.mixerGetNumDevs,@103")
#pragma comment(linker,"/export:mixerMessage=c:\\windows\\system32\\winmm.mixerMessage,@104")
#pragma comment(linker,"/export:mixerOpen=c:\\windows\\system32\\winmm.mixerOpen,@105")
#pragma comment(linker,"/export:mixerSetControlDetails=c:\\windows\\system32\\winmm.mixerSetControlDetails,@106")
#pragma comment(linker,"/export:mmDrvInstall=c:\\windows\\system32\\winmm.mmDrvInstall,@107")
#pragma comment(linker,"/export:mmGetCurrentTask=c:\\windows\\system32\\winmm.mmGetCurrentTask,@108")
#pragma comment(linker,"/export:mmTaskBlock=c:\\windows\\system32\\winmm.mmTaskBlock,@109")
#pragma comment(linker,"/export:mmTaskCreate=c:\\windows\\system32\\winmm.mmTaskCreate,@110")
#pragma comment(linker,"/export:mmTaskSignal=c:\\windows\\system32\\winmm.mmTaskSignal,@111")
#pragma comment(linker,"/export:mmTaskYield=c:\\windows\\system32\\winmm.mmTaskYield,@112")
#pragma comment(linker,"/export:mmioAdvance=c:\\windows\\system32\\winmm.mmioAdvance,@113")
#pragma comment(linker,"/export:mmioAscend=c:\\windows\\system32\\winmm.mmioAscend,@114")
#pragma comment(linker,"/export:mmioClose=c:\\windows\\system32\\winmm.mmioClose,@115")
#pragma comment(linker,"/export:mmioCreateChunk=c:\\windows\\system32\\winmm.mmioCreateChunk,@116")
#pragma comment(linker,"/export:mmioDescend=c:\\windows\\system32\\winmm.mmioDescend,@117")
#pragma comment(linker,"/export:mmioFlush=c:\\windows\\system32\\winmm.mmioFlush,@118")
#pragma comment(linker,"/export:mmioGetInfo=c:\\windows\\system32\\winmm.mmioGetInfo,@119")
#pragma comment(linker,"/export:mmioInstallIOProcA=c:\\windows\\system32\\winmm.mmioInstallIOProcA,@120")
#pragma comment(linker,"/export:mmioInstallIOProcW=c:\\windows\\system32\\winmm.mmioInstallIOProcW,@121")
#pragma comment(linker,"/export:mmioOpenA=c:\\windows\\system32\\winmm.mmioOpenA,@122")
#pragma comment(linker,"/export:mmioOpenW=c:\\windows\\system32\\winmm.mmioOpenW,@123")
#pragma comment(linker,"/export:mmioRead=c:\\windows\\system32\\winmm.mmioRead,@124")
#pragma comment(linker,"/export:mmioRenameA=c:\\windows\\system32\\winmm.mmioRenameA,@125")
#pragma comment(linker,"/export:mmioRenameW=c:\\windows\\system32\\winmm.mmioRenameW,@126")
#pragma comment(linker,"/export:mmioSeek=c:\\windows\\system32\\winmm.mmioSeek,@127")
#pragma comment(linker,"/export:mmioSendMessage=c:\\windows\\system32\\winmm.mmioSendMessage,@128")
#pragma comment(linker,"/export:mmioSetBuffer=c:\\windows\\system32\\winmm.mmioSetBuffer,@129")
#pragma comment(linker,"/export:mmioSetInfo=c:\\windows\\system32\\winmm.mmioSetInfo,@130")
#pragma comment(linker,"/export:mmioStringToFOURCCA=c:\\windows\\system32\\winmm.mmioStringToFOURCCA,@131")
#pragma comment(linker,"/export:mmioStringToFOURCCW=c:\\windows\\system32\\winmm.mmioStringToFOURCCW,@132")
#pragma comment(linker,"/export:mmioWrite=c:\\windows\\system32\\winmm.mmioWrite,@133")
#pragma comment(linker,"/export:mmsystemGetVersion=c:\\windows\\system32\\winmm.mmsystemGetVersion,@134")
#pragma comment(linker,"/export:sndPlaySoundA=c:\\windows\\system32\\winmm.sndPlaySoundA,@135")
#pragma comment(linker,"/export:sndPlaySoundW=c:\\windows\\system32\\winmm.sndPlaySoundW,@136")
#pragma comment(linker,"/export:timeBeginPeriod=c:\\windows\\system32\\winmm.timeBeginPeriod,@137")
#pragma comment(linker,"/export:timeEndPeriod=c:\\windows\\system32\\winmm.timeEndPeriod,@138")
#pragma comment(linker,"/export:timeGetDevCaps=c:\\windows\\system32\\winmm.timeGetDevCaps,@139")
#pragma comment(linker,"/export:timeGetSystemTime=c:\\windows\\system32\\winmm.timeGetSystemTime,@140")
#pragma comment(linker,"/export:timeGetTime=c:\\windows\\system32\\winmm.timeGetTime,@141")
#pragma comment(linker,"/export:timeKillEvent=c:\\windows\\system32\\winmm.timeKillEvent,@142")
#pragma comment(linker,"/export:timeSetEvent=c:\\windows\\system32\\winmm.timeSetEvent,@143")
#pragma comment(linker,"/export:waveInAddBuffer=c:\\windows\\system32\\winmm.waveInAddBuffer,@144")
#pragma comment(linker,"/export:waveInClose=c:\\windows\\system32\\winmm.waveInClose,@145")
#pragma comment(linker,"/export:waveInGetDevCapsA=c:\\windows\\system32\\winmm.waveInGetDevCapsA,@146")
#pragma comment(linker,"/export:waveInGetDevCapsW=c:\\windows\\system32\\winmm.waveInGetDevCapsW,@147")
#pragma comment(linker,"/export:waveInGetErrorTextA=c:\\windows\\system32\\winmm.waveInGetErrorTextA,@148")
#pragma comment(linker,"/export:waveInGetErrorTextW=c:\\windows\\system32\\winmm.waveInGetErrorTextW,@149")
#pragma comment(linker,"/export:waveInGetID=c:\\windows\\system32\\winmm.waveInGetID,@150")
#pragma comment(linker,"/export:waveInGetNumDevs=c:\\windows\\system32\\winmm.waveInGetNumDevs,@151")
#pragma comment(linker,"/export:waveInGetPosition=c:\\windows\\system32\\winmm.waveInGetPosition,@152")
#pragma comment(linker,"/export:waveInMessage=c:\\windows\\system32\\winmm.waveInMessage,@153")
#pragma comment(linker,"/export:waveInOpen=c:\\windows\\system32\\winmm.waveInOpen,@154")
#pragma comment(linker,"/export:waveInPrepareHeader=c:\\windows\\system32\\winmm.waveInPrepareHeader,@155")
#pragma comment(linker,"/export:waveInReset=c:\\windows\\system32\\winmm.waveInReset,@156")
#pragma comment(linker,"/export:waveInStart=c:\\windows\\system32\\winmm.waveInStart,@157")
#pragma comment(linker,"/export:waveInStop=c:\\windows\\system32\\winmm.waveInStop,@158")
#pragma comment(linker,"/export:waveInUnprepareHeader=c:\\windows\\system32\\winmm.waveInUnprepareHeader,@159")
#pragma comment(linker,"/export:waveOutBreakLoop=c:\\windows\\system32\\winmm.waveOutBreakLoop,@160")
#pragma comment(linker,"/export:waveOutClose=c:\\windows\\system32\\winmm.waveOutClose,@161")
#pragma comment(linker,"/export:waveOutGetDevCapsA=c:\\windows\\system32\\winmm.waveOutGetDevCapsA,@162")
#pragma comment(linker,"/export:waveOutGetDevCapsW=c:\\windows\\system32\\winmm.waveOutGetDevCapsW,@163")
#pragma comment(linker,"/export:waveOutGetErrorTextA=c:\\windows\\system32\\winmm.waveOutGetErrorTextA,@164")
#pragma comment(linker,"/export:waveOutGetErrorTextW=c:\\windows\\system32\\winmm.waveOutGetErrorTextW,@165")
#pragma comment(linker,"/export:waveOutGetID=c:\\windows\\system32\\winmm.waveOutGetID,@166")
#pragma comment(linker,"/export:waveOutGetNumDevs=c:\\windows\\system32\\winmm.waveOutGetNumDevs,@167")
#pragma comment(linker,"/export:waveOutGetPitch=c:\\windows\\system32\\winmm.waveOutGetPitch,@168")
#pragma comment(linker,"/export:waveOutGetPlaybackRate=c:\\windows\\system32\\winmm.waveOutGetPlaybackRate,@169")
#pragma comment(linker,"/export:waveOutGetPosition=c:\\windows\\system32\\winmm.waveOutGetPosition,@170")
#pragma comment(linker,"/export:waveOutGetVolume=c:\\windows\\system32\\winmm.waveOutGetVolume,@171")
#pragma comment(linker,"/export:waveOutMessage=c:\\windows\\system32\\winmm.waveOutMessage,@172")
#pragma comment(linker,"/export:waveOutOpen=c:\\windows\\system32\\winmm.waveOutOpen,@173")
#pragma comment(linker,"/export:waveOutPause=c:\\windows\\system32\\winmm.waveOutPause,@174")
#pragma comment(linker,"/export:waveOutPrepareHeader=c:\\windows\\system32\\winmm.waveOutPrepareHeader,@175")
#pragma comment(linker,"/export:waveOutReset=c:\\windows\\system32\\winmm.waveOutReset,@176")
#pragma comment(linker,"/export:waveOutRestart=c:\\windows\\system32\\winmm.waveOutRestart,@177")
#pragma comment(linker,"/export:waveOutSetPitch=c:\\windows\\system32\\winmm.waveOutSetPitch,@178")
#pragma comment(linker,"/export:waveOutSetPlaybackRate=c:\\windows\\system32\\winmm.waveOutSetPlaybackRate,@179")
#pragma comment(linker,"/export:waveOutSetVolume=c:\\windows\\system32\\winmm.waveOutSetVolume,@180")
#pragma comment(linker,"/export:waveOutUnprepareHeader=c:\\windows\\system32\\winmm.waveOutUnprepareHeader,@181")
#pragma comment(linker,"/export:waveOutWrite=c:\\windows\\system32\\winmm.waveOutWrite,@182")

#include <windows.h>
#include <ios>
#include <fstream>
#include <iostream>

#include <cstdint>
#include <sstream>
#include <iomanip>


#include <thread>
#include "nlohmannjson.hpp"

HMODULE hModule = LoadLibrary("c:\\windows\\system32\\winmm.dll");

using json = nlohmann::json;

struct Config {
    int boot_delay;
    int exp_multiplier;
    boolean glider_flight;
    boolean no_fall_damage;
    boolean no_stamina_loss;
    boolean no_durability_loss;
    boolean clone_item_splits;
    boolean free_craft;
};


VOID Log(LPCSTR szInput)
{
    std::ofstream log("shroudtopia.log", std::ios_base::app | std::ios_base::out);
    log << szInput;
    log << "\n";
    std::cout << "[### shroudtopia ###] " << szInput << std::endl;
}

uintptr_t findPattern(const char* pattern, const char* mask, uintptr_t start, size_t length) {
    for (uintptr_t i = start; i < start + length; i++) {
        bool found = true;
        for (size_t j = 0; mask[j] != '\0'; j++) {
            if (mask[j] != '?' && pattern[j] != *(char*)(i + j)) {
                found = false;
                break;
            }
        }
        if (found) return i;
    }
    return 0;
}

void PrintHexBytes(const uint8_t* data, size_t length, const std::string& label, uintptr_t baseAddress = 0) {
    std::ostringstream oss;
    oss << std::hex << baseAddress << label << " (size: " << std::dec << length << "):" << std::endl;
    for (size_t i = 0; i < length; ++i) {
        if (i % 16 == 0) oss << std::endl << std::setw(4) << std::setfill('0') << i << ": ";
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    Log(oss.str().c_str());
}

void PrintJumpDetails(uintptr_t baseAddress, const std::string& label) {
    std::ostringstream oss;
    oss << label << " baseAddress: " << std::hex << baseAddress << std::endl;
    uintptr_t nextInstruction = baseAddress + 5;
    uintptr_t offset = *(uintptr_t*)(baseAddress + 1);
    uintptr_t targetAddress = nextInstruction + offset;
    oss << "Next instruction address: " << std::hex << nextInstruction << std::endl;
    oss << "Offset: " << std::hex << offset << std::endl;
    oss << "Target address: " << std::hex << targetAddress << std::endl;
    Log(oss.str().c_str());
}

bool WriteMemory(uintptr_t address, const void* data, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        Log(std::string("VirtualProtect failed with error: ").append(std::to_string(GetLastError())).c_str());
        return false;
    }

    try {
        memcpy((void*)address, data, size);
    }
    catch (std::runtime_error err) {
        Log("Exception occurred while writing memory.");
        VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
        return false;
    }

    if (!VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect)) {
        Log(std::string("VirtualProtect restore failed with error: ").append(std::to_string(GetLastError())).c_str());
        return false;
    }

    

    return true;
}

void createConfig() {
    std::ofstream configFile("shroudtopia.json");
    if (configFile.is_open()) {
        json jConfig;
        jConfig["boot_delay"] = 3000;  
        jConfig["exp_multiplier"] = 5;
        jConfig["glider_flight"] = true; 
        jConfig["no_fall_damage"] = true; 
        jConfig["no_stamina_loss"] = true; 
        jConfig["no_durability_loss"] = true; 
        jConfig["clone_item_splits"] = true; 
        jConfig["free_craft"] = true; 
        configFile << jConfig.dump(4);  // Write pretty-printed JSON to file
        configFile.close();
    }
}

bool ReadConfig(Config& config) {
    std::ifstream configFile("shroudtopia.json");
    if (configFile.is_open()) {
        json jConfig;
        configFile >> jConfig;
        config.boot_delay = jConfig.value("boot_delay", 3000); 
        config.exp_multiplier = jConfig.value("exp_multiplier", 5); 
        config.glider_flight = jConfig.value("glider_flight", true);  
        config.no_fall_damage = jConfig.value("no_fall_damage", true); 
        config.no_stamina_loss = jConfig.value("no_stamina_loss", true);
        config.no_durability_loss = jConfig.value("no_durability_loss", true);
        config.clone_item_splits = jConfig.value("clone_item_splits", true);
        config.free_craft = jConfig.value("free_craft", true);
        return true;
    }
    return false;
}

LPVOID AllocateMemoryNearAddress(LPVOID address, SIZE_T dwSize) {
    HANDLE hProcess = GetCurrentProcess();
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    LPVOID lpBaseAddress = nullptr;

    // Calculate a range near the target address
    LPVOID lpMinAddress = (LPVOID)((DWORD_PTR)address - si.dwAllocationGranularity);
    LPVOID lpMaxAddress = (LPVOID)((DWORD_PTR)address + si.dwAllocationGranularity);

    // Attempt to allocate memory within the specified range
    lpBaseAddress = VirtualAllocEx(hProcess, lpMinAddress, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!lpBaseAddress) {
        // Retry with a wider range if necessary
        lpBaseAddress = VirtualAllocEx(hProcess, (LPVOID)((uintptr_t)lpMinAddress - 0x10000000), dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }

    return lpBaseAddress;
}

boolean ApplyExpMultiplier(const Config& config) {
    const char* pattern = "\x48\x03\xC3\x01\x34\x88\x48\x8B";
    const char* mask = "xxxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        void* newmem = AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
                0x0F, 0xAF, 0x35, 0x00, 0x00, 0x00, 0x00,  // imul esi, [multipler_exp]
                0x48, 0x03, 0xC3,                          // add rax, rbx
                0x01, 0x34, 0x88,                          // add [rax+rcx*4], esi
                0xE9, 0x00, 0x00, 0x00, 0x00               // jmp return
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x90                           // nop
            };

            uintptr_t multDataAddress = (uintptr_t)newmem + sizeof(newCode);

            *(int32_t*)(newCode + 3) = (int32_t)(multDataAddress - ((uintptr_t)newmem + 7));  // Offset to multipler_exp

            *(int32_t*)(newCode + 14) = (int32_t)((address + sizeof(patch)) - ((uintptr_t)newmem + sizeof(newCode)));

            int expMultiplier = config.exp_multiplier;
            memcpy((void*)multDataAddress, &config.exp_multiplier, sizeof(int));

            memcpy(newmem, newCode, sizeof(newCode));


            *(int32_t*)(patch + 1) = (uintptr_t)newmem - (address + 5);

            WriteMemory(address, patch, sizeof(patch));

            Log(std::string("Exp multiplier set to: ").append(std::to_string(config.exp_multiplier)).c_str());
            return true;
        }
    }

    Log("Failed to set exp multiplier");
    return false;
}

boolean ApplyGliderFlight(const Config& config) {
    if (!config.glider_flight) return true;

    const char* pattern = "\xF3\x0F\x10\x05\x00\x00\x00\x00\x77";
    const char* mask = "xxxx????x";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        void* newmem = AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            float glideData = -1.57f;

            // Construct the new code block
            uint8_t newCode[] = {
                0xF3, 0x0F, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00,  // movss xmm0, [GlideData]
                0xE9, 0x00, 0x00, 0x00, 0x00,                    // jmp returnAddress
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x90, 0x90, 0x90               // nop 3
            };

            uintptr_t returnAddress = address + sizeof(patch);

            uintptr_t glideDataAddress = (uintptr_t)newmem + sizeof(newCode);
            *(uint32_t*)(newCode + 4) = (uintptr_t)glideDataAddress - ((uintptr_t)newmem + 8);  // Offset to GlideData
            *(uint32_t*)(newCode + 9) = (uintptr_t)returnAddress - ((uintptr_t)newmem + sizeof(newCode));  // Offset to return address

            memcpy((void*)glideDataAddress, &glideData, sizeof(glideData));  // Write the float value at GlideData
            memcpy(newmem, newCode, sizeof(newCode));



            *(uint32_t*)(patch + 1) = (uintptr_t)newmem - (address + 5);
            WriteMemory(address, patch, sizeof(patch));

            Log("Applied GliderFlight");
            return true;
        }
    }
    Log("Failed to apply GliderFlight");
    return false;
}

boolean ApplyNoStaminaLoss(const Config& config) {
    if (!config.no_stamina_loss) return true;
    
    const char* pattern = "\x8B\x04\x81\x89\x44\x24\x3C";
    const char* mask = "xxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        void* newmem = AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
                0x53,                               // push rbx
                0x8B, 0x5C, 0x81, 0x08,             // mov ebx, [rcx+rax*4+8]
                0x89, 0x1C, 0x81,                   // mov [rcx+rax*4], ebx
                0x5B,                               // pop rbx
                0x8B, 0x04, 0x81,                   // mov eax, [rcx+rax*4]
                0x89, 0x44, 0x24, 0x3C,             // mov [rsp+3C], eax
                0xE9, 0x00, 0x00, 0x00, 0x00        // jmp return
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x66, 0x90                     // nop 2 (x86 nop)
            };

            int32_t returnAddress = (int32_t)((address + sizeof(patch)) - ((uintptr_t)newmem + sizeof(newCode) ));  // Offset to return to after executing our code
            *(int32_t*)(newCode + sizeof(newCode) - 4) = returnAddress;

            memcpy(newmem, newCode, sizeof(newCode));

            *(int32_t*)(patch + 1) = (int32_t)((uintptr_t)newmem - (address + 5));
            WriteMemory(address, patch, sizeof(patch));

            Log("Applied NoStaminaLoss");

            return true;
        }

    }

    Log("Failed to apply NoStaminaLoss");
    return false;
}

boolean ApplyNoFallDamage(const Config& config) {
    if (!config.no_fall_damage) return true;


    // 40 53 48 83 EC 60 41 B8 20 00 00 00 48 8D 54 24 20 48 8B D9 E8 ** ** ** ** 41 B8 20 00 00 00 48 8D 54 24 20 48 8B CB E8 ** ** ** ** 84 C0 0F 84 D9 00 00 00 48 89 7C 24 70
    // \x40\x53\x48\x83\xEC\x60\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xCB\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\xD9\x00\x00\x00\x48\x89\x7C\x24\x70
    // xxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxxxxxxxxxx
    const char* pattern = "\x40\x53\x48\x83\xEC\x60\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xCB\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\xD9\x00\x00\x00\x48\x89\x7C\x24\x70";
    const char* mask = "xxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxxxxxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        uint8_t patch[] = {
            0xC3
        };

        WriteMemory(address, patch, sizeof(patch));

        Log("Applied NoFallDamage");

        return true;
    }

    Log("Failed to apply NoFallDamage");
    return false;
}

boolean ApplyNoDurabilityLoss(const Config& config) {
    if (!config.no_durability_loss) return true;

    const char* pattern = "\x0F\x84\x00\x00\x00\x00\x8B\x00\x00\x48\x03\x00\x01";
    const char* mask = "xx????x??xx?x";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {

        void* newmem = AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
              0x85, 0xF6,
              0x0F, 0x89, 0x08, 0x00, 0x00, 0x00,
              0x48, 0x03, 0xC3,
              0xE9, 0x00, 0x00, 0x00, 0x00, // return addr
              0x48, 0x03, 0xC3,
              0x01, 0x34, 0x88,
              0xE9, 0x00, 0x00, 0x00, 0x00, // return addr
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,
                0x90
            };

            uintptr_t returnAddress = address + sizeof(patch);

            *(uint32_t*)(newCode + 12) = (uintptr_t)returnAddress - ((uintptr_t)newmem + 16);
            *(uint32_t*)(newCode + 23) = (uintptr_t)returnAddress - ((uintptr_t)newmem + sizeof(newCode));

            memcpy(newmem, newCode, sizeof(newCode));

            *(uint32_t*)(patch + 1) = (uintptr_t)newmem - ((uintptr_t)address + 5);

            WriteMemory(address, patch, sizeof(patch));

            Log("Applied NoDurabilityLoss");

            return true;
        }
    }

    Log("Failed to apply NoDurabilityLoss");
    return false;
}

boolean ApplyCloneItemSplits(const Config& config) {
    if (!config.clone_item_splits) return true;

    const char* pattern = "\x29\x00\x04\xEB\x00\x0F\x57";
    const char* mask = "x?xx?xx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        uint8_t patch[] = {
            0x90, 0x90, 0x90 // NOP 3
        };

        WriteMemory(address, patch, sizeof(patch));

        Log("Applied CloneItemSplits");

        return true;
    }

    Log("Failed to apply CloneItemSplits");
    return false;
}

boolean ApplyFreeCraft(const Config& config) {
    if (!config.free_craft) return true;

    const char* pattern = "\x43\x8B\x74\xF5\x04";
    const char* mask = "xxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {

        uint8_t patch[] = {
            0xBE, 0x00, 0x00, 0x00, 0x00
        };

        WriteMemory(address, patch, sizeof(patch));

        Log("Applied FreeCraft");

        return true;
    }

    Log("Failed to apply FreeCraft");
    return false;
}

boolean ApplyFreeBuild(const Config& config) {
    if (!config.free_craft) return true;

    const char* pattern = "\x48\x0F\x00\x00\x00\x8B\x00\x00\x8B\x00\x04\x89";
    const char* mask = "xx???x??x?xx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = findPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {

        uint8_t patch[] = {
            0x31, 0xF6, 0x90, 0x90
        };

        WriteMemory(address + 7, patch, sizeof(patch));

        Log("Applied FreeBuild");

        return true;
    }

    Log("Failed to apply FreeBuild");
    return false;
}

void PatchMemoryLoop() {
    Log("Thread started");

    Config config;
    if (!ReadConfig(config)) {
        createConfig();
        if (!ReadConfig(config)) {
            return;
        }
    }
    Log("Loaded config");

    static boolean appliedExpMultiplier = false;
    static boolean appliedGliderFlight = false;
    static boolean appliedNoFallDamage = false;
    static boolean appliedNoStaminaLoss = false;
    static boolean appliedNoDurabilityLoss = false;
    static boolean appliedCloneItemSplits = false;
    static boolean appliedFreeCraft = false;
    static boolean appliedFreeBuild = false;

    Log(std::string("Server boot delay: ").append(std::to_string(config.boot_delay)).c_str());
    Sleep(config.boot_delay);

    static int retries = 0;
    while (retries < 10) {

        if (!appliedExpMultiplier   )    appliedExpMultiplier   = ApplyExpMultiplier    (config);
        if (!appliedGliderFlight    )    appliedGliderFlight    = ApplyGliderFlight     (config);
        if (!appliedNoStaminaLoss   )    appliedNoStaminaLoss   = ApplyNoStaminaLoss    (config);
        if (!appliedNoFallDamage    )    appliedNoFallDamage    = ApplyNoFallDamage     (config);
        if (!appliedNoDurabilityLoss)    appliedNoDurabilityLoss= ApplyNoDurabilityLoss (config);
        if (!appliedCloneItemSplits )    appliedCloneItemSplits = ApplyCloneItemSplits  (config);
        if (!appliedFreeCraft       )    appliedFreeCraft       = ApplyFreeCraft        (config);
        if (!appliedFreeBuild       )    appliedFreeBuild       = ApplyFreeBuild        (config);

        retries++;
        Sleep(1000);  // Wait for 1 second before trying again
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Log("loaded 'winmm.dll'");
        std::thread(PatchMemoryLoop).detach();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}