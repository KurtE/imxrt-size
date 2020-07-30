#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printnumbers(unsigned flexram_config, unsigned itcm, unsigned dtcm, unsigned ocram, unsigned flash, int stack,
	unsigned ocramm, unsigned flashm)
{
	int retval = 0;
	unsigned dtcm_allocated = 0;
	unsigned itcm_allocated = 0;
	char dtcm_itcm_config[17] = "DDDDDDDDDDDDDDDD";
	char* psz = &dtcm_itcm_config[15]; // Crud code...
	printf("\nFlexRAM section ITCM+DTCM = 512 KB\n");
	printf("    Config : %08lx (", flexram_config);
	for (; flexram_config; flexram_config >>= 2) {
		if ((flexram_config & 3) == 2) {
			*psz-- = 'D';
			dtcm_allocated += 32;	// 32K per bank;
		}
		else if ((flexram_config & 3) == 3) {
			*psz-- = 'I';
			itcm_allocated += 32;	// 32K per bank;
		}
		else psz--; 
	}
	printf("%s)\n    ITCM : %6d B\t(%5.2f%% of %4d KB)\n", dtcm_itcm_config, itcm, itcm / (itcm_allocated * 1024.0) * 100, itcm_allocated);
	printf("    DTCM : %6d B\t(%5.2f%% of %4d KB)\n", dtcm, dtcm / (dtcm_allocated * 1024.0) * 100, dtcm_allocated);
	if (stack <= 0) {
		retval = -1;
		printf(">>>>> Error FlexRAM Filled no room for Stack: %d <<<<<\n", stack);
	}
	else {
		printf("    Available for Stack: %6d\n", stack);
	}
	printf("OCRAM: 512KB\n");
	printf("    DMAMEM: %6d B\t(%5.2f%% of %4d KB)\n", ocram, ocram / (ocramm * 1024.0) * 100, ocramm);
	ocram = ocramm*1024 - ocram;
	printf("    Available for Heap: %6d B\t(%5.2f%% of %4d KB)\n", ocram, ocram / (ocramm * 1024.0) * 100, ocramm);
	printf("Flash: %6d B\t(%5.2f%% of %4d KB)\n", flash, flash / (flashm * 1024.0) * 100, flashm);
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
	else if (teensy_model_identifier == 0x25) {
		retval = printnumbers(flexram_bank_config, etext - stext, ebss - sdata, heap_start - 0x20200000, flashimagelen, estack - ebss, 512, 7936);
	}

	return retval;
}
