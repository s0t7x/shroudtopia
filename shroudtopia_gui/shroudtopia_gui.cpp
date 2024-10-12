#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

#include "nlohmannjson.hpp"

using json = nlohmann::json;

json shroudtopiaData = R"({
    "active": true,
    "bootDelay": 3000,
    "enableLogging": true,
    "logLevel": "VERBOSE",
    "mods": {},
    "updateDelay": 500
})"_json; // Global JSON object to hold the data

std::string filename = "shroudtopia.json";

#define ID_ADD_MOD 1000
#define ID_ADD_PROP_OFFSET 2000
#define ID_TYPE_DROPDOWN_OFFSET 3000

const int MAX_WINDOW_WIDTH = 500;
const int MAX_WINDOW_HEIGHT = 500;

// Controls and dropdowns tracking
std::map<std::string, HWND> controls;
std::vector<HWND> typeDropdowns;
std::vector<std::pair<std::string, std::string>> modProperties;
std::vector<int> addPropertyButtons;

// Load JSON from file
void LoadJSON(const std::string& file)
{
    std::ifstream ifs(file);
    if (ifs.is_open())
    {
        ifs >> shroudtopiaData;
    }
}

// Save JSON to file
void SaveJSON(const std::string& file)
{
    std::ofstream ofs(file);
    if (ofs.is_open())
    {
        ofs << shroudtopiaData.dump(4);
    }
}

// Helper to create a control for each field
HWND CreateControl(HWND parent, const std::string& label, const nlohmann::json& value, int& yOffset, int id, const std::string& key)
{
    HWND hwndStatic = CreateWindow("STATIC", label.c_str(), WS_VISIBLE | WS_CHILD, 10, yOffset, 200, 20, parent, nullptr, nullptr, nullptr);
    HWND hwndControl = nullptr;

    if (value.is_boolean())
    {
        hwndControl = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 220, yOffset, 20, 20, parent, (HMENU)id, nullptr, nullptr);
        SendMessage(hwndControl, BM_SETCHECK, value.get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    }
    else if (value.is_number_integer())
    {
        hwndControl = CreateWindow("EDIT", std::to_string(value.get<int>()).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 220, yOffset, 100, 20, parent, (HMENU)id, nullptr, nullptr);
    }
    else if (value.is_string())
    {
        hwndControl = CreateWindow("EDIT", value.get<std::string>().c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 220, yOffset, 100, 20, parent, (HMENU)id, nullptr, nullptr);
    }

    yOffset += 30;
    return hwndControl;
}

