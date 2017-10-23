#include <bits/stdc++.h>
#include "SET_OPERLIB.h"
using namespace std;

WORD_SET SET;
vector<string> Train_label_list, Valid_label_list;
vector<vector<double> > Regression;
int Vs=0;

struct dis{
	double _dis;
	int index;
};

void READ_REGRESSION()
{
	fstream in1;
	in1.open("regression_dataset/train_set.csv",ios::in);
	string _strip,str;
	double data;
	char ch;
	getline(in1,_strip);
	while(getline(in1,str))
	{
		str=str.substr(str.find(",")+1);
		stringstream ss;
		ss << str;
		vector<double> array;
		Regression.push_back(array);
		for(int i=0;i<6;i++)
		{
			ss >> data >> ch;
			Regression[Regression.size()-1].push_back(data);
		}
	}
	in1.close();
}

void READ_SET()
{
	fstream in1,in2,out;
	in1.open("classification_dataset/train_set.csv",ios::in);
	in2.open("classification_dataset/test_set.csv",ios::in);
	
	
	out.open("TEST_SENTENDATA.txt",ios::out);
	string _strip,data;
	getline(in1,_strip);
	while(getline(in1,data))
	{
		string senten,label;
		senten = data.substr(0,data.find(","));
		label = data.substr(data.find(",")+1);
		out << senten << endl;
		Train_label_list.push_back(label);
	}
	getline(in2,_strip);
	while(getline(in2,data))
	{
		string senten,label;
		data=data.substr(data.find(",")+1);
		senten = data.substr(0,data.find(","));
		label = data.substr(data.find(",")+1);
		out << senten << endl;
		//Valid_label_list.push_back(label);
		Vs++;
	}
	SET.READ_DATA("TEST_SENTENDATA.txt");
	SET.GET_TF();
	SET.GET_TFIDF();
	
	in1.close();
	in2.close();
	out.close();
}

double get_dis(vector<double> a, vector<double> b ) //获得两个特征向量的距离 
{
	double dot=0,alen=0,blen=0;
	for(int i=0;i<a.size();i++)
	{
		dot += (a[i]*b[i]);
		alen += (a[i]*a[i]);
		blen += (b[i]*b[i]);
	}
	return (dot/(sqrt(alen)*sqrt(blen)));  //返回余弦距离 
} 

bool cmp(dis a,dis b)
{
	return a._dis>b._dis;
}

void VALID_ANALYSE()
{
	fstream out1,out2;
	out1.open("KNN_test_ans.csv",ios::out);
	out2.open("KNN_test_regre_ans.csv",ios::out);
	out1 << "textid,";
	out1 << "label" << endl;
	out2 << "textid,";
	out2 << "anger,disgust,fear,joy,sad,surprise" << endl;
	int Ts=Train_label_list.size();
	
	cout << Vs << endl;
	for(int i=Ts;i<Ts+Vs;i++)
	{ 
		//classification
		dis *__dis=new dis[Ts];
		for(int j=0;j<Ts;j++)
		{
			__dis[j]._dis=get_dis(SET.TFIDF[i],SET.TFIDF[j]);  //获取两个向量的距离
			__dis[j].index=j;  //存储在总表中位置
		} 
		
		sort(__dis,__dis+Ts,cmp);  //由于使用余弦距离，所以这里使用降序排序

		double *regre_res=new double[6];
		for(int j=0;j<6;j++)
		{
			regre_res[j]=0;   //初始化回归值 
		}
		
		double regre_sum=0;
		map<string,double> cnt;
		for(int j=0;j<13;j++) //k=13
		{
			cnt[Train_label_list[__dis[j].index]]+=__dis[j]._dis;       //分类计数，将余弦距离作为系数
			for(int k=0;k<6;k++)
			{
				regre_res[k]+=Regression[__dis[j].index][k]*__dis[j]._dis;   //同时进行回归值计算，
			}	                                  //将训练集概率乘上余弦距离，叠加到各个标签的概率值上
		}
		double max=0;
		string ans;
		for(map<string,double>::iterator iter=cnt.begin(); iter!=cnt.end(); iter++)  
		{
			if(iter->second >= max)
			{
				max=iter->second;
				ans=iter->first;     
			} //查找分类计数值最大的标签，并将之输出
		}
		out1 << i-Ts+1 << "," << ans << endl;
		
		
		//regression
		for(int k=0;k<6;k++)
		{
			regre_sum+=regre_res[k];
		}
		for(int k=0;k<6;k++)
		{
			if(regre_sum!=0)
			regre_res[k]=regre_res[k]/regre_sum;  
		}                              //回归值归一化
		out2 << i-Ts+1 << ",";
		for(int j=0;j<6;j++)
		{
			if(j!=0)
			out2 << ",";
			out2 << regre_res[j];
		}
		out2 << endl;
	}
	out1.close();
	out2.close();
}

int main()
{
	READ_SET();
	READ_REGRESSION();
	VALID_ANALYSE();
}
