#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;


class Action;
class Actor;

//We initialize each queue with a large value
//so input ports will have enough tokens to begin with
//when actions are connected to feed ports (i.e., they are not network inputs)
//the queue is cleared
InPort::InPort()
{
	queue = 100000;
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

void InPort::clearTokens()
{
	queue=0;
}
	
Action::Action(int l, int in, int out, Actor *a, int p)
{
	target=a;
	port=p;

	//clear tokens on corresponding port, unless it's network output
	if(target)
		target->clearTokens(p);

	rdl = new std::random_device;
	rdI = new std::random_device;
	rdO = new std::random_device;
	genl = new std::mt19937((*rdl)());
	genI = new std::mt19937((*rdI)());
	genO = new std::mt19937((*rdO)());

	latency_gen = new std::poisson_distribution<>(l);
	tokensIn_gen = new std::poisson_distribution<>(in);
	tokensOut_gen = new std::poisson_distribution<>(out);
	//latency=(*latency_gen)(*genl);
	//Begin with 0 latency, i.e., action idle and ready
	latency=0;
	tokensIn=(*tokensIn_gen)(*genI);
	tokensOut=(*tokensOut_gen)(*genO);
}
	
//returns TRUE if latency is 0
bool Action::finished()
{
	if(latency==0)
	{
		//cout << "Finished\n";
		return true;
	}
	else
	{
		//cout << "Not finished: latency " << latency << "\n";
		return false;
	}
}
//decreases latency
//when it reaches 0 (action finished)
//output tokens
void Action::decrease()
{
	latency--;
	if(latency==0)
	{
		//If target is NULL, then it's output port
		if(target)
		{
			//Feed tokens to its corresponding input port
			target->addTokens(port,tokensOut);
			cout << "Added " << tokensOut << " tokens to next actor\n";
		}
		else
		{
			cout << "Output " << tokensOut << "\n"; 
		}
	}
}

//returns number of required input tokens
int Action::requiredTokens()	
{
	//cout << "Require " << tokensIn << " tokens\n";
	return tokensIn;
}

void Action::trigger()
{
	
	latency=(*latency_gen)(*genl);
	tokensIn=(*tokensIn_gen)(*genI);
	tokensOut=(*tokensOut_gen)(*genO);
}	


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

Network::Network()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
			act_array[i][j]=NULL;
	}
}
//Creates a new actor at position i,j
void Network::addActor(string n, int i, int j)
{
	if(act_array[i][j])
	{
		cout << "Error: actor re-definition\n";
		return;
	}
	act_array[i][j] = new Actor(n);
}
//Creates a new connection between two actors at positions i,j and k,l
//i.e., creates a new action within actor i,j which outputs to actor k,l
//action latencies are default for now, will be updated
void Network::connect(int i, int j, int k, int l)
{
	Action *act;
	if((act_array[i][j]==NULL) || (act_array[k][l]==NULL))
	{
		cout << "Error: attempting to connect empty actors\n";
		return;
	}
	act = new Action(1,1,1,act_array[k][l],act_array[k][l]->get_free_port());
	act_array[i][j]->addAction(act);
}

//Creates a new connection to an output port
void Network::output(int i, int j)
{
	Action *act;
	if(act_array[i][j]==NULL)
	{
		cout << "Error: attempting to connect empty actor\n";
		return;
	}
	act = new Action(1,1,1,NULL,0);
	act_array[i][j]->addAction(act);
}

//runs the network for fixed iterations
void Network::run()
{
	int iterations;
	//As we start running, network input ports have 100000 tokens each
	//all other ports are empty

	//We want each actor to run once at every scheduling point

	cout << "Starting network run.....\n";

	for(iterations=100;iterations>0;iterations--)
	{
		print_state();
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<10;j++)
			{
				if(act_array[i][j])
					act_array[i][j]->run();
			}
		}
	}

	cout << "Finished network run\n";
}

void Network::print_statistics()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(act_array[i][j])
				cout << "Actor " << i << "," << j << " idle for " << act_array[i][j]->get_idle_time() << " cycles\n";
		}
	}
}

void Network::print_state()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(act_array[i][j])
			{
				//number of available input tokens
				//number of required input tokens
				//running or idle
				for(int k=0;k<4;k++)
				{
					//cout << "dbg " << act_array[i][j]->get_action_number();
					if(k<(act_array[i][j]->get_action_number()))
						cout << "in (" << act_array[i][j]->peekTokens(k) << ") req (" << act_array[i][j]->get_required(k) << ") ";
				}
				if(act_array[i][j]->last_state())
					cout <<  "IDLE\t";
				else
					cout << "RUNNING\t";
			}
		}
		cout << "\n";
	}	
}






















