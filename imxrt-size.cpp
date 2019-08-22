#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printnumbers(unsigned flexram_config, unsigned itcm, unsigned dtcm, unsigned ocram, unsigned flash, int stack,
	unsigned ocramm, unsigned flashm)
{
	int retval = 0;
	unsigned dtcm_allocated = 0;
	unsigned itcm_allocated = 0;
	printf("\rFlexRAM section ITCM+DTCM = 512 KB\r");
	printf("    Config : %08lx\r", flexram_config);
	for (; flexram_config; flexram_config >>= 2) {
		if ((flexram_config & 3) == 2) dtcm_allocated += 32;	// 32K per bank;
		else if ((flexram_config & 3) == 3) itcm_allocated += 32;	// 32K per bank;
	}
	printf("    ITCM : %6d B\t(%5.2f%% of %4d KB)\r", itcm, itcm / (itcm_allocated * 1024.0) * 100, itcm_allocated);
	printf("    DTCM : %6d B\t(%5.2f%% of %4d KB)\r", dtcm, dtcm / (dtcm_allocated * 1024.0) * 100, dtcm_allocated);
	if (stack <= 0) {
		retval = -1;
		printf(">>>>> Error FlexRAM Filled no room for Stack: %d <<<<<\r", stack);
	}
	else {
		printf("    Available for Stack: %6d\r", stack);
	}
	printf("OCRAM: 512KB\r");
	printf("    DMAMEM: %6d B\t(%5.2f%% of %4d KB)\r", ocram, ocram / (ocramm * 1024.0) * 100, ocramm);
	ocram = ocramm*1024 - ocram;
	printf("    Available for Heap: %6d B\t(%5.2f%% of %4d KB)\r", ocram, ocram / (ocramm * 1024.0) * 100, ocramm);
	printf("Flash: %6d B\t(%5.2f%% of %4d KB)\r", flash, flash / (flashm * 1024.0) * 100, flashm);
	return retval;
}


int main() {
	const int bl = 200;
	int retval = 0;
	char str[bl + 1];
	char* s;

	unsigned teensy_model_identifier = 0;
	unsigned stext = 0;
	unsigned etext = 0;
	unsigned sdata = 0;
	unsigned ebss = 0;
	unsigned flashimagelen = 0;
	unsigned heap_start = 0;
	unsigned flexram_bank_config = 0;
	unsigned estack = 0;

	do {
		s = fgets(str, sizeof(str), stdin);
		if (s) {
			str[bl] = 0;
			if (strstr(str, "_teensy_model_identifier")) teensy_model_identifier = strtol(str, NULL, 16);
			if (strstr(str, "T _stext")) stext = strtol(str, NULL, 16);
			if (strstr(str, "T _etext")) etext = strtol(str, NULL, 16);
			if (strstr(str, "D _sdata")) sdata = strtol(str, NULL, 16);
			if (strstr(str, "B _ebss")) ebss = strtol(str, NULL, 16);
			if (strstr(str, " _heap_start")) heap_start = strtol(str, NULL, 16);
			if (strstr(str, " _flashimagelen")) flashimagelen = strtol(str, NULL, 16);
			if (strstr(str, "B _estack")) estack = strtoul(str, NULL, 16);
			if (strstr(str, " _flexram_bank_config")) flexram_bank_config = strtoul(str, NULL, 16);
			//puts( str );
		}
	} while (s);

	if (teensy_model_identifier == 0x24) {
		retval = printnumbers(flexram_bank_config, etext - stext, ebss - sdata, heap_start - 0x20200000, flashimagelen, estack-ebss, 512, 1984);
	}

	return retval;
}
