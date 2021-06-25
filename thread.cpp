// ----------------------------------------------------------- 
// NAME : Eric Grant                    User ID: ewgrant 
// DUE DATE : 04/14/2021                                       
// Program Assignment 5
// FILE NAME : thread.cpp        
// PROGRAM PURPOSE :                                           
//      Simulate a scenario where santa takes reindeer in groups
//      to deliver presents and also helps elves with questions
//      Do so using monitors
// ----------------------------------------------------------- 

#include "thread.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

extern MyMonitor* monitor;
static Semaphore talk("talk",1);

//Monitor Code
//

// ----------------------------------------------------------- 
// FUNCTION  MyMonitor :                       
//     instantiate monitor                            
// PARAMETER USAGE :                                           
//    deer - total number of reindeer
//    trips - total trips needed to take
//    elves - total number of elves                  
// ----------------------------------------------------------- 
MyMonitor::MyMonitor(int deer, int trips, int elves) : 
    Monitor("myMonitor", HOARE),
    totalDeer(deer),
    totalTrips(trips),
    elvesWaiting(0),
    elfWaitList(new int[3]),
    deerHome(0),
    trips(0),
    groupPicked(0),
    elvesWithSanta(0),
    elvesWithSantaList(new int[3]),
    sleeping(new Condition("sleeping")),
    frontElves(new Condition("frontElves")),
    backElves(new Condition("backElves")),
    fullTeam(new Condition("fullTeam")),
    sleighSet(new Condition("sleighSet")),
    deerAtWork(new Condition("deerAtWork"))
{
    
}

