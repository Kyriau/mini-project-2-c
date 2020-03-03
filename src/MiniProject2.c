#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main() {

	int n, m;

	printf("How many consumer threads to you want? ");
	scanf("%i", &n);

	printf("How many seconds should the maximum request length be? ");
	scanf("%i", &m);

	printf("n=%i, m=%i\n", n, m);

	exit(0);

}