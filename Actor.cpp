#include<iostream>
#include<string>


using namespace std;

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

	public:
};

class Actor
{
	private:
	String name;


	//Number of input ports is limited to 4, as that suffices for our experiments
	//Can be updated to allow for arbitrary limit in the future
	InPort ports[4];
	Action* actions[4];

	public:

	Actor(String n)
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
			return ports[p].peekTokens(t);
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
				if(actions[i].ready())
				{
					//Ready to run, latency is 0
					if(peekTokens(i) >= actions[i].requiredTokens())
					{
						//Has enough tokens to trigger
						//get from input port
						subTokens(i,actions[i].requiredTokens());	
						//trigger action
						actions[i].trigger();
					}
				}
				else
				{
					//Not ready to run, decrease latency
					actions[i].decrease();
				}
			}
		}
	}
};








