
#pragma once

//= INCLUDES ===============
#include "RHI_Definitions.h"
//==========================

namespace Spartan
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice(const uint32_t api_version, const uint32_t driver_version, const uint32_t vendor_id, const RHI_PhysicalDevice_Type type, const char* name, const uint64_t memory, void* data)
        {
            this->vendor_id      = vendor_id;
            this->vendor_name    = get_vendor_name();
            this->type           = type;
            this->name           = name;
            this->memory         = static_cast<uint32_t>(memory / 1024 / 1024); // mb
            this->data           = data;
            this->api_version    = decode_driver_version(api_version);
            this->driver_version = decode_driver_version(driver_version);
        }

        /*
            0x10DE - Nvidia
            0x8086 - Intel
            0x1002 - Amd
            0x13B5 - ARM
            0x5143 - Qualcomm
            0x1010 - ImgTec
            
        */
        bool IsNvidia()   const { return vendor_id == 0x10DE || name.find("Nvidia") != std::string::npos; }
        bool IsAmd()      const { return vendor_id == 0x1002 || vendor_id == 0x1022 || name.find("Amd") != std::string::npos; }
        bool IsIntel()    const { return vendor_id == 0x8086 || vendor_id == 0x163C || vendor_id == 0x8087 || name.find("Intel") != std::string::npos;}
        bool IsArm()      const { return vendor_id == 0x13B5 || name.find("Arm,") != std::string::npos; }
        bool IsQualcomm() const { return vendor_id == 0x5143 || name.find("Qualcomm") != std::string::npos; }

        const std::string& GetName()          const { return name; }
        const std::string& GetDriverVersion() const { return driver_version; }
        const std::string& GetApiVersion()    const { return api_version; }
        const std::string& GetVendorName()    const { return vendor_name; }
        uint32_t GetMemory()                  const { return memory; }
        void* GetData()                       const { return data; }
        RHI_PhysicalDevice_Type GetType()     const { return type; }

    private:
        std::string get_vendor_name()
        {
            std::string name = "Unknown";

            if (IsNvidia())
            {
                name = "Nvidia";
            }
            else if (IsAmd())
            {
                name = "AMD";
            }
            else if (IsIntel())
            {
                name = "Intel";
            }
            else if (IsArm())
            {
                name = "Arm";
            }
            else if (IsQualcomm())
            {
                name = "Qualcomm";
            }

            return name;
        }

        std::string decode_driver_version(const uint32_t version);

        std::string api_version      = "Unknown"; // version of api supported by the device
        std::string driver_version   = "Unknown"; // vendor-specified version of the driver.
        uint32_t vendor_id           = 0; // unique identifier of the vendor
        std::string vendor_name      = "Unknown";
        RHI_PhysicalDevice_Type type = RHI_PhysicalDevice_Type::Undefined;
        std::string name             = "Unknown";
        uint32_t memory              = 0;
        void* data                   = nullptr;
    };
}
