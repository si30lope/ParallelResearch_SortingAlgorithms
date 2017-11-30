#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "timer.h"
/* including the pthread header file */
#include <pthread.h>

int* a;
int chunkSize;
int phase;
int index;
int N, numThreads;

pthread_mutex_t mutex;

void bitonicSort(int[],int,int,int);

void merge(int a[],int low, int count, int left){
  if(count>1){
    int half= count/2;
    int i,temp;
    for(i=low;i<low+half;i++){
      if(left==(a[i]>a[i+half])){
        temp= a[i];
        a[i]=a[i+half];
        a[i+half]=temp;
      }
    }
    merge(a,low,half,left);
    merge(a,low+half,half,left);
  }
}

void bitonicSort(int a[], int low,int count, int left){
  if(count>1){
    int half= count/2;
    bitonicSort(a,low,half,1);
    bitonicSort(a,low+half,half,0);
    merge(a,low,count,left);
  }
}

int * randomNumberGenerator(){
  int* array=malloc(sizeof(int)*N);
  srand(time(NULL));   // should only be called once
  int r = rand(); 
  int i;
  for(i=0;i<N;i++){
    r = rand(); 
    array[i]=r%N;
  }
  return array;
}

void *threadwork(void *arg){
  int* crap=(int *)arg;
  int rank=crap[0];
  int currentTask;
  pthread_mutex_lock(&mutex);
  currentTask=index*chunkSize;
  index++;
  pthread_mutex_unlock(&mutex);
//  int chunkSize=(N/numThreads);
//  int start =rank* chunkSize;
//  int end =start + chunkSize-1;




  while(phase>0) {
//    printf("In the loop: %d %d  Rank= %d, chunk%d index%d\n",N,currentTask ,rank, chunkSize,index);
    if (rank > (N / chunkSize)) {
      break;
    }
//    if(currentTask+chunkSize-1>N){
//      printf("How The Fuck %d +%d = %d \n",currentTask,chunkSize,currentTask+chunkSize);
//    }
    else {
      //printf("In the loop: %d Rank= %d, currtask=%d to %d, left= %d\n", N, rank, currentTask, chunkSize + currentTask - 1, (index) % 2);
      bitonicSort(a, currentTask, chunkSize , (index) % 2);
    }


    if((index-2)*chunkSize+chunkSize-1>=N){
      if(phase==1){
        break;
      }
      pthread_mutex_lock(&mutex);
      phase>>=1;
      index=0;
      chunkSize<<=1;
//      printf("Shifting Here to phase: %d ChunkSize: %d\n",phase,chunkSize);
    }else{
      pthread_mutex_lock(&mutex);

    }
//    pthread_mutex_lock(&mutex);
    currentTask=index*chunkSize;
    index++;
    pthread_mutex_unlock(&mutex);

  }
//  printf("Killing Thread %d %d\n ",rank,phase);
  pthread_exit(0);
}

/* The main program starts here */
int main(int argc, char *argv[]) {
  int i=0;
  int rc;
  if(argc==3){
    numThreads = atoi(argv[1]);
    N= atoi(argv[2]);
  }else{
    printf("\tBitonicSort Needs a Number Range or 8 by default\n");
    printf("\tWill be ran with one thread.\n");
    N=8;
    numThreads=1;
  }

  pthread_t* thread=malloc(sizeof(pthread_t)*numThreads);
  pthread_setconcurrency(numThreads+1);
  pthread_mutex_init(&mutex, NULL);
  
  a=randomNumberGenerator();
  int b[N];
  if(((N != 0) && !(N & (N - 1)))){
    printf("BitonicSort Only works when Array Length is k^2");
  }
  
  struct timeval startclock;
  struct timeval endclock;
  gettimeofday(&startclock, NULL);
  
 /* printf("Unsorted Array\n");
  for(i=0;i<N;i++) {
    printf("%d \t",a[i]);
  }
  for(i=0;i<N;i++){
    b[i]=a[i];
  }
  printf("\n");
*/




  phase=N>>1;
  chunkSize=2;
  index=0;

  for(i=0;i<numThreads;i++){
    rc = pthread_create(&(thread[i]),  NULL, &threadwork, (void *)&(i));

    if(rc != 0) {
      fprintf(stderr,"Error: cant create thread %d \n",i);
    }
  }

  for(i=0;i<numThreads;i++){
    pthread_join(thread[i], NULL);
  }


  /*  printf("\nSorted Array\n");
  for(i=0;i<N;i++) {
    printf("%d \t",a[i]);
  }
  printf("\n");*/

  /*for(i=0;i<N-1;i+=2){
    printf("%s \t",a[i]>a[i+1]?"\\":a[i]==a[i+1]?"-":"/");
  }
  printf("\n");*/

  pthread_mutex_destroy(&mutex);
  gettimeofday(&endclock, NULL);
  printf ("Total time is T=%.5f\n", diffgettime(startclock, endclock));

  free(thread);
  free(a);
  return 0;
}