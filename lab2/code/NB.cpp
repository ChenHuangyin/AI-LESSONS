#include <bits/stdc++.h>
#include "SET_OPERLIB.h"
using namespace std;

WORD_SET SET,V_SET;
vector<string> Train_label_list, Valid_label_list;
vector<vector<double> > Regression;

map<string,double> label_cnt,s_label_cnt;
map<string,double> *word_cnt;

void READ_REGRESSION()
{
	fstream in1;
	in1.open("周四下午_NB/周四下午/train_set.csv",ios::in);
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
	fstream in1,in2,out1,out2;
	in1.open("周四下午_NB/周四下午/train_set.csv",ios::in);
	in2.open("周四下午_NB/周四下午/test_set.csv",ios::in);
	out1.open("TRAIN_SENTENDATA.txt",ios::out);
	out2.open("TEST_SENTENDATA.txt",ios::out);
	string _strip,data;
	getline(in1,_strip);
	while(getline(in1,data))
	{
		string senten,label;
		senten = data.substr(0,data.find(","));
		label = data.substr(data.find(",")+1);
		out1 << senten << endl;
		Train_label_list.push_back(label);
	}
	

	getline(in2,_strip);
	while(getline(in2,data))
	{
		string senten,label;
		data=data.substr(data.find(",")+1);
		senten = data.substr(0,data.find(","));
		label = data.substr(data.find(",")+1);
		out2 << senten << endl;
		Valid_label_list.push_back(label);
	}
	SET.READ_DATA("TRAIN_SENTENDATA.txt");
	SET.GET_TF();
	SET.GET_L_TF();
	SET.GET_TFIDF();
	V_SET.READ_DATA("TEST_SENTENDATA.txt");
	
	word_cnt = new map<string,double> [SET.WORD.size()];
	for(int i=0;i<SET.SENTENCE_LIST.size();i++)
	{
		SET.SENTENCE_LIST[i].label = Train_label_list[i];
		s_label_cnt[Train_label_list[i]]++;
		for(int j=0;j<SET.SENTENCE_LIST[i].word_list.size();j++)
		{
			word_cnt[SET.WORD_INDEX[SET.SENTENCE_LIST[i].word_list[j].content]][Train_label_list[i]]+=1;  // word_cnt[总表index][label]=cnt; 
		}
	}
	
	for(int i=0;i<Train_label_list.size();i++)
	{
		for(int j=0;j<SET.SENTENCE_LIST[i].word_list.size();j++)
		{
			
			if(label_cnt[Train_label_list[i]] == 0)
			label_cnt[Train_label_list[i]] = 1;
			else
			label_cnt[Train_label_list[i]]++;
		} 
		
	}
	in1.close();
	in2.close();
	out1.close();
	out2.close();
}

void VALID_ANALYSE()
{
	fstream out1,out2;
	out1.open("check_NB_test_ans.csv",ios::out);
	out2.open("check_NB_test_regre_ans.csv",ios::out);
	out1 << "textid,";
	out1 << "label" << endl;
	out2 << "textid,";
	out2 << "anger,disgust,fear,joy,sad,surprise" << endl;
	int Ts=Train_label_list.size();
	int Vs=Valid_label_list.size();
	string _labels[]={"anger","disgust","fear","joy","sad","surprise"};

	//classify
	for(int i=0;i<V_SET.SENTENCE_LIST.size();i++)
	{
		map<string,double> rating;
		
		for(int j=0;j<6;j++)
		{
			if(rating[_labels[j]] == 0)
			rating[_labels[j]] = 1;  //待累乘数归1
			
			for(int k=0;k<V_SET.SENTENCE_LIST[i].word_list.size();k++)
			{
				rating[_labels[j]]*=(double)( word_cnt[SET.WORD_INDEX[V_SET.SENTENCE_LIST[i].word_list[k].content]][_labels[j]]+1)/(double)(label_cnt[_labels[j]]+SET.WORD.size());
			}  //求取这个单词，在label相同训练集句子中的占这些句子总单词的比例
			rating[_labels[j]]*=s_label_cnt[_labels[j]]/(double)Ts;  //乘上这个label在训练集的比例
		}
		double max=0; 
		int maxi=0;
		for(int j=0;j<6;j++)
		{
			if(rating[_labels[j]]>max)
			{
				max=rating[_labels[j]];
				maxi=j;
			}
		}  //寻找并输出最大的rating值，并输出对应的label值
		out1 << i+1 << "," << _labels[maxi] << endl;
		out1 << "joy_Rate," << rating["joy"] << endl;
		out1 << "sad_Rate," << rating["sad"] << endl;
	}
	
	//regression
	for(int i=0;i<V_SET.SENTENCE_LIST.size();i++)
	{
		map<string,double> rating;
		
		for(int j=0;j<6;j++)
		{
			for(int p=0;p<SET.SENTENCE_LIST.size();p++)
			{
			    double tmp=1;
				for(int k=0;k<V_SET.SENTENCE_LIST[i].word_list.size();k++)
				{
					if(SET.WORD_INDEX[V_SET.SENTENCE_LIST[i].word_list[k].content]!=0)  //!important 忽略掉没出现过的单词
					tmp*=(SET.L_TF[p][SET.WORD_INDEX[V_SET.SENTENCE_LIST[i].word_list[k].content]]);  
					//累乘拉普拉斯平滑后的TF矩阵
				}
				rating[_labels[j]]+=tmp*Regression[p][j]; //最后乘上训练集对应回归值后累加到预测概率上
			}
		}
		double sum=0;
		for(int j=0;j<6;j++)
		{
			sum+=rating[_labels[j]]; //求和
		}
		out2 << i+1 << ",";
		for(int j=0;j<6;j++)
		{
			out2 << rating[_labels[j]]/sum << ",";
		}
		out2 << endl; //回归值归一化
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
