import numpy as np
import csv
def load_set(file):
    final_labelMat = []; trainMat = []
    fr=open(file)
    with fr as csvfile:
        rd = csv.reader(csvfile)
        for row in rd:
            trainMat.append(row[:-1])
            final_labelMat.append(row[-1])
    return final_labelMat,trainMat

def count_dif(trainMat,i):
    tmp = np.array(trainMat)
    label_list = []
    for j in range(tmp.shape[0]):
        label_list.append(trainMat[j][i])
    return (label_list)  

def gini(trainMat,labelMat,i):
    label_list = count_dif(trainMat,i)
    Gini_Set = set(label_list)
    Gini_Res = 0
    for counter in Gini_Set:
        pos_cnt = 0; neg_cnt = 0
        for i in range(len(label_list)):
            if label_list[i] == counter:
                if labelMat[i] == "1":
                    pos_cnt += 1
                elif labelMat[i] == "-1":
                    neg_cnt += 1
        pos_rate = (float(pos_cnt)/(float(pos_cnt+neg_cnt)))
        neg_rate = (float(neg_cnt)/(float(pos_cnt+neg_cnt)))
        Gini_Res += (float(label_list.count(counter))/float(len(label_list)))*(1-pos_rate*pos_rate-neg_rate*neg_rate)
    return Gini_Res

def solve():
    final_labelMat,trainMat=load_set("train.csv") 
    Gini_list = []
    for i in range(len(trainMat[0])):
        print ("第%d组属性的Gini指数为: %f" %(i,gini(trainMat,final_labelMat,i)))
        Gini_list.append(gini(trainMat,final_labelMat,i))
    print ("统计可得，第%d组的Gini指数最小,为%f，适宜作为根结点" %(Gini_list.index(min(Gini_list)),min(Gini_list)))

solve()
