#define HEAD_INFO
#include "sfmt/SFMT.h"
#include "head.h"
#include "MeasureM.h"
#include "memoryusage.h"

using namespace std;

void handle_error(const char* msg);

class Graph
{
public:    	
	unsigned int n, m;
    	
	vector<vector<int>> gT;	
	vector<vector<double>> probT;	

    enum InfluModel {IC, LT};
    InfluModel influModel;
    void setInfuModel(InfluModel p)
    {
        influModel = p;
        TRACE(influModel == IC);        
        TRACE(influModel == LT);        
    }

    string folder;
    //folder="src/"+folder;
    string graph_file;
    // void readNM()
    // {
    //     string attr = "src/Tested-Dataset/"+folder +"attribute.txt";
    //     cout<<"attr: "<<(attr).c_str()<<endl;
    //     ifstream cin((attr).c_str());
    //     ASSERT(!cin == false);
    //     string s;
    //     while (cin >> s)
    //     {
    //         if (s.substr(0, 2) == "n=")
    //         {
    //             n = atoi(s.substr(2).c_str());
    //             continue;
    //         }
    //         if (s.substr(0, 2) == "m=")
    //         {
    //             m = atoi(s.substr(2).c_str());
    //             continue;
    //         }
    //         ASSERT(false);
    //     }
    //     TRACE(n, m );
    //     cin.close();
    // }

    void readGraph()
    {
		size_t length;
        //graph_file="src/Tested-Dataset/"+graph_file;
		int fd = open((graph_file).c_str(), O_RDWR);
        cout<<"graph file: "<<(graph_file).c_str()<<endl;
		if (fd == -1)
			handle_error("open");
		struct stat sb;
		int rc = fstat(fd, &sb);
		if (rc == -1)
			handle_error("fstat");

		length = sb.st_size;
		auto ptr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));  //byte by byte
		auto f = ptr;

		int gap = 2 * sizeof(int) + sizeof(double);
        //ASSERT(fin != false);
        unsigned int readCnt = 0;
        for (unsigned int i = 0; i < m; i++)
        {
            readCnt ++;
            unsigned int u, v;
            double p;            
			memcpy(&u, f, sizeof(int));
			memcpy(&v, f + sizeof(int), sizeof(int));
			memcpy(&p, f + 2 * sizeof(int), sizeof(double));
			f += gap;            
           
            ASSERT( u < n );
            ASSERT( v < n );
           
			gT[v].push_back(u);
			probT[v].push_back(p);			
        }        
        
        ASSERT(readCnt == m);
		rc = munmap(ptr, length);
		close(fd);
        //fclose(fin);
    }

    Graph(string folder, string graph_file): folder(folder), graph_file(graph_file)
    {
        //readNM();
		
		gT = vector<vector<int>>(n, vector<int>());		
		probT = vector<vector<double>>(n, vector<double>());		

        readGraph();       
    }
};

double sqr(double t)
{
    return t * t;
}

void handle_error(const char* msg) 
{
	perror(msg);
	exit(255);
}

