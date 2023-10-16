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
		static unsigned int left_Q;
		static unsigned int left_n;				

		static void AdaptiveSelect(InfGraph &g, const Argument & arg, const double factor, const double epsilon, const double delta)
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
				g.build_TRR_r((int64)sample, left_n / left_Q, 1.*(left_n % left_Q) / left_Q);
				batch_set.clear();
				//calculate the upper bound here				
				double influence = g.build_seedset(batch, batch_set);  //we need to be careful here.			

				double lower = sqr(sqrt(influence + 2. * a1 / 9.) - sqrt(a1 / 2.)) - a1 / 18;
				double upper = sqr(sqrt(influence / factor + a2 / 2.) + sqrt(a2 / 2.));				

				double ratio = lower / upper;

				if (ratio > factor*(1 - epsilon))				
				{
					//cout << "root_num: " << left_n / left_Q << endl;
					for (auto it : batch_set)
					{
						g.seedSet.push_back(it); 
						std::cout<<it<<", ";
					}
					g.realization(batch_set, active_node);
					return;
				}
				sample *= 2;
				//cout<<"The number of sample is "<<sample<<endl;
			}
			batch_set.clear();
			g.build_seedset(batch, batch_set);
			for (auto it : batch_set) {g.seedSet.push_back(it); std::cout<<it<<", ";}
			g.realization(batch_set, active_node);
		}

public:
        static void SeedMinimize(InfGraph &g, const Argument &arg)
        {                  			            					
			
			cout << "expected spread: " << arg.Q << endl;

			double total_spread = 0;			
			double total_time = 0;
						
			double seed_num = 0;			
			const double factor = 1. - pow(1. - 1. / arg.batch, arg.batch);

			for (int i = 0; i < arg.time; i++)
            {				
				//the preparation work before each time.						
				//g.load_possible_world(to_string(i), arg);
				g.generate_possible_world(arg);
				g.init_hyper_graph();				
				active_node = 0;				
				left_Q = arg.Q; //change the left_node as the quota
				left_n = g.n;
				
				high_resolution_clock::time_point startTime = high_resolution_clock::now();				

				while (active_node <  arg.Q )  //set the eta as 1;
 				{							
					left_Q = arg.Q - active_node;
					left_n = g.n - active_node;
					//cout<<"Left Q is "<<left_Q<<endl;
					if (left_Q <= arg.batch)
					{
						g.RandBatch(arg.batch);
						active_node+=arg.batch;
						break;
					}

					const double delta = 1. / left_n;															
					const double epsilon_prime = (arg.epsilon - delta) / (1 - delta);					
					AdaptiveSelect(g, arg, factor, epsilon_prime, delta);
				}
				
				high_resolution_clock::time_point endTime = high_resolution_clock::now();
				duration<double> interval = duration_cast<duration<double>>(endTime - startTime);

				total_time += (double)interval.count();
				seed_num += g.seedSet.size();
			
				total_spread += active_node;

				cout << "SingleSeed " << g.seedSet.size() << endl;
				cout << "SingleRuntime " << (double)interval.count() << endl;		
				cout << "SingleSpread " << active_node << endl;
            }            
			
			cout << "RunningTime(s) " << total_time / arg.time << endl;			

            disp_mem_usage();			
			cout << "AdaptiveSpread " << total_spread / arg.time << endl;
			cout << "TotalSeed " << seed_num / arg.time << endl;
        }
};

unsigned int  ASM::active_node = 0;
unsigned int  ASM::left_Q = 0;
unsigned int  ASM::left_n = 0;
