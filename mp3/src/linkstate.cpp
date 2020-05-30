#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include <fstream>
#include<set>
#include<unordered_map>
#include <sstream>
#include <vector>
#include <limits.h>
#include <algorithm>

using namespace std;

struct Node
{
	int ID;
	vector<int> neighbor;
};
vector<Node> Graph;
vector<vector<int>> CostGraph;
set<int> All_Nodes;
unordered_map<int, vector<int> > NeighborMap;
unordered_map<int, unordered_map<int, vector<int>> > MinPath;
unordered_map<int, vector<int>> MinCost;
ofstream ofs("output.txt", ios::out);


void parseTOPO(char* topofile)
{
	ifstream file;
	file.open(topofile);
	string line;
	set<int> Nodes;
	vector<pair<int, int>> temp;
	vector<int> All_link_cost;

	if(file.is_open())
	{
		while(getline(file, line))
		{
			stringstream ss(line);
			string node1, node2, cost;
			ss >> node1;
			ss >> node2;
			ss >> cost;
			int n1 = atoi(node1.c_str());
			int n2 = atoi(node2.c_str());
			int Cost = atoi(cost.c_str());
			Nodes.insert(n1);
			Nodes.insert(n2);
			pair<int, int> Link(n1,n2);
			temp.push_back(Link);
			All_link_cost.push_back(Cost);
		}
	}
	
	//build the cost map
	All_Nodes = Nodes;
	int total = Nodes.size();
	vector<vector<int>> Path_Cost(total+1, vector<int>(total+1 , INT_MAX));
	for (int i = 0; i < temp.size(); ++i)
	{
		Path_Cost[temp[i].first][temp[i].second] = All_link_cost[i];
		Path_Cost[temp[i].second][temp[i].first] = All_link_cost[i];
	}

	for (int i = 1; i < Path_Cost.size(); ++i)
	{
		Path_Cost[i][i] = 0;
	}
	CostGraph = Path_Cost;
	/*for (int i = 1; i < Path_Cost.size(); ++i)
	{
		for (int j = 1; j < Path_Cost[0].size(); ++j)
		{
			cout<< Path_Cost[i][j]<<" ";
		}
		cout<<endl;
	}*/

	//Use Nodes set to build each node
	set<int>::iterator it;
	for (it = Nodes.begin(); it != Nodes.end(); ++it)
	{
		Node temp;
		int id = *it;
		temp.ID = id;
		vector<int> neigh;
		for (int i = 1; i < Path_Cost.size(); ++i)
		{
			int a = Path_Cost[id][i];
			if (a == 0 || a == INT_MAX)
			{
				continue;
			}
			neigh.push_back(i);
		}
		temp.neighbor = neigh;
		Graph.push_back(temp);
		NeighborMap[id] = neigh;
	}
	//debug message
	/*for (int i = 0; i < Graph.size(); ++i)
	{
		cout<<"ID: "<<Graph[i].ID<<endl;
		for (int j = 0; j < Graph[i].neighbor.size(); ++j)
		{
			cout<<Graph[i].neighbor[j]<<" ";
		}
		cout<<endl;
	}*/
}

int findMinID(vector<int>& temp, set<int>visited)
{
	int MIN = INT_MAX;
	int ans;
	for(int i = 0 ; i < temp.size() ; ++i)
	{
		if (temp[i] == 0 || temp[i] == INT_MAX || visited.find(i) != visited.end())
		{
			continue;
		}
		
		if (temp[i] < MIN )
		{
			MIN = temp[i];
			ans = i;
		}
	}

	return ans;
}

