/*

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted
int index;
int numThreads;
pthread_mutex_t mutex;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void descending(void);
void print(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void impBitonicSort(int);
int* randomNumberGenerator(void);




void* threadwork(void *arg){
  int* crap=(int *)arg;
  int rank=crap[0];
  
//  printf("Rank number %d\n",rank);
  
  impBitonicSort(rank);
  pthread_exit(0);
}

/** the main program **/ 
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
  if(((N != 0) && !(N & (N - 1)))){
    printf("BitonicSort Only works when Array Length is k^2\n");
  }index=0;

  gettimeofday (&startwtime, NULL);
  
  for(i=0;i<=numThreads;i++){
    rc =pthread_create(&(thread[i]),  NULL, &threadwork, (void *)&(i));

    if(rc != 0) {
      fprintf(stderr,"Error: cant create thread %d \n",i);
    }
  }

  for(i=0;i<numThreads;i++){
    pthread_join(thread[i], NULL);
  }
  
  
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);

  test();

}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
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


/** procedure init() : initialize array "a" with data **/
void descending() {
  int i;
  for (i = 0; i < N; i++) {
    // a[i] = rand() % N; // (N - i);
    a[i] = (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}

/*
  imperative version of bitonic sort
*/


void impBitonicSort(int rank) {
  int out, mid,inn;
  int c=0;  
  out=2;
  
  int currentTask;
  
  
  while(out<=N){
    if(rank>N/out){
      printf("Killing Thread[%d] at %d\n",rank, N/out);
      break;
    }
    do{
        pthread_mutex_lock(&mutex);
        currentTask=index*out;
        index++;
        pthread_mutex_unlock(&mutex);
        if(currentTask+out>N){
          break;
        }
//        printf("|Phase number %d|  Rank= %d\n", c, rank);
        mid=out>>1;
        while(mid>0){
          inn=currentTask;
          //printf("%d %d %d and rank[%d] from %d -> %d\n",inn,mid,out,rank, inn,(currentTask+out-1));
          while(inn<currentTask+out){       
            int ij=inn^mid;
            if ((ij)>inn) {
              if ((inn&out)==0 && a[inn] > a[ij]) 
                  exchange(inn,ij);
              if ((inn&out)!=0 && a[inn] < a[ij])
                  exchange(inn,ij);
            }

            inn++;
          }

          mid=mid>>1;
        }
    }while(1);
//    printf("Rank[%d] Out = %d\n\n ",rank,out);
    out=2*out;
    pthread_mutex_lock(&mutex);
    currentTask=index*out;
    index=0;
    pthread_mutex_unlock(&mutex);
    c++;
    
  }
}



  