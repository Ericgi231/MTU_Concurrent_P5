#include "ThreadClass.h"

//Monitors
//
class MyMonitor : public Monitor {
	public:
		MyMonitor(int deer, int trips, int elves);
		//Elf Functions
		void AskQuestion(int index);
		//Deer Functions
        void ReindeerBack(int index);
        void WaitOthers(int index);
        void WaitSleigh();
        void FlyOff();
		//Santa Function
		void Sleep();
		void OpenDoor();
		void SendOutElves();
		int ElvesWoke();
		int DeerWoke();
		void GatherDeer();
        void ReadySleigh();
        void TeamGoesOut();
        void TeamGetsBack();
		void CheckIfRetire();
	private:
		Condition* sleeping;
		Condition* fullTeam;
		Condition* sleighSet;
		Condition* frontElves;
		Condition* backElves;
		Condition* deerAtWork;
		int totalDeer;
		int totalTrips;
		int groupPicked;
		int elvesWaiting;
		int* elfWaitList;
		int elvesWithSanta;
		int* elvesWithSantaList;
		int deerHome;
		int trips;
};

//Threads
//
class ElfThread : public Thread {
	public:
		ElfThread(int i);
	private:
		int index;
		void ThreadFunc();
};

class DeerThread : public Thread {
	public:
		DeerThread(int i);
	private:
		int index;
		void ThreadFunc();
};

class SantaThread : public Thread {
	public:
		SantaThread();
	private:
		void ThreadFunc();
};