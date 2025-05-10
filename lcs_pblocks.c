#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define BLOCK_SIZE 64
#define max(a, b) ((a) > (b) ? (a) : (b))
#define NUM_THREADS 8

typedef unsigned short mtype;

char *read_seq(char *fname)
{
    FILE *fseq = fopen(fname, "rt");
    if (!fseq)
    {
        perror("Error opening file");
        exit(1);
    }

    fseek(fseq, 0L, SEEK_END);
    long size = ftell(fseq);
    rewind(fseq);

    char *seq = (char *)calloc(size + 1, sizeof(char));
    if (!seq)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    int i = 0;
    while (!feof(fseq))
    {
        seq[i] = fgetc(fseq);
        if ((seq[i] != '\n') && (seq[i] != EOF))
            i++;
    }
    seq[i] = '\0';
    fclose(fseq);
    return seq;
}

mtype **allocateScoreMatrix(int sizeA, int sizeB)
{
    mtype **matrix = malloc((sizeB + 1) * sizeof(mtype *));
    for (int i = 0; i <= sizeB; i++)
        matrix[i] = calloc(sizeA + 1, sizeof(mtype));
    return matrix;
}

void freeScoreMatrix(mtype **matrix, int sizeB)
{
    for (int i = 0; i <= sizeB; i++)
        free(matrix[i]);
    free(matrix);
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

int LCS_blocked_omp(mtype **matrix, int sizeA, int sizeB, char *seqA, char *seqB)
{
    int n_blocks_i = (sizeB + BLOCK_SIZE) / BLOCK_SIZE;
    int n_blocks_j = (sizeA + BLOCK_SIZE) / BLOCK_SIZE;

    for (int d = 0; d <= n_blocks_i + n_blocks_j - 2; d++)
    {
        #pragma omp parallel for num_threads(NUM_THREADS) schedule(dynamic)
        for (int bi = 0; bi <= d; bi++)
        {
            int bj = d - bi;
            if (bi >= n_blocks_i || bj >= n_blocks_j)
                continue;

            int i_start = bi * BLOCK_SIZE + 1;
            int i_end = min(i_start + BLOCK_SIZE, sizeB + 1);
            int j_start = bj * BLOCK_SIZE + 1;
            int j_end = min(j_start + BLOCK_SIZE, sizeA + 1);

            for (int i = i_start; i < i_end; i++)
            {
                for (int j = j_start; j < j_end; j++)
                {
                    if (seqA[j - 1] == seqB[i - 1])
                        matrix[i][j] = matrix[i - 1][j - 1] + 1;
                    else
                        matrix[i][j] = max(matrix[i - 1][j], matrix[i][j - 1]);
                }
            }
        }
    }
    return matrix[sizeB][sizeA];
}

int main()
{
    double ts_1, ts_2, t_seq, total_time;
    ts_1 = omp_get_wtime();

    // char *seqA = read_seq("fileA_G.in");
    // char *seqB = read_seq("fileB_G.in");
    char *seqA = read_seq("fileA_G.in");
    char *seqB = read_seq("fileB_G.in");

    int sizeA = strlen(seqA);
    int sizeB = strlen(seqB);

    mtype **matrix = allocateScoreMatrix(sizeA, sizeB);
    ts_2 = omp_get_wtime();
    t_seq = ts_2 - ts_1;

    // printf("\n%d\n", omp_get_max_threads());

    double t1 = omp_get_wtime();
    int score = LCS_blocked_omp(matrix, sizeA, sizeB, seqA, seqB); // fazer o cálculo do tempo dentro da função
    double t2 = omp_get_wtime();
    double t_par = t2 - t1;

    ts_1 = omp_get_wtime();

    freeScoreMatrix(matrix, sizeB);
    free(seqA);
    free(seqB);

    ts_2 = omp_get_wtime();
    t_seq += ts_2 - ts_1;
    total_time = t_seq + t2 - t1;
    printf("Tempo total: %.4f\n", total_time);
    printf("Tempo sequencial: %.4f\n", t_seq);
    printf("Tempo paralelo: %.4f\n", t2 - t1);
    printf("Porcentagem do tempo sequencial: %.2f%%\n", t_seq * 100 / total_time);
    printf("Porcentagem do tempo paralelo: %.2f%%\n", t_par * 100 / total_time);
    printf("Num threads: %d\n", NUM_THREADS);
    printf("Score: %d\n", score);
    
    return 0;
}
