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
void Network::connect(int i, int j, int k, int l, int a_lat, int i_lat, int o_lat)
{
	Action *act;
	if((act_array[i][j]==NULL) || (act_array[k][l]==NULL))
	{
		cout << "Error: attempting to connect empty actors\n";
		return;
	}
	act = new Action(a_lat,i_lat,o_lat,act_array[k][l],act_array[k][l]->get_free_port());
	act_array[i][j]->addAction(act);
}

//Creates a new connection to an output port
void Network::output(int i, int j, int a_lat, int i_lat, int o_lat)
{
	Action *act;
	if(act_array[i][j]==NULL)
	{
		cout << "Error: attempting to connect empty actor\n";
		return;
	}
	act = new Action(a_lat,i_lat,o_lat,&(output_counter[iter_number]));
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
				cout << "Input actor " << act_array[i][j]->get_name() << "\n";
				compute_forward_ratio(act_array[i][j],1.0);
			}
		}
	}
}

void Network::compute_forward_ratio(Actor *a, double prop_ratio)
{
	double a_throughput_out;
	double b_throughput_in;

	double gating_ratio;

	int runtime, gatetime;

	//we're just using index 0 for now

	cout << "Computing actor " << a->get_name() << "\n";

	if((a->get_target(0))!=NULL)
	{
		
		a_throughput_out=a->get_production_rate(0);
		b_throughput_in=(a->get_target(0))->get_consumption_rate(0);

		

		//Does B consume more quickly?
		if(b_throughput_in > a_throughput_out)
		{
			//gate B, and move to next actor
			gating_ratio = compute_gating_ratio(a_throughput_out, b_throughput_in);
			
			//updated with propagated forward ratio from previously gated actors
			gating_ratio = gating_ratio * prop_ratio;

			//Set actor B run and gate times
			runtime=(int)(1000/gating_ratio);
			gatetime=1000-runtime;

			cout << "Computed runtime " << runtime << " gatetime " << gatetime <<"\n";


			(a->get_target(0))->set_runtime(runtime);
			(a->get_target(0))->set_gatetime(gatetime);

			//Move on to next actor pair
			compute_forward_ratio((a->get_target(0)),gating_ratio);
		}
		else
		{
			
			//Does A produce more quickly?
			if(b_throughput_in < a_throughput_out)
			{
				//gate A, and propagate back to previous actors
			}
			//if they're the same	
			else
			{
				(a->get_target(0))->set_runtime(1000);
				(a->get_target(0))->set_gatetime(0);
				//Move on to next actor pair
				compute_forward_ratio((a->get_target(0)),gating_ratio);
			}
		}
	}	
	else
	{
		//This is terminal actor, just return
		cout << "Output actor " << a->get_name() << "\n";
		return;
	}
}

//This is the function where the different metrics are applied
//Always assumes B is greater than A, caller must use accordingly
double Network::compute_gating_ratio(double a, double b)
{
	
	if(a >= b)
	{
		cout << "Calculation error\n";
		return 1.0;
	}
	//just mean ratio
	return b/a;
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




