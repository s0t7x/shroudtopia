// shroudtopia_gui.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "shroudtopia_gui.h"

#include "nlohmannjson.hpp"

#include <windows.h>
#include <fstream>
#include <string>
#include <commdlg.h> // For OpenFile dialog

#include "Resource.h"

using json = nlohmann::json;

json config = {
    {"active", true},
    {"boot_delay", 3000},
    {"update_delay", 500},
    {"bypass_altar_limit", false},
    {"glider_flight", false},
    {"infinite_item_use", false},
    {"no_craft_cost", false},
    {"no_fall_damage", false},
    {"no_stamina_loss", false}
};

#define CONFIG_FILE "shroudtopia.json"

// Global variable to store JSON data
json configJson;

HWND hCheckboxes[6];
HWND hBootDelayEdit, hUpdateDelayEdit;

void LoadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        try {
            file >> config;
        }
        catch (...) {
            // In case of any error, we revert to default values
            MessageBox(NULL, "Error loading config, using default values.", "Error", MB_OK | MB_ICONERROR);
        }
    }
    file.close();
}

// Function to save config to a file
void SaveConfig(const std::string& filePath) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << config.dump(4);  // Pretty print the JSON with indentation
    }
    file.close();
}

// Function to update the UI controls with the config values
void UpdateUIFromConfig() {
    // Update the boot delay text box
    SetWindowTextA(hBootDelayEdit, std::to_string(config["boot_delay"].get<int>()).c_str());

    // Update the update delay text box
    SetWindowTextA(hUpdateDelayEdit, std::to_string(config["update_delay"].get<int>()).c_str());

    // Update the checkbox states
    SendMessage(hCheckboxes[0], BM_SETCHECK, config["bypass_altar_limit"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hCheckboxes[1], BM_SETCHECK, config["glider_flight"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hCheckboxes[2], BM_SETCHECK, config["infinite_item_use"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hCheckboxes[3], BM_SETCHECK, config["no_craft_cost"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hCheckboxes[4], BM_SETCHECK, config["no_fall_damage"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hCheckboxes[5], BM_SETCHECK, config["no_stamina_loss"].get<bool>() ? BST_CHECKED : BST_UNCHECKED, 0);
}

// Function to update the config from the UI controls
void UpdateConfigFromUI() {
    char buffer[256];

    // Boot delay
    GetWindowTextA(hBootDelayEdit, buffer, sizeof(buffer));
    config["boot_delay"] = std::stoi(buffer);

    // Update delay
    GetWindowTextA(hUpdateDelayEdit, buffer, sizeof(buffer));
    config["update_delay"] = std::stoi(buffer);

    // Boolean checkboxes
    config["bypass_altar_limit"] = (SendMessage(hCheckboxes[0], BM_GETCHECK, 0, 0) == BST_CHECKED);
    config["glider_flight"] = (SendMessage(hCheckboxes[1], BM_GETCHECK, 0, 0) == BST_CHECKED);
    config["infinite_item_use"] = (SendMessage(hCheckboxes[2], BM_GETCHECK, 0, 0) == BST_CHECKED);
    config["no_craft_cost"] = (SendMessage(hCheckboxes[3], BM_GETCHECK, 0, 0) == BST_CHECKED);
    config["no_fall_damage"] = (SendMessage(hCheckboxes[4], BM_GETCHECK, 0, 0) == BST_CHECKED);
    config["no_stamina_loss"] = (SendMessage(hCheckboxes[5], BM_GETCHECK, 0, 0) == BST_CHECKED);
}

// Function to handle file loading
void ImportConfigFile(HWND hWnd) {
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "JSON\0*.json\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        LoadConfig(ofn.lpstrFile);
        UpdateUIFromConfig();
    }
}

// Function to handle file exporting
void ExportConfigFile(HWND hWnd) {
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "JSON\0*.json\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        UpdateConfigFromUI();
        SaveConfig(ofn.lpstrFile);
    }
}

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE hInst;                                // Aktuelle Instanz
CHAR szTitle[MAX_LOADSTRING];                  // Titelleistentext
CHAR szWindowClass[MAX_LOADSTRING];            // Der Klassenname des Hauptfensters.

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Hier Code einfügen.

    // Globale Zeichenfolgen initialisieren
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_SHROUDTOPIAGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Anwendungsinitialisierung ausführen:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHROUDTOPIAGUI));

    MSG msg;

    // Hauptnachrichtenschleife:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_MYICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_SHROUDTOPIAGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDC_MYICON));

    return RegisterClassEx(&wcex);
}

