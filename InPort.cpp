#include<iostream>
#include<string>
#include<map>
#include<random>
#include<iomanip>
#include"types.h"

using namespace std;


InPort::InPort()
{
	queue = 0;
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

