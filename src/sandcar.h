#pragma once
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include "ttools.h"

using namespace std;

const string path = ".\\data\\SandCar\\";
const string licenseFile = path + "licenses.csv";

struct Items {
	int id = 0;  //物品id（识别码
	string name = "";  //物品名称
	string function = "";  //物品作用（描述
	int type = 0;  //物品种类，1为装备/2为消耗品

	Items(int Id = 0, string Name = "", string Function = "", int Type = 0)
	{
		this->id = Id, this->name = Name, this->function = Function, this->type = Type;
	}
};

const Items items2[1005] = {
	Items(1000, "", "", 0),
	Items(1001, "废铁", "普通的废铁，一切的基石", 2),
	Items(1002, "木板", "普通的木板，可以用来支撑一些东西", 2),
	Items(1003, "石头", "普通的石头，甚至可以扔出去", 2),
	Items(1004, "鸟肉", "不知名的渡渡鸟肉，可以充饥", 2),
	Items(1005, "植物根茎", "不知名的植物根茎，可以充饥", 2),
	Items(1006, "狗肉", "野狗的肉，煮熟之后很香", 2),
	Items(1007, "沙子", "沙漠中最常见的沙土，有很多功能", 2),
	Items(1008, "铁管", "一些器材的基础材料", 2),
	Items(1009, "小学课本", "小学的课本，有一些不认识的字", 2)
};

const Items items1[1005] = {
	Items(2000, "", "", 1),
	Items(2001, "心的证明", "SAN+1，证明你勇敢的心", 1),
	Items(2002, "鬼手", "STR+1，力量的延伸", 1),
};

struct Members {
	int STR = 40,  //力量
	    DEX = 40,  //灵巧
		INT = 40,  //智力
		SAN = 40;  //理智
	
	int strEXP = 0,  //力量经验
	    dexEXP = 0,  //灵巧经验
		intEXP = 0,  //智力经验
		sanEXP = 0;  //理智经验

	int FATE = 2;  //命运
	int CP = 0;  //贡献点数

	bool flag = 0;  //判断是否存在（是否上车

	time_t fateTime = 0;  //上次领取命运的时间
	time_t funTime = 0;   //上次操作的时间

	vector<Items> bag;  //携带的物品
};

struct SandCar {
	double repair = 0;  //总体修复度

	int engine = 0,  //发动机
		fuel = 0,    //燃料
		wheel_1 = 0, //车轮1
		wheel_2 = 0, //车轮2
		wheel_3 = 0, //车轮3
		wheel_4 = 0, //车轮4
		clutch = 0,  //离合器
		gearbox = 0, //变速箱
		transmission = 0,  //传动轴
		transaxle = 0, //驱动桥
		frame = 0,     //车架
		frontAxle = 0, //前轴
		suspension = 0,//悬架
		steeringOperatingMechanism = 0,  //转向操作机构
		steeringEquipment = 0,  //转向器
		steeringLinkage = 0,  //转向传动机构
		driverCab = 0, //驾驶室
		boxcar = 0,    //车厢
		battery = 0,   //蓄电池
		lightingEquipment = 0,  //照明设备
		instrument = 0;//仪表

	double engineRepair = 0;  //发动机类的总体修复度
	double	chassisRepair = 0;  //底盘类的总体修复度
	double	carBodyRepair = 0;  //车身类的总体修复度
	double	electricAccessoryRepair = 0;  //电气设备的总体修复度

	bool flag = 0;  //判断是否存在（是否初始化
	
	void updata()
	{
		repair = (engine + wheel_1 + wheel_2 + wheel_3 + wheel_4 + clutch
		         + gearbox + transmission + transaxle + frame + frontAxle
				 + suspension + steeringOperatingMechanism + steeringEquipment
				 + steeringLinkage + driverCab + boxcar + battery + lightingEquipment + instrument) / 20.0;
		engineRepair = engine;
		chassisRepair = (wheel_1 + wheel_2 + wheel_3 + wheel_4 + clutch + gearbox 
						+ transmission + transaxle + frame + frontAxle + suspension
						+ steeringOperatingMechanism + steeringEquipment + steeringLinkage) / 14.0;
		carBodyRepair = (driverCab + boxcar) / 2.0;
		electricAccessoryRepair = (battery + lightingEquipment + instrument) / 3.0;
	}

};

struct Invader {
	string name = "巨大野兽之灾难";
	int HP = 0;
};

struct Centers {
	int level = 0,  //据点等级
		scientificResearch = 0,  //科研等级
		publicSecurity = 50,  //治安状况
		MemberNumber = 0;  //据点中玩家数量
	int levelUpLimit = (level + 1) * 10;
	bool invaders = 0;  //是否存在入侵者
	Invader inv;  //存储入侵者的详细信息
	bool flag = 0;  //表明据点是否存在

	vector<pair<Items, unsigned int>> bag;  //物品及其数量

	void updata()
	{
		levelUpLimit = (level + 1) * 10;
	}
};

