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

	for(int i=0;i<100;i++)
	{
		idle[i]=0;
	}
	iter_number=0;

	was_idle=false;
	for(int i=0;i<4;i++)
	{
		actions[i]=NULL;
		port_taken[i]=false;
		previous[i]=NULL;
	}
	gated = false;

	runtime=1000;
	gatetime=0;

	current_time=0;
}

string Actor::get_name()
{
	return name;
}


//Return true if one of the actions connects to output port
bool Actor::is_output()
{
	for(int i=0;i<4;i++)
	{
		if(actions[i]->get_target()==NULL)
			return true;
	}
	return false;
}

Actor *Actor::get_target(int i)
{
	if(actions[i])
		return actions[i]->get_target();
	else
		return NULL;
}


//returns actor which targets input port "i"
Actor *Actor::get_previous(int i)
{
	return previous[i];
}

void Actor::set_previous(Actor *a,int p)
{
	previous[p] = a;
}





//returns consumption/production rates of action "i"
double Actor::get_production_rate(int i)
{
	if(actions[i])
		return actions[i]->get_production_rate();
	else
		return 0.0;
}
double Actor::get_consumption_rate(int i)
{
	if(actions[i])
		return actions[i]->get_consumption_rate();
	else
		return 0.0;
}

//Resets the actor to initial state 
//Also updates "iter_number"
void Actor::soft_reset()
{
	was_idle=false;
	gated = false;
	iter_number++;
	for(int i=0;i<4;i++)
	{
		ports[i].clearTokens();
	}
}

//Resets the actor to initial state completely
void Actor::hard_reset()
{
	was_idle=false;
	gated = false;
	for(int i=0;i<4;i++)
	{
		ports[i].clearTokens();
	}
	for(int i=0;i<100;i++)
	{
		idle[i]=0;
	}
	iter_number=0;
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
		idle[iter_number]++;
		was_idle=true;
	}
	else
		was_idle=false;
}

int Actor::get_idle_time()
{
	return calculateMean(idle);
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
	//cout << "gating...\n";
	gated=true;
	current_time=gatetime;
}
void Actor::ungate_actor()
{
	gated=false;
	current_time=runtime;
}

int Actor::get_runtime(){return runtime;}
int Actor::get_gatetime(){return gatetime;}
int Actor::get_current_time(){return current_time;}
void Actor::dec_current_time(){current_time--;}

void Actor::set_runtime(int t)
{
	runtime=t;
	current_time=runtime;
}
void Actor::set_gatetime(int t)
{
	gatetime=t;
}