unordered_map<int, vector<int> > findPath(vector<int>& predecessor, int entry)
{
	cout<<"In findPath function..."<<endl;
	
	for (int i = 1; i < predecessor.size(); ++i)
	{
		cout<<predecessor[i]<<" ";
	}

	unordered_map<int, vector<int> > entry_path;
	for (int i = 1; i < predecessor.size(); ++i)
	{
		vector<int> path;
		path.push_back(i);
		//path.push_back(predecessor[i]);
		int ptr = predecessor[i];
		cout<<"ptr: "<<ptr<<endl;
		cout<<"Entry: " << entry<<endl;
		if (ptr == 0) // isolated node
		{
			path.clear();
			entry_path[i] = path;
			continue;
		}
		while(ptr != entry)
		{
			//cout<<"ptr: "<<ptr<<endl;
			path.push_back(ptr);
			ptr = predecessor[ptr];
		}
		std::reverse(path.begin(), path.end());
		for (int i = 0; i < path.size(); ++i)
		{
			cout<<path[i]<<" ";
		}
		cout<<endl;
		entry_path[i] = path;
	}
	return entry_path;
}


void Dijkstra()
{
	int total = Graph.size();
	set<int>::iterator it;

	for (int i = 0; i < Graph.size(); ++i)
	{ 
		cout<<"Current node: "<< Graph[i].ID<<endl;
		vector<int> min_cost(total+1, 0); 
		vector<int> predecessor(total+1, i+1);
		set<int> unvisited = All_Nodes;
		set<int> visited;
		visited.insert(Graph[i].ID);
		unvisited.erase(Graph[i].ID);
		min_cost = CostGraph[Graph[i].ID];
		set<int> isolated; // try to find each isolated point from entry

		while(!unvisited.empty())
		{
			cout<<"Current node: "<< Graph[i].ID<<endl;
			cout<<"unvisited size: "<<unvisited.size()<<endl;
			cout<<"visited size: "<<visited.size()<<endl;
			bool all_unreachable = true;

			for (it = unvisited.begin() ; it!=unvisited.end() ; ++it)
			{
				if (min_cost[*it] != INT_MAX)
				{
					all_unreachable = false;
					break;
				}
			}
			if (all_unreachable)
			{
				break;
			}
			int nextID = findMinID(min_cost, visited);

			cout<< "nextID: "<<nextID<<endl;
			visited.insert(nextID);
			unvisited.erase(nextID);
			//update the min_cost;
			for (int n = 0; n < NeighborMap[nextID].size(); ++n)
			{
				//cout<<"NeighborMap[nextID][n]: " << NeighborMap[nextID][n] << endl;

				if(visited.find(NeighborMap[nextID][n]) != visited.end()) // neighbor is visited
				{
					continue;
				}
				int cur_min = min_cost[ NeighborMap[nextID][n] ];
				int compare_cost = min_cost[nextID] + CostGraph[nextID][ NeighborMap[nextID][n] ];

				if(compare_cost < cur_min)
				{
					cout<<"update..."<<endl;
					min_cost[ NeighborMap[nextID][n] ] = compare_cost;
					predecessor [ NeighborMap[nextID][n] ] = nextID;
					
				}

				else if (compare_cost == cur_min)
				{
					cout<<"tie breaking..."<<endl;
					if (nextID < predecessor[NeighborMap[nextID][n]])
					{
						predecessor[NeighborMap[nextID][n]] = nextID;
					}
				}

			}

		}
		for (int i = 1; i < predecessor.size(); ++i)
		{
			if (min_cost[i] == INT_MAX)
			{
				predecessor[i] = 0;
			}
		}
		unordered_map<int , vector<int>> node_path = findPath(predecessor, i+1);
		MinPath[i+1] = node_path;
		MinCost[i+1] = min_cost;
		//MinPath[i] = findPath(predecessor, i);
		//debug message
		for (int i = 1; i < min_cost.size(); ++i)
		{
			cout<<predecessor[i]<<" ";
		}
		cout<<endl;
	}

	for (int i = 0; i < Graph.size(); ++i)
    {

   		for (int j = 0; j < Graph.size(); ++j)
   		{ 
   			if (MinPath[Graph[i].ID][Graph[j].ID].size() == 0)
   			{
   				continue;
   			}

   			ofs<<Graph[j].ID<<" ";
   			ofs<<MinPath[Graph[i].ID][Graph[j].ID][0]<<" ";
   			ofs<<MinCost[Graph[i].ID][Graph[j].ID]<<endl;
   		}
   		ofs<<endl;
    }
    
}

