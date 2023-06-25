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
	//�����shuffle���ɵĳ��з���˳��������Ϊ��ʼ��
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
	//ֻ����ʧ�������к��ǰN/2������ܲ����Ӵ�������һ��vec�洢֮������vec�����������
	vector<int> par;
	for (int i = 0; i < N/2; i++)
		par.push_back(i);
	vector<float> cost;
	vector<int> index;
	//����evaluation_time��
	for (int evaluation_time = 0; evaluation_time < 3000; evaluation_time++)
	{
		//����N����ľ���
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
		//���������õ�N/2����
		random_shuffle(par.begin(), par.end());
		//����N/2���Ӵ���ÿ�Ը�ĸ���������Ӵ�
		for (int i = 0; i < N/2; i+=2)
		{
			//ȡ��������ĸ��index
			int l1 = index[par[i]], l2 = index[par[i + 1]];
			//���ѡ��һ�����򽻲��λ�ã�min(i1,i2)��max(i1,i2)ֹ
			int i1 = rand() % n, i2 = rand() % n;
			if (i1 > i2)
			{
				int t = i1;
				i1 = i2;
				i2 = t;
			}
			//�������������ԭ��������ȫ������һ���µı�ţ�����ʹ�ý��沿�ֵ�ÿ������t1�ı��a��Ϊ��ż��Ӧ���ֵ�����t2��ͬʱԭ��ʹ�øñ��t2�����ֵı�ű�Ϊa
			//�Ƚ���ʼ��������l1,l2��ŵ����֣����ֵ���ŵĶ�Ӧ��ϵ
			for (int j = 0; j < n; j++)
			{
				tp1[l1][j] = j;
				tp2[l1][j] = j;
				tp1[l2][j] = j;
				tp2[l2][j] = j;
			}
			//ÿ��λ�����ν���
			for (int j = i1; j <= i2; j++)
			{
				//ȡ����������
				int t1 = res[l1][j], t2 = res[l2][j];
				int a,b;
				//��ȡt1Ŀǰ�ı��
				a = tp1[l1][t1];
				//��t1�ı�Ÿ�Ϊt2
				tp1[l1][t1] = t2;
				//���Ϊt2�����ֵı�Ÿ�Ϊt1Ŀǰ�ı��
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
			//���������������滻Ϊ��Ӧ��ţ���ɽ���
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
		//���л����沿��
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
	//�����������
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
	//�����㷨���TSP
	time_t start, end;
	start = clock();
	f1 (n, d, result);
	end = clock();
	cout << "Time useage: " << (float)(end - start) / 1000 << " s" << endl;
	//�����ܾ���
	float s = 0;
	for (int i = 0; i < n - 1; i++)
	{
		s += d[result[i]][result[i + 1]];
	}
	s += d[result[n - 1]][result[0]];
	cout << s << endl;
	
}