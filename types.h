#ifndef TYPES_H
#define TYPES_H

#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>

using namespace std;

class Port;
class Action;
class Actor;



class InPort
{
	private:
	int queue;

	public:
	InPort();
	void addTokens(int t);
	void subTokens(int t);
	int peekTokens();
	void clearTokens();
};

class Action
{
	private:
	int latency;
	int tokensIn;
	int tokensOut;
	Actor *target;
	int port;

	std::random_device *rdl;
	std::random_device *rdI;
	std::random_device *rdO;
	std::mt19937 *genl;
	std::mt19937 *genI;
	std::mt19937 *genO;
	std::poisson_distribution<> *latency_gen;
	std::poisson_distribution<> *tokensIn_gen;
	std::poisson_distribution<> *tokensOut_gen;

	public:
	Action(int l, int in, int out, Actor *a, int p);
	
	//returns TRUE if latency is 0
	bool ready();
	//decreases latency
	void decrease();

	//returns number of required input tokens
	int requiredTokens()	;

	void trigger();

};
class Actor
{
	private:
	string name;


	//Number of input ports is limited to 4, as that suffices for our experiments
	//Can be updated to allow for arbitrary limit in the future
	InPort ports[4];
	bool port_taken[4];
	Action* actions[4];

	public:

	Actor(string n);
	//Input ports management functions
	void addTokens(int p, int t);
	void subTokens(int p, int t);
	int peekTokens(int p);
	//connection functions
	//returns first free input port
	int get_free_port();

	//clears corresponding port queue
	void clearTokens(int p);

	//Adds a new action to slot "i";
	void addAction(Action *a);

	//This function is called at every scheduling cycle
	//For each action, checks if it is runnable: i.e., if poisson latency is 0
	//If yes, check if enough tokens to run it, then trigger: this generates new poisson latency
	//If not enough tokens, wait
	//If not runnable, decrease poisson latency
	void run();
};

class Network
{
	private:
	//This 10x10 structure is lazy, but really simplifies printing results
	//Can be updated to allow for arbitrarily large networks in the future
	Actor *act_array[10][10];

	public:
	Network();
	//Creates a new actor at position i,j
	void addActor(string n, int i, int j);
	//Creates a new connection between two actors at positions i,j and k,l
	//i.e., creates a new action within actor i,j which outputs to actor k,l
	//action latencies are default for now, will be updated
	void connect(int i, int j, int k, int l);
	//runs the network for fixed iterations
	void run();
};	





#endif
