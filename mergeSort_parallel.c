#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timer.h"
/* including the pthread header file */
#include <pthread.h>

int* a;
int chunkSize;
int phase;
int index;
int N, numThreads;

pthread_mutex_t mutex;

void merge(int[],int,int,int);


void merge(int arr[], int lower,int mid,int higher){
  int i,j,k;
  int size=mid-lower+1;
  int siz1=higher-mid;
  int low[size],high[siz1];
  for(i=0;i<size;i++){
    low[i]=arr[lower+i];
  }
  for(j=0;j<siz1;j++){
    high[j]=arr[mid+1+j];
  }

  j=i=0;
  k=lower;
  while (i<size&&j<siz1){
    if(low[i]<=high[j]){
      arr[k]=low[i];
      i++;
    }else {
      arr[k]=high[j];
      j++;
    }
    k++;
  }


  fflush(stdout);
  while(i<size){
    arr[k]=low[i];
    i++;
    k++;
  }
  while(j<siz1){
    arr[k]=high[j];
    j++;
    k++;
  }
}

void mergeSort(int arr[], int lower, int higher){
  int mid;
  if(lower<higher){
    mid=lower+(higher-lower)/2;
    mergeSort(arr,lower,mid);
    mergeSort(arr,mid+1,higher);
    merge(arr,lower,mid,higher);
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
      mergeSort(a, currentTask, chunkSize);
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
    printf("\tMergeSort Needs a Number Range or 8 by default\n");
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
    printf("MergeSort Only works when Array Length is k^2");
  }
  
  struct timeval startclock;
  struct timeval endclock;
  gettimeofday(&startclock, NULL);
  
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

  pthread_mutex_destroy(&mutex);
  gettimeofday(&endclock, NULL);
  printf ("Total time is T=%.5f\n", diffgettime(startclock, endclock));

  free(thread);
  free(a);
  return 0;
}