string getLicensePlateNumber(int64_t x)
{
	ifstream inFile;
	string licensePlateNumber;

	inFile.open(licenseFile, ios::in);
	string lineStr;
	while (getline(inFile, lineStr))
	{
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		while(getline(ss,str,','))
			lineArray.push_back(str);
		
		if(lineArray[1] == num2str(x))
		{
			licensePlateNumber = lineArray[0];
			break;
		} 
	}

	return licensePlateNumber;
}

Items createItem(int x)
{
	Items i;
	if (x < 1000 || x > 3000)
		return i;
	if (x < 2000)
		i = items2[x - 1000];
	else
		i = items1[x - 2000];
	return i;
}

Members getMemberInfo(string lpn, int64_t x)
{
	Members m;
	ifstream inFile;
	inFile.open(path + lpn + num2str(x) + ".csv", ios::in);
	string lineStr, str;
	vector<string> lineArray;
				
	for (int i = 1; i <= 8; i++)
	{
		getline(inFile, lineStr);
		stringstream ss(lineStr);
		if (i == 1)
		{
			if (ss.str() == "")
			{
				m.flag = 0;
				return m;
			}
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.STR = str2num(lineArray[lineArray.size() - 2]);
			m.strEXP = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 2)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.DEX = str2num(lineArray[lineArray.size() - 2]);
			m.dexEXP = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 3)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.INT = str2num(lineArray[lineArray.size() - 2]);
			m.intEXP = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 4)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.SAN = str2num(lineArray[lineArray.size() - 2]);
			m.sanEXP = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 5)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.FATE = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 6)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.CP = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 7)
		{
			vector<string> itemArray;
			while (getline(ss, str, ','))
				itemArray.push_back(str);
			for(unsigned int j = 1; j < itemArray.size(); j++)
				m.bag.push_back(createItem(str2num(itemArray[j])));
		}
		if (i == 8)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			m.fateTime = str2time(lineArray[lineArray.size() - 2]);
			m.funTime = str2time(lineArray[lineArray.size() - 1]);
		}
	}

	m.flag = 1;

	return m;
}

SandCar getCarInfo(string lpn)
{
	SandCar sc;
	ifstream inFile;
	inFile.open(path + lpn + ".csv", ios::in);
	string lineStr, str;
	vector<string> lineArray;
				
	for (int i = 1; i <= 22; i++)
	{
		getline(inFile, lineStr);
		stringstream ss(lineStr);
		if (i == 1)
		{
			if (ss.str() == "")
			{
				sc.flag = 0;
				return sc;
			}
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.repair = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 2)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.engine = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 3)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.fuel = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 4)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.wheel_1 = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 5)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.wheel_2 = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 6)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.wheel_3 = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 7)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.wheel_4 = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 8)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.clutch = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 9)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.gearbox = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 10)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.transmission = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 11)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.transaxle = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 12)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.frame = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 13)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.frontAxle = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 14)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.suspension = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 15)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.steeringOperatingMechanism = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 16)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.steeringEquipment = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 17)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.steeringLinkage = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 18)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.driverCab = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 19)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.boxcar = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 20)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.battery = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 21)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.lightingEquipment = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 22)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			sc.instrument = str2num(lineArray[lineArray.size() - 1]);
		}
	}

	sc.updata();
	sc.flag = 1;

	return sc;
}

Centers getCenterInfo(string lpn)
{
	Centers ct;
	ifstream inFile;
	inFile.open(path + lpn + ".csv", ios::in);
	string lineStr, str;
	vector<string> lineArray;
				
	for (int i = 1; i <= 22; i++)
	{
		getline(inFile, lineStr);
		if (i == 1)
		{
			stringstream ss(lineStr);
			if (ss.str() == "")
			{
				ct.flag = 0;
				return ct;
			}
		}
	}
	
	for (int i = 1; i <= 6; i++)
	{
		getline(inFile, lineStr);
		stringstream ss(lineStr);
		if (i == 1)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			ct.publicSecurity = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 2)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			ct.level = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 3)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			ct.scientificResearch = str2num(lineArray[lineArray.size() - 1]);
		}
		if (i == 4)
		{
			while (getline(ss, str, ','))
				lineArray.push_back(str);
			ct.inv.name = lineArray[lineArray.size() - 2];
			ct.inv.HP = str2num(lineArray[lineArray.size() - 1]);
			if (ct.inv.HP) ct.invaders = 1;
		}
		if (i == 5)
		{
			vector<string> itemArray;
			while (getline(ss, str, ','))
				itemArray.push_back(str);
			for (unsigned int j = 1; j < itemArray.size(); j += 2)
			{
				pair<Items, unsigned> pr;
				Items tmpi;
				tmpi = createItem(str2num(itemArray[j]));
				pr = make_pair(tmpi, str2num(itemArray[j + 1]));
				ct.bag.push_back(pr);
			}
		}
		if (i == 6)
		{
			int cnt = 0;
			while (getline(ss, str, ','))
				cnt++;
			ct.MemberNumber = cnt - 1;
		}
	}
	
	ct.updata();
	ct.flag = 1;

	return ct;
}