// Dynamically create UI controls based on JSON content
void CreateControls(HWND hwnd)
{
    // Clear all existing controls before redrawing
    for (auto& [key, control] : controls)
    {
        DestroyWindow(control);
    }
    controls.clear();
    typeDropdowns.clear();
    modProperties.clear();
    addPropertyButtons.clear();

    int yOffset = 10;

    // Create controls for root-level JSON fields
    for (auto& [key, value] : shroudtopiaData.items())
    {
        if (key == "mods")
            continue; // Mods handled separately

        int id = controls.size() + 1;
        HWND control = CreateControl(hwnd, key, value, yOffset, id, key);
        controls[key] = control;
    }

    // Create controls for mods
    for (auto& [modName, modPropertiesJson] : shroudtopiaData["mods"].items())
    {
        HWND hwndStatic = CreateWindow("STATIC", modName.c_str(), WS_VISIBLE | WS_CHILD, 10, yOffset, 200, 20, hwnd, nullptr, nullptr, nullptr);
        yOffset += 30;

        // Create controls for each mod property
        for (auto& [propName, propValue] : modPropertiesJson.items())
        {
            std::string fullKey = modName + "." + propName;
            int id = controls.size() + 1;

            // Create property control
            HWND control = CreateControl(hwnd, propName, propValue, yOffset, id, fullKey);
            controls[fullKey] = control;

            // Create type dropdown
            HWND hwndDropdown = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 330, yOffset - 30, 100, 100, hwnd, (HMENU)(ID_TYPE_DROPDOWN_OFFSET + id), nullptr, nullptr);
            SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)"bool");
            SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)"string");
            SendMessage(hwndDropdown, CB_ADDSTRING, 0, (LPARAM)"number");

            int typeIndex = propValue.is_boolean() ? 0 : propValue.is_string() ? 1 : 2;
            SendMessage(hwndDropdown, CB_SETCURSEL, typeIndex, 0);

            typeDropdowns.push_back(hwndDropdown);
            modProperties.emplace_back(modName, propName);
        }

        // Add "Add Property" button
        int addPropID = ID_ADD_PROP_OFFSET + addPropertyButtons.size();
        HWND hwndButton = CreateWindow("BUTTON", "Add Property", WS_VISIBLE | WS_CHILD, 10, yOffset, 200, 30, hwnd, (HMENU)addPropID, nullptr, nullptr);
        yOffset += 40;
        addPropertyButtons.push_back(addPropID);
    }

    // Add "Add Mod" button at the end
    HWND hwndAddModButton = CreateWindow("BUTTON", "Add Mod", WS_VISIBLE | WS_CHILD, 10, yOffset, 200, 30, hwnd, (HMENU)ID_ADD_MOD, nullptr, nullptr);
}

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndScroll;
    static int scrollPos = 0;
    static SCROLLINFO si;

    switch (uMsg)
    {
    case WM_CREATE:
        // Initialize scroll bar
        hwndScroll = CreateWindowEx(0, "SCROLLBAR", nullptr, WS_CHILD | WS_VISIBLE | SBS_VERT,
            MAX_WINDOW_WIDTH - 20, 0, 20, MAX_WINDOW_HEIGHT, hwnd, (HMENU)1, GetModuleHandle(nullptr), nullptr);

        // Set scroll bar properties
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        si.nMin = 0;
        si.nMax = 100;
        si.nPage = 50;
        si.nPos = 0;
        SetScrollInfo(hwndScroll, SB_CTL, &si, TRUE);

        // Create initial controls
        CreateControls(hwnd);
        break;

    case WM_VSCROLL:
        // Handle scrolling
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwndScroll, SB_CTL, &si);

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            scrollPos = max(0, scrollPos - 10);
            break;
        case SB_LINEDOWN:
            scrollPos = min(si.nMax, scrollPos + 10);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            scrollPos = HIWORD(wParam);
            break;
        }

        si.fMask = SIF_POS;
        si.nPos = scrollPos;
        SetScrollInfo(hwndScroll, SB_CTL, &si, TRUE);

        InvalidateRect(hwnd, nullptr, TRUE);
        break;

    case WM_COMMAND:
        // Handle type dropdown, add mod/property logic
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            HWND dropdown = (HWND)lParam;
            int selectedIndex = (int)SendMessage(dropdown, CB_GETCURSEL, 0, 0);

            for (size_t i = 0; i < typeDropdowns.size(); ++i)
            {
                if (dropdown == typeDropdowns[i])
                {
                    auto& [modName, propName] = modProperties[i];
                    nlohmann::json& modValue = shroudtopiaData["mods"][modName][propName];

                    if (selectedIndex == 0) modValue = false;
                    else if (selectedIndex == 1) modValue = "";
                    else if (selectedIndex == 2) modValue = 0;

                    SaveJSON(filename);
                    break;
                }
            }
        }
        else if (HIWORD(wParam) == BN_CLICKED)
        {
            if (LOWORD(wParam) == ID_ADD_MOD)
            {
                // Add new mod logic
                std::string newModName = "Mod" + std::to_string(modProperties.size() + 1);
                shroudtopiaData["mods"][newModName] = nlohmann::json::object();
                SaveJSON(filename);
                CreateControls(hwnd);
            }
            else if (LOWORD(wParam) >= ID_ADD_PROP_OFFSET)
            {
                // Add new property logic
                int index = LOWORD(wParam) - ID_ADD_PROP_OFFSET;
                std::string modName = modProperties[index].first;
                shroudtopiaData["mods"][modName]["New Property"] = false;
                SaveJSON(filename);
                CreateControls(hwnd);
            }
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    LoadJSON(filename);

    const char CLASS_NAME[] = "JSON Editor";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "JSON Editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, MAX_WINDOW_WIDTH, MAX_WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}