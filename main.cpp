#include<iostream>
#include<string>
#include"types.h"

using namespace std;


int main()
{
	//Creates dataflow network
	Actor a("first");

	Action *act = new Action(1,1,1,NULL,0);

	a.addAction(act,0);
	a.run();
	a.run();
	a.run();
	a.run();



	//Other stuff



	return 0;
}
