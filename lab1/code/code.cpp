#include<bits/stdc++.h>
using namespace std;

struct word{
	string content;
	int num; //单词出现次数 
	int index; //单词总表中的位置 
	word():content(""),num(0){}
	word(string _A,int i):content(_A),num(1),index(i){}
};

struct senten{
	vector<word> word_list;
	int length;  //有多少个单词 
	senten():length(0){}
};

struct Triple_point{
	int row,col,num;
	Triple_point(int r,int c,int n):row(r),col(c),num(n){}
};

vector<string> WORD; //总单词表 
vector<int> WORD_NUM; //单词出现在了多少句中 
vector<senten> SENTENCE_LIST;


void READ_DATA()
{
	fstream in;  
	string GET;
	in.open("semeval",ios::in);
	
	int i,j,k; 
	while(in){
		getline(in,GET);
		in.get();
		GET=GET.substr(GET.find("\t")+1);
		GET=GET.substr(GET.find("\t")+1);  //除去TAB,提取所需句子 
	    
	    string WORD_DATA;
		senten SENTEN_DATA; 
		int index;
	    stringstream ss;  //用字符串流分割单词 
	    ss << GET;
	    int COUNT=0;  //句子单词计数器 
	    while(ss >> WORD_DATA){
	    	COUNT++;
	    	if(find(WORD.begin(),WORD.end(),WORD_DATA)==WORD.end()) //WORD表中第一次出现
			{
				index = WORD.size();
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
					break; 
				}
			}
			if(i==SENTEN_DATA.word_list.size())   //当前句子没出现这个单词,插入到句子的单词表中 
			{
				SENTEN_DATA.word_list.push_back(word(WORD_DATA,index));
				WORD_NUM[index]++;    //单词出现在了一个新的句子，IDF计数++ 
			}
		}
	    SENTEN_DATA.length=COUNT;
	    SENTENCE_LIST.push_back(SENTEN_DATA);
	}
	in.close();
}

void GET_ONE_HOT()
{
	fstream out;
	out.open("onehot.txt",ios::out);
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
			if(j!=0)
			out << " ";
			
			out << bucket[j];
		}
		out << endl;
	}
	out.close();
}

void GET_TF()
{
	fstream out;
	out.open("tf.txt",ios::out);
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		int bucket[3000];                //桶存储，存储对应总表下标的单词在这个句子中的出现次数 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=SENTENCE_LIST[i].word_list[j].num;
		}
		for(int j=0;j<WORD.size();j++)
		{
			if(j!=0)
			out << " ";
			
			out << (double)bucket[j]/(double)(SENTENCE_LIST[i].length);
		}
		out << endl;
	}
	out.close();
}

void GET_TFIDF()
{
	fstream out;
	out.open("tfidf.txt",ios::out);
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		int bucket[3000];                //桶存储，存储对应总表下标的单词在这个句子中的出现次数 
		memset(bucket,0,sizeof(bucket));
		for(int j=0;j<SENTENCE_LIST[i].word_list.size();j++)   //遍历求解 
		{
			bucket[SENTENCE_LIST[i].word_list[j].index]=SENTENCE_LIST[i].word_list[j].num;
		}
		for(int j=0;j<WORD.size();j++)
		{
			if(j!=0)
			out << " ";
			
			out << ((double)bucket[j]/(double)(SENTENCE_LIST[i].length))*log( (double)SENTENCE_LIST.size()/(1+WORD_NUM[j]) ); //按公式求解tfidf 
		}
		out << endl;
	}
	out.close();
}

void GET_SMATRIX()
{
	fstream out;
	out.open("smatrix.txt",ios::out);
	out << "[" <<  SENTENCE_LIST.size() << "]" << endl;
	out << "[" << WORD.size() << "]" << endl;
	int smatrix_length=0;
	for(int i=0;i<SENTENCE_LIST.size();i++)
	{
		smatrix_length+=SENTENCE_LIST[i].word_list.size();
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

void AplusB()
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
	for(i=0;i<row1;i++)
		for(j=0;j<col1;j++)
		{
			if(i==A[aindex].row && j==A[aindex].col)
			{
				if(i==B[bindex].row && j==B[bindex].col)
				{
					ans.push_back(Triple_point(i,j,A[aindex].num+B[bindex].num));
					aindex++,bindex++;
				}
				else
				{
					ans.push_back(Triple_point(i,j,A[aindex].num));
					aindex++;
				}
			}
			else
			{
				if(i==B[bindex].row && j==B[bindex].col)
				{
					ans.push_back(Triple_point(i,j,B[bindex].num));
					bindex++;
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

int main()
{
	READ_DATA();
	GET_ONE_HOT();
	GET_TF();
	GET_TFIDF();
	GET_SMATRIX();
	AplusB();
}
