#include <algorithm>
#include <iostream>
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


//typedef unsigned long long int;

#define ASSERT(v) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; exit(1);}}
#define SELECT 1  //SELECT determines whether to treat readNM() as a function.

using namespace std;

void handle_error(const char* msg) {
	perror(msg);
	exit(255);
}

#if SELECT==0
void readNM(int &n, int& m)
{
	ifstream cin("attribute.txt");
	ASSERT(!cin == false);
	string s;
	while (cin >> s)
	{
		if (s.substr(0, 2) == "n=")
		{
			n = atoi(s.substr(2).c_str());
			continue;
		}
		if (s.substr(0, 2) == "m=")
		{
			m = atoi(s.substr(2).c_str());
			continue;
		}
		ASSERT(false);
	}	
	cin.close();
}
#endif



int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "Usage: ./pw dataset_name number" << endl;
		exit(0);
	}	

	int n, m;
	
	vector<string> dataset={"facebook", "sample", "nethept", "epinions", "dblp", "livejournal", "twitter", "orkut", "youtube", "pokec"};
	uint8_t dataset_No = atoi(argv[1]);
	string folder="/home/cfeng/graphInfo/Tested-Dataset/"+dataset[dataset_No];
	int num = atoi(argv[2]);

	sfmt_t sfmtSeed;
	//srand(95082);
	srand(time(NULL));
	//sfmt_init_gen_rand(&sfmtSeed, 95082);
	sfmt_init_gen_rand(&sfmtSeed, rand());

///-----------------------------------------------
#if SELECT==0
	readNM(n, m);
#endif

//string prefix=argv[1];

#if SELECT!=0
	
	string attr = folder +"/"+ "attribute.txt";
	//string attr = "src/Tested-Dataset/Epinions/attribute.txt";
	std::cout << "read file from " << attr << '\n';
	ifstream cin(attr);
	ASSERT(!cin == false);
	string s;
	while (cin >> s)
	{
		if (s.substr(0, 2) == "n=")
		{
			n = atoi(s.substr(2).c_str());
			continue;
		}
		if (s.substr(0, 2) == "m=")
		{
			m = atoi(s.substr(2).c_str());
			continue;
		}
		ASSERT(false);
	}	
	cin.close();
#endif


	vector<vector<int>> in_edge(n, vector<int>());	

	size_t length;
	string graph_file = folder +"/"+"graph_ic.inf";
	//cout<<graph_file<<endl;
	int fd = open((graph_file).c_str(), O_RDWR);
	if (fd == -1)
		handle_error("open");
	struct stat sb;
	int rc = fstat(fd, &sb);
	if (rc == -1)
		handle_error("fstat");

	length = sb.st_size;
	auto ptr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));  //byte by byte
	auto f = ptr;
	

	int gap = 2 * sizeof(int)+sizeof(double);
	//ASSERT(fin != false);
	
	cout << n << " " << m << endl;
	for (int i = 0; i < m; i++)
	{
		int a, b;
		double p;
		//int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
		memcpy(&a, f, sizeof(int));
		memcpy(&b, f + sizeof(int), sizeof(int));
		memcpy(&p, f + 2 * sizeof(int), sizeof(double));
		f += gap;
		
		ASSERT(a < n);
		ASSERT(b < n);
		
		in_edge[b].push_back(a);				
		
	}	
	
	rc = munmap(ptr, length);
	close(fd);

	//IC
	for (int k = 0; k < num; k++)
	{
		string index = to_string(k);
		string outfile = folder+"/"+dataset[dataset_No] + "_" + index;
		//cout<<folder<<endl;
		//cout<<outfile<<endl;
		ofstream output(outfile);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < in_edge[i].size(); j++)
			{
				//if ((double)rand() / RAND_MAX < 1.0 / in_edge[i].size())output << in_edge[i][j] << " " << i << endl;
				if (sfmt_genrand_real1(&sfmtSeed) < 1.0 / in_edge[i].size()) output << in_edge[i][j] << " " << i << endl;
				//if ((double)rand() / RAND_MAX < probT[i][j])output << in_edge[i][j] << " " << i << endl;
				//if ((double)rand() / RAND_MAX < probT[i][j])output << in_edge[i][j] << " " << i << endl;				
			}
		}
				
		output.close();				
	}
	
	return 0;

}