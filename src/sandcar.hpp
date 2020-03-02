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
#include <algorithm>
#include "ttools.hpp"

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

	int calcSTR()  //计算包括装备加成的力量
	{
		int tmp = STR;
		for (unsigned int i = 0; i < bag.size(); i++)
			if (bag[i].id == 2002) tmp += 1;
		return tmp;
	}
	int calcSAN()  //计算包括装备加成的理智
	{
		int tmp = SAN;
		for (unsigned int i = 0; i < bag.size(); i++)
			if (bag[i].id == 2001) tmp += 1;
		return tmp;
	}
	int calcDEX()  //计算包括装备加成的灵巧
	{
		int tmp = DEX;
		return tmp;
	}
	int calcINT()  //计算包括装备加成的智慧
	{
		int tmp = INT;
		return tmp;
	}
	int maxQuality()
	{
		return max(calcDEX(), calcINT(), calcSAN(), calcSTR());
	}
	void updata()
	{
		while (sanEXP >= 100)
			SAN++, sanEXP -= 100;
		while (intEXP >= 100)
			INT++, intEXP -= 100;
		while (dexEXP >= 100)
			DEX++, dexEXP -= 100;
		while (strEXP >= 100)
			STR++, strEXP -= 100;
	}
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
	string name = "";
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
	vector<string> passengers;  //在车上的人的QQ号（字符串）

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

void updataMemberInfo(Members m, string lpn, int64_t x)
{
	ofstream outFile;
	outFile.open(path + lpn + num2str(x) + ".csv", ios::out);
	outFile << "STR" << "," << m.STR << "," << m.strEXP << endl;
	outFile << "DEX" << "," << m.DEX << "," << m.dexEXP << endl;
	outFile << "INT" << "," << m.INT << "," << m.intEXP << endl;
	outFile << "SAN" << "," << m.SAN << "," << m.sanEXP << endl;
	outFile << "FATE" << "," << m.FATE << endl;
	outFile << "CP" << "," << m.CP << endl;
	outFile << "物品";
	for (unsigned int i = 0; i < m.bag.size(); i++)
		outFile << "," << m.bag[i].id;
	outFile << endl;
	outFile << m.fateTime << "," << m.funTime;
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
				cnt++, ct.passengers.push_back(str);
			ct.MemberNumber = cnt - 1;
		}
	}
	
	ct.updata();
	ct.flag = 1;

	return ct;
}

void updataCenterInfo(SandCar s, Centers c, string lpn)
{
	ofstream outFile;
	outFile.open(path + lpn + ".csv", ios::out);
	outFile << "车的修复度" << "," << s.repair << endl;
	outFile << "发动机" << "," << s.engine << endl;
	outFile << "燃料" << "," << s.fuel << endl;
	outFile << "车轮1" << "," << s.wheel_1 << endl;
	outFile << "车轮2" << "," << s.wheel_2 << endl;
	outFile << "车轮3" << "," << s.wheel_3 << endl;
	outFile << "车轮4" << "," << s.wheel_4 << endl;
	outFile << "离合器" << "," << s.clutch << endl;
	outFile << "变速箱" << "," << s.gearbox << endl;
	outFile << "传动轴" << "," << s.transmission << endl;
	outFile << "驱动桥" << "," << s.transaxle << endl;
	outFile << "车架" << "," << s.frame << endl;
	outFile << "前轴" << "," << s.frontAxle << endl;
	outFile << "悬架" << "," << s.suspension << endl;
	outFile << "转向操纵机构" << "," << s.steeringOperatingMechanism << endl;
	outFile << "转向器" << "," << s.steeringEquipment << endl;
	outFile << "转向传动机构" << "," << s.steeringLinkage << endl;
	outFile << "驾驶室" << "," << s.driverCab << endl;
	outFile << "车厢" << "," << s.boxcar << endl;
	outFile << "蓄电池" << "," << s.battery << endl;
	outFile << "照明设备" << "," << s.lightingEquipment << endl;
	outFile << "仪表" << "," << s.instrument << endl;

	outFile << "治安状况" << "," << c.publicSecurity << endl;
	outFile << "据点等级" << "," << c.level << endl;
	outFile << "科研等级" << "," << c.scientificResearch << endl;
	if (!c.invaders || c.inv.HP <= 0)
		outFile << "入侵者" << "," << "0" << "," << "0" << endl;
	else
		outFile << "入侵者" << "," << c.inv.name << "," << c.inv.HP << endl;
	outFile << "物品";
	for (unsigned int i = 0; i < c.bag.size(); i++)
		outFile << "," << c.bag[i].first.id << "," << c.bag[i].second;
	outFile << endl;
	outFile << "车上的乘客";
	for (unsigned int i = 1; i < c.passengers.size(); i++)
		outFile << "," << c.passengers[i];
	return;
}



