#ifndef SET_OPERLIB
#define SET_OPERLIB

#include<bits/stdc++.h>
using namespace std;

struct word{
	string content;
	int num; //单词在该句中的出现次数 
	int index; //单词总表中的位置 
	string label;
	word():content(""),num(0){}
	word(string _A,int i):content(_A),num(1),index(i){}
};

struct senten{
	vector<word> word_list;
	int length;  //有多少个单词 
	string label;
	senten():length(0){}
};

struct Triple_point{
	int row,col,num;  //三元组行、列、值
	Triple_point(int r,int c,int n):row(r),col(c),num(n){}
};

class WORD_SET{
	public:
		WORD_SET():len(0){}
		int len;
		vector<string> WORD; //总单词表 
		vector<int> WORD_NUM; //总表对应下标的单词出现在了多少句中 
		map<string,int> WORD_INDEX; //总表索引 
		vector<senten> SENTENCE_LIST; //句子总表
		vector< vector<int> > ONE_HOT;
		vector< vector<double> > TF;
		vector< vector<double> > L_TF;
		vector< vector<double> > TFIDF;
		void READ_DATA(const string &file_loc);  //文件读取
		void GET_ONE_HOT();
		void GET_TF();
		void GET_L_TF();
		void GET_TFIDF();
		void GET_SMATRIX();
		void AplusB();
};


void WORD_SET::READ_DATA(const string &file_loc)  //文件读取
{
	fstream in;  
	string GET;
	in.open(file_loc.c_str(),ios::in);
	int i,j,k; 
	while(getline(in,GET)){    

	    string WORD_DATA;
		senten SENTEN_DATA; 
		int index;
	    stringstream ss;  //用字符串流分割单词 
	    ss << GET;
	    int COUNT=0;  //句子单词计数器 
	    while(ss >> WORD_DATA){
//	    	cout << WORD_DATA << endl;
//			system("pause");	
	    	COUNT++;
	    	if(find(WORD.begin(),WORD.end(),WORD_DATA)==WORD.end()) //WORD表中第一次出现
			{
				
				index = WORD.size();
				WORD_INDEX[WORD_DATA]=WORD.size();
				WORD.push_back(WORD_DATA); //插入WORD总表，用于计算One-Hot矩阵
				WORD_NUM.push_back(0);   //出现新单词，新建IDF计数器 
				
			}
			else
			{
				index = find(WORD.begin(),WORD.end(),WORD_DATA)-WORD.begin();
			} 
	    	
			for(i=0;i<SENTEN_DATA.word_list.size();i++)
			{
				if(SENTEN_DATA.word_list[i].content==WORD_DATA)  //当前句子出现过这个单词,此单词计数++ 
				{
					SENTEN_DATA.word_list[i].num++;
					len++;
					break; 
				}
			}
			if(i==SENTEN_DATA.word_list.size())   //当前句子没出现这个单词,插入到句子的单词表中 
			{
				SENTEN_DATA.word_list.push_back(word(WORD_DATA,index));
				WORD_NUM[index]++;    //单词出现在了一个新的句子，IDF计数++ 
				len++;
			}
		}
	    SENTEN_DATA.length=COUNT;
	    SENTENCE_LIST.push_back(SENTEN_DATA);
	}
	in.close();
}

void WORD_SET::GET_ONE_HOT()
{
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		vector<int> a;
		ONE_HOT.push_back(a);
		int bucket[3000];                //桶排列，将要输出1的位置标出 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=1;
		}
		for(int j=0;j<WORD.size();j++)
		{
			ONE_HOT[ONE_HOT.size()-1].push_back(bucket[j]);
		}
	}
}

void WORD_SET::GET_TF()
{
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		vector<double> a;
		TF.push_back(a);
		int bucket[3000];                //桶存储，存储对应总表下标的单词在这个句子中的出现次数 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=SENTENCE_LIST[i].word_list[j].num;
		}
		for(int j=0;j<WORD.size();j++)
		{
			TF[TF.size()-1].push_back((double)bucket[j]/(double)(SENTENCE_LIST[i].length));
			
		}
		
	}
}

