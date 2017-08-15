#ifndef TYPES_H
#define TYPES_H

#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>

using namespace std;

class InPort;
class Action;
class Actor;
class Network;


//defines the gating strategy to apply
#define NONE	0
#define MEANS	1
#define	KL	2




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
	//we use this to actually simulate the action
	int latency;
	int tokensIn;
	int tokensOut;
	Actor *target;
	int port;
	//pointer to output counter
	int *out_cnt;

	//these are used to guide the gating strategy
	int g_l, g_in, g_out;

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
	Action(int l, int in, int out, int *o);
	
	//returns TRUE if latency is 0, i.e., action is finished
	bool finished();
	//decreases latency
	void decrease();

	//returns number of required input tokens for action to trigger
	int requiredTokens();

	void trigger();

	//returns consumption/production rates
	double get_production_rate();
	double get_consumption_rate();

	//returns the target
	Actor *get_target();
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
	Actor* previous[4];

	//record the number of cycles the actor was completely idle, i.e., no actions to fire
	//We'll run the network 100 times to get mean and standard variation for each result
	//hence, we'll use 100 token counters for gathering results
	int idle[100]; 
	int iter_number;

	//set if actor was idle in last run
	bool was_idle;

	//If set, actor is not scheduled to run
	bool gated;

	//gating controls
	int runtime;
	int gatetime;

	int current_time;

	public:

	Actor(string n);

	string get_name();


	void soft_reset();
	//Resets the actor to initial state completely
	void hard_reset();




	//Input ports management functions
	void addTokens(int p, int t);
	void subTokens(int p, int t);
	int peekTokens(int p);
	//connection functions
	//returns first free input port
	int get_free_port();
	int get_action_number();

	//clears corresponding port queue
	void clearTokens(int p);

	//Adds a new action to slot "i";
	void addAction(Action *a);

	//Check required tokens for action
	int get_required(int p);

	//Return true if one of the actions connects to output port
	bool is_output();

	//This function is called at every scheduling cycle
	//For each action, checks if it is runnable: i.e., if poisson latency is 0
	//If yes, check if enough tokens to run it, then trigger: this generates new poisson latency
	//If not enough tokens, wait
	//If not runnable, decrease poisson latency
	void run();


	//Gating functions
	void gate_actor();
	void ungate_actor();
	int get_runtime();
	int get_gatetime();
	int get_current_time();
	void dec_current_time();
	void set_runtime(int t);
	void set_gatetime(int t);


	//Data collection functions

	//returns number of idle cycles
	int get_idle_time();
	//returns true if actor was idle in last run
	bool last_state();
	//Check if gated
	bool is_gated();

	//returns target actor of action "i"
	Actor *get_target(int i);
	//returns actor which targets input port "i"
	Actor *get_previous(int i);
	//sets actor which targets input port "p"
	void set_previous(Actor *a,int p);

	//returns consumption/production rates of action "i"
	double get_production_rate(int i);
	double get_consumption_rate(int i);

};

class Network
{
	private:
	//This 10x10 structure is lazy, but really simplifies printing results
	//Can be updated to allow for arbitrarily large networks in the future
	Actor *act_array[10][10];
	bool is_input[10][10];

	//type of gating strategy to apply
	int calc_type;

	public:

	//We'll run the network 100 times to get mean and standard variation for each result
	//hence, we'll use 100 token counters for gathering results
	int output_counter[100];
	//this one counts which iteration we're currently running, to determine which ounter to use
	int iter_number;



	Network();

	//Resets the network to initial state, except for the metrics counters
	//Also updates "iter_number" so we'll start using a new counter
	void soft_reset();
	//Resets the network to initial state completely
	void hard_reset();

	//Creates a new actor at position i,j
	void addActor(string n, int i, int j);
	//Creates a new connection between two actors at positions i,j and k,l
	//i.e., creates a new action within actor i,j which outputs to actor k,l
	void connect(int i, int j, int k, int l, int a_lat, int i_lat, int o_lat);
	//Creates a new connection to an output port
	void output(int i, int j, int a_lat, int i_lat, int o_lat);
	//feeds tokens to a port (used for network input)
	void feed_input(int i, int j, int p, int t);
	//runs the network for a certain number of iterations using a gating strategy
	void run(int i, int strategy);
	
	//Debug/visualization
	void print_state();


	//determines gating rates for each actor accoridng to a strategy
	void calc_gating(int type);
	void compute_forward_ratio(Actor *a, double prop_ratio);
	void propagate_backward_ratio(Actor *a, double prop_ratio);
	double compute_gating_ratio(double a, double b);


	//Data collection
	void print_statistics();
};	

//Helper functions defined in "helper.cpp"
int calculateMean(int data[]);



#endif
