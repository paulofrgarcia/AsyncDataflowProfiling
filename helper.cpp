#include<cmath>

using namespace std;



int calculateMean(int data[])
{
	int sum = 0, mean;

	for(int i = 0; i < 100; ++i)
	{
		sum += data[i];
	}

	mean = sum/100;

	return mean;
}
