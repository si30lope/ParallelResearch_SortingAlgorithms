#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"


int thread_count;
int bar_count = 0;
pthread_mutex_t bar_mutex;
pthread_cond_t bar_cond;
long N;
int *list1, *list2;
int *l_a, *l_b;

int* randomNumberGenerator();
int* ascNumberGenerator();
int* desNumberGenerator();
int* arbNumberGenerator();
void printer(char title[], int list[], double time);
void *Bitonic_sort(void* rank);
void Bitonic_sort_incr(int th_count, int dim, int my_first, int local_n,
      int my_rank);
void Bitonic_sort_decr(int th_count, int dim, int my_first, int local_n,
      int my_rank);
void Merge_split_lo(int my_rank, int my_first, int local_n,
      int partner);
void Merge_split_hi(int my_rank, int my_first, int local_n,
      int partner);
int  Compare(const void* x_p, const void* y_p);
void Barrier(void);
void test(void);

/*--------------------------------------------------------------------*/


int main(int argc, char* argv[]) {
  long       thread;
  pthread_t* thread_handles; 
  double     start, finish;
  int        gen_list, output_list;
  
  int i=0;
  int rc,t;
  //printf("Argument Format: numthreads numElements [1]Ascending [2]Descending [3]Random [4]File\n");
  if(argc==4){
    thread_count = atoi(argv[1]);
    N= atoi(argv[2]);
    t= atoi(argv[3]);
    switch(t){
      case 1:
        list1=ascNumberGenerator();
        break;
      case 2:
        list1=desNumberGenerator();
        break;
      case 3:
        list1=randomNumberGenerator();
        break;
      case 4:
        list1=arbNumberGenerator();
    //    printf("The amount of numbers isin this file where %d\n",N);
        break;
    }    
  }else if(argc==3){
    thread_count = atoi(argv[1]);
    N= atoi(argv[2]);
    list1=randomNumberGenerator();
  }else{
    printf("\tBitonicSort Needs a Number Range or 8 by default\n");
    printf("\tWill be ran with one thread.\n");
    N=8;
    thread_count=1;
    list1=randomNumberGenerator();
  }  
  thread_handles = malloc (thread_count*sizeof(pthread_t));
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_cond_init(&bar_cond, NULL);
  
  list2 = malloc(N*sizeof(int));
  l_a = list1;
  l_b = list2;  
  
  struct timeval startclock;
  struct timeval endclock;
  gettimeofday(&startclock, NULL);
  
  for (thread = 0; thread < thread_count; thread++){
    pthread_create(&thread_handles[thread], NULL,
        Bitonic_sort, (void*) thread);
  }
  
  for (thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }
  gettimeofday(&endclock, NULL);
  //printer("The sorted list is", NULL, diffgettime(startclock, endclock));
  printer(NULL, NULL, diffgettime(startclock, endclock));

  
  free(list1);
  free(list2);
  pthread_mutex_destroy(&bar_mutex);
  pthread_cond_destroy(&bar_cond);
  free(thread_handles);
  return 0;
}  /* main */

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

void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (l_a[i-1] <= l_a[i]);
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
    printf("in T%.5f\n", time);
  }else{
    printf("\n");
  }
}  /* Print_list */


/*-----------------------------------------------------------------
 * Function:     Compare
 * Purpose:      Compare two ints and determine their relative sizes
 * In args:      x_p, y_p
 * Ret val:      -1 if *x_p < *y_p
 *                0 if *x_p == *y_p
 *               +1 if *x_p > *y_p
 * Note:         For use by qsort library function
 */
int Compare(const void* x_p, const void* y_p) {
   int x = *((int*)x_p);
   int y = *((int*)y_p);

   if (x < y)
      return -1;
   else if (x == y)
      return 0;
   else /* x > y */
      return 1;
}  /* Compare */


/*-------------------------------------------------------------------
 * Function:        Bitonic_sort
 * Purpose:         Implement bitonic sort of a list of ints
 * In arg:          rank
 * In globals:      barrier, thread_count, n (list size), list1
 * Out global:      l_a
 * Scratch globals: list2, l_b
 * Return val:      Ignored
 */
void *Bitonic_sort(void* rank) {
   long tmp = (long) rank;
   int my_rank = (int) tmp; 
   int local_n = N/thread_count;
   int my_first = my_rank*local_n;
   unsigned th_count, and_bit, dim;
   /* Sort my sublist */
   qsort(list1 + my_first, local_n, sizeof(int), Compare); 
   Barrier();
   for (th_count = 2, and_bit = 2, dim = 1; th_count <= thread_count; 
         th_count <<= 1, and_bit <<= 1, dim++) {
      if ((my_rank & and_bit) == 0)
         Bitonic_sort_incr(th_count, dim, my_first, local_n, my_rank);
      else
         Bitonic_sort_decr(th_count, dim, my_first, local_n, my_rank);
   }

   return NULL;
}  /* Bitonic_sort */

