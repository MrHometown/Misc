#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<chrono>

#ifndef NUM_THREADS
#define NUM_THREADS 5
#endif

using namespace std;


bool isPrime(int x){
	if(x%2 == 0) return false;

	for (int i = 2; i*i <= x; ++i){
		if(x % i == 0) return false;
	}
	return true;
}

struct ThreadData{
	int from;
	int to;
	int primes;

	ThreadData(){}
	ThreadData(int from, int to):from{from}, to{to}, primes{0}{}
};


void FindPrimes_singleThread(int from, int to){
	cout << "   SingleThread\t\t";
	cout.flush();

	int primeCount_singleThread = 0;
	auto begin = chrono::high_resolution_clock::now();


	//Run
	for (int i = from; i <= to; ++i){
		if(isPrime(i)) ++primeCount_singleThread;
	}

	//Time
	auto end = chrono::high_resolution_clock::now();
	auto time = end-begin;

	//Out
	cout <<time/chrono::milliseconds(1)/1000.0<<"s\t(" << primeCount_singleThread <<")\n";
}
void *FindPrimes_multiThread_Thread(void* threadarg){
	ThreadData* data = (ThreadData*) threadarg;

	//Run
	for (int i = data->from; i <= data->to; ++i){
		if(isPrime(i)) ++(data->primes);
	}

	pthread_exit(NULL);
}
void FindPrimes_multiThread(int from, int to){
	cout << "   MultiThread(" << NUM_THREADS << ")\t";
	cout.flush();

	//Setup
	pthread_t  threads 	  [NUM_THREADS];
	ThreadData threaddata [NUM_THREADS];
	int range = ((to-from)+.5)/NUM_THREADS;

	auto begin = chrono::high_resolution_clock::now();

	//Create threads
	int at = from;
	for (int i = 0; i < NUM_THREADS; ++i){
		//Thread Data
		threaddata[i] = ThreadData(at, at + range - 1);
		if(threaddata[i].to > to) threaddata[i].to = to;

		pthread_create(&threads[i], NULL, FindPrimes_multiThread_Thread, (void *) &threaddata[i]);

		at += range;
	}

	//Join threads
	int primeCount_multiThread {0};
	void* status;
	for (int i = 0; i < NUM_THREADS; ++i){
		pthread_join(threads[i], &status);
		primeCount_multiThread += threaddata[i].primes;
	}

	//Time
	auto end = chrono::high_resolution_clock::now();
	auto time = end-begin;

	//Out
	cout <<time/chrono::milliseconds(1)/1000.0<<"s\t(" << primeCount_multiThread <<")\n";
}


int main(){
	//Range
	int from = 1000000;
	int to = from + 11000000;

	cout << "==============================================\n";
	cout << "  Finding primes in range ["<<from<<';'<<to<<"]\n";
	cout << "----------------------------------------------\n";

	//Run
	FindPrimes_singleThread(from, to);
    FindPrimes_multiThread(from, to);

    cout << "==============================================\n";
    return 0;
}