//Santa
//
// ----------------------------------------------------------- 
// FUNCTION  OpenDoor :                       
//     Print that santa is helping elves                  
// ----------------------------------------------------------- 
void MyMonitor::OpenDoor(){
    MonitorBegin();

    talk.Wait();
    printf("Santa is helping elves\n");
    talk.Signal();

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  SendOutElves :                       
//     print that santa has helped evles and free them                            
// ----------------------------------------------------------- 
void MyMonitor::SendOutElves(){
    MonitorBegin();

    talk.Wait();
    printf("Santa answers the question posted by elves %d, %d, %d\n", elfWaitList[0]+1, elfWaitList[1]+1, elfWaitList[2]+1);
    talk.Signal();

    //let out the three elves waiting in front
    frontElves->Signal();
    frontElves->Signal();
    frontElves->Signal();

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  ElvesWoke :                       
//     return if elves are ready               
// ----------------------------------------------------------- 
int MyMonitor::ElvesWoke(){
    MonitorBegin();

    int temp = 0;
    if (elvesWaiting == 3) {
        temp = 1;
    }

    MonitorEnd();
    return temp;
}

// ----------------------------------------------------------- 
// FUNCTION  DeerWoke :                       
//     return if deer are ready                  
// ----------------------------------------------------------- 
int MyMonitor::DeerWoke(){
    MonitorBegin();

    int temp = 0;
    if (deerHome == totalDeer) {
        temp = 1;
    }

    MonitorEnd();
    return temp;
}

// ----------------------------------------------------------- 
// FUNCTION  GatherDeer :                       
//     signal that deer are all gathered            
// ----------------------------------------------------------- 
void MyMonitor::GatherDeer() {
    MonitorBegin();

    for (int i = 0; i < totalDeer; i++) {
        fullTeam->Signal();
    }

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  ReadySleigh :                       
//      signal that sleigh is ready for deer        
// ----------------------------------------------------------- 
void MyMonitor::ReadySleigh() {
    MonitorBegin();

    talk.Wait();
    printf("Santa is preparing sleighs\n");
    talk.Signal();

    for (int i = 0; i < totalDeer; i++) {
        sleighSet->Signal();
    }

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  TeamGoesOut :                       
//     print that santa has flown out and increase trip num          
// ----------------------------------------------------------- a
void MyMonitor::TeamGoesOut() {
    MonitorBegin();

    trips++;

    talk.Wait();
    printf("The team flies off (%d)!\n", trips);
    talk.Signal();

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  TeamGetsBack :                       
//     signal that deer have returned from delivery and
//     can now go on vacation                           
// ----------------------------------------------------------- 
void MyMonitor::TeamGetsBack() {
    MonitorBegin();

    for (int i = 0; i < totalDeer; i++) {
        deerAtWork->Signal();
    }

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  Sleep :                       
//     tell santa to sleep if no one needs him 
// ----------------------------------------------------------- 
void MyMonitor::Sleep(){
    MonitorBegin();

    if (!(elvesWaiting == 3 || deerHome == totalDeer)) {
        sleeping->Wait();
    }

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  CheckIfRetire:                       
//     Check if it is time to retire                
// ----------------------------------------------------------- 
void MyMonitor::CheckIfRetire(){
    MonitorBegin();

    if (totalTrips == trips) {
        talk.Wait();
        printf("After (%d) deliveries, Santa retires and is on vacation!\n", trips);
        talk.Signal();
        Exit();
    }

    MonitorEnd();
}

//Elf
//
// ----------------------------------------------------------- 
// FUNCTION  AskQuestion :                       
//    have elf ask question then wait for santa to answer
//    if third elf to ask then wake santa           
// PARAMETER USAGE :                                           
//    index - elf index              
// ----------------------------------------------------------- 
void MyMonitor::AskQuestion(int index){
    MonitorBegin();

    //announce elf has problem
    talk.Wait();
    printf("          Elf %d has a problem\n", index+1);
    talk.Signal();

    //if three elves grouped in front, wait before getting in line
    if (groupPicked) {
        backElves->Wait();
    }

    //declare they are waiting in line
    elfWaitList[elvesWaiting] = index;
    elvesWaiting++;

    //if enough elves wake santa, set group
    if (elvesWaiting == 3) {
        talk.Wait();
        printf("          Elves %d, %d, %d wake up the santa\n", elfWaitList[0]+1, elfWaitList[1]+1, elfWaitList[2]+1);
        talk.Signal();
        groupPicked = 1;
        sleeping->Signal();
    } else {
        backElves->Signal();
    }
        
    //wait for santa to help then move on
    frontElves->Wait();
    elvesWaiting--;

    //if last elf to move on, free the front group for anyone else waiting in the back
    if (elvesWaiting == 0) {
        talk.Wait();
        printf("          Elves %d, %d, %d return to work\n", elfWaitList[0]+1, elfWaitList[1]+1, elfWaitList[2]+1);
        talk.Signal();
        groupPicked = 0;
        backElves->Signal();
    }

    MonitorEnd();
}

//Deer
//
// ----------------------------------------------------------- 
// FUNCTION  FlyOff :                       
//     have deer wait till work is over then leave for vacation                  
// ----------------------------------------------------------- 
void MyMonitor::FlyOff(){
    MonitorBegin();

    deerHome--;
    deerAtWork->Wait();

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  ReindeerBack :                       
//    signal that reindeer is home at north pole and ready to work        
// PARAMETER USAGE :                                           
//    index - reindeer index               
// ----------------------------------------------------------- 
void MyMonitor::ReindeerBack(int index){
    MonitorBegin();

    //post that a deer is home
    talk.Wait();
    printf("     Reindeer %d returns\n", index+1);
    talk.Signal();

    //declare a deer is home
    deerHome++;

    //if last deer wake up santa before waiting
    if (deerHome == totalDeer) {
        talk.Wait();
        printf("     The last reindeer %d wakes up santa\n", index+1);
        talk.Signal();
        sleeping->Signal();
    }

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION WaitOthers :                       
//    have deer wait for all other deer to be gathered by santa      
// PARAMETER USAGE :                                           
//    index - reindeer index               
// ----------------------------------------------------------- 
void MyMonitor::WaitOthers(int index){
    MonitorBegin();

    fullTeam->Wait();

    MonitorEnd();
}

// ----------------------------------------------------------- 
// FUNCTION  WaitSleigh :                       
//    have reindeer wait for sleigh to be ready                     
// ----------------------------------------------------------- 
void MyMonitor::WaitSleigh(){
    MonitorBegin();

    sleighSet->Wait();

    MonitorEnd();
}

//Thread Code
//

//Santa
//
SantaThread::SantaThread() {

}

// ----------------------------------------------------------- 
// FUNCTION  SantaThread::ThreadFunc :                       
//    have santa sleep until needed then help elves or
//    deliver presents as needed 
// ----------------------------------------------------------- 
void SantaThread::ThreadFunc()
{
    Thread::ThreadFunc();

    talk.Wait();
    printf("Santa thread starts\n");
    talk.Signal();

    int deer = 0;
    int elf = 0;

    while(1){
        monitor->Sleep();
        elf = monitor->ElvesWoke();
        deer = monitor->DeerWoke();
        if(deer){ //deer are ready to deliver toys
            monitor->GatherDeer();
            monitor->ReadySleigh();
            monitor->TeamGoesOut();
            Delay(50);
            monitor->TeamGetsBack();
            monitor->CheckIfRetire();
        }
        if(elf){ //elves have question
            monitor->OpenDoor();
            Delay(50);
            monitor->SendOutElves();
        }        
    }
}

//Elves
//
ElfThread::ElfThread(int i) : index(i){

}

// ----------------------------------------------------------- 
// FUNCTION  ElfThread::ThreadFunc :                       
//    have elves ask questions forever
// ----------------------------------------------------------- 
void ElfThread::ThreadFunc()
{
    Thread::ThreadFunc();

    talk.Wait();
    printf("          Elf %d starts\n", index+1);
    talk.Signal();

    while(1){
        Delay(100);
        monitor->AskQuestion(index);
        Delay(100);
    }
}

//Deer
//
DeerThread::DeerThread(int i) : index(i){

}

// ----------------------------------------------------------- 
// FUNCTION  DeerThread::ThreadFunc :                       
//    Have deer go through their standard work flow forever
// ----------------------------------------------------------- 
void DeerThread::ThreadFunc()
{
    Thread::ThreadFunc();

    talk.Wait();
    printf("     Reindeer %d starts\n", index+1);
    talk.Signal();

    while(1){
        Delay(300);
        monitor->ReindeerBack(index);
        monitor->WaitOthers(index);
        monitor->WaitSleigh();
        monitor->FlyOff();
        Delay(100);
    }
}