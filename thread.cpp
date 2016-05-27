#include "stdafx.h"

namespace mmh {
	size_t get_cpu_core_count()
	{
		static bool initialised;
		static pfc::array_t<size_t> core_masks;

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

					core_masks.set_size(core_count);

					for (t_size i = 0, j = 0; i < slpi_count; i++) {
						const auto & slpi = slpis[i];
						if (slpi.Relationship == RelationProcessorCore)
							core_masks[j++] = slpi.ProcessorMask;
					}
				}
			}
			initialised = true;
		}

		DWORD mask_process = 0, mask_system = 0;
		GetProcessAffinityMask(GetCurrentProcess(), &mask_process, &mask_system);

		t_size cpu_count = 0, core_mask_count = core_masks.get_count();
		for (t_size i = 0; i < core_mask_count; i++) {
			if (mask_process & core_masks[i])
				cpu_count++;
		}
		return cpu_count > 0 ? cpu_count : 1;
	}
}