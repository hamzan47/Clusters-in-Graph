#include <iostream>
#include <fstream>
using namespace std;

struct vertex {
	string name;
	int location;
	vertex* next;
};

struct cluster {
	int nodes, edges;
	float density;
	vertex* next;
	cluster* next_head;
};

struct Graph{
	cluster* head;
};

class graph {
private:
	string* key;
	float threshold;
	int num,** matrix,*degree;
	Graph* root;
	void creatematrix()
	{
		string end1, end2;
		bool e1, e2;
		float weight;
		int j=0,k=0;
		fstream assignment("Assignment 5-PPI.txt", ios::in);
		assignment.seekg(ios::beg);
		matrix = new int *[num];
		for (int i = 0; i < num; i++)
		{
			matrix[i] = new int[num];
		}
		for (int i = 0; i < num; i++)
		{
			for (int l = 0; l < num;l++)
				matrix[i][l] = 0;
		}
		while (assignment >> end1)
		{
			e1 = false;
			e2 = false;
			assignment >> end2;
			assignment >> weight;
			for (int i = 0; i < num; i++)
			{
				if (end1 == key[i])
				{
					j = i;
				}				
			}
			for (int i = 0; i < num; i++)
			{
				if (end2 == key[i])
				{
					k = i;
				}
			}
			matrix[j][k] = 1;
			matrix[k][j] = 1;
		}
		assignment.close();
	}
	void commonnodes()
	{
		int** temp = new int*[num],** temp1 = new int*[num];
		for (int i = 0; i < num; i++)
		{
			temp[i] = new int[num];
			temp1[i] = new int[num];
		}
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++)
			{
				temp[i][j] = matrix[i][j];
			}
		}
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++)
			{
				temp1[i][j] = 0;
				for (int k = 0; k < num; k++)
				{
					temp1[i][j] += matrix[i][k] * temp[k][j];
				}
			}
		}
		matrix = temp1;
		for (int i = 0; i < num; i++)
			matrix[i][i] = 0;
	}
	void insertvertex(string temp)
	{
		bool check = true;
		for (int i = 0; i < num; i++)
		{
			if (temp == key[i])
			{
				check = false;
				break;
			}
		}
		if (num == 0)
		{
			num++;
			key = new string[num];
			key[num - 1] = temp;
			return;
		}
		if (check == false)
			return;
		else
		{
			num++;
			string* temp1 = new string[num];
			for (int i = 0; i < num-1; i++)
			{
				temp1[i] = key[i];
			}
			temp1[num - 1] = temp;
			key = temp1;
		}
	}
	void displaymatrix()
	{

		for (int i = 0; i < num; i++)
		{
			cout << key[i] << ": \n";
			for (int j = 0; j < num; j++)
			{
				cout << key[j] << " " << matrix[i][j] << "\n";
			}
		}
	}
	void creategraph()
	{
		string end1,end2;
		float weight;
		fstream assignment("Assignment 5-PPI.txt", ios::in);
		while (assignment>>end1)
		{
			assignment >> end2;
			assignment >> weight;
			insertvertex(end1);
			insertvertex(end2);
		}
		assignment.close();
		creatematrix();
		commonnodes();
		degrees();
	//	displaymatrix();
		createlist();
	}
	void degrees()
	{
		degree = new int[num];
		for (int i = 0; i < num; i++)
		{
			degree[i] = 0;
			for (int j = 0; j < num; j++)
			{
				degree[i] += matrix[i][j];
			}
		}
	}
	void createlist()
	{
		int max,node=0;
		bool check;
		while (1)
		{
			check = false;
			max = -1;
			for (int i = 0; i < num; i++)
			{
				if (degree[i] > max)
				{
					max = degree[i];
					node = i;
					check = true;
				}
			}
			if (check == false)
				break;
			createcluster(node);
		}
	}
	void createcluster(int node)
	{
		cluster* temp = new cluster;
		temp->next = NULL;
		temp->next_head = NULL;
		temp->nodes = 0;
		temp->edges = 0;
		temp->density = 1;
		computecluster(node,temp);
		temp->next_head = root->head;
		root->head = temp;
	}
	void computecluster(int node, cluster *mycluster)
	{
		vertex* temp = new vertex,* temp1;
		int reject=99999;
		mycluster->nodes++;
		temp->name = key[node];
		temp->location = node;
		temp->next = NULL;
		degree[node] = -1;
		for (int i = 0; i < num; i++)
			matrix[i][node] = 0;
		mycluster->next = temp;
		int max,val1=0,counter=0,val2=0;
		while (counter<num)
		{
			max = 0;
			temp = mycluster->next;
			while (temp)
			{
				for (int i = 0; i < num; i++)
				{
					if (matrix[temp->location][i] > max)
					{
						max = matrix[temp->location][i];
						val1 = i;
						val2 = temp->location;
					}
				}
				temp = temp->next;
			}
			if (degree[val1] > -1)
			{
				int check1 = computeedges(mycluster, val1);
				int check2 = mycluster->nodes + 1;
				float check = density(check1, check2);
				if (check >= threshold)
				{
					mycluster->density = check;
					mycluster->nodes++;
					mycluster->edges = check1;
					degree[val1] = -1;
					temp1 = new vertex;
					temp1->name = key[val1];
					temp1->location = val1;
					temp1->next = mycluster->next;
					mycluster->next = temp1;
					for (int i = 0; i < num; i++)
						matrix[i][val1] = -1;
				}
			}
			if (mycluster->density <= threshold)
				break;
			counter++;
		}
	}
	float density(int val1,int val)
	{
		float temp = (2 * float(val1)) / (float(val) * (float(val) - 1));
		return temp;
	}
	int computeedges(cluster* mycluster, int node)
	{
		vertex* temp = mycluster->next;
		int temp1 = mycluster->edges;
		while (temp)
		{
			if (matrix[temp->location][node] > 0)
				temp1++;
			temp = temp->next;
		}
		return temp1;
	}
public:
	graph(float density)
	{
		threshold = density;
		root = new Graph;
		root->head = NULL;
		num = 0;
		creategraph();
	}
	void display()
	{
		int counter=1;
		cluster *temp = root->head;
		vertex* temp1;
		while (temp)
		{
			temp1 = temp->next;
			cout << "\nCLUSTER " << counter<<"\nDensity: "<<temp->density<<"\nEdges: "<<temp->edges<<"\nNodes: "<<temp->nodes<<endl;
			while (temp1)
			{
				cout << temp1->name << "\n";
				temp1 = temp1->next;
			}
			temp = temp->next_head;
			counter++;
		}
	}
};

int main()
{
	float density;
	cout << "Enter the threshold density for the cluster: ";
	cin >> density;
	graph mygraph(density);
	mygraph.display();
}