//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "header\exporter.h"
#include <iostream>

using namespace std;

int main( int argc, char const * argv[] )
	{
	int const code = aniexp::exporter(argc,argv);
	if(code)
		{
		cout << endl;
		cout << "Press ENTER to exit... ";
		cin.get();
		}
	return code;
	}
