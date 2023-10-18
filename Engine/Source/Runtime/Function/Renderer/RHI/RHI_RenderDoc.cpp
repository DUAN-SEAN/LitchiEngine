/*
Copyright(c) 2016-2023 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ===========================
#include "Runtime/Core/pch.h"
#include "RHI_RenderDoc.h"
#include "RHI_Implementation.h"
#include "../Core/Window.h"
#include "../Core/Definitions.h"
#include "../Rendering/Renderer.h"
#include "../Logging/Log.h"
#include "renderdoc/app/renderdoc_app.h"
#if defined(_MSC_VER) // Windows
#include <windows.h>
#endif
//======================================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
    static RENDERDOC_API_1_5_0* rdc_api = nullptr;
    static void* rdc_module             = nullptr;

#if defined(_MSC_VER) // Windows
    static vector<wstring> get_renderdoc_dll_paths()
    {
        vector<wstring> dll_paths;
        static const wchar_t* installer_folders_path = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer\\Folders");

        // Open installer folders key
        HKEY hkey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, installer_folders_path, 0, KEY_READ, &hkey) != ERROR_SUCCESS) 
            return dll_paths;

        TCHAR    achClass[MAX_PATH] = TEXT(""); // buffer for class name
        DWORD    cchClassName = MAX_PATH;       // size of class string
        DWORD    cSubKeys = 0;                  // number of subkeys
        DWORD    cbMaxSubKey;                   // longest subkey size
        DWORD    cchMaxClass;                   // longest class string
        DWORD    c_values;                      // number of values for keyPath
        DWORD    cchMaxValue;                   // longest value name
        DWORD    cbMaxValueData;                // longest value data
        DWORD    cbSecurityDescriptor;          // size of security descriptor
        FILETIME ftLastWriteTime;               // last write time

        // Get the class name and the value count.
        DWORD query_result = RegQueryInfoKey(
            hkey,                  // keyPath handle
            achClass,              // buffer for class name
            &cchClassName,         // size of class string
            nullptr,               // reserved
            &cSubKeys,             // number of subkeys
            &cbMaxSubKey,          // longest subkey size
            &cchMaxClass,          // longest class string
            &c_values,             // number of values for this keyPath
            &cchMaxValue,          // longest value name
            &cbMaxValueData,       // longest value data
            &cbSecurityDescriptor, // security descriptor
            &ftLastWriteTime);     // last write time

        constexpr uint32_t MAX_VALUE_NAME = 8192;
        TCHAR ach_value[MAX_VALUE_NAME];
        wchar_t enum_value[MAX_VALUE_NAME] = TEXT("");

        if (c_values)
        {
            //printf("\nNumber of values: %d\n", cValues);
            for (DWORD i = 0, retCode = ERROR_SUCCESS; i < c_values; i++)
            {
                DWORD cchValue = MAX_VALUE_NAME;
                ach_value[0] = '\0';
                DWORD type = REG_SZ;
                DWORD size;
                memset(enum_value, '\0', MAX_VALUE_NAME);

                // MSDN:  https://docs.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regenumvaluea
                // If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, the string may not have been stored with 
                // the proper null-terminating characters. Therefore, even if the function returns ERROR_SUCCESS, the application 
                // should ensure that the string is properly terminated before using it; otherwise, it may overwrite a buffer.
                retCode = RegEnumValue(hkey, i,
                    ach_value,
                    &cchValue,
                    nullptr,
                    &type,
                    nullptr,
                    &size);

                if (type != REG_SZ || retCode != ERROR_SUCCESS)
                    continue;

                retCode = RegQueryInfoKey(
                    hkey,                  // keyPath handle 
                    achClass,              // buffer for class name 
                    &cchClassName,         // size of class string 
                    nullptr,               // reserved 
                    &cSubKeys,             // number of subkeys 
                    &cbMaxSubKey,          // longest subkey size 
                    &cchMaxClass,          // longest class string 
                    &c_values,              // number of values for this keyPath 
                    &cchMaxValue,          // longest value name 
                    &cbMaxValueData,       // longest value data 
                    &cbSecurityDescriptor, // security descriptor 
                    &ftLastWriteTime);     // last write time 

                wstring path(ach_value);
                if (path.find(L"RenderDoc") != wstring::npos)
                {
                    // many paths qualify:
                    // 
                    // "C:\\Program Files\\RenderDoc\\plugins\\amd\\counters\\"
                    // "C:\\Program Files\\RenderDoc\\"
                    // "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\RenderDoc\\"
                    //
                    // Only consider the ones the contain the dll we want
                    const wstring rdc_dll_path = path += TEXT("renderdoc.dll");
                    WIN32_FIND_DATA find_file_data = { 0 };
                    HANDLE file_handle = FindFirstFile(rdc_dll_path.c_str(), &find_file_data);
                    if (file_handle != INVALID_HANDLE_VALUE)
                    {
                        dll_paths.push_back(path);
                    }
                }
            }
        }

        RegCloseKey(hkey);

        return dll_paths;
    }
#endif

    void RHI_RenderDoc::OnPreDeviceCreation()
    {
         // Load RenderDoc module and get a pointer to it's API
        if (rdc_api == nullptr)
        {
            pRENDERDOC_GetAPI rdc_get_api = nullptr;
#if defined(_MSC_VER) // Windows
            // If RenderDoc is already injected into the engine, use the existing module
            rdc_module = ::GetModuleHandleA("renderdoc.dll");

            // If RenderDoc is not injected, load the module now
            if (rdc_module == nullptr)
            {
                vector<wstring> RDocDllPaths = get_renderdoc_dll_paths();
                SP_ASSERT_MSG(!RDocDllPaths.empty(), "Could not find any install locations for renderdoc.dll");
                wstring module_path = RDocDllPaths[0]; // assuming x64 is reported first
                rdc_module = ::LoadLibraryW(module_path.c_str());
            }

            SP_ASSERT_MSG(rdc_module != nullptr, "Failed to get RenderDoc module");

            // Get the address of RENDERDOC_GetAPI
            rdc_get_api = (pRENDERDOC_GetAPI)::GetProcAddress(static_cast<HMODULE>(rdc_module), "RENDERDOC_GetAPI");
#else // Linux
            SP_ASSERT_MSG(false, "Not implemented");
#endif
            SP_ASSERT_MSG(rdc_get_api != nullptr, "Failed to RENDERDOC_GetAPI function address from renderdoc.dll");
            SP_ASSERT_MSG(rdc_get_api(eRENDERDOC_API_Version_1_5_0, (void**)&rdc_api) != 0, "Failed to get RenderDoc API pointer");
        }

        SP_ASSERT_MSG(rdc_api != nullptr, "RenderDoc API has not been initialised");

        // Disable muting of validation/debug layer messages
        rdc_api->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);

        // Disable overlay
        rdc_api->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
    }

    void RHI_RenderDoc::Shutdown()
    {
        if (rdc_module != nullptr)
        {
#if defined(_MSC_VER) // Windows
            ::FreeLibrary(static_cast<HMODULE>(rdc_module));
#else // Linux
            SP_ASSERT_MSG(false, "Not implemented");
#endif
        }
    }

    void RHI_RenderDoc::FrameCapture()
    {
        SP_ASSERT_MSG(rdc_api != nullptr, "RenderDoc is not initialized, ensure that RenderDoc is set to true in RHI_Context");

        // Trigger a capture, this will make RenderDoc capture the entire next frame
        rdc_api->TriggerCapture();

        LaunchRenderDocUi();
    }

    void RHI_RenderDoc::StartCapture()
    {
        SP_ASSERT_MSG(rdc_api != nullptr, "RenderDoc is not initialized, ensure that RenderDoc is set to true in RHI_Context");

        rdc_api->StartFrameCapture(nullptr, nullptr);
    }

    void RHI_RenderDoc::EndCapture()
    {
        SP_ASSERT_MSG(rdc_api != nullptr, "RenderDoc is not initialized, ensure that RenderDoc is set to true in RHI_Context");

        rdc_api->EndFrameCapture(nullptr, nullptr);

        LaunchRenderDocUi();
    }

    void RHI_RenderDoc::LaunchRenderDocUi()
    {
        // If the RenderDoc UI is already running, make sure it's visible.
        if (rdc_api->IsTargetControlConnected())
        {
            DEBUG_LOG_INFO("Bringing RenderDoc to foreground...");
            rdc_api->ShowReplayUI();
        }
        // If the RenderDoc UI is not running, launch it and connect.
        else
        {
            DEBUG_LOG_INFO("Launching RenderDoc...");
            if (rdc_api->LaunchReplayUI(true, "") == 0)
            {
                DEBUG_LOG_ERROR("Failed to launch RenderDoc");
            }
        }
    }

    bool RHI_RenderDoc::IsEnabled()
    {
        return RHI_Context::renderdoc;
    }
    
}
