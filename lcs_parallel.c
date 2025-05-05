#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

typedef unsigned short mtype; // unsigned short = 2 bytes

char* read_seq(char *fname) {
    FILE *fseq = fopen(fname, "rt");
    if (!fseq) {
        printf("Erro ao ler o arquivo %s\n", fname);
        exit(1);
    }

    fseek(fseq, 0L, SEEK_END);
    long size = ftell(fseq);
    rewind(fseq);

    char *seq = (char *) calloc(size + 1, sizeof(char));
    if (!seq) {
        printf("Erro ao alocar memória para %s\n", fname);
        exit(1);
    }

    int i = 0;
    while (!feof(fseq)) {
        char c = fgetc(fseq);
        if (c != '\n' && c != EOF) {
            seq[i++] = c;
        }
    }
    seq[i] = '\0';
    fclose(fseq);
    return seq;
}

mtype** allocateScoreMatrix(int sizeA, int sizeB) {
    mtype **matrix = (mtype **) malloc((sizeB + 1) * sizeof(mtype *));
    for (int i = 0; i <= sizeB; i++) {
        matrix[i] = (mtype *) calloc(sizeA + 1, sizeof(mtype));
    }
    return matrix;
}

void initScoreMatrix(mtype **matrix, int sizeA, int sizeB) {
    for (int i = 0; i <= sizeB; i++) matrix[i][0] = 0;
    for (int j = 0; j <= sizeA; j++) matrix[0][j] = 0;
}

int LCS_parallel(mtype **matrix, int sizeA, int sizeB, char *seqA, char *seqB) {
    int i, j, d;

    for (d = 2; d <= sizeA + sizeB; d++) {
        // Diagonal d: i + j = d
        #pragma omp parallel for private(i,j) shared(matrix, seqA, seqB)
        for (i = 1; i <= sizeB; i++) {
            j = d - i;
            if (j >= 1 && j <= sizeA) {
                if (seqA[j - 1] == seqB[i - 1]) {
                    matrix[i][j] = matrix[i - 1][j - 1] + 1;
                } else {
                    matrix[i][j] = max(matrix[i - 1][j], matrix[i][j - 1]);
                }
            }
        }
    }
    return matrix[sizeB][sizeA];
}

void freeScoreMatrix(mtype **matrix, int sizeB) {
    for (int i = 0; i <= sizeB; i++)
        free(matrix[i]);
    free(matrix);
}

int main() {
    char *seqA = read_seq("fileA.in");
    char *seqB = read_seq("fileB.in");
    int sizeA = strlen(seqA);
    int sizeB = strlen(seqB);

    mtype **matrix = allocateScoreMatrix(sizeA, sizeB);
    initScoreMatrix(matrix, sizeA, sizeB);

    mtype score = LCS_parallel(matrix, sizeA, sizeB, seqA, seqB);
    printf("\nLCS length: %d\n", score);

    freeScoreMatrix(matrix, sizeB);
    free(seqA);
    free(seqB);
    return EXIT_SUCCESS;
}
