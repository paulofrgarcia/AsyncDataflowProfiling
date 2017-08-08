#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;

	
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

Action::Action(int l, int in, int out, int *o)
{
	target=NULL;
	port=0;

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

	out_cnt = o;
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
			//cout << "Added " << tokensOut << " tokens to next actor\n";
		}
		else
		{
			//cout << "Output " << tokensOut << "\n"; 
			(*out_cnt)+=tokensOut;
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