/*-------------------------------------------------------------------
 * Function:      Bitonic_sort_incr
 * Purpose:       Use parallel bitonic sort to sort a list into
 *                   increasing order.  This implements a butterfly
 *                   communication scheme among the threads
 * In args:       th_count:  the number of threads participating
 *                   in this sort
 *                dim:  base 2 log of th_count
 *                my_first:  the subscript of my first element in l_a
 *                local_n:  the number of elements assigned to each
 *                   thread
 *                 my_rank:  the calling thread's global rank
 * In/out global:  l_a pointer to current list.
 * Scratch global: l_b pointer to temporary list.
 */
void Bitonic_sort_incr(int th_count, int dim, int my_first, int local_n,
      int my_rank) {
   int stage;
   int partner;
   int* tmp;
   unsigned eor_bit = 1 << (dim - 1);

   for (stage = 0; stage < dim; stage++) {
      partner = my_rank ^ eor_bit;
      if (my_rank < partner)
         Merge_split_lo(my_rank, my_first, local_n, partner);
      else
         Merge_split_hi(my_rank, my_first, local_n, partner);
      eor_bit >>= 1;
      Barrier();
      if (my_rank == 0) {
         char title[1000];
         tmp = l_a;
         l_a = l_b;
         l_b = tmp;
      }
      Barrier();
   } 
       
}  /* Bitonic_sort_incr */


/*-------------------------------------------------------------------
 * Function:      Bitonic_sort_decr
 * Purpose:       Use parallel bitonic sort to sort a list into
 *                   decreasing order.  This implements a butterfly
 *                   communication scheme among the threads
 * In args:       th_count:  the number of threads participating
 *                   in this sort
 *                dim:  base 2 log of th_count
 *                my_first:  the subscript of my first element in l_a
 *                local_n:  the number of elements assigned to each
 *                   thread
 *                 my_rank:  the calling thread's global rank
 * In/out global:  l_a pointer to current list.
 * Scratch global: l_b pointer to temporary list.
 */
void Bitonic_sort_decr(int th_count, int dim, int my_first, int local_n,
      int my_rank) {
   int stage;
   int partner;
   int* tmp;
   unsigned eor_bit = 1 << (dim - 1);

   for (stage = 0; stage < dim; stage++) {
      partner = my_rank ^ eor_bit;
      if (my_rank > partner)
         Merge_split_lo(my_rank, my_first, local_n, partner);
      else
         Merge_split_hi(my_rank, my_first, local_n, partner);
      eor_bit >>= 1;
      Barrier();
      if (my_rank == 0) {
         tmp = l_a;
         l_a = l_b;
         l_b = tmp;

      }
      Barrier();
   } 
       
}  /* Bitonic_sort_decr */


/*-------------------------------------------------------------------
 * Function:        Merge_split_lo
 * Purpose:         Merge two sublists in array l_a keeping lower half
 *                  in l_b
 * In args:         partner, local_n 
 * In/out global:   l_a
 * Scratch:         l_b
 */
void Merge_split_lo(int my_rank, int my_first, int local_n,
      int partner) {
   int ai, bi, xi, i;


   ai = bi = my_first;
   xi = partner*local_n;

   for (i = 0; i < local_n; i++)
      if (l_a[ai] <= l_a[xi]) {
         l_b[bi++] = l_a[ai++];
      } else {
         l_b[bi++] = l_a[xi++];
      }

}  /* Merge_split_lo */


/*-------------------------------------------------------------------
 * Function:        Merge_split_hi
 * Purpose:         Merge two sublists in array l_a keeping upper half
 *                  in l_b
 * In args:         partner, local_n 
 * In/out global:   l_a
 * Scratch:         l_b
 */
void Merge_split_hi(int my_rank, int my_first, int local_n,
      int partner) {
   int ai, bi, xi, i;

   ai = bi = my_first + local_n - 1;
   xi = (partner+1)*local_n - 1;

   for (i = 0; i < local_n; i++)
      if (l_a[ai] >= l_a[xi])
         l_b[bi--] = l_a[ai--];
      else
         l_b[bi--] = l_a[xi--];

}  /* Merge_split_hi */


/*-------------------------------------------------------------------
 * Function:  Barrier
 * Purpose:   Block all threads until all threads have called
 *            Barrier
 * Globals:   bar_count, bar_mutex, bar_cond
 */
void Barrier(void) {
   pthread_mutex_lock(&bar_mutex);
   bar_count++;
   if (bar_count == thread_count) {
      bar_count = 0;
      pthread_cond_broadcast(&bar_cond);
   } else {
      while (pthread_cond_wait(&bar_cond, &bar_mutex) != 0);
   }
   pthread_mutex_unlock(&bar_mutex);
}  /* Barrier */