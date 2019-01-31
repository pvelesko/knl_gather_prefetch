#include <iostream>
#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include <set>
#define N 2500

#define DOUBLE_IN_L1 2666 // 32,000/(8+4)
#define DOUBLE_IN_L2 83333  // 1,000,000/(8+4)
#define TRASH 25000000

void print256_num(__m256i var)
{
    int *val = (int*) &var;
    printf("Numerical: %i %i %i %i %i %i %i %i \n", 
           val[0], val[1], val[2], val[3], val[4], val[5], 
           val[6], val[7]);
}

//namespace std{
int main(int argc, char* argv)
{

    int idx[N];
    double val[N];
    volatile double trash[TRASH];

    for (int i=0; i<N; i++)
        val[i] = i;

//    std::set<int> idx_set;
    //fill up index array to touch every val element randomly
    for (int i=0; i<N; i++)
    {
        int ridx = rand() % N;
        // check if already used
        // if not, make the current idx[i] point to this val in vals
        idx[i] = ridx;
        //printf("idx[%i]=%i\n", i, idx[i]);
        // add to set
//        idx_set.insert(ridx);
        
    }

    double t;
    double sum;


    for (int i=0; i<TRASH; i++)
        sum += trash[i];
    printf("Cache Trash %d\n", sum);


    // first touch
    t = omp_get_wtime();
    sum = 0;
    for (int i=0; i<N; i++)
    {
        sum += val[idx[i]];
    }
    t = omp_get_wtime() - t;
    printf("First touch: \n");
    std::cout << t << std::endl;

    for (int i=0; i<TRASH; i++)
        sum += trash[i];
    printf("Cache Trash %d\n", sum);

    for (int i=0; i<N; i++)
    {
        if (i % 8 == 0)
        {
            void* addr = idx + i;
            //printf("prefetching val[%i] at %X offset of %i\n", i, addr, sizeof(int)*i);
            __m256i vindex = _mm256_loadu_si256 ( (__m256i*)addr);
            //print256_num(vindex);
            _mm512_prefetch_i32gather_pd ( vindex, val, 8, 1);
        }
    }

    // hot cache
    t = omp_get_wtime();
    sum = 0;
    for (int i=0; i<N; i++)
    {
        sum += val[idx[i]];
    }
    t = omp_get_wtime() - t;
    printf("Second touch: \n");
    std::cout << t << std::endl;

    t = omp_get_wtime();
    sum = 0;
    for (int i=0; i<N; i++)
    {
        //std::cout << val[idx[i]] << std::endl;
//        if (i % 8 == 0)
//        {
//            void* addr = idx + i;
//            //printf("prefetching val[%i] at %X offset of %i\n", i, addr, sizeof(int)*i);
//            __m256i vindex = _mm256_loadu_si256 ( (__m256i*)addr);
//            //print256_num(vindex);
//            _mm512_prefetch_i32gather_pd ( vindex, val, 1, 2);
//        }
        sum += val[idx[i]];

    }
    t = omp_get_wtime() - t;
    std::cout << t << std::endl;
}
//}

