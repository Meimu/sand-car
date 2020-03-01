#pragma once
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;

bool isNumber(char ch)
{
	return ch >= '0' && ch <= '9';
}

int str2num(string str)
{
	int res = 0;
	for (unsigned int i = 0; i < str.length(); i++)
		res *= 10, res += str[i] - '0';
	return res;
}

time_t str2time(string str)
{
	time_t res = 0;
	for (unsigned int i = 0; i < str.length(); i++)
		res *= 10, res += str[i] - '0';
	return res;
}

string num2str(int64_t i)
{
	stringstream ss;
	ss << i;
	return ss.str();
} 

string dou2str(double i)
{
	stringstream ss;
	ss.setf(ios::fixed);
	ss << setprecision(2) << i;
	return ss.str();
} 

string cha2str(char c)
{
	stringstream ss;
	ss << c;
	return ss.str();
}

int abs(int a)
{
	return a >= 0 ? a : -a;
}
