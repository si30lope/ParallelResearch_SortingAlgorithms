#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timer.h"

/*
* This method is the processes of zipping up two
* sorted arrays
*/
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

/*
* This method seperates the array into segements
* recursively and merges them together
*/
void mergeSort(int arr[], int lower, int higher){
  int mid;
  if(lower<higher){
    mid=lower+(higher-lower)/2;
    mergeSort(arr,lower,mid);
    mergeSort(arr,mid+1,higher);
    merge(arr,lower,mid,higher);
  }
}

/*
* Generates a array of random numbers
*/
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
  int size=0;
  if(argc==2){
    size= atoi(argv[1]);
  }else{
    printf("MergeSort Needs a Number Range or 8 by default\n");
    size=8;
  }

  int* a=randomNumberGenerator(size);
  int i;
  //Prints Unsorted Array
  /*
  printf("Unsorted Array\n");
  for(i=0;i<size;i++)
    printf("%d \t",a[i]);
  printf("\n");*/

  struct timeval startclock;
  struct timeval endclock;
  gettimeofday(&startclock, NULL);
  mergeSort(a, 0, size-1);
  //Prints Sorted Array
/*  printf("Sorted Array\n");
  for(i=0;i<size;i++)
    printf("%d \t",a[i]);
  printf("\n");*/

  gettimeofday(&endclock, NULL);
  printf ("Total time is T=%.5f\n", diffgettime(startclock, endclock));
  return 0;
}
