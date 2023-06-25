#include<iostream>
#include<cmath>
#include<time.h>
#include<fstream>
#include<omp.h>
#include <algorithm>
#include <vector>
#include<cstdlib>


#define max_N 10000
#define N 3000

using namespace std;

template<typename T> std::vector<int> argsort(const std::vector<T>& array)
{
	const int array_len(array.size());
	std::vector<int> array_index(array_len, 0);
	for (int i = 0; i < array_len; ++i)
		array_index[i] = i;

	std::sort(array_index.begin(), array_index.end(),
		[&array](int pos1, int pos2) {return (array[pos1] < array[pos2]); });

	return array_index;
}

float d[max_N][max_N];
int result[max_N];
int res[N][max_N];
int tp1[N][max_N], tp2[N][max_N];
void f1(int n, float d[][max_N],int result[]) {
	//将随机shuffle生成的城市访问顺序排列作为初始解
	vector<int> vec;
	for (int i = 0; i < n; i++)
		vec.push_back(i);
	for (int i = 0; i < N; i++)
	{
		random_shuffle(vec.begin(), vec.end());
		for (int j = 0; j < n; j++)
		{
			res[i][j] = vec[j];
		}
	}
	//只有损失升序排列后的前N/2个解才能产生子代，先用一个vec存储之后对这个vec进行随机打乱
	vector<int> par;
	for (int i = 0; i < N/2; i++)
		par.push_back(i);
	vector<float> cost;
	vector<int> index;
	//进化evaluation_time次
	for (int evaluation_time = 0; evaluation_time < 3000; evaluation_time++)
	{
		//计算N个解的距离
		for (int i = 0; i < N; i++)
		{
			float s = 0;
			for (int j = 0; j < n-1; j++)
			{
				s += d[res[i][j]][res[i][j + 1]];
			}
			s += d[res[i][n - 1]][res[i][0]];
			cost.push_back(s);
		}
		index = argsort(cost);
		//随机打乱最好的N/2个解
		random_shuffle(par.begin(), par.end());
		//生成N/2个子代，每对父母生成两个子代
		for (int i = 0; i < N/2; i+=2)
		{
			//取出两个父母的index
			int l1 = index[par[i]], l2 = index[par[i + 1]];
			//随机选择一个基因交叉的位置，min(i1,i2)起max(i1,i2)止
			int i1 = rand() % n, i2 = rand() % n;
			if (i1 > i2)
			{
				int t = i1;
				i1 = i2;
				i2 = t;
			}
			//交叉过程视作将原本的数字全部赋予一个新的编号，依次使得交叉部分的每个数字t1的编号a变为配偶相应部分的数字t2，同时原先使用该编号t2的数字的编号变为a
			//先将初始化两个解l1,l2编号到数字，数字到编号的对应关系
			for (int j = 0; j < n; j++)
			{
				tp1[l1][j] = j;
				tp2[l1][j] = j;
				tp1[l2][j] = j;
				tp2[l2][j] = j;
			}
			//每个位点依次交叉
			for (int j = i1; j <= i2; j++)
			{
				//取出两个数字
				int t1 = res[l1][j], t2 = res[l2][j];
				int a,b;
				//获取t1目前的编号
				a = tp1[l1][t1];
				//将t1的编号改为t2
				tp1[l1][t1] = t2;
				//编号为t2的数字的编号改为t1目前的编号
				tp1[l1][tp2[l1][t2]] = a;
				b = tp2[l1][t2];
				tp2[l1][t2] = t1;
				tp2[l1][a] = b;
				a = tp1[l2][t2];
				tp1[l2][t2] = t1;
				tp1[l2][tp2[l2][t1]] = a;
				b = tp2[l2][t1];
				tp2[l2][t1] = t2;
				tp2[l2][a] = b;
			}
			int l3 = index[i + N / 2], l4 = index[i + 1 + N / 2];
			//将解中所有数字替换为相应编号，完成交叉
			for (int j = 0; j < n; j++)
			{
				res[l3][j] = tp1[l1][res[l1][j]];
				res[l4][j] = tp1[l2][res[l2][j]];
			}
		}
		cost.clear();
	}
	for (int i = 0; i < n; i++)
		result[i] = res[index[0]][i];
}

