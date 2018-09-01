#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/times.h>
#include <math.h>

#define min(c,d) \
   ({ __typeof__ (c) _c = (c); \
       __typeof__ (d) _d = (d); \
     _c > _d ? _d : _c; })

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


int **I; //image
float **K; //filter
int **J; // new filtered image

const int SIZE_OF_BUFFER=1024;
int Width, Height, Pixel, c;
FILE *pgm, *result, *filter;

void close_everything(char *buf);
void get_matrix(char *buf);
void get_filter(char *buf);
void get_parameters(char *buf);
void *make_picture(void *arg);
void run_threads(int number_of_threads);
void save_result();



int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Invalid number of arguments.\n");
        exit(1);
    }
    
    if ((pgm = fopen(argv[2], "r")) == NULL || (filter = fopen(argv[3], "r")) == NULL || (result = fopen(argv[4], "w")) == NULL) {
        printf("Wrong path of file.\n");
        exit(1);
    }
    int number_of_threads = atoi(argv[1]);
    char *buffer = (char *)calloc(SIZE_OF_BUFFER, sizeof(char));

    get_parameters(buffer);
    get_matrix(buffer);
    get_filter(buffer);

    J = (int **)calloc(Height, sizeof(int *));
    for (int i = 0; i < Height; ++i) {
        J[i] = (int *)calloc(Width, sizeof(int));
    }

    clock_t clock1, clock2;
    clock1 = times(NULL);
    run_threads(number_of_threads);
    clock2 = times(NULL);

    save_result();
    double time = (double)(clock2 - clock1)/sysconf(_SC_CLK_TCK);
    printf("Number of threads: %d.\n", number_of_threads);
    printf("Real time: %.2f\n\n\n", time);;

    close_everything(buffer);
    return 0;
}

void close_everything(char *buffer) {
    fclose(filter);
    fclose(result);
    fclose(pgm);
    free(buffer);
}


void get_matrix(char *buffer) {
    I = (int **)calloc(Height, sizeof(int *));
    for (int i = 0; i < Height; ++i) {
        I[i] = (int *)calloc(Width, sizeof(int));
    }
    char *tmp = strtok(buffer, " \n\t\v\f\r");
    for (int i = 0; i < Height; ++i) {
        for (int j = 0; j < Width; ++j) {
            if(tmp == NULL) {
                if (fgets(buffer, SIZE_OF_BUFFER, pgm) == NULL) {
                    printf("Problems with pgm file occured.\n");
                    close_everything(buffer);
                    free(tmp);
                    exit(1);
                }
                tmp = strtok(buffer, " \n\t\v\f\r");
            }
            I[i][j] = (int)strtol(tmp, NULL, 10);
            tmp = strtok(NULL, " \n\t\v\f\r");
		//printf("%s ",tmp);
        }
    }
    free(tmp);
}


void get_filter(char *buffer) {
    K = (float **)calloc(c, sizeof(float *));
    for (int i = 0; i < c; ++i) {
        K[i] = (float *)calloc(c, sizeof(float));
    }

    if(fgets(buffer, SIZE_OF_BUFFER, filter) != NULL) {
        char *tmp = strtok(buffer, " \n\t\v\f\r");
        for (int i = 0; i < c; ++i) {
            for (int j = 0; j < c; ++j) {
                if(tmp == NULL) {
                    if (fgets(buffer, SIZE_OF_BUFFER, filter) == NULL) {
                        printf("Problems with filter file occured.\n");
                        close_everything(buffer);
                        free(tmp);
                        exit(1);
                    }
                    tmp = strtok(buffer, " \n\t\v\f\r");
                }
                K[i][j] = strtof(tmp, NULL);
                tmp = strtok(NULL, " \n\t\v\f\r");
            }
        }
        free(tmp);
    } else {
        printf("Problems with filter file occured.\n");
        close_everything(buffer);
        exit(1);
    }
}


void get_parameters(char *buffer) {
    if(fgets(buffer, SIZE_OF_BUFFER, filter) != NULL) {
        c = (int)strtol(buffer, NULL, 10);
    } else {
        printf("Problems with filter file occured.\n");
        close_everything(buffer);
        exit(1);
    }

    for(int i = 0; i < 4; i++) {
        if(fgets(buffer, SIZE_OF_BUFFER, pgm) != NULL) {
            if(i == 1) {
                Width = (int) strtol(strtok(buffer, " \n\t\v\f\r"), NULL, 10);
                Height = (int) strtol(strtok(NULL, " \n\t\v\f\r"), NULL, 10);
            } else if(i == 2) {
                Pixel= (int) strtol(buffer, NULL, 10);
            }
        } else {
            printf("Problems with pgm file occured.\n");
            close_everything(buffer);
            exit(1);
        }
    }

    if (Pixel > 255) {
        printf("Invalid value of pixel.\n");
        close_everything(buffer);
        exit(1);
    }
}


void *make_picture(void *arg) {
    int *numbers = (int *)arg;
    double value = 0;
    int _ceil = (int)ceil(c/2);
    printf("Executing thread for columns %d - %d\n", numbers[0]+1, numbers[1]);

    for (int x = 0; x < Height; ++x) {
        for (int y = numbers[0]; y < numbers[1]; ++y) {

            value = 0;
            for (int i = 1; i <= c; ++i) {
                for (int j = 1; j <= c; ++j) {
                    value += (I[min((Height-1), max(0, (x-_ceil+i)))][min((Width-1), max(0, (y-_ceil+j)))] * K[i-1][j-1]);
                }
            }
            J[x][y] = (int)round(value);
        }
    }
    free(numbers);
    return 0;
}


void run_threads(int number_of_threads) {
    int number_of_columns = Width/number_of_threads;
    pthread_t *threads = (pthread_t *)calloc(number_of_threads, sizeof(pthread_t));

    for (int i = 0; i < number_of_threads; ++i) {
        int *numbers = (int *)calloc(2, sizeof(int));
        numbers[0] = i*number_of_columns;
        if (i != number_of_threads-1) numbers[1] = i*number_of_columns + number_of_columns;
        else numbers[1] = Width;
        pthread_create(&threads[i], NULL, make_picture, (void *)numbers);
    }

    for (int i = 0; i < number_of_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
}


void save_result() {
    fprintf(result, "P2\n%d %d\n%d\n", Width, Height, Pixel);

    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            fprintf(result, "%d ", J[i][j]);
        }
        fprintf(result, "\n");
    }
}
