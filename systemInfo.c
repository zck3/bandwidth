/*============================================================================
  bandwidth, a benchmark to measure memory transfer bandwidth.
  Copyright (C) 2005-2024, 2026 by Zack T Smith.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  The author may be reached at 3 at zs3 dot me.
 *===========================================================================*/

#include "defs.h"
#include "systemInfo.h"
#include "OOC/OS.h"

//-----------------------------------------------------------------------------
// Name:	getSystemInfo
// Purpose:	Prints some useful system info and constructs the graph title.
// Returns:	The graph title.
//-----------------------------------------------------------------------------
MutableString *getSystemInfo (CPU *cpu, Hardware *hardware)
{
	if (!cpu || !hardware) {
		return NULL;
	}

	MutableSet *features = $(cpu, features);
	MutableString *graphTitle = new(MutableString);

#if defined(__x86_64__) || defined(__i386__)
	String *sse4String = _String("sse41");
	if ($(features, contains, sse4String)) {
		benchmarks->use_sse4 = true;
	}
	release(sse4String);

	if (options.perform_direct_tests) {
		String *string;
#ifdef IS_64BIT
		string = _String("movdir64b");
#else
		string = _String("movdiri");
#endif
		if ($(features, contains, string)) {
			benchmarks->use_direct_transfers = true;
		}
		release(string);
	}

	benchmarks->use_sse2 = options.perform_128bit_tests;
	benchmarks->use_avx = options.perform_256bit_tests;
	benchmarks->use_avx512 = options.perform_512bit_tests;
#endif

	//------------------------------------------------------------
	// Attempt to obtain information about the system.
	//
	String *string = $(hardware, systemMake);
	if (string) {
		$(console, printf, "Computer make: ");
		$(console, printObject, string);
		$(console, newline);
		$(graphTitle, appendString, string);
		$(graphTitle, appendCharacter, ' ');
		release (string);
	}

	string = $(hardware, systemModel);
	if (string) {
		$(console, printf, "Computer model: ");
		$(console, printObject, string);
		$(console, newline);
		$(graphTitle, appendString, string);
		$(graphTitle, appendCharacter, ' ');
		release (string);
	}

	string = $(cpu, family);
	if (string) {
		$(console, printf, "CPU family: ");
		$(console, printObject, string);
		$(console, newline);
		release (string);
	}

	String *cpuMake = $(cpu, make); // e.g. Intel, AMD, Qualcomm
	String *cpuModel = $(cpu, model);

	// Check whether the model has the make already prepended to it.
	bool dont_append_make = cpuModel != NULL && $(cpuModel, hasPrefix, cpuMake);

	if (!dont_append_make) {
		if (cpuMake) {
			$(console, printf, "CPU brand: ");
			$(console, printObject, cpuMake);
			$(console, newline);
			$(graphTitle, appendString, cpuMake);
			$(graphTitle, appendCharacter, ' ');
		}
	}
	release (cpuMake);

	if (cpuModel) {
		if ($(string, containsCString, "(R)")) {
			// TODO Remove the (R)
		}
		$(console, printf, "CPU model: ");
		$(console, printObject, cpuModel);
		$(console, newline);
		$(graphTitle, appendString, cpuModel);
		$(graphTitle, appendCharacter, ' ');
		release (cpuModel);
	}

	unsigned regSize = $(cpu, registerSize);
	$(console, printf, "CPU register size: %u bits\n", regSize);

	$(console, printf, "CPU vector sizes: ");
	bool hasVectors = false;
	if ($(cpu, has128bitVectors)) {
		$(console, printf, "128 ");
		hasVectors = true;
	}
	if ($(cpu, has256bitVectors)) {
		$(console, printf, "256 ");
		hasVectors = true;
	}
	if ($(cpu, has512bitVectors)) {
		$(console, printf, "512 ");
		hasVectors = true;
	}
	if (!hasVectors) {
		$(console, printf, "(none)");
	} else {
		$(console, printf, "bits");
	}
	$(console, newline);

	string = $(cpu, instructionSet);
	$(console, printf, "CPU instruction set: ");
	$(console, printObject, string);
	$(console, newline);
	$(graphTitle, appendString, string);
	$(graphTitle, appendCharacter, ' ');
	release (string);

	if ($(features, count)) {
		$(console, printf, "CPU features: ");
		Array *sorted = $(features, asArray, true);
		$(sorted, print, NULL);
		$(console, newline);
		release(sorted);
	}

	unsigned nCores = $(cpu, nCores);
	if (nCores) {
		$(console, printf, "CPU total cores: %u\n", nCores);
	}

#ifndef __APPLE__
	for (unsigned core = 0; core < nCores; core++) {
#else
	unsigned core = 0; {
#endif
		unsigned L1i = $(cpu, levelNCacheSize, core, 1, false);
		unsigned L1d = $(cpu, levelNCacheSize, core, 1, true);
		unsigned L2 = $(cpu, levelNCacheSize, core, 2, false);
		unsigned L3 = $(cpu, levelNCacheSize, core, 3, false);
		float maxSpeed = $(cpu, maximumSpeed, core);
		float minSpeed = $(cpu, minimumSpeed, core);

		bool have_cache_info = L1i > 0 || L1d > 0 || L2 > 0 || L3 > 0;
		bool have_speed_info = maxSpeed > 0.f;
		bool have_any_core_info = have_cache_info || have_speed_info;

		if (have_any_core_info) {
#ifndef __APPLE__
			$(console, printf, "CPU core #%u: ", core);
#else
			$(console, printf, "CPU current core: ");
#endif

			if (have_speed_info) {
				$(console, printf, "max %.2g GHz", maxSpeed);
				if (minSpeed) {
					$(console, printf, ", min %.2g GHz", minSpeed);
				}
				if (have_cache_info) {
					$(console, printf, "; ");
				}
			}
			if (have_cache_info) {
				$(console, printf, "caches: L1 instruction %ukB, L1 data %ukB", L1i, L1d);
				if (L2 > 0) {
					if (L2 >= 1024 && ((L2 & 1023)==0)) {
						$(console, printf, ", L2 %uMB", L2 >> 10);
					} 
					else if (L2 >= 1024) {
						$(console, printf, ", L2 %f.02MB", (float)L2/1024.f);
					} else {
						$(console, printf, ", L2 %ukB", L2);
					}
				}

				if (L3 > 0) {
					if (L3 >= 1024 && ((L3 & 1023)==0)) {
						$(console, printf, ", L3 %uMB", L3 >> 10);
					} 
					else if (L3 >= 1024) {
						$(console, printf, ", L3 %.02fMB", (float)L3/1024.f);
					} else {
						$(console, printf, ", L3 %ukB", L3);
					}
				}
			}
			$(console, newline);
		}
	}

	if (cpu->cacheLineSize) {
		$(console, printf, "Cache line size: %u bytes\n", cpu->cacheLineSize);
	}

	size_t pagesize = OS_memoryPageSize();
	$(console, printf, "Memory page size: %lu bytes\n", pagesize);
	if (pagesize == 4096) {
		if (cpu->nInstructionTLBEntries || cpu->nDataTLBEntries || cpu->nUnifiedTLBEntries) {
			$(console, printf, "TLB entries (4kB pages): %u instruction, %u data, %u shared\n", 
				cpu->nInstructionTLBEntries, 
				cpu->nDataTLBEntries, 
				cpu->nUnifiedTLBEntries);
		}
	}

	size_t totalRAMInMegabytes = OS_totalRAM();
	if (!totalRAMInMegabytes) {
		totalRAMInMegabytes = $(hardware, totalRAM);
	}
	if (totalRAMInMegabytes) {
		if (totalRAMInMegabytes >= 1024 && !(totalRAMInMegabytes % 1024)) {
			$(console, printf, "Physical memory: %lu GB\n", totalRAMInMegabytes >> 10);
		}
		else {
			$(console, printf, "Physical memory: %lu MB\n", totalRAMInMegabytes);
		}
	}

	unsigned long freeRAMInMegabytes = OS_freeRAM();
	if (freeRAMInMegabytes > 0) {
		if (freeRAMInMegabytes >= 1024 && !(freeRAMInMegabytes % 1024)) {
			$(console, printf, "Memory available: %lu GB\n", freeRAMInMegabytes >> 10);
		}
		else {
			$(console, printf, "Memory available: %lu MB\n", freeRAMInMegabytes);
		}
	}

	string = OS_kernelName();
	if (string) {
		$(console, printf, "Kernel name: ");
		$(console, printObject, string);
		$(console, newline);
		$(graphTitle, appendString, string);
		$(graphTitle, appendCharacter, ' ');
		release (string);
	}

	string = OS_kernelRelease();
	if (string) {
		$(console, printf, "Kernel release: ");
		$(console, printObject, string);
		$(console, newline);
		$(graphTitle, appendString, string);
		$(graphTitle, appendCharacter, ' ');
		release (string);
	}

	string = OS_type();
	if (string) {
		$(console, printf, "OS type: ");
		$(console, printObject, string);
		$(console, newline);
		if (!$(string, equalsCString, "Linux")) {
			$(graphTitle, appendString, string);
			$(graphTitle, appendCharacter, ' ');
		}
		release (string);
	}

	string = OS_name();
	if (string) {
		$(console, printf, "OS name: ");
		$(console, printObject, string);
		$(console, newline);
		if (!$(string, equalsCString, "Linux")) {
			$(graphTitle, appendString, string);
			$(graphTitle, appendCharacter, ' ');
		}
		release (string);

		float version = OS_version();
		if (version != 0.f) {
			$(console, printf, "OS version: %.1f\n", version);
			$(graphTitle, appendFormat, "%.1f ", version);
		}
	}

	// Print physical core temperatures.
	for (int core = 0; core < nCores; core++) {
		float temp = $(cpu, temperature, core);
		if (temp > 0.f) {
			$(console, printf, "Physical core %d temperature: %.2fC\n", core, temp);
		}
	}
	// Print thermal zone temperatures.
#define MAX_THERMAL_ZONES 128
	for (int zone = 0; zone < MAX_THERMAL_ZONES; zone++) {
		float temp = $(hardware, temperature, zone);
		if (temp > 0.f) {
			$(console, printf, "Thermal zone %d temperature: %.2fC\n", zone, temp);
		} else {
			break;
		}
	}

	int len = $(graphTitle, length);
	if (len > 0) {
		MutableString *cleansed = new(MutableString);

		Char prev = $(graphTitle, at, 0);
		$(cleansed, appendCharacter, prev);

		for (int i=1; i < len; i++) {
			Char ch = $(graphTitle, at, i);
			if (ch == ' ') {
				if (prev != ' ') {
					$(cleansed, appendCharacter, ch);
				}	
			} else {
				$(cleansed, appendCharacter, ch);
			}
			prev = ch;
		}
		if ($(cleansed, length) < len) {
			release(graphTitle);
			graphTitle = cleansed;
		} else {
			release(cleansed);
		}
	}

	return graphTitle;
}