void f2(int n, float d[][max_N], int result[]) {
	vector<int> vec;
	for (int i = 0; i < n; i++)
		vec.push_back(i);
	for (int i = 0; i < N; i++)
	{
		random_shuffle(vec.begin(), vec.end());
		for (int j = 0; j < n; j++)
		{
			res[i][j] = vec[j];
		}
	}
	vector<int> par;
	for (int i = 0; i < N / 2; i++)
		par.push_back(i);
	vector<float> cost;
	vector<int> index;
	for (int evaluation_time = 0; evaluation_time < 3000; evaluation_time++)
	{
		for (int i = 0; i < N; i++)
		{
			float s = 0;
			for (int j = 0; j < n - 1; j++)
			{
				s += d[res[i][j]][res[i][j + 1]];
			}
			s += d[res[i][n - 1]][res[i][0]];
			cost.push_back(s);
		}
		index = argsort(cost);
		random_shuffle(par.begin(), par.end());
		//并行化交叉部分
		#pragma omp parallel for num_threads(2)
		for (int i = 0; i < N / 2; i += 2)
		{
			int l1 = index[par[i]], l2 = index[par[i + 1]];
			int i1 = rand() % n, i2 = rand() % n;
			if (i1 > i2)
			{
				int t = i1;
				i1 = i2;
				i2 = t;
			}
			for (int j = 0; j < n; j++)
			{
				tp1[l1][j] = j;
				tp2[l1][j] = j;
				tp1[l2][j] = j;
				tp2[l2][j] = j;
			}
			for (int j = i1; j <= i2; j++)
			{
				int t1 = res[l1][j], t2 = res[l2][j];
				int a, b;
				a = tp1[l1][t1];
				tp1[l1][t1] = t2;
				tp1[l1][tp2[l1][t2]] = a;
				b = tp2[l1][t2];
				tp2[l1][t2] = t1;
				tp2[l1][a] = b;
				a = tp1[l2][t2];
				tp1[l2][t2] = t1;
				tp1[l2][tp2[l2][t1]] = a;
				b = tp2[l2][t1];
				tp2[l2][t1] = t2;
				tp2[l2][a] = b;
			}
			int l3 = index[i + N / 2], l4 = index[i + 1 + N / 2];
			for (int j = 0; j < n; j++)
			{
				res[l3][j] = tp1[l1][res[l1][j]];
				res[l4][j] = tp1[l2][res[l2][j]];
			}
		}
		cost.clear();
	}
	for (int i = 0; i < n; i++)
		result[i] = res[index[0]][i];
}

int main()
{
	/*
	vector<float> vec;
	for (int i = 0; i < 10; i++)
		vec.push_back((float)i/2.0);
	random_shuffle(vec.begin(), vec.end());
	vector<int> index;
	index = argsort(vec);
	for (int i = 0; i < 10; i++)
	{
		cout << vec[i]<<' ';
	}
	cout << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << index[i] << ' ';
	}
	cout << endl;*/
	//输入各个距离
	ifstream in("data.txt");
	if (!in.is_open()) {
		cerr << "Failed to open file!" << endl;
		return -1;
	}
	int n;
	in >> n;
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			in >> d[i][j];
		}
	}
	//进化算法求解TSP
	time_t start, end;
	start = clock();
	f1 (n, d, result);
	end = clock();
	cout << "Time useage: " << (float)(end - start) / 1000 << " s" << endl;
	//给出总距离
	float s = 0;
	for (int i = 0; i < n - 1; i++)
	{
		s += d[result[i]][result[i + 1]];
	}
	s += d[result[n - 1]][result[0]];
	cout << s << endl;
	
}