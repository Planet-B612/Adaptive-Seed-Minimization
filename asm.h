#include <chrono>
#include <ctime>
#include <ratio>

#define e exp(1)
#define c 2*(exp(1)-2)

using namespace std::chrono;

class Math{
    public:
        static double log2(int n){
            return log(n) / log(2);
        }
        static double logcnk(int n, int k) {
            double ans = 0;
            for (int i = n - k + 1; i <= n; i++)
            {
                ans += log(i);
            }
            for (int i = 1; i <= k; i++)
            {
                ans -= log(i);
            }
            return ans;
        }
};

class ASM
{
    private:               
		static unsigned int active_node; //this is to record the number of the active node.
		static unsigned int left_eta;
		static unsigned int left_n;				

		static void AdaptiveSelect(InfGraph &g, const Argument & arg, const double factor, const double epsilon, const double delta, const vector<float> cost)
		{			
			const unsigned int batch = arg.batch;
			const double alpha = sqrt(log(6.0 / delta));
			const double beta = sqrt((Math::logcnk(left_n, batch) + log(6.0 / delta)) / factor);

			const double theta_o = 2 * sqr(alpha + beta);
			const double theta_max = 2 * left_n*sqr(alpha + beta) / epsilon / epsilon / arg.batch;

			const double i_max = ceil(log(left_n / arg.batch / epsilon / epsilon) / log(2)) + 1;

			const double a1 = log(3 * i_max / delta) + Math::logcnk(left_n, batch);			
			const double a2 = log(3 * i_max / delta);

			double sample = theta_o;

			vector<int>batch_set;
			
			while (sample < theta_max)
			{
				g.build_TRR_r(sample, left_n / left_eta, 1.*(left_n % left_eta) / left_eta);
				batch_set.clear();
				//calculate the upper bound here				
				// double influence = g.build_seedset(batch, batch_set);  //we need to be careful here.
				double influence = g.max_ratio(batch, batch_set, cost, g.hyperG, g.hyperGT);
				int coverage_1=g.coverage(batch_set, g.hyperG_1, g.hyperGT_1);

				double lower = sqr(sqrt(1.0*coverage_1 + 2. * a1 / 9.) - sqrt(a1 / 2.)) - a1 / 18;
				double upper = sqr(sqrt(influence / factor + a2 / 2.) + sqrt(a2 / 2.));				

				double ratio = lower / upper;

				if (ratio > factor*(1 - epsilon))				
				{
					//cout << "root_num: " << left_n / left_eta << endl;
					for (auto it : batch_set)
					{
						g.seedSet.push_back(it); 
						// std::cout<<it<<", ";
					}
					g.realization(batch_set, active_node);
					return;
				}
				sample *= 2;
				//cout<<"The number of sample is "<<sample<<endl;
			}
			batch_set.clear();
			// g.build_seedset(batch, batch_set);
			g.max_ratio(batch, batch_set, cost, g.hyperG, g.hyperGT);
			for (auto it : batch_set) 
			{
				g.seedSet.push_back(it); 
				// std::cout<<it<<", ";
			}
			g.realization(batch_set, active_node);
		}

public:
        static pair<double, double> SeedMinimize(InfGraph &g, const Argument &arg, const vector<float> cost, int k)
        {                  			            					
			
			cout << "expected spread: " << arg.eta << endl;

			double total_spread = 0;			
			double total_time = 0;
						
			double seed_num = 0;			
			const double factor = 1. - pow(1. - 1. / arg.batch, arg.batch);
			double total_cost=0.0;
			for (int i = 0; i < arg.time; i++)
            {				
				//the preparation work before each time.						
				g.load_possible_world(to_string(i), arg);
				// g.generate_possible_world(arg);
				g.init_hyper_graph();				
				active_node = 0;				
				left_eta = arg.eta; //change the left_node as the quota
				left_n = g.n;
				
				high_resolution_clock::time_point startTime = high_resolution_clock::now();				

				while (active_node <  arg.eta )  //set the eta as 1;
 				{							
					left_eta = arg.eta - active_node;
					left_n = g.n - active_node;
					//cout<<"Left eta is "<<left_eta<<endl;
					// if (left_eta <= arg.batch)
					// {
					// 	g.RandBatch(arg.batch);
					// 	active_node+=arg.batch;
					// 	break;
					// }
					g.numRRsets = 0;  // added
					const double delta = arg.epsilon/(100.0*(1-1.0/e)*(1-arg.epsilon)*left_eta);														
					const double epsilon_prime = 99.0*arg.epsilon/(100.0-arg.epsilon);				
					AdaptiveSelect(g, arg, factor, epsilon_prime, delta, cost);
				}
				
				high_resolution_clock::time_point endTime = high_resolution_clock::now();
				duration<double> interval = duration_cast<duration<double>>(endTime - startTime);

				total_time += (double)interval.count();
				seed_num += g.seedSet.size();
			
				total_spread += active_node;

				for(auto seed:g.seedSet)	total_cost+=cost[seed];

				cout << "SingleSeed " << g.seedSet.size() << endl;
				cout << "SingleRuntime " << (double)interval.count() << endl;		
				cout << "SingleSpread " << active_node << endl;
				
				double single_cost = 0.0;
				for(auto seed:g.seedSet)	single_cost+=cost[seed];
				cout << "SingleCost " << single_cost << endl;
				
				ofstream out_seeds("../results/newseeds/ASTI_" + arg.dataset[k] + to_string(arg.eta) + arg.model + "_" + to_string(i) + ".txt", ios::out);
				assert((!out_seeds.fail()));
				for (auto node : g.seedSet)
				{
					out_seeds << node << endl;
				}
				out_seeds.close();
            }            
			
			cout << "RunningTime(s) " << total_time / arg.time << endl;			
            disp_mem_usage();			
			cout << "Average_Spread " << total_spread / arg.time << endl;
			cout << "Average_Seed_Num" << seed_num / arg.time << endl;
			//return 1.0*seed_num/arg.time;
			return make_pair(1.0*total_cost/arg.time, 1.0*total_spread / arg.time);
        }
};

unsigned int  ASM::active_node = 0;
unsigned int  ASM::left_eta = 0;
unsigned int  ASM::left_n = 0;
