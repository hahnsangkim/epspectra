/* -*- Mode: c++ -*-
 *
 *  Copyright 1997 Massachusetts Institute of Technology
 *  Modified  2002 INRIA 
 * 
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation, and that the name of M.I.T. not be used in advertising or
 *  publicity pertaining to distribution of the software without specific,
 *  written prior permission.  M.I.T. makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as is"
 *  without express or implied warranty.
 * 
 */

#ifndef _STRLCYCLECOUNT_H_
#define _STRLCYCLECOUNT_H_

extern "C" {
#include <stdio.h>
#ifdef THREADS
#include<pthread.h>
#endif
}

//brettv 6/24/99 I junked the old user-level perfmon stuff.
//PERMON now requires the kernel driver to operation

#ifdef PERFMON
#if defined (__i386__)
#ifdef PERFCTR
//#include <libperfctr.h>
#else
extern "C" {
#include "rdtsc.h"
#include<pthread.h> //mutual exclusion
}
#endif

//#define P6_DCU_MISS_OUTSTANDING 4 
//#define P6_RESOURCE_STALLS 64 
//#define PERF1_CODE p6_events[P6_DCU_MISS_OUTSTANDING]
//#define PERF0_NAME "DCU miss cycles"
//#define PERF2_CODE p6_events[P6_RESOURCE_STALLS]
//#define PERF1_NAME "Resource stalls"

#define PERF1_NAME "number of cycles while DCU miss outstanding"
#define PERF2_NAME "cycles during resource related stalls"

//#define START_COUNTS() start_counters(PERF1_CODE.id,PERF2_CODE.id) //hkim
#define START_COUNTS() NULL //hkim
//#define STOP_COUNTS() perfctr_disable()//hkim
#else
//not supported
#error PERMON only supported on x86
/*alpha  #define CYCLE_COUNT() ({ __volatile__ unsigned long time; \
	     __asm__("rpcc %0" :"=r" (time)); \
	     (unsigned long)((time >> 32) + (time & 0x00000000ffffffff)); })*/
#endif //x86

//Global counts:
//-------------

//Per module counts:
//-----------------
#ifdef PERFCTR
typedef struct {
  unsigned long long counter[3];
} pmcs;
#endif
class StrlCycleCount {
private:
  unsigned long long totalSamples;
#ifdef PERFCTR
  pmcs total;
#else
  double PMCCycle;//hkim
  double PMCforMainSink;
  double AwaitTCycle;//verify await tick cycle
#endif
#ifdef THREADS
  pthread_key_t mystart; 
  pthread_mutex_t mutex;
#else
#ifdef PERFCTR
  pmcs start;
#else
  pthread_mutex_t mutex;//mutual exclusion
  tsc first; //hkim
  tsc last; //hkim
  tsc f_main;//for MainSink
  tsc l_main;//for MainSink
  tsc AwaitTfirst;
  tsc AwaitTlast;
#endif
#endif  
  //*** the rest of this stuff is Pspectra internal performance stuff
public:
#ifdef PERFMON
  unsigned long long num_AwaitTCycle; //await tick
  unsigned long long num_PMCCycle;
  unsigned long long num_PMCforMainSink;
#endif
  //By occurrence:
  //--------------
  //no one computed the data ahead of mine, and I can skip it
  unsigned long long skippedData; //number of units skipped

  //couldn't mark data because there wasn't enough room in the downstream
  //buffer
  unsigned long long bufferFullOnWrite; //# of occurrences

  //data is computed (ready) but other threads won't know
  //  because we're using an oversimplified linked list.
  //(i.e. out of order completion of writing threads.)
  //if both this and blockOnMarkedData are high
  //more work should be done here to fix our writeLL data
  //structure
  unsigned long long WPdelayedUpdate; //#occurrences

  //no one has marked the data ahead of mine yet so I have to wait!
  //(i.e. I couldnt' skip data)
  unsigned long long blockOnUnmarkedData; //#occurrences

  //# of times work() didn't finish all its work
  unsigned long long uncompleteWork; //#occurrences

  //By duration:
  //--------------
  //cycles are kind of bogus right now -- should use perfctr stuff
  //FIX

  //the previous thread didn't finish data I need!
  unsigned long long blockOnMarkedData; //#cycles spent waiting