void WORD_SET::GET_L_TF()
{
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		vector<double> a;
		L_TF.push_back(a);
		int bucket[3000];                //桶存储，存储对应总表下标的单词在这个句子中的出现次数 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=SENTENCE_LIST[i].word_list[j].num;
		}
		for(int j=0;j<WORD.size();j++)
		{
			L_TF[L_TF.size()-1].push_back( ((double)bucket[j]+0.009) / ((double)(SENTENCE_LIST[i].length)+0.009*(double)WORD.size()));
			//拉普拉斯平滑 
		}
		 
	}

}

void WORD_SET::GET_TFIDF()
{
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		vector<double> a;
		TFIDF.push_back(a);
		int bucket[3000];                //桶存储，存储对应总表下标的单词在这个句子中的出现次数 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=SENTENCE_LIST[i].word_list[j].num;
		}
		for(int j=0;j<WORD.size();j++)
		{
			TFIDF[TFIDF.size()-1].push_back((((double)bucket[j]/(double)(SENTENCE_LIST[i].length))*log((double)SENTENCE_LIST.size()/(1+WORD_NUM[j]))));
			//cout << (((double)bucket[j]/(double)(SENTENCE_LIST[i].length))*log((double)SENTENCE_LIST.size()/(1+WORD_NUM[j]))) << endl;
		}
	}
}

void WORD_SET::GET_SMATRIX()
{
	fstream out;
	out.open("smatrix.txt",ios::out);
	out << "[" <<  SENTENCE_LIST.size() << "]" << endl;
	out << "[" << WORD.size() << "]" << endl;
	int smatrix_length=0;
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		smatrix_length+=SENTENCE_LIST[i].word_list.size(); //将每个句子出现不同的单词数相加
	}
	out << "[" << smatrix_length << "]" << endl;
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		int bucket[3000];                //桶排列，将要输出1的位置标出 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=1;
		}
		for(int j=0;j<WORD.size();j++)
		{
			if(bucket[j]==1)
			{
				out << "[" << i << "," << j << ",1" << "]" << endl;
			}
		}
	}
	out.close();
}

void WORD_SET::AplusB()
{
	fstream in1,in2;
	fstream out;
	in1.open("matrix1.txt",ios::in);
	in2.open("matrix2.txt",ios::in);
	out.open("plus_output.txt",ios::out);
	
	int row1,row2,col1,col2,num1,num2,row,col;
	int i,j,k;
	
	vector<Triple_point> A,B,ans;
	in1 >> row1 >> col1 >> num1;
	in2 >> row2 >> col2 >> num2;
	while(in1 >> i >> j >> k)
	{
		A.push_back(Triple_point(i,j,k));
	}
	while(in2 >> i >> j >> k)
	{
		B.push_back(Triple_point(i,j,k));
	}
	in1.close();
	in2.close();
	int aindex=0,bindex=0;
	for(i=0;i<row1;i++)    //遍历所有可能的行列搭配
		for(j=0;j<col1;j++)
		{
			if(i==A[aindex].row && j==A[aindex].col)  
			{
				if(i==B[bindex].row && j==B[bindex].col)  //与矩阵A当前下标匹配也与矩阵B当前下标匹配
				{
					ans.push_back(Triple_point(i,j,A[aindex].num+B[bindex].num)); 
					//A、B对应值相加插入到结果矩阵ans中
					aindex++,bindex++; //两者下标都++
				}
				else
				{
					ans.push_back(Triple_point(i,j,A[aindex].num)); 
					//只有矩阵A当前下标匹配，将A此下标下的对应值插入到结果矩阵ans中
					aindex++; //A下标++
				}
			}
			else
			{
				if(i==B[bindex].row && j==B[bindex].col)
				{
					ans.push_back(Triple_point(i,j,B[bindex].num)); 
					//只有矩阵B当前下标匹配，将B此下标下的对应值插入到结果矩阵ans中
					bindex++; //B下标++
				}
			}
		}
	out << "[" <<  row1 << "]" << endl;
	out << "[" << col1 << "]" << endl;
	out << "[" <<  ans.size() << "]" << endl;
	
	for(i=0;i<ans.size();i++)
	out << "[" << ans[i].row << "," << ans[i].col << "," << ans[i].num << "]" << endl;
	out.close();
}

#endif
