#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WEIGHTINESS	32
#define MAX_NODES	256
#define SYSFS_PATH	"/sys/kernel/mm/mempolicy/weighted_interleave"

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
	int i;

	for (i = 1; i < n; i++)
		g = gcd(g, arr[i]);

	return g;
}

static int write_weight(int node, unsigned int weight)
{
	char path[256];
	FILE *f;

	snprintf(path, sizeof(path),
		 SYSFS_PATH "/node%d", node);

	f = fopen(path, "w");
	if (!f) {
		perror(path);
		return -1;
	}

	fprintf(f, "%u\n", weight);
	fclose(f);

	return 0;
}

static int detect_nodes(void)
{
	int count = 0;
	int i;
	char path[256];

	for (i = 0; i < MAX_NODES; i++) {
		snprintf(path, sizeof(path),
			 SYSFS_PATH "/node%d", i);

		if (access(path, F_OK) == 0)
			count++;
		else
			break;
	}

	return count;
}

int main(void)
{
	unsigned long bw[MAX_NODES];
	unsigned int weight[MAX_NODES];
	unsigned long total = 0;
	int nodes;
	int i;

	if (access(SYSFS_PATH, F_OK) != 0) {
		printf("Error: %s not found\n", SYSFS_PATH);
		printf("Please enable CONFIG_NUMA and weighted interleave in kernel.\n");
		return 1;
	}

	nodes = detect_nodes();

	if (nodes <= 0) {
		printf("No nodeX entries found under %s\n", SYSFS_PATH);
		printf("Kernel may not support weighted interleave.\n");
		return 1;
	}

	printf("Detected %d nodes\n\n", nodes);

	for (i = 0; i < nodes; i++) {
		printf("Enter bandwidth for node%d (MB/s): ", i);
		fflush(stdout);

		if (scanf("%lu", &bw[i]) != 1) {
			printf("Invalid input\n");
			return 1;
		}

		total += bw[i];
	}

	if (!total) {
		printf("Total bandwidth is zero\n");
		return 1;
	}

	for (i = 0; i < nodes; i++) {
		weight[i] = (WEIGHTINESS * bw[i]) / total;
		if (weight[i] == 0)
			weight[i] = 1;
	}

	{
		unsigned int g = gcd_array(weight, nodes);

		for (i = 0; i < nodes; i++)
			weight[i] /= g;
	}

	printf("\nCalculated weights:\n");

	for (i = 0; i < nodes; i++)
		printf("node%d: bw=%lu -> weight=%u\n",
		       i, bw[i], weight[i]);

	printf("\nWrite these weights to sysfs? [y/N]: ");
	fflush(stdout);

	/* 清理 stdin 中残留换行 */
	while (getchar() != '\n')
		;

	int c = getchar();

	if (c != 'y' && c != 'Y') {
		printf("Aborted.\n");
		return 0;
	}

	printf("\nWriting sysfs:\n");

	for (i = 0; i < nodes; i++)
		if(!write_weight(i, weight[i]))
			printf("node%d: weight=%u written successfully\n", i, weight[i]);

	return 0;
}