  //blocked in sync() 
  unsigned long long blockOnSync; //#cycles spent waiting

public:
  /*** Performance monitoring procedures ***/
  void startCount() {
#ifdef THREADS
    pmcs *p=(pmcs *) pthread_getspecific(mystart);
    if(p == 0) {
      //init thread specific data
      p=new pmcs;
      pthread_setspecific(mystart,(void *) p);
      /*      pthread_mutex_lock(&mutex);
      if(START_COUNTS()<0) {
	perror("StrlCycleCount: could not start counts");
	fprintf(stderr,"Must be running on a system with the /dev/perfctr driver installed.\n");
	exit(-1);
      }
      pthread_mutex_unlock(&mutex);*/
    } 
#ifdef PARANOID
    else if(p->counter[0]!=0) {
      fprintf(stderr, "StrlCycleCount(%p): counter already started! \n", this);
      exit(-1);
    }
#endif
#else
#ifdef PERFCTR
    pmcs *p = &start;
#endif
#ifdef PARANOID
    if(start.counter[0]) {
      fprintf(stderr, "StrlCycleCount(%p): counter already started! \n", this);
      exit(-1);
    }
#endif
#endif

#ifdef PERFCTR
    struct perfctr_state state;
    if( perfctr_read(&state) < 0 ) {
      perror("perfctr_read");
      exit(1);
    }
    p->counter[0] = state.counters.tsc.u64;
    p->counter[1] = state.counters.pmc[0].u64;
    p->counter[2] = state.counters.pmc[1].u64;
#else
    if (first.lo==0){
      //pthread_mutex_lock(&mutex);
      RDTSC(first); //hkim
      //pthread_mutex_unlock(&mutex);
    }
#endif
}

void stopCount() {
#ifdef THREADS
    pmcs *p=(pmcs *) pthread_getspecific(mystart);
#ifdef PARANOID
    if(p == 0 || !p->counter[0] ) {
      fprintf(stderr, "StrlCycleCount(%p): counter not running! \n", this);
      exit(-1);
    }
#endif
#else
#ifdef PERFCTR
    pmcs *p = &start;
#endif
#ifdef PARANOID
    if(!start.counter[0]) {
      fprintf(stderr, "StrlCycleCount: counter not running! \n");
      exit(-1);
    }
#endif
#endif
    
#ifdef PERFCTR
    struct perfctr_state state;
    if( perfctr_read(&state) < 0 ) {
      perror("perfctr_read");
      exit(1);
    }
#else
    if(first.lo!=0){
      //pthread_mutex_lock(&mutex);
      RDTSC(last);//hkim
      double temp = CLOCK_CYCLES(first,last);
      if(PMCCycle>temp)
	PMCCycle=temp;
      ++num_PMCCycle;
      first.hi=0;first.lo=0;
      last.hi=0;last.lo=0;
    }
#endif

#ifdef THREADS
    pthread_mutex_lock(&mutex);
#endif

#ifdef PARANOID
    if(state.counters.tsc.u64 < p->counter[0] ||
       state.counters.pmc[0].u64 < p->counter[1] ||
       state.counters.pmc[1].u64 < p->counter[2]) {
      fprintf(stderr, "Counters running backwards!\n");
      exit(-1);
    }
#endif

#ifdef PERFCTR
    total.counter[0] += (state.counters.tsc.u64 - p->counter[0]);
    total.counter[1] += (state.counters.pmc[0].u64 - p->counter[1]);
    total.counter[2] += (state.counters.pmc[1].u64 - p->counter[2]);
#endif
#ifdef THREADS
    pthread_mutex_unlock(&mutex);
#endif

#ifdef PARANOID
#ifdef THREADS
    p->counter[0]=0;
#else
    start.counter[0] = 0;
#endif
#endif
}

#ifdef PERFMON
void startCountMS(){
  RDTSC(f_main);
}
void stopCountMS(){
  RDTSC(l_main);
  double temp=CLOCK_CYCLES(f_main,l_main);
  if(PMCforMainSink>temp)
    PMCforMainSink=temp;
  ++num_PMCforMainSink;
  f_main.hi=0;f_main.lo=0;
  l_main.hi=0;l_main.lo=0;
}
double getTotalCyclesMS() {
  return PMCforMainSink*num_PMCforMainSink;
}
#endif

void resetCount() {
    totalSamples=0;
#ifdef PERFCTR
    for(int i=0;i<3;i++) {
      total.counter[i]=0;
    }    
#else
    unsigned long long lld64=0xffffffff;
    lld64=(lld64<<8)+lld64;
    PMCCycle=lld64;
    num_PMCCycle=0;
    AwaitTCycle=lld64;
    num_AwaitTCycle=0;
    PMCforMainSink=lld64;
    num_PMCforMainSink=0;
#endif
    
    skippedData = bufferFullOnWrite = WPdelayedUpdate =
      blockOnUnmarkedData = uncompleteWork =
      blockOnMarkedData = blockOnSync = 0;
  }
double getTotalCycles() {
#ifdef PERFCTR 
    return total.counter[0];
#else
    return PMCCycle*num_PMCCycle;
#endif
  }
double getTotalCycles(int measure) {
#ifdef PERFCTR 
    if(measure < 3) return total.counter[measure];
    else return 0;
#else
    return PMCCycle*num_PMCCycle;
#endif
  }

  static char *measurementName(int n) {
    switch(n) {
    case 0:
      return "cycles";
    case 1:
      //      return PERF1_NAME; //hkim
    case 2:
      //     return PERF2_NAME; //hkim
    default:
      return NULL;
    }
  }
#ifdef PERFMON
void startAwaitTCount() {
  RDTSC(AwaitTfirst); //hkim
}

void stopAwaitTCount() {
  RDTSC(AwaitTlast);
  double temp= CLOCK_CYCLES(AwaitTfirst,AwaitTlast);
  if(AwaitTCycle>temp)
    AwaitTCycle=temp;
  ++num_AwaitTCycle;
  AwaitTfirst.hi=0;AwaitTfirst.lo=0;
  AwaitTlast.hi=0;AwaitTlast.lo=0;
}
void increaseNumTick(){
  ++num_AwaitTCycle;
}
  double getAwaitTCycles() {
    return AwaitTCycle;
  }
unsigned long long getNumAwaitTick() {
  return num_AwaitTCycle;
}
#endif

void updateSamples(int n) {totalSamples += n;}
unsigned long long getTotalSamples() {return totalSamples;}

void print_stats();
StrlCycleCount();
};

#endif //PERFMON
#endif
