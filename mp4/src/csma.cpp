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
#include <time.h>
#include <iomanip>
#include <math.h>

using namespace std;

int NodesNum;
int PacketLen;
vector<int> RandomNum;
int MaxRetran;
int TimeInterv;
struct Node
{
	int collision_count;
	int node_coll_num;
	int backoff;
	int SuccessTranNum;
	int RandomIdx;
};
vector<Node> AllNodes;
int BusyTime;
int IdleTime;
int GlobalColliNum;
int GlobalSuccTranNum;
ofstream ofs("output.txt", ios::out);
ofstream q1("q1.txt", ios::out);
ofstream q2("q2.txt", ios::out);
ofstream q3("q3.txt", ios::out);
ofstream q4("q4.txt", ios::out);
ofstream q5("q5.txt", ios::out);

void InputParser (char* inputfile)
{
	ifstream file;
	file.open(inputfile);
	string line;

	if (file.is_open())
	{
		while(getline(file, line))
		{
			stringstream ss(line);
			string character, tmp;
			ss >> character;

			if (character == "N")
			{
				ss >> tmp;
				NodesNum = atoi(tmp.c_str());	
			}

			else if(character == "L")
			{
				ss >> tmp;
				PacketLen = atoi(tmp.c_str());
			}

			else if(character == "R")
			{
				ss >> tmp;
				while(!tmp.empty())
				{
					RandomNum.push_back(atoi(tmp.c_str()));
					tmp.clear();
					ss >> tmp;
				}
			}

			else if(character == "M")
			{
				ss >> tmp;
				MaxRetran = atoi(tmp.c_str());
			}

			else if(character == "T")
			{
				ss >> tmp;
				TimeInterv = atoi(tmp.c_str());
			}

		}
	}

	cout << "NodesNum: " << NodesNum << endl;
	cout << "PacketLen: " << PacketLen << endl;
	for (int i = 0; i < RandomNum.size(); ++i)
	{
		cout << RandomNum[i] << " ";
	}
	cout << endl;
	cout << "MaxRetran: " << MaxRetran << endl;
	cout << "TimeInterv: " << TimeInterv << endl;

	for (int i = 0; i < NodesNum; ++i)
	{
		Node node;
		node.collision_count = 0;
		node.backoff = rand() % (RandomNum[0] + 1);
		node.SuccessTranNum = 0;
		node.RandomIdx = 0;
		node.node_coll_num = 0;
		AllNodes.push_back(node);
	}

}

void csma()
{
	bool channelOccupied = false;
	Node* cur;

	for (int t = 0; t < TimeInterv; ++t)
	{
		if (channelOccupied)
		{
			channelOccupied = false;
			++GlobalSuccTranNum; 
			++(cur -> SuccessTranNum);
			cur -> backoff = rand() % ((RandomNum[ cur -> RandomIdx ]) + 1);
		}

		vector<Node*> Ready_to_send; 

		for (int i = 0; i < AllNodes.size(); ++i)
		{
			if (AllNodes[i].backoff == 0)
			{
				Ready_to_send.push_back(&AllNodes[i]);
			}
		}

		if (Ready_to_send.size() == 0) 
		{
			++IdleTime;
			for (int i = 0; i < AllNodes.size(); ++i)
			{
				--AllNodes[i].backoff;
			}
			continue;
		}

		else if (Ready_to_send.size() == 1)
		{
			cur = Ready_to_send[0];
			channelOccupied = true;
			BusyTime = BusyTime + PacketLen;
			t = t + PacketLen - 1;
			continue;
		}

		else
		{
			++GlobalColliNum;
			for (int i = 0; i < Ready_to_send.size(); ++i)
			{
				++(Ready_to_send[i] -> collision_count);
				++(Ready_to_send[i] -> node_coll_num);
				if (Ready_to_send[i] -> collision_count == MaxRetran)
				{
					Ready_to_send[i] -> collision_count = 0;
					Ready_to_send[i] -> RandomIdx = 0;
					Ready_to_send[i] -> backoff = rand() % ((RandomNum[ Ready_to_send[i] -> RandomIdx ]) + 1);
				}

				else
				{
					++(Ready_to_send[i] -> RandomIdx);
					Ready_to_send[i] -> backoff = rand() % ((RandomNum[ Ready_to_send[i] -> RandomIdx ]) + 1);
				}
			}
		}
		
	}

	cout << endl;
	cout << "-----OutCome-----" << endl;
	cout << "BusyTime: " << BusyTime << endl;
	cout << "IdleTime: " << IdleTime << endl;
	cout << "GlobalColliNum: " << GlobalColliNum << endl;
	cout << "GlobalSuccTranNum: " << GlobalSuccTranNum << endl;
	cout << endl;
	
/*	for (int i = 0; i < AllNodes.size(); ++i)
	{
		cout << i << endl;
		cout << "node_coll_num: " << AllNodes[i].node_coll_num << endl;
		cout << "SuccessTranNum: " << AllNodes[i].SuccessTranNum << endl;
		cout << endl;
	}*/

}

