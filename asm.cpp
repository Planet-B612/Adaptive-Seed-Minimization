#define HEAD_INFO

#include "sfmt/SFMT.h"
#include "asm_head.h"

class Argument{
public:
	double eta=0.1;		// the percentage of total node number taken as quota.
    double eta_delta=0.1;
    // unsigned int dataset_No=0;
    double epsilon=0.5;
    string model="IC";    
	unsigned int batch=1;
	string seedfile;
    int time=10;  //this is the number of times needed to measure the result.	
    int start_time=0; // the time to start the algorithm, default is 0.	
    //std::vector<string> dataset;
    int cost_type=0; // 0 for 0.01 degree, 1 for random cost, 2 for uniform cost
    //vector<string> dataset={"facebook", "sample", "nethept", "epinions", "dblp", "livejournal", "twitter", "orkut", "youtube", "pokec"};
    // vector<string> dataset={"facebook", "sample", "nethept", "epinions", "dblp", "livejournal", "twitter", "orkut", "youtube", "pokec"};
    vector<string> dataset = {"facebook", "dblp", "flickr","nethept","epinions", "youtube", "pokec", "orkut", "livejournal", "friendster","DBLP_sym","Youtube_sym","twitter","citeseer","Flickr_sym","wikitalk","wikitalkar"};
    int dataset_No= 4;
    int cur_dataset_No=0;

};

#include "asm_graph.h"
#include "asm_infgraph.h"
#include "asm.h"

void OutputSeedSetToFile(vector<int> seed_set, string seedfile)
{
    ofstream of;
    of.open(seedfile);
    for (int seed: seed_set)
    {
        of << seed << " ";
    }
	of << endl;
    of.close();
}

// void run_with_parameter(InfGraph &g, const Argument & arg)
// {         
// 	ASM::SeedMinimize(g, arg);

//     //INFO(g.seedSet);
//     OutputSeedSetToFile(g.seedSet, arg.seedfile);        
// }

void Run(int argn, char **argv)
{
    Argument arg;
    // string result_dir="backup.txt";
	// std::fstream result_bk(result_dir, ios::app);
	// ASSERT(!result_bk.fail());

    for (int i = 0; i < argn; i++)
    {
        if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
        {
            cout << "./imm -dataset *** -epsilon *** -k ***  -model IC|LT -seedfile *** -time *** -batch ***" << endl;
            return ;
        }	

		if (argv[i] == string("-dataset_No"))			
            arg.dataset_No = stoi(argv[i + 1]);
		if (argv[i] == string("-epsilon"))
			arg.epsilon = atof(argv[i + 1]);
		if (argv[i] == string("-eta"))
			arg.eta = atof(argv[i + 1]);		
		if (argv[i] == string("-model"))
			arg.model = argv[i + 1]== "LT" ? "LT" : "IC";
		if (argv[i] == string("-seedfile"))
			arg.seedfile = argv[i + 1];
		if (argv[i] == string("-batch"))
			arg.batch = atoi(argv[i + 1]);
        if (argv[i] == string("-start_time"))
			arg.start_time = atoi(argv[i + 1]);
		if (argv[i] == string("-time"))
			arg.time = atoi(argv[i + 1]);
        if (argv[i] == string("-cost_type"))
			arg.cost_type = atoi(argv[i + 1]);
    }
    ASSERT(arg.model == "IC" || arg.model == "LT");
    // for(auto k:arg.dataset_No)
    {
        auto k=arg.dataset_No;
        string graph_path = "/data/gongyao/graphInfo/"+arg.dataset[k]; 
        // for(auto i=0;i<5;i++)
        {           
            InfGraph g(arg.dataset[k], graph_path);  // should not be put above, since the mRR-sets should be cleared each time            
            if (arg.eta > 1.0) 
            {
                arg.eta = arg.eta/g.n;
                arg.eta_delta= arg.eta_delta/g.n;
            }
            arg.eta+= arg.eta_delta;
            arg.eta*= g.n;
            // result_bk<<"# The parameters are: Aiming eta="<<arg.eta<<", model="<<arg.model<<", epsilon_Inf="<<arg.epsilon<<", Rnd_cost="<<to_string(arg.Rnd_cost)<<", batch="<<arg.batch<<", time="<<arg.time<<endl;

            if (arg.model == "IC")
                g.setInfuModel(InfGraph::IC);
            else if (arg.model == "LT")
                g.setInfuModel(InfGraph::LT);
            else
                ASSERT(false);

            vector<float> cost(g.n);
            string cost_file;
            if(arg.cost_type==0)
            {
                cost_file="/data/gongyao/graphInfo/"+arg.dataset[k]+"_cost_Rand.txt";
                std::ifstream inFile;
                inFile.open(cost_file);
                if(!inFile)
                {
                    cout<<"cannot open the cost file at "<<cost_file<<endl;
                    exit(1);
                }
                float nodeCost;
                inFile.seekg(0, std::ios_base::beg);
                for(size_t i=0;i<g.n;i++)
                {
                    inFile>>nodeCost;
                    cost[i]=nodeCost;
                }
                inFile.close();
            }
            else if(arg.cost_type==1)
            {
                cost_file="/data/gongyao/graphInfo/"+arg.dataset[k]+"_cost_001DEG.txt";
                cout << cost_file << endl;
                std::ifstream inFile;
                inFile.open(cost_file);
                if(!inFile)
                {
                    cout<<"cannot open the cost file at "<<cost_file<<endl;
                    exit(1);
                }
                float nodeCost;
                inFile.seekg(0, std::ios_base::beg);
                for(size_t i=0;i<g.n;i++)
                {
                    inFile>>nodeCost;
                    cost[i]=nodeCost;
                }
                inFile.close();
            }
            else{
                for(size_t i=0;i<g.n;i++)
                {
                    cost[i]=1.0;
                }
            }


            high_resolution_clock::time_point startTime = high_resolution_clock::now();
            pair<double, double> ASM_res;
            ASM_res=ASM::SeedMinimize(g, arg, cost,k);
            double avg_cost=ASM_res.first;
            double avg_spread=ASM_res.second;
            auto memory=getProcMemory();
            high_resolution_clock::time_point endTime = high_resolution_clock::now();
			duration<double> interval = duration_cast<duration<double>>(endTime - startTime);
            auto avg_runtime= (double)interval.count()/arg.time;
            // result_bk<<"("<<arg.dataset[k]<<", eta = "<<arg.eta<<", Alg = ASTI"<<", avg_cost = "<<avg_cost<<", avg_runtime="<<avg_runtime<<", memory = "<<memory<<", spread = "<<avg_spread<<")"<<endl;
            cout<<"("<< "eps= " << arg.epsilon <<", batch= "<< arg.batch<< ", dataset= "<< arg.dataset[k]<<", eta = "<<arg.eta<<", Alg = ASTI"<<", avg_cost = "<<avg_cost<<", avg_runtime="<<avg_runtime<<", memory = "<<memory<<", spread = "<<avg_spread<<")"<<endl;
        }
    }
}


int main(int argn, char **argv)
{
    __head_version = "v1";
    OutputInfo info(argn, argv);
    
    Run( argn, argv );
    // disp_mem_usage();
    cout<<"Memory(MB) "<<getProcMemory()<<endl;
}