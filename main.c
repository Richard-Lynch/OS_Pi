#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 


int NUM_THREADS = 4;
int NUM_STEPS;
int TOTAL_STEPS = 10000;
long double A_location = 0;
long double B_location = 1;
long double STEP_SIZE = 0;
long double A_CURR = 0;
long double AREA_TOTAL= 0;

struct timespec diff_t(struct timespec start, struct timespec end);

long double function (long double x){
    long double result = 4.00L/(1.00L+(x*x));
    return result;
}

typedef struct {
    //Or whatever information that you need
    long double a;
    long double b;
    int index;
} bounds;

void *intergrate(void *bound){
// create some local vars
    int loops = 0;
    long double* area = malloc(sizeof(long double));
    *area = 0.00L;

    long double A = ((bounds*)bound)->a;
    long double B = ((bounds*)bound)->b;
    int index = ((bounds*)bound)->index;

    long double step_size = (B-A)/NUM_STEPS;

    long double a = A;
    long double b = a+step_size;
    long double ab = ((b+a)/2);

    long double fa;
    long double fb;
    long double fab;

    while(a < B){
    // calculate new values
        fa = fabsl(function(a));
        fb = fabsl(function(b));
        fab = fabsl(function(ab));

//simposons rule ( first order )
        // area = h/3(fa + 4fab + fb);
        *area += ((step_size/2)/3)*(fa + 4*fab + fb);
 
// iterate area
        a = b;
        b = a + step_size;
        ab = ((b+a)/2);
        loops++;
    }
    // output area
    //printf("Thread: %d Loops: %d Area: %Lf\n",index, loops, *area);
    pthread_exit((void*)area);
}

int main(int argc, const char* argv[]){
    printf("hello\n");
struct timespec time1, time2;


// read args
    int max_threads;
    if (argc < 2 || (max_threads = atoi(argv[1])) <= 0)
    { //convert the first arg to an int and set n
        //if there is not arg entered
        printf("This program computes pi\n"
               "\"usage: main n\" , where n is the number of threads and p is the number of steps per thread\n");
        exit(1);
    }

    long int results_s[max_threads];
    long int results_ns[max_threads];
    
for(int i = 0; i < max_threads; i++){
// initilise vals
    A_location = 0L;
    B_location = 1L;
    STEP_SIZE = 0L;
    A_CURR = 0L;
    AREA_TOTAL= 0L;

    NUM_THREADS = i+1;
    NUM_STEPS = TOTAL_STEPS/NUM_THREADS;
// create thread vars
    pthread_t threads[NUM_THREADS];
    int rc, t;
    bounds thread_bounds[NUM_THREADS];
    STEP_SIZE = (B_location - A_location)/NUM_THREADS;

// start clock
    clock_t start = clock();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
printf("Creating threads.\n");
// create first thread
    for(t = 0; t<NUM_THREADS; t++){
    // create bounds for curr thread
        thread_bounds[t].a = A_CURR;
        A_CURR = A_CURR + STEP_SIZE;
        thread_bounds[t].b = A_CURR;
        thread_bounds[t].index = t;
    // create thread
        rc = pthread_create(&threads[t], NULL, intergrate, (void*)&thread_bounds[t]);        
        if(rc){
            printf("ERROR - return code from pthread_create: %d ERROR: %d\n", t, rc);
            exit(-1);
        }
        // printf("Finished creating thread %d\n", t);
    }

// join the threads
    void* temp_area;
    for(t = 0; t<NUM_THREADS; t++){
    // join thread
        rc = pthread_join(threads[t], &temp_area);
        if(rc){
            printf("ERROR - return code from pthread_join: %d ERROR: %d\n", t, rc);
            exit(-1);
        }
        //printf("Finished Joining thread %d: area: %Lf\n",t, *(long double*)temp_area);
        AREA_TOTAL += *(long double*)temp_area;
        free (temp_area);
    }
    //printf("Finished Joining threads\n");
// stop the cock
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    clock_t diff = clock() - start;

// output result
    printf("---RESULTS----\n");
    printf("Total number of threads: %d \n", NUM_THREADS);
    printf("Step Size: %Lf\n", STEP_SIZE);
    printf("Small Step Size: %Lf\n", STEP_SIZE/NUM_STEPS);
    printf("Steps per thread: %d\n", NUM_STEPS);
    printf("Result: %Lf\n", AREA_TOTAL);
// output time
    long double msec = diff * 1000.00 / CLOCKS_PER_SEC;

    printf("Time taken %ld Seconds %ld Nanoseconds \n", (long int)diff_t(time1,time2).tv_sec, (long int)diff_t(time1,time2).tv_nsec);
    printf("Time taken %Lf Milliseconds\n", msec);
    results_s[i] = (long int)diff_t(time1,time2).tv_sec;
    results_ns[i] = (long int)diff_t(time1,time2).tv_nsec;
}

FILE *f = fopen("output.txt", "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1);
}


    printf("\n----Summary----\n");
	fprintf(f, "%ld, %ld\n", results_s[0], results_ns[0]);
    printf("%d thread : %ld seconds: %ld nanoseconds\n", 1, results_s[0], results_ns[0]);
    for(int j = 1; j<max_threads; j++){
	fprintf(f, "%ld, %ld\n", results_s[j], results_ns[j]);

	//fprintf(f, "%ld,\n", results[j]);
    printf("%d thread : %ld seconds: %ld nanoseconds\n", j+1, results_s[j], results_ns[j]);
       // printf("%d threads: %ld milliseconds\n", j+1, results[j]);
    }
    return 0;
}



struct timespec diff_t(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

