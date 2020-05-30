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
vector<vector<int>> CostGraph(10, vector<int>(10 , 999));
vector<vector<int>> temp_CostGraph(10, vector<int>(10 , 999));
set<int> All_Nodes;
//unordered_map<int, vector<int> > NeighborMap;
vector<vector<int>> predecessor(10, vector<int>(10, 0));
unordered_map<int, unordered_map<int, vector<int>> > MinPath;
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
	vector<vector<int>> Path_Cost(total+1, vector<int>(total+1 , 999));
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
			if (a == 0 || a == 999)
			{
				continue;
			}
			neigh.push_back(i);
		}
		temp.neighbor = neigh;
		Graph.push_back(temp);
		//NeighborMap[id] = neigh;
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
	entry_path.clear();
	for (int i = 1; i < predecessor.size(); ++i)
	{
		vector<int> path;
		path.push_back(i);
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


void Distance_Vector()
{
	int total = Graph.size();
	cout << total << endl;
	//vector<vector<int>> temp_CostGraph;
	//temp_CostGraph.assign(CostGraph.begin(), CostGraph.end());

	//cout << CostGraph.size()<<endl;

	//vector<int> min_cost(total+1, 0); 
	//vector<int> predecessor(total+1, i+1);
	//min_cost = CostGraph[Graph[i].ID];


	for (int i = 1; i <= total; ++i)
	{
		for (int j = 1; j <= total; ++j)
		{
			predecessor[i][j] = i;
		}
	}

	/*for (int i = 1; i <= total; ++i)
	{
		for (int j = 1; j <= total; ++j)
		{
			cout << predecessor[i][j] << " ";
		}
		cout << endl;
	}*/


	for (int i = 0; i < Graph.size(); ++i) // times to stable
	{ 
		cout<<"Current Time: "<< Graph[i].ID<<endl;


		for(int row = 1 ; row <= Graph.size() ; ++row) 
		{
			//cout << "row: " << row << endl;
			for (int col = 1; col <= Graph.size(); ++col)
			{
				//cout << "col: " << col <<endl;
				int cur_val = temp_CostGraph[row][col];
				vector<int> neighbor_row = temp_CostGraph[col];

				for (int idx = 1; idx <= neighbor_row.size(); ++idx)
				{
					int d_r_to_c = temp_CostGraph[row][col];
					//int correspond_col = neighbor_row[idx];
					int col_to_neigh = neighbor_row[idx];

					if (d_r_to_c + col_to_neigh < temp_CostGraph[row][idx])
					{
						cout << "update..." << endl;
						temp_CostGraph[row][idx] = d_r_to_c + col_to_neigh;
						predecessor[row][idx] = col;
					}
				}

			}

		}

	}

	cout << "Min cost " <<endl;
		for (int i = 1; i < temp_CostGraph.size(); ++i)
		{
			for (int j = 1; j < temp_CostGraph[0].size(); ++j)
			{
				cout << temp_CostGraph[i][j] << " " ;
			}
			cout <<endl;
		}
		cout << endl;
		for (int i = 1; i <= total; ++i)
		{
			for (int j = 1; j <= total; ++j)
			{
				if (temp_CostGraph[i][j] == 999)
				{
					predecessor[i][j] = 0;
				}
				cout << predecessor[i][j] << " ";
			}
			cout << endl;
		}

		cout << "After Converge ... " <<endl;

		for (int row = 1; row < predecessor.size(); ++row)
		{
			//unordered_map<int , vector<int>> node_path = findPath(predecessor[row], row);
			//cout << "!!" <<endl;
			MinPath[row] = findPath(predecessor[row], row);
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
   			ofs<<temp_CostGraph[Graph[i].ID][Graph[j].ID]<<endl;
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

			if (temp_CostGraph[Src][Dst] == 999)
			{
				ofs << "from " << src << " to " << dst << " cost infinite hops unreachable message "<<msg<<endl;
				return;
			}

			ofs << "from " << src << " to " << dst << " cost " << to_string(temp_CostGraph[Src][Dst]) << " hops ";
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
			cout << "new_Cost: " << new_Cost << endl;

			if (new_Cost == -999)
			{
				cout << "break link" << endl;
				cout <<"n1: " << n1 <<endl;
				cout <<"n2: " << n2 <<endl;
				CostGraph[n1][n2] = 999;
				CostGraph[n2][n1] = 999;

			}

			else
			{
				CostGraph[n1][n2] = new_Cost;
				CostGraph[n2][n1] = new_Cost;

			}
			cout<<"!"<<endl;
			MinPath.clear();
			cout<<"!!"<<endl;
			//temp_CostGraph.clear();
			cout << "before Distance_Vector..." <<endl;
			temp_CostGraph = CostGraph;
			predecessor = CostGraph;
			Distance_Vector();
			cout << "Next change" << endl;
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
    temp_CostGraph = CostGraph;
    predecessor = CostGraph;
    Distance_Vector();

    MessageSent(argv[2]);
    cout << "Before change Topo..." <<endl;
    ChangeTopo(argv[3], argv[2]);
    cout << "Complete change Topo..." <<endl;



    /*FILE *fpOut;
    fpOut = fopen("output.txt", "w");
    fclose(fpOut);*/
    

    return 0;
}

