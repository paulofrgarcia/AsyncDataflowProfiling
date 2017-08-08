#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;


Actor::Actor(string n)
{
	name=n;
	idle=0;
	was_idle=false;
	for(int i=0;i<4;i++)
	{
		actions[i]=NULL;
		port_taken[i]=false;
	}
	gated = false;
}

//returns first free input port
int Actor::get_free_port()
{
	for(int i=0;i<4;i++)
	{
		if(port_taken[i]==false)
		{
			port_taken[i]=true;
			return i;
		}
	}
	cout << "Error: attempted too many connections to actor " << name;
}
//peeks first free input port
int Actor::get_action_number()
{
	for(int i=0;i<4;i++)
	{
		if(actions[i]==NULL)
			return i;
	}
	return 4;
}

void Actor::clearTokens(int p)
{
	ports[p].clearTokens();
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
//Adds a new action 
void Actor::addAction(Action *a)
{
	for(int i=0;i<4;i++)
	{
		if(actions[i]==NULL)
		{
			actions[i]=a;
			return;
		}
	}
	cout << "Error: attempted to add too many actions to actor " << name;
}
	

//This function is called at every scheduling cycle
//For each action, checks if it is runnable: i.e., if poisson latency is 0
//If yes, check if enough tokens to run it, then trigger: this generates new poisson latency
//If not enough tokens, wait
//If not runnable, decrease poisson latency
void Actor::run()
{
	bool no_action=true;
	//cout << "Running actor " << name << "\n";
	for(int i=0;i<4;i++)
	{
		if(actions[i])
		{

			//If action is still running, decrease latency

			//If action not running, see if enough tokens are available
				//If yes, trigger

			//Else, do nothing
			if(!actions[i]->finished())
			{
				//Not finished, decrease latency
				actions[i]->decrease();
				no_action=false;
				continue;
			}
			//Action is not running
			if(peekTokens(i) >= actions[i]->requiredTokens())
			{
				//Has enough tokens to trigger
				//get from input port
				subTokens(i,actions[i]->requiredTokens());	
				//trigger action
				actions[i]->trigger();
				no_action=false;
			}
		}
	}
	if(no_action)
	{
		idle++;
		was_idle=true;
	}
	else
		was_idle=false;
}

int Actor::get_idle_time()
{
	return idle;
}

bool Actor::last_state()
{
	return was_idle;
}

int Actor::get_required(int p)
{
	if(actions[p])
		return actions[p]->requiredTokens();
	else
		return 0;
}

bool Actor::is_gated()
{
	return gated;
}

void Actor::gate_actor()
{
	gated=true;
}
void Actor::ungate_actor()
{
	gated=false;
}

















