#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;



Network::Network()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			act_array[i][j]=NULL;
			is_input[i][j]=false;
		}
	}
	for(int i=0;i<100;i++)
	{
		output_counter[i] = 0;
	}
	iter_number=0;
}

//Resets the network to initial state, except for the metrics counters
//Also updates "iter_number" so we'll start using a new counter
void Network::soft_reset()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(act_array[i][j])
				act_array[i][j]->soft_reset();
		}
	}
	iter_number++;
}
//Resets the network to initial state completely
void Network::hard_reset()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(act_array[i][j])
				act_array[i][j]->hard_reset();
		}
	}
	for(int i=0;i<100;i++)
	{
		output_counter[i] = 0;
	}
	iter_number=0;
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
	act = new Action(1,1,1,&(output_counter[iter_number]));
	act_array[i][j]->addAction(act);
}



void Network::feed_input(int i, int j, int p, int t)
{
	act_array[i][j]->addTokens(p,t);
	is_input[i][j]=true;
}







//runs the network for a certain number of iterations
void Network::run(int i, int strategy)
{
	int iterations;
	
	//We want each actor to run once at every scheduling point

	//cout << "Starting network run.....\n";

	for(iterations=i;iterations>0;iterations--)
	{
		//print_state();
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<10;j++)
			{
				if(act_array[i][j])
				{
					if(strategy==NONE)
					{
						act_array[i][j]->run();
					}
					else
					{
						if(!(act_array[i][j]->is_gated()))
						{
							//check if should gate
							act_array[i][j]->run();
							act_array[i][j]->dec_current_time();

							if(act_array[i][j]->get_current_time() == 0)
							{
								act_array[i][j]->gate_actor();
							}
						}	
						else
						{
							//check if should ungate
							act_array[i][j]->dec_current_time();

							if(act_array[i][j]->get_current_time() == 0)
							{
								act_array[i][j]->ungate_actor();
							}
						}
					}
				}
			}
		}
	}

	//cout << "Finished network run\n";
}


//This is the interesting data collection bit

void Network::print_statistics()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(act_array[i][j])
				cout << "Actor " << i << "," << j << " idle for " << act_array[i][j]->get_idle_time() << " cycles average\n";
		}
	}
	cout << "Total of " << calculateMean(output_counter) << " tokens output average\n";
}



//determines gating rates for each actor
void Network::calc_gating()
{
	//We won't worry about feedback for now, just feed forward
	//It's simpler, and it will reveal interesting things when we apply to feedback

	//We also ignore multiple paths on same actor for now



	//we start for every input port
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(is_input[i][j])
			{
				compute_forward_ratio(act_array[i][j]);
			}
		}
	}
}

void Network::compute_forward_ratio(Actor *a)
{
	double a_throughput_out;
	double b_throughput_in;

	//we're just using index 0 for now

	if((a->get_target(0))!=NULL)
	{
		
		a_throughput_out=a->get_production_rate(0);
		b_throughput_in=(a->get_target(0))->get_consumption_rate(0);
	}	
	else
	{
		//This is terminal actor, just return
		return;
	}
}





/////////////////DEBUG ONLY
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




