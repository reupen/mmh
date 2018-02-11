#include "stdafx.h"

namespace mmh {
size_t get_cpu_core_count()
{
    struct CoreInfo {
        size_t mask;
        uint8_t flags;
    };
    static bool initialised;
    static pfc::array_t<CoreInfo> core_info;

    if (!initialised) {
        DWORD slpi_array_size = 0;
        if (!GetLogicalProcessorInformation(nullptr, &slpi_array_size) && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            const auto slpi_count = slpi_array_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            pfc::array_staticsize_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> slpis(slpi_count);
            slpi_array_size = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) * slpi_count;

            memset(&slpis[0], 0, slpi_array_size);

            if (GetLogicalProcessorInformation(&slpis[0], &slpi_array_size)) {
                t_size core_count = 0;
                for (t_size i = 0; i < slpi_count; i++) {
                    if (slpis[i].Relationship == RelationProcessorCore)
                        core_count++;
                }

                core_info.set_size(core_count);

                for (t_size i = 0, j = 0; i < slpi_count; i++) {
                    const auto& slpi = slpis[i];
                    if (slpi.Relationship == RelationProcessorCore) {
                        auto& item = core_info[j++];
                        item.mask = slpi.ProcessorMask;
                        item.flags = slpi.ProcessorCore.Flags;
                    }
                }
            }
        }
        initialised = true;
    }

    DWORD_PTR mask_process = 0, mask_system = 0;
    GetProcessAffinityMask(GetCurrentProcess(), &mask_process, &mask_system);

    t_size core_count = 0, core_info_count = core_info.get_count();
    for (t_size i = 0; i < core_info_count; i++) {
        const auto& item = core_info[i];
        // MSDN: If the value of this member is 1, the logical processors identified by the value of
        // the ProcessorMask member share functional units, as in Hyperthreading or SMT. Otherwise,
        // the identified logical processors do not share functional units.
        if (item.flags == 1) {
            if (mask_process & item.mask)
                core_count++;
        } else {
            size_t intersection = mask_process & item.mask;
            while (intersection) {
                if (intersection & 1)
                    core_count++;
                intersection >>= 1;
            }
        }
    }
    return core_count > 0 ? core_count : 1;
}
} // namespace mmh