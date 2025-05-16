#pragma once
#include "asm_iheap.h"
#include <queue>	//priority_queue
#include <utility>  // pair
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include "sfmt/SFMT.h"
using namespace std::chrono;
//using namespace std;

class InfGraph: public Graph
{
private:
    vector<bool> visit;
    vector<int> visit_mark;
public:
    vector<vector<int>> hyperG;
	vector<vector<int>> hyperG_1;
    vector<vector<int>> hyperGT;
    vector<vector<int>> hyperGT_1;	
	vector<vector<int>>PO;
	vector<bool>active_set;
	deque<int> q;
	sfmt_t sfmtSeed;
	vector<int> seedSet;
	int numRRsets;

    InfGraph(string folder, string graph_file): Graph(folder, graph_file)
    {
        srand(time(NULL));
        sfmt_init_gen_rand(&sfmtSeed , rand());
        visit = vector<bool> (n);
        visit_mark = vector<int> (n);
		
		hyperG.resize(n, vector<int>());
		hyperG_1.resize(n, vector<int>());
		PO.resize(n, vector<int>());
		
		//node_influence = vector<int>(n, 0);	
		numRRsets = 0;	
		active_set = vector<bool>(n, false);
    }

    void init_hyper_graph()
	{		
		for (unsigned int i = 0; i < hyperG.size(); ++i)hyperG[i].clear();
		for (unsigned int i = 0; i < hyperGT.size(); ++i)vector<int>().swap(hyperGT[i]);
		for (unsigned int i = 0; i < hyperG_1.size(); ++i)hyperG_1[i].clear();
		for (unsigned int i = 0; i < hyperGT_1.size(); ++i)vector<int>().swap(hyperGT_1[i]);
		hyperGT.clear();		
		hyperGT_1.clear();
		seedSet.clear(); 				
		fill(active_set.begin(), active_set.end(), false);		
    }

	/// Genrerate a possible world, PO.
	void generate_possible_world(const Argument arg)
	{
		for (unsigned int i = 0; i < PO.size(); ++i)PO[i].clear();
		if(influModel==IC)
		{
			for(long unsigned int v=0;v<gT.size();v++)
			{
				for(long unsigned int u=0;u<gT[v].size();u++)
				{
					int node=gT[v][u];
					auto rand=sfmt_genrand_real1(&sfmtSeed);
					//double p = probT[v][node];
					double p = probT[v][u];
					if(rand<p)
					//if(sfmt_genrand_real1(&sfmtSeed)<probT[v][node])
					{
						PO[node].push_back(v);
					}
				}
			}
		}
		else
		{
			for(long unsigned int v=0;v<gT.size();v++)
			{
				if(gT[v].size()==0)
				{
					continue;
				}
				int index = sfmt_genrand_uint32(&sfmtSeed) % gT[v].size();
				int u = gT[v][index];
				PO[u].push_back(v);
			}
		}

		// ifstream inFile((graph_file).c_str());
        // if(!inFile)
	    // {
        //     cout<<"cannot open roots file."<<endl;
        //     exit(1);
	    // }
        // inFile.seekg(0, std::ios_base::beg);
        // uint32_t u,v;
        // //double p;
        // //inFile>>n>>m;
		// inFile.ignore(10000, '\n'); // ignore the first line which records n and m
        // //gT = vector<vector<uint32_t>>(n, vector<uint32_t>());		
		// //probT = vector<vector<double>>(n, vector<double>());
        // while(!inFile.eof())
        // {
        //     inFile>>u>>v;

        //     gT[v].push_back(u);
        //     if((folder=="nethept")||(folder=="epinions"))
        //     {
        //         inFile.ignore(10000, '\n');  // For well-formed dataset, this line may ignore the next line which is useful.
        //     }
        // }
	}
	
