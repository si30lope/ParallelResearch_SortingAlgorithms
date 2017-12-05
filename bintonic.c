#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "timer.h"

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

int* randomNumberGenerator(int N){
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

/* The main program starts here */
int main(int argc, char *argv[]) {
  int N;
  if(argc==2){
    N= atoi(argv[1]);
  }else{
     printf("BitonicSort Needs a Number Range or 8 by defautl");
     N=8;
  }
  
  struct timeval startclock;
  struct timeval endclock;
  gettimeofday(&startclock, NULL);
  
  int* a=randomNumberGenerator(N);
  int i;
  if(((N != 0) && !(N & (N - 1)))){
    printf("BitonicSort Only works when Array Length is k^2");
  }
  printf("Unsorted Array\n");
  for(i=0;i<N;i++)
    printf("%d \t",a[i]);
  
  bitonicSort(a, 0, N,1);
  
  printf("Sorted Array\n");
  for(i=0;i<N;i++)
    printf("%d \t",a[i]);
  printf("\n");
  
  gettimeofday(&endclock, NULL);
  printf ("Total time is T=%.5f\n", diffgettime(startclock, endclock));
  return 0;
}