int windowWidth = 300;
int windowHeight = 310;

//
//   FUNKTION: InitInstance(HINSTANCE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTARE:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

   // Get the screen dimensions
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   // Calculate the centered position
   int x = (screenWidth - windowWidth) / 2;
   int y = (screenHeight - windowHeight) / 2;

   HWND hWnd = CreateWindowEx(
       0,                              // Optional window styles
       szWindowClass, szTitle,
       WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
       x, y, windowWidth, windowHeight,
       NULL,                           // Parent window    
       NULL,                           // Menu
       hInstance,                      // Instance handle
       NULL                            // Additional application data
   );

   if (!hWnd)
   {
      return FALSE;
   }


   // Set the window position and size
   SetWindowPos(hWnd, NULL, x, y, windowWidth, windowHeight, SWP_NOZORDER);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

#define IDC_CHECKBOX_START 1100

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK: Verarbeitet Meldungen für das Hauptfenster.
//
//  WM_COMMAND  - Verarbeiten des Anwendungsmenüs
//  WM_PAINT    - Darstellen des Hauptfensters
//  WM_DESTROY  - Ausgeben einer Beendenmeldung und zurückkehren
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        // Create edit controls for boot and update delay
        CreateWindow("STATIC", "Boot Delay (ms):", WS_VISIBLE | WS_CHILD, 10, 10, 150, 20, hWnd, NULL, NULL, NULL);
        hBootDelayEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 170, 10, 100, 20, hWnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Update Delay (ms):", WS_VISIBLE | WS_CHILD, 10, 40, 150, 20, hWnd, NULL, NULL, NULL);
        hUpdateDelayEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 170, 40, 100, 20, hWnd, NULL, NULL, NULL);

        // Create checkboxes for the boolean values
        hCheckboxes[0] = CreateWindow("BUTTON", "Bypass Altar Limit", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 70, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 0), NULL, NULL);
        hCheckboxes[1] = CreateWindow("BUTTON", "Glider Flight", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 100, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 1), NULL, NULL);
        hCheckboxes[2] = CreateWindow("BUTTON", "Infinite Item Use", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 130, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 2), NULL, NULL);
        hCheckboxes[3] = CreateWindow("BUTTON", "No Craft Cost", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 160, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 3), NULL, NULL);
        hCheckboxes[4] = CreateWindow("BUTTON", "No Fall Damage", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 190, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 4), NULL, NULL);
        hCheckboxes[5] = CreateWindow("BUTTON", "No Stamina Loss", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 220, 200, 20, hWnd, (HMENU)(IDC_CHECKBOX_START + 5), NULL, NULL);

        // Initialize UI with current config values
        LoadConfig(CONFIG_FILE);
        UpdateUIFromConfig();

        SetTimer(hWnd, 1, 500, NULL); // Set timer to 0.5 seconds
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        if (wmId >= IDC_CHECKBOX_START && wmId < IDC_CHECKBOX_START + 6) {
            int checkboxIndex = LOWORD(wParam) - IDC_CHECKBOX_START; // Assuming you define IDC_CHECKBOX_START
            // Toggle the checkbox state
            int checkState = SendMessage(hCheckboxes[checkboxIndex], BM_GETCHECK, 0, 0);
            SendMessage(hCheckboxes[checkboxIndex], BM_SETCHECK, (checkState == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED, 0);
        }
        // Parse the menu selections
        switch (wmId)
        {
        //case ID_FILE_SAVE:
        //    UpdateConfigFromUI();
        //    SaveConfig(CONFIG_FILE);
        //    break;

        case ID_FILE_IMPORT:
            ImportConfigFile(hWnd);
            break;

        case ID_FILE_EXPORT:
            ExportConfigFile(hWnd);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
    }
    break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // You can add additional drawing code here if needed
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_TIMER:
        if (wParam == 1) { // Check for the timer identifier
            UpdateConfigFromUI();
            SaveConfig(CONFIG_FILE);
        }
        break;

    case WM_GETMINMAXINFO: 
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMaxTrackSize.x = windowWidth; // Set maximum width
        mmi->ptMaxTrackSize.y = windowHeight; // Set maximum height
        mmi->ptMinTrackSize.x = windowWidth; // Set minimum width
        mmi->ptMinTrackSize.y = windowHeight; // Set minimum height
        return 0;
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Meldungshandler für Infofeld.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