	//initialize PO each time it loads a possible world.
	void load_possible_world(string index, const Argument arg)
	{	
		//initialization
		for (unsigned int i = 0; i < PO.size(); ++i)PO[i].clear();

		//int index1 = 0;
		// while (arg.dataset[index1] != '/')index1++;
		// int index2 = ++index1;
		// while (arg.dataset[index2] != '/')index2++;
		//string file_name = arg.dataset + arg.dataset.substr(index1, index2 - index1) + "_" + index;
		string pw_path="/data/gongyao/realization/" + arg.dataset[arg.dataset_No] + "_pw_ic" + index + ".txt";
		cout << pw_path <<endl;
		// pw_path+="_pw_ic.txt";
		ifstream load_pw;
		load_pw.open(pw_path);
		uint32_t i, nbr;
		while(!load_pw.eof())
		{
			load_pw>>i>>nbr;
			PO[i].push_back(nbr);
		}
		PO[i].pop_back();  // the last row is empty, due to the mechanism of eof, a duplicated nbr will be added. Thus, we need to pop_back here

		// string a = dataset[arg.dataset_No];
		// auto len=a.length();
		// //cout<<"str_length "<<len<<"  substr "<<arg.dataset.substr(0, len-1)<<endl;
		// string file_name = "/home/cfeng/graphInfo/Tested-Dataset/"+dataset[arg.dataset_No] +"/"+ dataset[arg.dataset_No].substr(0,len) + "_" + index;
		// //string file_name = "src/Tested-Dataset/"+arg.dataset + arg.dataset.substr(0,len-1) + "_" + "99";

		// if (influModel == LT)file_name += "_lt";		

		// size_t length;
		// auto ptr = map_file(file_name.c_str(), length);
		// auto f = ptr;
		// auto tep = f;
		// char buffer[256];

		// auto l = f + length;

		// unsigned int t1, t2;

		// while (f && f != l)
		// {
		// 	if ((f = static_cast<char*>(memchr(f, '\n', l - f))))
		// 	{
		// 		memset(buffer, 0, sizeof(buffer));
		// 		memcpy(buffer, tep, f - tep);
		// 		f++;
		// 		tep = f;
		// 		stringstream s;
		// 		s << buffer;
		// 		s >> t1 >> t2;
		// 		ASSERT(t1 < n);
		// 		ASSERT(t2 < n);

		// 		PO[t1].push_back(t2);
		// 	}
		// }
		// munmap(ptr, length);
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

    void build_TRR_r(double R, int root_num, double prob)
    {
		//cout<<"building mRR"<<endl;
        if( R > INT_MAX ){
            cout<<"Error:R too large"<<endl;
            exit(1);
        }        
		unsigned int counter = 0;
		auto cur_RR_num= hyperGT.size();
		while (counter < R)
		{
			BuildTRR(cur_RR_num+counter, root_num, prob, hyperG, hyperGT);
			BuildTRR(cur_RR_num+counter, root_num, prob, hyperG_1, hyperGT_1);
			++counter;
		}
		numRRsets = hyperGT.size();
    }

	#include "asm_discrete_rrset.h"
 
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
  } // func: build_seedset()

	static bool comp(pair<uint32_t, double> a, pair<uint32_t, double> b)
	{
		return a.second > b.second;
	} // sort in descending order.

	double max_ratio(int k, vector<int>&seed_set, const vector<float> cost, vector<vector<int>> &FR_set, vector<vector<int>> &RR_set)
	{
		vector<pair<uint32_t, double>> ratio;
		vector<bool> RR_mark(RR_set.size(), false);
		// vector<int> deduction(n, 0);
		int total_coverage=0;

		for(int i=0;i<k;i++)
		{
			for(uint32_t id=0;id<n;id++)
			{
				uint32_t node=id;
				uint32_t coverage=FR_set[node].size();
				for(auto RRid:FR_set[node])
				{
					if(RR_mark[RRid]) coverage--;
				}
				float node_cost=cost[node];
				ratio.push_back(make_pair(node, 1.0*coverage/node_cost));
			}
			std::sort(ratio.begin(),ratio.end(),comp);
			int seed=ratio[0].first;
			total_coverage=total_coverage+ratio[0].second*cost[seed];
			seed_set.push_back(seed);
			for(auto RRid:FR_set[seed])
			{
				RR_mark[RRid]=true;
			}
			ratio.clear();
		}
		return total_coverage;
	}

 }; //cls