void OuputPrint()
{
	//std::cout.precision(4); 
	double utilization = (double)(BusyTime) / (double)(TimeInterv);
	double IdleFrac = (double)(IdleTime) / (double)(TimeInterv);
	double Var_SuccTran = 0.0;
	double Var_Collisions = 0.0;
	double SuccSum = 0.0;
	double SuccMean = 0.0;
	double ColliSum = 0.0;
	double ColliMean = 0.0;

	for (int i = 0; i < AllNodes.size(); ++i)
	{
		SuccSum += AllNodes[i].SuccessTranNum;
		ColliSum += AllNodes[i].node_coll_num;
	}

	SuccMean = SuccSum / double(NodesNum);
	ColliMean = ColliSum / double(NodesNum);

	for (int i = 0; i < AllNodes.size(); ++i)
	{
		int tmp1 = AllNodes[i].SuccessTranNum - SuccMean;
		int tmp2 = AllNodes[i].node_coll_num - ColliMean;

		Var_SuccTran += pow(tmp1, 2);
		Var_Collisions += pow(tmp2, 2);
	}

	Var_SuccTran = Var_SuccTran / double(NodesNum);
	Var_Collisions = Var_Collisions / double(NodesNum);


	ofs << "Channel utilization (in percentage): "  <<std::setprecision(4) << fixed << utilization << endl;
	ofs << "Channel Idle fraction (in percentage): " << std::setprecision(4) << fixed << IdleFrac << endl;
	ofs << "Total number of collisions: " << GlobalColliNum << endl;
	ofs << "Variance in number of successful transmissions (across all nodes): " << std::setprecision(4) << fixed << Var_SuccTran << endl;
	ofs << "Variance in number of collisions (across all nodes): " << std::setprecision(4) << fixed << Var_Collisions << endl;
}


int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 2) {
        printf("Usage: ./csma input.txt\n");
        return -1;
    }
    //srand(time(NULL));
    InputParser(argv[1]);
    csma();
    OuputPrint();


    for (int i = 5; i <= 500 ; i = i+11)
    {
    	NodesNum = i;
    	AllNodes.clear();
    	BusyTime = 0;
    	IdleTime = 0;
    	GlobalSuccTranNum = 0;
    	GlobalColliNum = 0;
    	for (int i = 0; i < NodesNum; ++i)
		{
			Node node;
			node.collision_count = 0;
			node.backoff = rand() % (RandomNum[0] + 1);
			node.SuccessTranNum = 0;
			node.RandomIdx = 0;
			node.node_coll_num = 0;
			AllNodes.push_back(node);
		}
		csma();
		q1 << i << " " <<std::setprecision(4) << fixed << (double)(BusyTime) / (double)(TimeInterv) << endl;
		q2 << i << " " <<std::setprecision(4) << fixed << (double)(IdleTime) / (double)(TimeInterv) << endl;
		q3 << i << " " << GlobalColliNum << endl;
    }

    /*for (int k = 1; k < 32; k = k*2)
    {
    	cout << "k: " << k << endl;
    	RandomNum.clear();
    	RandomNum.push_back(k);

    	for (int j = 1; j < NodesNum; ++j)
    	{
    		RandomNum.push_back(RandomNum[j-1] * 2);
    	}

    	for (int i = 5; i <= 500 ; i = i+5)
    	{
    		NodesNum = i;
    		AllNodes.clear();
    		BusyTime = 0;
    		IdleTime = 0;
    		GlobalSuccTranNum = 0;
    		GlobalColliNum = 0;
    		for (int i = 0; i < NodesNum; ++i)
			{
				Node node;
				node.collision_count = 0;
				node.backoff = rand() % (RandomNum[0] + 1);
				node.SuccessTranNum = 0;
				node.RandomIdx = 0;
				node.node_coll_num = 0;
				AllNodes.push_back(node);
			}
			csma();
			q4 << i << " " <<std::setprecision(4) << fixed << (double)(BusyTime) / (double)(TimeInterv) << endl;

    	}
    }*/


    /*for (int k = 1; k <=5 ; k++)
    {
    	cout << "k: " << k << endl;   

    	for (int i = 5; i <= 500 ; i = i+5)
    	{
    		PacketLen = 20 * k;
    		NodesNum = i;
    		AllNodes.clear();
    		BusyTime = 0;
    		IdleTime = 0;
    		GlobalSuccTranNum = 0;
    		GlobalColliNum = 0;
    		for (int i = 0; i < NodesNum; ++i)
			{
				Node node;
				node.collision_count = 0;
				node.backoff = rand() % (RandomNum[0] + 1);
				node.SuccessTranNum = 0;
				node.RandomIdx = 0;
				node.node_coll_num = 0;
				AllNodes.push_back(node);
			}
			csma();
			q5 << i << " " <<std::setprecision(4) << fixed << (double)(BusyTime) / (double)(TimeInterv) << endl;

    	}

    }*/

    return 0;
}