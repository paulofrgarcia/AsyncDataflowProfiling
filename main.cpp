#include<iostream>
#include<string>
#include"types.h"

using namespace std;


int main()
{
	Network my_network;

	//Build the network
	my_network.addActor("a",0,0);
	my_network.addActor("b",0,1);
	my_network.addActor("c",0,2);
	my_network.addActor("d",0,3);

	my_network.connect(0,0,0,1,1,10,10);
	my_network.connect(0,1,0,2,1,8,8);
	my_network.connect(0,2,0,3,1,10,10);
	
	my_network.output(0,3,1,4,4);


	//feed input
	my_network.feed_input(0,0,0,50000);

	//Calculate gating rates
	my_network.calc_gating();


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
	/*
	Data collection:

	

	*/


	return 0;
}
