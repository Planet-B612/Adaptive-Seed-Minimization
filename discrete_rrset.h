/*
* BFS starting from one node
*/

int BuildTRR(int hyperiiid, int root_num, double prob)
{			
	root_num += (sfmt_genrand_real1(&sfmtSeed) <= prob);		
	
	unsigned int n_visit_mark = 0;
	q.clear();

//	high_resolution_clock::time_point start1 = high_resolution_clock::now();
	while (root_num > 0)	
	{
		int node = sfmt_genrand_uint32(&sfmtSeed) % n;
		while (active_set[node] || visit[node])node = sfmt_genrand_uint32(&sfmtSeed) % n;
		q.push_back(node);
		--root_num;
		visit_mark[n_visit_mark++] = node;
		visit[node] = true;
		hyperG[node].push_back(hyperiiid);		
	}

	if (influModel == IC)
	{
		while (!q.empty())
		{
			int i = q.front();
			q.pop_front();				
			for (int j = 0; j < (int)gT[i].size(); j++)
			{
				int v = gT[i][j];
				if (active_set[v] || visit[v])continue;

				double randDouble = sfmt_genrand_real1(&sfmtSeed);
				if (randDouble > probT[i][j])continue;

				visit_mark[n_visit_mark++] = v;
				visit[v] = true;
				q.push_back(v);	
				hyperG[v].push_back(hyperiiid);				
			}
		}
	}
	else if (influModel == LT)
	{
		while (!q.empty())
		{
			int expand = q.front();
			q.pop_front();			

			if (gT[expand].size() == 0)
				continue;
			
			
			int index = sfmt_genrand_uint32(&sfmtSeed) % gT[expand].size();
			int v = gT[expand][index];
			if (active_set[v] || visit[v])continue;

			visit[v] = true;
			visit_mark[n_visit_mark++] = v;
			q.push_back(v);
			hyperG[v].push_back(hyperiiid);			
		}
	}
	else
		ASSERT(false);


	for (unsigned int i = 0; i < n_visit_mark; i++)visit[visit_mark[i]] = false;	

	hyperGT.push_back(vector<int>(visit_mark.begin(), visit_mark.begin() + n_visit_mark));

	return 0;
}

void realization(vector<int> batch_set, unsigned int & active_node)
{
	q.clear();
	unsigned int counter = 0;
	for (auto seed_node : batch_set)
	{		
		++counter;
		active_set[seed_node] = true;
		q.push_back(seed_node);
	}

	while (!q.empty())
	{
		int expand = q.front();
		q.pop_front();
		for (auto v : PO[expand])
		{
			if (active_set[v])continue;
			q.push_back(v);
			++counter;
			active_set[v] = true;
		}
	}

	active_node += counter;		
	//update the graph
	for (unsigned int i = 0; i < hyperG.size(); ++i)hyperG[i].clear();
	for (unsigned int i = 0; i < hyperGT.size(); ++i)vector<int>().swap(hyperGT[i]);
	hyperGT.clear();
}

void RandBatch(int batch)
{
	int node = sfmt_genrand_uint32(&sfmtSeed) % n;
	while (batch > 0)
	{		
		while (active_set[node])node = sfmt_genrand_uint32(&sfmtSeed) % n;
		seedSet.push_back(node);
		cout<<"The random seed is "<<node<<endl;
		--batch;
	}
}
