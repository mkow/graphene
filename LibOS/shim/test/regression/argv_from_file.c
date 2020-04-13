#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	if (argc != 7
		|| strcmp(argv[0], "./argv_from_file")
        || strcmp(argv[1], "THIS")
        || strcmp(argv[2], "SHOULD")
        || strcmp(argv[3], "GO")
        || strcmp(argv[4], "TO")
        || strcmp(argv[5], "THE")
        || strcmp(argv[6], "APP")) {
        return 1;
    }
    puts("Success!");
	return 0;
}
