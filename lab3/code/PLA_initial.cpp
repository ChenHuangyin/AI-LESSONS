#include <bits/stdc++.h>
using namespace std;

vector<vector<double> > train_set;
vector<vector<double> > test_set;
vector<int> labels;
vector<double> initial_weight;
vector<double> pocket_weight;
vector<int> val_labels;

void load_file()
{
	fstream in;
	in.open("train.csv",ios::in);
	string line_data;
	while(getline(in,line_data))
	{
		vector<double> row;
		row.push_back(1);
		bool is_end = false;
		while(line_data.length()!=0)
		{
			
			stringstream ss;
			double get;
			string tmp = line_data.substr(0,line_data.find(","));
			line_data = line_data.substr(line_data.find(",")+1);
			
			ss << tmp;
			ss >> get;
			row.push_back(get);
			
			if(is_end) break;
			if(line_data.find(",")==line_data.npos) is_end = true;
		}
		int tmp_label = row[row.size()-1];
		row.pop_back();
		labels.push_back(tmp_label);
		train_set.push_back(row);
	}
}

int sign(double x)
{
	if(x>0) return 1;
	else if(x<0) return -1;
	else return 0;
	
}

void init_weight()
{
	for(int i=0;i<train_set[0].size();i++)
	{
		initial_weight.push_back(1);
		pocket_weight.push_back(0);
	}
}

void initial_algro()
{
	int limit=8000;
	for(int i=0;i<limit;i++)
	{
		bool best_fit = false;
		for(int j=0;j<train_set.size();j++)
		{
			double sum=0;
			for(int k=0;k<train_set[j].size();k++)
			{
				sum+=initial_weight[k]*train_set[j][k];
			}
			if(sign(sum)!=labels[j])
			{
				for(int k=0;k<train_set[j].size();k++)
				{
					initial_weight[k]+=(double)labels[j]*train_set[j][k];
				}
				break;
			}
			else if(j==train_set.size()-1)
			best_fit = true;
		}
		if(best_fit) break;
	}
}

double pocket_count(vector<double> weight)
{
	int TP=0,FN=0,TN=0,FP=0;
	
	for(int i=0;i<train_set.size();i++)
	{
		double sum=0;
		for(int j=0;j<train_set[i].size();j++)
		{
			sum+=weight[j]*train_set[i][j];
		}
		if(sign(sum)==labels[i] && labels[i]==1) TP++;
		else if(sign(sum)==labels[i] && labels[i]==-1) TN++;
		else if(sign(sum)==1 && labels[i]==-1) FP++;
		else if(sign(sum)==-1 && labels[i]==1) FN++;
	}
	double Accuracy = (double)(TP+TN)/(double)(TP+FP+TN+FN);
	double Recall = (double)(TP)/(double)(TP+FN);
	double Precision = (double)(TP)/(double)(TP+FP);
	double F1 = 2*Precision*Recall/(Precision*Recall);
	return F1;
}

void pocket_algro()
{
	int limit=8000;
	vector<double> tmp_weight (pocket_weight);
	
	double cur=0;
	for(int i=0;i<limit;i++)
	{
		for(int j=0;j<train_set.size();j++)
		{
			double sum=0;
			for(int k=0;k<train_set[j].size();k++)
			{
				sum+=tmp_weight[k]*train_set[j][k];
			}
			if(sign(sum)!=labels[j])
			{
				for(int k=0;k<train_set[j].size();k++)
				{
					tmp_weight[k]+=(double)labels[j]*train_set[j][k];
				}
				if(pocket_count(tmp_weight) > cur)
				{
					pocket_weight.assign(tmp_weight.begin(),tmp_weight.end());
					
					cur = pocket_count(pocket_weight);
				}
				
				break;
			}	
		}
	}
}

void load_test()
{
	fstream in;
	in.open("test.csv",ios::in);
	string line_data;
	while(getline(in,line_data))
	{
		vector<double> row;
		bool is_end = false;
		row.push_back(1);
		while(line_data.length()!=0)
		{
			
			stringstream ss;
			double get;
			string tmp = line_data.substr(0,line_data.find(","));
			line_data = line_data.substr(line_data.find(",")+1);
			
			ss << tmp;
			ss >> get;
			row.push_back(get);
			
			if(is_end) break;
			if(line_data.find(",")==line_data.npos) is_end = true;
			if(line_data == "?") break;
		}
		test_set.push_back(row);
	}
}

void test(int mode)
{
	fstream out;
	out.open("test_result.csv",ios::out);
	
	for(int i=0;i<test_set.size();i++)
	{
		double sum=0;
		for(int j=0;j<test_set[i].size();j++)
		{
			if(mode == 0)
			sum += test_set[i][j]*initial_weight[j];
			else if(mode == 1)
			sum += test_set[i][j]*pocket_weight[j];
		}
		out << sign(sum) << endl;
	}
}

int main()
{
	load_file();
	init_weight();
	initial_algro();
//	pocket_algro();
	load_test();
	test(0);
}
