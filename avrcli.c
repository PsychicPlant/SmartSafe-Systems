
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char ping[] = "ping\n";
	write(STDOUT_FILENO, &ping, sizeof(ping));
	//dprintf(STDOUT_FILENO, "Success!!\n");
	//char *a = argv[1];
	//int num = atoi(a);
	return 0;
}

