/*************************************************************************
    > File Name: wi_bw_tool.c
  > Author:
  > Mail:
  > Created Time: 2026年03月17日 星期二 10时41分22秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WEIGHTINESS 32
#define MAX_NODES 256

static unsigned int gcd(unsigned int a, unsigned int b)
{
	while (b) {
		unsigned int t = a % b;
		a = b;
		b = t;
	}
	return a;
}

static unsigned int gcd_array(unsigned int *arr, int n)
{
	unsigned int g = arr[0];

	for (int i = 1; i < n; i++)
		g = gcd(g, arr[i]);

	return g;
}

static int write_weight(int node, unsigned int weight)
{
	char path[256];
	FILE *f;

	snprintf(path, sizeof(path),
		"/sys/kernel/mm/mempolicy/weighted_interleave/node%d", node);

	f = fopen(path, "w");
	if (!f) {
		perror(path);
		return -1;
	}

	fprintf(f, "%u\n", weight);
	fclose(f);

	return 0;
}

int main(int argc, char **argv)
{
	unsigned long bw[MAX_NODES];
	unsigned int weight[MAX_NODES];
	unsigned long total = 0;
	int nodes = argc - 1;

	if (nodes <= 0) {
		printf("usage: %s bw0 bw1 bw2 ...\n", argv[0]);
		return 1;
	}

	for (int i = 0; i < nodes; i++) {
		bw[i] = strtoul(argv[i + 1], NULL, 10);
		total += bw[i];
	}

	if (!total) {
		printf("total bandwidth is zero\n");
		return 1;
	}

	for (int i = 0; i < nodes; i++) {
		weight[i] = (WEIGHTINESS * bw[i]) / total;
		if (weight[i] == 0)
			weight[i] = 1;
	}

	unsigned int g = gcd_array(weight, nodes);

	for (int i = 0; i < nodes; i++)
		weight[i] /= g;

	printf("Calculated weights:\n");

	for (int i = 0; i < nodes; i++)
		printf("node%d: bw=%lu -> weight=%u\n", i, bw[i], weight[i]);

	printf("\nWrite these weights to sysfs? [y/N]: ");
	fflush(stdout);

	int c = getchar();

	/* 清理输入缓冲区 */
	while (getchar() != '\n' && !feof(stdin));

	if (c != 'y' && c != 'Y') {
		printf("Aborted. Not writing to sysfs.\n");
		return 0;
	}

	printf("\nWriting sysfs:\n");

	for (int i = 0; i < nodes; i++)
		if(!write_weight(i, weight[i]))
			printf("node%d: weight=%u written successfully\n", i, weight[i]);

	return 0;
}

