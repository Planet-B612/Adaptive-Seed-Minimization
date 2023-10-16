#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <stdio.h>
// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

//for cusomer
#include <unistd.h>  //close open
#include <vector>
#include "sfmt/SFMT.h"
#include <fstream>
#include <unistd.h>
#include <string>
#include <time.h>
#include <cstdlib>
using namespace std;

int main(int argn, char **argv)
{
    int fd = open("/home/cfeng/graphInfo/Tested-Dataset/nethept/graph_lt.inf", O_RDWR);
    struct stat sb;
	int rc = fstat(fd, &sb);
    size_t length = sb.st_size;
    auto ptr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));  //byte by byte
    auto f = ptr;
	int gap = 2 * sizeof(int)+sizeof(double);
    for (int i = 0; i < 3; i++)
	{
		int a, b;
		double p;
		//int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
		memcpy(&a, f, sizeof(int));
		memcpy(&b, f + sizeof(int), sizeof(int));
		memcpy(&p, f + 2 * sizeof(int), sizeof(double));
		f += gap;
		cout<<a<<", "<<b<<"; "<<p<<endl;
		// in_edge[b].push_back(a);		
		// probT[b] = p;
	}
	
	rc = munmap(ptr, length);
	close(fd);


    // std::ifstream file;
    // file.open("/home/cfeng/graphInfo/Tested-Dataset/nethept/graph_ic.inf",O_RDWR);
    // if(!file)
	// {
	// 	cout<<"cannot open roots file."<<endl;
	// 	exit(1);
	// }
    // int u,v;
    // double p;
    // for(uint8_t i=0;i<4;i++)
    // {
    //     file>>u>>v>>p;
    //     cout<<u<<", "<<v<<"; "<<p<<endl;
    // }

    // __head_version = "v1";
    // OutputInfo info(argn, argv);
    
    // Run( argn, argv );
    //cout<<"Memory(MB) "<<getProcMemory()<<endl;
}