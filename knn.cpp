// knn.cpp : 定义控制台应用程序的入口点。
//

#include<string>
#include<vector>
#include<map>
#include<set>
#include<fstream>
#include<sstream>
#include<cassert>
#include<cmath>
#include<iostream>
using namespace std;

struct sample
{
	string type;
	vector<double> features;
};

struct typeDistance
{
	string type;
	double distance;
};

bool operator <(const typeDistance & lhs, const typeDistance & rhs)
{
	return lhs.distance < rhs.distance;
}

void readTrain(vector<sample>& train, const string& file)
{
	ifstream fin(file.c_str());
	if (!fin)
	{
		cerr << "File error" << endl;
		exit(1);
	}

	string line;
	double d = 0.0;
	while (getline(fin, line))
	{
		istringstream sin(line);
		sample ts;
		sin >> ts.type;
		while (sin >> d)
		{
			ts.features.push_back(d);
		}
		train.push_back(ts);
	}
	fin.close();
}

void readTest(vector<sample>& test, const string& file)
{
	ifstream fin(file.c_str());
	if (!fin)
	{
		cerr << "File error!" << endl;
		exit(1);
	}

	double d = 0.0;
	string line;
	while (getline(fin, line))
	{
		istringstream sin(line);
		sample ts;
		sin >> ts.type;
		while (sin >> d)
		{
			ts.features.push_back(d);
		}
		test.push_back(ts);
	}
	fin.close();
}

double euclideanDistance(const vector<double>& v1, const vector<double>& v2)
{
	assert(v1.size() == v2.size());
	double ret = 0.0;
	for (vector<double>::size_type i = 0; i != v1.size(); i++)
	{
		ret += (v1[i] - v2[i])*(v1[i] - v2[i]);
	}
	return sqrt(ret);
}

void initDistanceMatrix(vector<vector<double>>& dm, const vector<sample>& train,  const vector<sample>& test)
{
	for (vector<sample>::size_type i = 0; i != test.size(); i++)
	{
		vector<double> vd;
		for (vector<sample>::size_type j = 0; j != train.size(); j++)
		{
			vd.push_back(euclideanDistance(test[i].features, train[j].features));
		}
		dm.push_back(vd);
	}

}

void knnProcess(const vector<sample>& test, const vector<sample>& train, const vector<vector<double>>& dm, unsigned  k,  vector<sample>& res)
{
	for (vector<sample>::size_type i = 0; i != test.size(); i++)
	{
		multimap<double, string> dts;
		for (vector<double>::size_type j = 0; j != dm[i].size(); j++)
		{
			if (dts.size() < k)
			{
				dts.insert(make_pair(dm[i][j], train[j].type));
			}
			else
			{
				multimap<double, string>::iterator it = dts.end();
				it--;
				if (dm[i][j] < it->first)
				{
					dts.erase(it);
					dts.insert(make_pair(dm[i][j], train[j].type));
				}
			}
		}
		map<string, double> tds;
		string type = "";
		double weight = 0.0;
		for (multimap<double, string>::const_iterator cit = dts.begin(); cit != dts.end(); cit++)
		{
			tds[cit->second] += 1.0 / cit->first;
			if (tds[cit->second] > weight)
			{
				weight = tds[cit->second];
				type = cit->second;
			}
		}
		res[i].type = type;
	}
}

void writeResult(const vector<sample>& result, const string& file)
{
	ofstream fout(file.c_str());
	if (!fout)
	{
		cerr << "file error!" << endl;
		exit(1);
	}

	for (vector<sample>::size_type i = 0; i != result.size(); i++)
	{
		fout << result[i].type << '\t';
		for (vector<double>::size_type j = 0; j != result[i].features.size(); j++)
		{
			fout << result[i].features[j] << ' ';
		}
		fout << endl;
	}
}

double calcuAccuracy(const vector<sample>& test, const vector<sample>& result)  //计算准确度，test为测试数据，真实值
{                                                                               //result为算法计算结果，实际值
	double count = 0.0;
	for (vector<sample>::size_type i = 0; i != test.size(); i++)
	if (test[i].type == result[i].type)
		count++;
	return count / test.size();
}




void knn(const string& file1, const string& file2, const string& file3, int k)
{
	vector<sample> train, test;
	vector<sample> result;
	readTrain(train, file1.c_str());
	readTest(test, file2.c_str());
	result = test;
	vector<vector<double>> dm;
	initDistanceMatrix(dm, train, test);
	knnProcess(test, train, dm, k, result);
	writeResult(result, file3.c_str());
	cout << "k = " << k  <<"  the accuracy is " << calcuAccuracy(test, result) << endl;
}

int main()
{
	for (int i = 1; i <= 20; i++)
		knn("letter.txt", "sum.txt", "res.txt", i*10);     // 计算k=10到200的准确度
	return 0;
}
