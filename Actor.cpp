#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;


class Action;
class Actor;


InPort::InPort()
{
	queue = 0;
}
void InPort::addTokens(int t)
{
	queue += t;
}
void InPort::subTokens(int t)
{
	queue -= t;
}
int InPort::peekTokens()
{
	return queue;
}

	
Action::Action(int l, int in, int out, Actor *a, int p)
{
	target=a;
	port=p;
	rdl = new std::random_device;
	rdI = new std::random_device;
	rdO = new std::random_device;
	genl = new std::mt19937((*rdl)());
	genI = new std::mt19937((*rdI)());
	genO = new std::mt19937((*rdO)());

	latency_gen = new std::poisson_distribution<>(l);
	tokensIn_gen = new std::poisson_distribution<>(in);
	tokensOut_gen = new std::poisson_distribution<>(out);
	latency=(*latency_gen)(*genl);
	tokensIn=(*tokensIn_gen)(*genI);
	tokensOut=(*tokensOut_gen)(*genO);
}
	
	//returns TRUE if latency is 0
bool Action::ready()
{
	if(latency==0)
	{
		cout << "Ready\n";
		return true;
	}
	else
	{
		cout << "Not ready: latency " << latency << "\n";
		return false;
	}
}
//decreases latency
void Action::decrease()
{
	latency--;
}

//returns number of required input tokens
int Action::requiredTokens()	
{
	cout << "Require " << tokensIn << " tokens\n";
	return tokensIn;
}

void Action::trigger()
{
	//If target is NULL, then it's output port
	if(target)
	{
		//Feed tokens to its corresponding input port
		target->addTokens(port,tokensOut);
	}
	else
	{
		cout << "Output " << tokensOut << "\n"; 
	}
	latency=(*latency_gen)(*genl);
	tokensIn=(*tokensIn_gen)(*genI);
	tokensOut=(*tokensOut_gen)(*genO);
}	


	Actor::Actor(string n)
	{
		name=n;
		for(int i=0;i<4;i++)
			actions[i]=NULL;
	}
	//Input ports management functions
	void Actor::addTokens(int p, int t)
	{
		if(p<4)
			ports[p].addTokens(t);
		else
			cout << "Error: request for non-existent port in actor " << name << "\n";
	}
	void Actor::subTokens(int p, int t)
	{
		if(p<4)
			ports[p].subTokens(t);
		else
			cout << "Error: request for non-existent port in actor " << name << "\n";
	}
	int Actor::peekTokens(int p)
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
	void Actor::addAction(Action *a,int i)
	{
		actions[i]=a;
	}
	

	//This function is called at every scheduling cycle
	//For each action, checks if it is runnable: i.e., if poisson latency is 0
	//If yes, check if enough tokens to run it, then trigger: this generates new poisson latency
	//If not enough tokens, wait
	//If not runnable, decrease poisson latency
	void Actor::run()
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