const int MaxNumberadjA = 85;
const int MaxNumberadjB = 85;
const int MaxNumbern = 85;
const string adjA[100] = {
	"",
    "勇猛的", 
    "雷光的", 
    "愤怒的", 
    "智慧的", 
    "光耀的", 
    "沉睡的", 
    "迷路的", 
    "天启的", 
    "勤劳的", 
    "睿智的", 
    "厄运的", 
    "红色的", 
    "炮击制御的", 
    "斩铁的", 
    "古今无双的", 
    "愚笨的", 
    "环境适应的", 
    "瞬影的", 
    "神秘的", 
    "舍身的", 
    "不可见的", 
    "被选中的", 
    "姑息的", 
    "暴虐的", 
    "万年社畜的", 
    "永不放弃的", 
    "虔诚的", 
    "亚空间而来的", 
    "愤怒的", 
    "英杰的", 
    "魔力吸收的", 
    "月兔的", 
    "燃烧的", 
    "嗜血的", 
    "胆怯的", 
    "狂暴的", 
    "巨牙的", 
    "诡计多端的", 
    "回旋的", 
    "漏电的", 
    "厄运的", 
    "口蜜腹剑的", 
    "人面兽心的", 
    "仗势欺人的", 
    "协调的", 
    "温顺的", 
    "孤独的", 
    "坚强的", 
    "俊敏的", 
    "技艺娴熟的", 
    "无修正的", 
    "跳跃的", 
    "从母亲那收到的", 
    "从狗爪下逃脱的", 
    "超危险的", 
    "变得精神的", 
    "被称为英雄的", 
    "黄昏的", 
    "猎杀小猫的", 
    "潜藏在塔里的", 
    "防御者的", 
    "超糟糕的", 
    "服从混沌的", 
    "像恶魔一样的", 
    "爱上钓鱼的", 
    "倾国的", 
    "微笑的", 
    "一身黑色的", 
    "嗜血的", 
    "成为信徒的", 
    "处事不惊的", 
    "无瑕疵的", 
    "让人变得幸福的", 
    "准备计谋的", 
    "无法忘记的", 
    "可爱的", 
    "可口的", 
    "发狂的", 
    "沉溺于快乐之中的", 
    "寻找同伴的", 
    "罪人的", 
    "继承遗产的", 
    "让人哭出来的", 
    "微热的", 
    "无聊的"
};
const string adjB[100] = {
	"",
    "灾祸", 
    "命运", 
    "绯红", 
    "邪恶", 
    "生命", 
    "迷雾", 
    "天上天下", 
    "流水", 
    "寄生", 
    "复读机", 
    "高中生", 
    "死神", 
    "大结界师", 
    "宝剑", 
    "灭杀王", 
    "混乱", 
    "魔性之光", 
    "避矢", 
    "未知", 
    "连击的", 
    "长生", 
    "宝剑", 
    "幻想世界", 
    "地狱", 
    "永无休假的", 
    "难以受伤的", 
    "被绞刑", 
    "神之手", 
    "刺猬", 
    "大切断", 
    "念动力", 
    "精英幽灵", 
    "火焰", 
    "四臂", 
    "闭目", 
    "巨大", 
    "石肤", 
    "洛基", 
    "落单", 
    "雷电法王", 
    "倒霉", 
    "大王", 
    "饼干碎", 
    "黄牛党", 
    "公正", 
    "闪光", 
    "理想主义者", 
    "工具人", 
    "洁癖", 
    "失败者", 
    "毁灭", 
    "瞬间", 
    "幻影", 
    "有希望", 
    "重", 
    "虐杀", 
    "异邦", 
    "有勇气", 
    "爱情", 
    "午后", 
    "一", 
    "最棒", 
    "绿", 
    "明日", 
    "拿出精神", 
    "雄叫", 
    "巨汉", 
    "中立", 
    "恋爱", 
    "叫唤太多", 
    "腐烂", 
    "忘却", 
    "白色", 
    "雪", 
    "震撼", 
    "圣母", 
    "美人", 
    "白白", 
    "丧尸", 
    "恶臭", 
    "预言", 
    "怪人", 
    "永远", 
    "日暮", 
    "新星"
};
const string noun[100] = {
	"",
    "犀牛", 
    "暴龙", 
    "巨鹰", 
    "泡泡", 
    "水晶", 
    "食人花", 
    "堕落者淤泥", 
    "九头鸟", 
    "农场主", 
    "斯芬克斯", 
    "侦探", 
    "绞肉机", 
    "龙男巫", 
    "白虎", 
    "须佐之男", 
    "猪王", 
    "尸魂公主", 
    "库弗林", 
    "收藏者", 
    "混沌加农", 
    "暴虐之神", 
    "龙杀弓手", 
    "司法造天使", 
    "四驱车", 
    "艾尔", 
    "树妖", 
    "先知", 
    "苹果树妖", 
    "铠龙守卫", 
    "死神长", 
    "淑女贤者", 
    "狂骨", 
    "鱼人", 
    "伯格妖", 
    "美杜莎", 
    "隐形兽", 
    "巨蟒", 
    "使徒", 
    "鹌鸟", 
    "阿尔芒·法利埃", 
    "安德鲁·杰克逊", 
    "蜜蜂", 
    "偷猎者", 
    "大海牛", 
    "佛罗里达彩鹫", 
    "墨西哥灰熊", 
    "鼹鼠", 
    "军号手", 
    "神官", 
    "魔术师", 
    "乱交", 
    "傀儡", 
    "寨子", 
    "子弹", 
    "战车", 
    "圣子", 
    "绝对神", 
    "战士", 
    "地震", 
    "大公", 
    "族", 
    "悲剧", 
    "宝石", 
    "叛军", 
    "豹", 
    "拂晓", 
    "肉", 
    "儿子", 
    "雪月花", 
    "右手", 
    "人妻", 
    "麻药", 
    "鬼魂", 
    "病人", 
    "希望", 
    "公主", 
    "内脏", 
    "兄贵", 
    "时代", 
    "先辈", 
    "后代", 
    "尸体", 
    "笑", 
    "肉体", 
    "帝王"
};

Invader createInvaders(int x)
{
    int x1 = _random(MaxNumberadjA);
    int x2 = _random(MaxNumberadjB);
    int x3 = _random(MaxNumbern);
    Invader inv;
    inv.name = adjA[x1] + adjB[x2] + noun[x3];
    inv.HP = x1 + x2 + x3 + x;
    return inv;
}