void MessageSent(char* messagefile)
{
	cout<<"In MessageSent..."<<endl;
	ifstream Mess;
	string line;
	Mess.open(messagefile);
	vector< pair<int,int> > Src_Dst;
	vector<string> message;
	stringstream ss;

	if(Mess.is_open())
	{
		while(getline(Mess, line))
		{
			string src, dst, msg;
			
			int space_1 = line.find_first_of(' ');
			src = line.substr(0,space_1);
			//cout<<src<<endl;
			line = line.substr(space_1+1);
			int space_2 = line.find_first_of(' ');
			dst = line.substr(0,space_2);
			//cout<<dst<<endl;
			msg = line.substr(space_2+1);
			//cout<<msg<<endl;

			int Src = atoi(src.c_str());
			int Dst = atoi(dst.c_str());
			pair<int, int> src_dst(Src, Dst); 
			Src_Dst.push_back(src_dst);
			message.push_back(msg);

			if (MinCost[Src][Dst] == INT_MAX)
			{
				ofs << "from " << src << " to " << dst << " cost infinite hops unreachable message "<<msg<<endl;
				return;
			}

			ofs << "from " << src << " to " << dst << " cost " << to_string(MinCost[Src][Dst]) << " hops ";
			ofs << src << " ";

			for (int i = 0; i < MinPath[Src][Dst].size()-1; ++i)
			{
				ofs << to_string(MinPath[Src][Dst][i]) << " ";
			} 

			ofs << "message " << msg;
			ofs <<  endl;
			ofs <<  endl;
		}
	}
	//cout<< ss.str();

}

void ChangeTopo(char* changesfile, char* messagefile)
{
	cout<<"In change Topo..."<<endl;
	ifstream file;
	string line;
	file.open(changesfile);

	if(file.is_open())
	{
		while(getline(file, line))
		{

			stringstream ss(line);
			string node1, node2, new_cost;
			ss >> node1;
			ss >> node2;
			ss >> new_cost;
			int n1 = atoi(node1.c_str());
			int n2 = atoi(node2.c_str());
			int new_Cost = atoi(new_cost.c_str());

			if (new_Cost == -999)
			{
				cout << "break link" <<endl;
				CostGraph[n1][n2] = INT_MAX;
				CostGraph[n2][n1] = INT_MAX;

				for (int i = 0; i < NeighborMap[n1].size(); ++i)
				{
					if (NeighborMap[n1][i] == n2)
					{
						NeighborMap[n1].erase(NeighborMap[n1].begin() + i);
						break;
					}
					
				}

				for (int i = 0; i < NeighborMap[n2].size(); ++i)
				{
					if (NeighborMap[n2][i] == n1)
					{
						NeighborMap[n2].erase(NeighborMap[n2].begin() + i);
						break;
					}
					
				}		
			}

			else
			{
				CostGraph[n1][n2] = new_Cost;
				CostGraph[n2][n1] = new_Cost;

				vector<int>::iterator it;

				it = find(NeighborMap[n1].begin(), NeighborMap[n1].end(), n2);
				if (it == NeighborMap[n1].end())
				{
					NeighborMap[n1].push_back(n2);
					sort(NeighborMap[n1].begin(), NeighborMap[n1].end());
				}

				it = find(NeighborMap[n2].begin(), NeighborMap[n2].end(), n1);
				if (it == NeighborMap[n2].end())
				{
					NeighborMap[n2].push_back(n1);
					sort(NeighborMap[n2].begin(), NeighborMap[n2].end());
				}

			}
			MinCost.clear();
			MinPath.clear();
			Dijkstra();
			MessageSent(messagefile);		
		}

	}
}


int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    parseTOPO(argv[1]);
    Dijkstra();

    MessageSent(argv[2]);

    ChangeTopo(argv[3], argv[2]);



    /*FILE *fpOut;
    fpOut = fopen("output.txt", "w");
    fclose(fpOut);*/
    

    return 0;
}

