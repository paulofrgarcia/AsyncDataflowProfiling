double calculateSD(int data[])
{
	int sum = 0, mean;
	double standardDeviation = 0.0;

	for(int i = 0; i < 100; ++i)
	{
		sum += data[i];
	}

	mean = sum/100;

	for(i = 0; i < 100; ++i)
		standardDeviation += pow((double)(data[i] - mean), 2);

	return sqrt(standardDeviation / 100);
}

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
