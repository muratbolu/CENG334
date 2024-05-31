#include "identifier.h"

uint8_t* parse_identifier(int argc, char* argv[])
{
	size_t identifier_length = argc - 2;
	uint8_t* identifier = new uint8_t[identifier_length];
	for (size_t i = 0; i < identifier_length; i++) {
		unsigned int temp;
		sscanf(argv[2U + i], "%x", &temp);
		identifier[i] = (uint8_t)temp;
	}
	return identifier;
}
