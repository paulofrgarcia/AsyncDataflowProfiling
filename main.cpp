#include<iostream>
#include<string>
#include"types.h"

using namespace std;


int main()
{
	Network my_network;

	my_network.addActor("",0,0);
	my_network.addActor("",0,1);
	my_network.addActor("",0,2);

	my_network.connect(0,0,0,1);
	my_network.connect(0,0,0,2);
	my_network.connect(0,1,0,2);

	my_network.run();


	/*
	Data collection:

	

	*/


	return 0;
}
