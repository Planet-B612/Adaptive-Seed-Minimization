#include "iheap.h"
#include <queue>	//priority_queue
#include <utility>  // pair

#include <unordered_set>
#include <unordered_map>
#include <chrono>
using namespace std::chrono;

class InfGraph: public Graph
{
private:
    vector<bool> visit;
    vector<int> visit_mark;
public:
    vector<vector<int>> hyperG;
    vector<vector<int>> hyperGT;
		
	vector<vector<int>>PO;
	vector<bool>active_set;

	deque<int> q;
	sfmt_t sfmtSeed;
	vector<int> seedSet;


    InfGraph(string folder, string graph_file): Graph(folder, graph_file)
    {
        srand(time(NULL));
        sfmt_init_gen_rand(&sfmtSeed , rand());
        visit = vector<bool> (n);
        visit_mark = vector<int> (n);
		
		hyperG.resize(n, vector<int>());
		PO.resize(n, vector<int>());
		
		//node_influence = vector<int>(n, 0);		
		active_set = vector<bool>(n, false);
    }

    void init_hyper_graph(){		
		for (unsigned int i = 0; i < hyperG.size(); ++i)hyperG[i].clear();
		for (unsigned int i = 0; i < hyperGT.size(); ++i)vector<int>().swap(hyperGT[i]);
		hyperGT.clear();		
		seedSet.clear(); 				
		fill(active_set.begin(), active_set.end(), false);		
    }
	
	//initialize PO each time it loads a possible world.
	void load_possible_world(string index, const Argument & arg)
	{	
		//initialization
		for (unsigned int i = 0; i < PO.size(); ++i)PO[i].clear();

		//int index1 = 0;
		// while (arg.dataset[index1] != '/')index1++;
		// int index2 = ++index1;
		// while (arg.dataset[index2] != '/')index2++;
		//string file_name = arg.dataset + arg.dataset.substr(index1, index2 - index1) + "_" + index;

		auto len=arg.dataset.length();
		//cout<<"str_length "<<len<<"  substr "<<arg.dataset.substr(0, len-1)<<endl;
		string file_name = "src/Tested-Dataset/"+arg.dataset + arg.dataset.substr(0,len-1) + "_" + index;
		//string file_name = "src/Tested-Dataset/"+arg.dataset + arg.dataset.substr(0,len-1) + "_" + "99";

		if (influModel == LT)file_name += "_lt";		

		size_t length;
		auto ptr = map_file(file_name.c_str(), length);
		auto f = ptr;
		auto tep = f;
		char buffer[256];

		auto l = f + length;

		unsigned int t1, t2;

		while (f && f != l)
		{
			if ((f = static_cast<char*>(memchr(f, '\n', l - f))))
			{
				memset(buffer, 0, sizeof(buffer));
				memcpy(buffer, tep, f - tep);
				f++;
				tep = f;
				stringstream s;
				s << buffer;
				s >> t1 >> t2;
				ASSERT(t1 < n);
				ASSERT(t2 < n);

				PO[t1].push_back(t2);
			}
		}
		munmap(ptr, length);
	}
	
	char* map_file(const char* fname, size_t& length)
	{
		int fd = open(fname, O_RDONLY);
		cout<<"fname in map_file "<<fname<<endl;
		if (fd == -1)
			handle_error("open");

		// obtain file size
		struct stat sb;
		if (fstat(fd, &sb) == -1)
			handle_error("fstat");

		length = sb.st_size;

		char* addr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
		if (addr == MAP_FAILED)
			handle_error("mmap");

		// TODO close fd at some point in time, call munmap(...)
		close(fd);
		//cout<<"finished loading possible world"<<endl;
		return addr;
	}

    void build_TRR_r(uint64 R, int root_num, double prob)
    {
		//cout<<"building mRR"<<endl;
        if( R > INT_MAX ){
            cout<<"Error:R too large"<<endl;
            exit(1);
        }        
		unsigned int counter = 0;
		while (counter < R)BuildTRR(counter++, root_num, prob);		
    }

    #include "discrete_rrset.h"
	
 
  double build_seedset(unsigned int k, vector<int>&batch_set)
  {	  
	//cout<<"begin building seed set "<<endl;
	vector<int>marginal_gain(n, 0);
	int tep = 0;
	for (unsigned int i = 0; i < n; ++i)
	{		
		marginal_gain[i] = hyperG[i].size(); // initial marginal gain of each node.
		tep = tep> marginal_gain[i] ? tep : marginal_gain[i]; // find out the largest marginal_gain
	}

	vector<vector<int>>node_dis(tep + 1, vector<int>());
	for (unsigned int i = 0; i < n; ++i)node_dis[marginal_gain[i]].push_back(i);

	long long influence = 0;

	long long numEdge = hyperGT.size();
	
	// check if an edge is removed
	vector<bool> edgeMark(numEdge, false);
	// check if an node is remained in the heap
	vector<bool> nodeMark(n + 1, true); //actually, no use; what is the use of this?	

	while (batch_set.size() < k && numEdge > 0)
	{
		int index = node_dis.size() - 1;
		int maxInd = -1;
		for (; index > 0; --index)
		{
			if (node_dis[index].size() == 0)
			{
				node_dis.pop_back();  //this might damage the performance of this program
				continue;
			}
			for (int j = node_dis[index].size() - 1; j >= 0; --j)
			{
				if (marginal_gain[node_dis[index][j]] != index)
				{
					node_dis[marginal_gain[node_dis[index][j]]].push_back(node_dis[index][j]);
					node_dis[index].pop_back();
					continue;
				}
				maxInd = node_dis[index][j];
				break;
			}
			if (maxInd == -1)continue;			

			batch_set.push_back(maxInd);

			node_dis[index].pop_back(); 

			influence += marginal_gain[maxInd];

			marginal_gain[maxInd] = 0;
			nodeMark[maxInd] = false;
						
			for (auto index : hyperG[maxInd])
			{
				if (hyperGT[index].size() == 0 || edgeMark[index])continue;				
				for (auto node : hyperGT[index])
				{
					if (nodeMark[node])--marginal_gain[node];
				}
				edgeMark[index] = true;
				--numEdge;
			}			
			break;
		}
	}
	if (batch_set.size() < k)
	{
		int node = sfmt_genrand_uint32(&sfmtSeed) % n;
		while (batch_set.size() < k)
		{
			while (active_set[node])node = sfmt_genrand_uint32(&sfmtSeed) % n;
			batch_set.push_back(node);	//we might select the same node more than once		
		}
	}
	return 1.0*influence;
  }
 };



