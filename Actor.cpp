#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>


using namespace std;


class Action;
class Actor;


class InPort
{
	private:
	int queue;

	public:
	InPort()
	{
		queue = 0;
	}
	void addTokens(int t)
	{
		queue += t;
	}
	void subTokens(int t)
	{
		queue -= t;
	}
	int peekTokens()
	{
		return queue;
	}
};

class Action
{
	private:
	int latency;
	int tokensIn;
	int tokensOut;
	Actor *target;

	std::random_device *rd;
	std::mt19937 *gen;
	std::poisson_distribution<> *latency_gen;
	std::poisson_distribution<> *tokensIn_gen;
	std::poisson_distribution<> *tokensOut_gen;

	public:
	Action(int l, int in, int out, Actor *a)
	{
		target=a;

		rd = new std::random_device;
		gen = new std::mt19937((*rd)());

		latency_gen = new std::poisson_distribution<>(l);
		tokensIn_gen = new std::poisson_distribution<>(in);
		tokensOut_gen = new std::poisson_distribution<>(out);

		latency=(*latency_gen)(*gen);
		tokensIn=(*tokensIn_gen)(*gen);
		tokensOut=(*tokensOut_gen)(*gen);
	}
	
	//returns TRUE if latency is 0
	bool ready()
	{
		if(latency==0)
			return true;
		else
			return false;
	}

	//decreases latency
	void decrease()
	{
		latency--;
	}

	//returns number of required input tokens
	int requiredTokens()	
	{
		return tokensIn;
	}

	void trigger()
	{}	

};

class Actor
{
	private:
	string name;


	//Number of input ports is limited to 4, as that suffices for our experiments
	//Can be updated to allow for arbitrary limit in the future
	InPort ports[4];
	Action* actions[4];

	public:

	Actor(string n)
	{
		name=n;
		for(int i=0;i<4;i++)
			actions[i]=NULL;
	}
	//Input ports management functions
	void addTokens(int p, int t)
	{
		if(p<4)
			ports[p].addTokens(t);
		else
			cout << "Error: request for non-existent port in actor " << name << "\n";
	}
	void subTokens(int p, int t)
	{
		if(p<4)
			ports[p].subTokens(t);
		else
			cout << "Error: request for non-existent port in actor " << name << "\n";
	}
	int peekTokens(int p)
	{
		if(p<4)
			return ports[p].peekTokens();
		else
		{
			cout << "Error: request for non-existent port in actor " << name << "\n";
			return 0;
		}
	}

	//Adds a new action to slot "i"
	void addAction(Action *a,int i)
	{
		actions[i]=a;
	}
	

	//This function is called at every scheduling cycle
	//For each action, checks if it is runnable: i.e., if poisson latency is 0
	//If yes, check if enough tokens to run it, then trigger: this generates new poisson latency
	//If not enough tokens, wait
	//If not runnable, decrease poisson latency
	void run()
	{
		for(int i=0;i<4;i++)
		{
			if(actions[i])
			{
				if(actions[i]->ready())
				{
					//Ready to run, latency is 0
					if(peekTokens(i) >= actions[i]->requiredTokens())
					{
						//Has enough tokens to trigger
						//get from input port
						subTokens(i,actions[i]->requiredTokens());	
						//trigger action
						actions[i]->trigger();
					}
				}
				else
				{
					//Not ready to run, decrease latency
					actions[i]->decrease();
				}
			}
		}
	}
};








