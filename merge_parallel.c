#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timer.h"
/* including the pthread header file */
#include <pthread.h>

int* a;
int N, thread_count;
pthread_t* thread;
int bar_count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void merge(int,int,int);
void Barrier(void);

typedef struct MyRange{
  int start;
  int end;
  int rank;
}MyRange;

void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }
  printf(" %s\n",(pass) ? "Correct" : "Wrong");
}

/*-------------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Print a list of ints to stdout
 * In args:   list, n
 */
void printer(char title[], int list[], double time) {
  int i;
  
  if(title!=NULL){
    printf("%s:\n", title);
  }
  if(list!=(int*)NULL){
    for (i = 0; i < N; i++){
       printf("%d ", list[i]);
    }
    printf("\n");
  }else{
    test();
  }
  if(time){
    printf("in %.5f seconds\n", time);
  }else{
    printf("\n");
  }
}  /* Print_list */

void merge(int lower,int mid,int higher){
  int i,j,k;
  int size=mid-lower+1;
  int siz1=higher-mid;
  int low[size],high[siz1];
  for(i=0;i<size;i++){
    low[i]=a[lower+i];
  }
  for(j=0;j<siz1;j++){
    high[j]=a[mid+1+j];
  }

  j=i=0;
  k=lower;
  while (i<size&&j<siz1){
    if(low[i]<=high[j]){
      a[k]=low[i];
      i++;
    }else {
      a[k]=high[j];
      j++;
    }
    k++;
  }


  fflush(stdout);
  while(i<size){
    a[k]=low[i];
    i++;
    k++;
  }
  while(j<siz1){
    a[k]=high[j];
    j++;
    k++;
  }
}

void mergeSort(int lower, int higher){
  int mid;
  if(lower<higher){
    mid=lower+(higher-lower)/2;
    mergeSort(lower,mid);
    mergeSort(mid+1,higher);
    merge(lower,mid,higher);
  }
}

void *threadwork(void *arg){
  MyRange* crap=(MyRange *)arg;
  MyRange my_stuff=crap[0];
  mergeSort(my_stuff.start, my_stuff.end);
  pthread_exit(0);
}

void setupMergeThreads(int lower, int higher, int depth, int lower_threads,int higher_threads){
  int mid,rc,mid_threads;
  if((1<<depth)>thread_count){
    MyRange my_stuff;
    my_stuff.start=lower;
    my_stuff.end=higher;
    my_stuff.rank=higher_threads-1;
    rc = pthread_create(&(thread[higher_threads-1]),  NULL, &threadwork, (void *)&my_stuff);
    if(rc != 0) {
      fprintf(stderr,"Error: cant create thread %d \n",lower/depth);
    }

    pthread_join(thread[higher_threads-1], NULL);
  }else if(lower<higher){
    mid=lower+(higher-lower)/2;
    mid_threads=lower_threads+(higher_threads-lower_threads)/2;
    depth++;
    setupMergeThreads(lower,mid,depth,lower_threads,mid_threads);
    setupMergeThreads(mid+1,higher,depth,mid_threads+1,higher_threads);
    merge(lower,mid,higher);
  }
}

int* randomNumberGenerator(){
  int* array=malloc(sizeof(int)*N);
  srand(time(NULL));   // should only be called once
  int r = rand();
  int i;
  for(i=0;i<N;i++){
    array[i]=r%N;
    r = rand();
  }
  return array;
}

int* ascNumberGenerator(){
  int* array=malloc(sizeof(int)*N);
  int i;
  for(i=0;i<N;i++){
    array[i]=i;
  }
  return array;
}

int* desNumberGenerator(){
  int* array=malloc(sizeof(int)*N);
  int i;
  for(i=0;i<N;i++){
    array[i]=N-i;
  }
  return array;
}

int* arbNumberGenerator(){
  int* array=malloc(sizeof(int)*N);
  FILE *fp;
  char filename[40];
  char ch;

  printf("Enter the filename to be opened \n");
  scanf("%s", filename);
  /*  open the file for reading */
  fp = fopen(filename, "r");
  char buffer[500];
  int lineno = 0;

  while ( !feof(fp))
  {
    // read in the line and make sure it was successful
    if (fgets(buffer,500,fp) != NULL)
    {
      printf("%d: %s\n",lineno++,buffer);
      array[lineno-1]=atoi(buffer);
    }
  }
  N=lineno;
  return array;
}

/* The main program starts here */
int main(int argc, char *argv[]) {
  int i=0;
  int rc,t;
//  printf("Argument Format: numthreads numElements [1]Ascending [2]Descending [3]Random [4]File\n");
  if(argc==4){
    thread_count = atoi(argv[1]);
    N= atoi(argv[2]);
    t= atoi(argv[3]);
    switch(t){
      case 1:
        a=ascNumberGenerator();
        break;
      case 2:
        a=desNumberGenerator();
        break;
      case 3:
        a=randomNumberGenerator();
        break;
      case 4:
        a=arbNumberGenerator();
        printf("The amount of numbers isin this file where %d\n",N);
        break;
    }
  }else{
    printf("\tMergeSort Needs a Number Range or 8 by default\n");
    printf("\tWill be ran with one thread.\n");
    N=8;
    thread_count=1;
    a=randomNumberGenerator();
  }if(!((N != 0) && !(N & (N - 1)))){
    //printf("MergeSort Only works when Array Length is k^2\n");
  }

  //printer("Unsorted Array: ", a,0);
  
  thread=malloc(sizeof(pthread_t)*thread_count);
  pthread_setconcurrency(thread_count+1);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  struct timeval startclock, endclock;
  gettimeofday(&startclock, NULL);

  setupMergeThreads(0,N,1,0,thread_count);
  gettimeofday(&endclock, NULL);

  
  gettimeofday(&endclock, NULL);
  printer(NULL, NULL, diffgettime(startclock, endclock));
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  free(thread);
  free(a);
  return 0;
}

/*-------------------------------------------------------------------
 * Function:  Barrier
 * Purpose:   Block all threads until all threads have called
 *            Barrier
 * Globals:   bar_count, bar_mutex, bar_cond
 */
void Barrier(void) {
   pthread_mutex_lock(&mutex);
   bar_count++;
   if (bar_count == thread_count) {
      bar_count = 0;
      pthread_cond_broadcast(&cond);
   } else {
      while (pthread_cond_wait(&cond, &mutex) != 0);
   }
   pthread_mutex_unlock(&mutex);
}  /* Barrier */