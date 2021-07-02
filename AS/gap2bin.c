#include "string.h"
#include "stdio.h"
#include "unistd.h" // for unlink

#include "stdbool.h"

const char* codeFileName = NULL;
const char* romFileName = NULL;
const char* shareFileName = NULL;
int z80Length = 0;

void printUsage() { printf("usage: gap2bin.exe inputcodefile.p outputromfile.bin sharefile.h\n"); }
bool buildRom(FILE* from, FILE* to);
void editShareFile();

int main(int argc, char *argv[])
{
//	for(int i = 0 ; i < argc ; i++)
//		printf("arg %d is %s\n", i, argv[i]);

	if(argc > 2)
		argc--, argv++; // skip the exe filename

	if(argc < 2)
		printUsage();

	while(argc)
	{
		char* arg = argv[0];
		argc--, argv++; // pop arg

		if(!strcasecmp(arg, "-h") || !strcasecmp(arg, "--help"))
			printUsage(), argc = 0;
		else if(!codeFileName)
			codeFileName = arg;
		else if(!romFileName)
			romFileName = arg;
        else if(!shareFileName)
			shareFileName = arg;
	}

	if(codeFileName && romFileName)
	{
		printf("\ngap2bin.exe: generating %s from %s", romFileName, codeFileName);

		FILE* from = fopen(codeFileName, "rb");
		if(from)
		{
			FILE* to = fopen(romFileName, "wb");
			if(to)
			{
				bool built = buildRom(from, to);
				fclose(to);
				fclose(from);
				if(built)
				{
				    editShareFile();
					printf(" ... done.");
				}
				else
				{
					unlink(romFileName); // error; delete the rom because it's probably hosed
				}
			}
			else
				printf("\nERROR: Failed to access file \"%s\".", romFileName);
		}
		else
			printf("\nERROR: Failed to load file \"%s\".", codeFileName);
	}

	printf("\n");
	return 0;
}

void editShareFile()
{
	if(shareFileName && z80Length > 0)
	{
		FILE* share = fopen(shareFileName, "rb+");
		if(share)
		{
			fseek(share, 0, SEEK_SET);
			fprintf(share, "comp_z80_size 0x%X ", z80Length);
			fclose(share);
		}
	}
}

bool buildRom(FILE* from, FILE* to)
{
	if(fgetc(from) != 0x89) printf("\nWarning: First byte of a .p file should be $89");
	if(fgetc(from) != 0x14) printf("\nWarning: Second byte of a .p file should be $14");

	int cpuType = 0, segmentType = 0, granularity = 0, lastType = 0;
	signed long start = 0, lastStart = 0;
	unsigned short length = 0, lastLength = 0;
	static const int scratchSize = 4096;
	unsigned char scratch [scratchSize];

	int check_size = 0;
	bool done=false;

	while(!done)
	{
		int tempbuf;
		int temp_length;
		unsigned char headerByte = fgetc(from);

		if(ferror(from) || feof(from))
			break;

		switch(headerByte)
		{
			case 0x00: // "END" segment
				done=true; break;
			case 0x80: // "entry point" segment
				fseek(from, 3, SEEK_CUR);
				continue;
			case 0x81:  // code or data segment
				tempbuf = fgetc(from);
				tempbuf = fgetc(from);
				tempbuf = fgetc(from);
				break;
			default:
				if(headerByte > 0x81)
					{ printf("\nERROR: Unsupported segment header $%02X", headerByte); return false; }
				tempbuf = headerByte;
				break;
		}

		if(!done)
		{
			tempbuf=0;
			tempbuf = fgetc(from); // integers in AS .p files are always little endian
			tempbuf |= fgetc(from) << 8;
			tempbuf |= fgetc(from) << 16;
			tempbuf |= fgetc(from) << 24;
			temp_length = fgetc(from);
			temp_length |= fgetc(from) << 8;

			if(tempbuf+temp_length > check_size) check_size = tempbuf+temp_length;

			fseek(from,temp_length,SEEK_CUR);
		}
	}

	for (int i = 0; i < check_size; i++)
		fputc(0xFF, to);

	fseek(to,0,SEEK_SET);
	fseek(from,2,SEEK_SET);

	while(true)
	{
		unsigned char headerByte = fgetc(from);
		if(ferror(from) || feof(from))
			break;

		switch(headerByte)
		{
			case 0x00: // "END" segment
				return true;
			case 0x80: // "entry point" segment
				fseek(from, 3, SEEK_CUR);
				continue;
			case 0x81:  // code or data segment
				cpuType = fgetc(from);
				segmentType = fgetc(from);
				granularity = fgetc(from);
				if(granularity != 1)
					{ printf("\nERROR: Unsupported granularity %d.", granularity); return false; }
				break;
			default:
				if(headerByte > 0x81)
					{ printf("\nERROR: Unsupported segment header $%02X", headerByte); return false; }
				cpuType = headerByte;
				break;
		}

		start = fgetc(from); // integers in AS .p files are always little endian
		start |= fgetc(from) << 8;
		start |= fgetc(from) << 16;
		start |= fgetc(from) << 24;
		length = fgetc(from);
		length |= fgetc(from) << 8;

		if(length == 0)
		{
			printf("\nERROR: zero length segment ($%X).", length);
			return false;
		}

		if(start < 0)
		{
			printf("\nERROR: negative start address ($%X).", start), start = 0;
			return false;
		}

		if(cpuType == 0x51) // 0x51 is the type for Z80 family (0x01 is for 68000)
        {
            if (start == 0)
                z80Length = 0;

            start = lastStart + lastLength;
            z80Length += length;
        }

		lastStart = start;
		lastLength = length;
		lastType = cpuType;


		fseek(to, start, SEEK_SET);

		while(length)
		{
			int copy = length;
			if(copy > scratchSize)
				copy = scratchSize;
			fread(scratch, copy, 1, from);
			fwrite(scratch, copy, 1, to);
			length -= copy;
		}

	}

	return true;
}
