#pragma once
//#include <string>
using namespace std;

#include <time.h>

class Debugging
{
public:
	Debugging(void);
	~Debugging(void);

	static void Log(const char *filename, const char *txt)
	{
			FILE *f= fopen(filename, "at");
			if (f == NULL) return;

			char buf[100];
			_strtime(buf);
			//string out = buf;
			strcat(buf," ");
			strcat(buf,txt);
						
			fputs(buf, f);			
			fflush(f);
			fclose(f);
	}
};
