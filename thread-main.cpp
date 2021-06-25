// ----------------------------------------------------------- 
// NAME : Eric Grant                    User ID: ewgrant 
// DUE DATE : 04/14/2021                                       
// Program Assignment 5
// FILE NAME : thread-main.cpp        
// PROGRAM PURPOSE :                                           
//      Simulate a scenario where santa takes reindeer in groups
//      to deliver presents and also helps elves with questions
//      Do so using monitors
// ----------------------------------------------------------- 

#include <iostream>
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

MyMonitor* monitor;

// ----------------------------------------------------------- 
// FUNCTION  main :                         
//    create threads and wait for santa to die
// PARAMETER USAGE :                                           
//    argv, argc    
//    argc contains:
//    e, number of elves
//    r, number of reindeer
//    t, number of runs that need to be made
// ----------------------------------------------------------- 
int main(int argv, char* argc[])
{
    //vars
    //
    if (argv != 4) {
        printf("useage ./prog5 <elves> <deer> <trips>\n");
        return 0;
    }

    int e = atoi(argc[1]);
    int r = atoi(argc[2]);
    int t = atoi(argc[3]);

    if (e == 0) {
        e = 7;
    }

    if (r == 0) {
        r = 9;
    }

    if (t == 0) {
        t = 5;
    }

    monitor = new MyMonitor(r, t, e);

    //Threads
    //
    SantaThread* santa;
    ElfThread* elves[e];
    DeerThread* deer[r];

    //spawn santa
    santa = new SantaThread();
    santa -> Begin();

    //spawn elves
    for (int i = 0; i < e; i++) {
        elves[i] = new ElfThread(i);
        elves[i] -> Begin();
    }

    //spawn deer
    for (int i = 0; i < r; i++) {
        deer[i] = new DeerThread(i);
        deer[i] -> Begin();
    }

    //wait for death
    santa -> Join();
}