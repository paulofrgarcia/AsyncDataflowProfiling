#include<iostream>
#include<string>
#include"types.h"

using namespace std;


int main()
{
	//Creates dataflow network
	Actor a("first");
	Actor b("second");


	Action *act = new Action(1,1,1,&b,0);

	a.addAction(act);

	a.addTokens(0,100);


	a.run();
	b.run();
	a.run();
	a.run();
	a.run();
	a.run();
	a.run();
	a.run();
	a.run();



	//Other stuff



	return 0;
}
