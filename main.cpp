#include<iostream>
#include<string>
#include"types.h"

using namespace std;

//no feedback, no divergence, no convergence
void build_nnn_network(Network *my_network)
{
	//Build the network
	my_network->addActor("a",0,0);
	my_network->addActor("b",0,1);
	my_network->addActor("c",0,2);
	my_network->addActor("d",0,3);

	my_network->connect(0,0,0,1,1,10,10);
	my_network->connect(0,1,0,2,1,8,8);
	my_network->connect(0,2,0,3,1,10,10);
	
	my_network->output(0,3,1,4,4);

}

//no feedback, no divergence, convergence
void build_nny_network(Network *my_network)
{
	//Build the network
	my_network->addActor("a",0,0);
	my_network->addActor("b",1,0);
	my_network->addActor("c",0,1);
	my_network->addActor("d",0,2);

	my_network->connect(0,0,0,1,1,10,10);
	my_network->connect(1,0,0,1,1,8,8);
	my_network->connect(0,1,0,2,1,10,10);
	
	my_network->output(0,2,1,4,4);

}
//no feedback, divergence, no convergence
//no feedback, divergence, convergence
//feedback, no divergence, no convergence
//feedback, no divergence, convergence
//feedback, divergence, no convergence
//feedback, divergence, convergence



int main()
{
	Network my_network;

	build_nnn_network(&my_network);



	//Ungated run
	my_network.feed_input(0,0,0,50000);
	cout << "\n\nRunning without gating\n"; 
	//run for 100 iterations
	for(int i=0;i<100;i++)
	{
		
		my_network.feed_input(0,0,0,50000);
		//run for 1000 cycles
		my_network.run(100000,NONE);
		//soft reset to update counters
		my_network.soft_reset();
	}
	my_network.print_statistics();
	my_network.hard_reset();



	//Mean gating run
	my_network.feed_input(0,0,0,50000);
	//Calculate gating rates
	cout << "\n\nCalculating MEAN gating times:\n"; 
	my_network.calc_gating(MEANS);
	cout << "\n\nRunning with gating\n";
	//run for 100 iterations
	for(int i=0;i<100;i++)
	{
		
		my_network.feed_input(0,0,0,50000);
		//run for 1000 cycles
		my_network.run(100000,MEANS);
		//soft reset to update counters
		my_network.soft_reset();
	}
	my_network.print_statistics();
	my_network.hard_reset();



	//KL gating run
	my_network.feed_input(0,0,0,50000);
	//Calculate gating rates
	cout << "\n\nCalculating KL gating times:\n"; 
	my_network.calc_gating(KL);
	cout << "\n\nRunning with gating\n";
	//run for 100 iterations
	for(int i=0;i<100;i++)
	{
		
		my_network.feed_input(0,0,0,50000);
		//run for 1000 cycles
		my_network.run(100000,KL);
		//soft reset to update counters
		my_network.soft_reset();
	}
	my_network.print_statistics();

	/*
	Data collection:

	

	*/


	return 0;
}
