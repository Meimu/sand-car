#include <iostream>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>

#include "ttools.h"
#include "rrandom.h"
#include "sandcar.h"

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

CQ_INIT {
    on_enable([] { logging::info("启用", "插件已启用"); });

    on_private_message([](const PrivateMessageEvent &e) {
        try {
            auto msgid = send_private_message(e.user_id, e.message); // 直接复读消息
            logging::info_success("私聊", "私聊消息复读完成, 消息 Id: " + to_string(msgid));
            send_message(e.target,
                         MessageSegment::face(111) + "这是通过 message 模块构造的消息~\n实际上只复读XD"); // 使用 message 模块构造消息
        } catch (ApiError &e) {
            logging::warning("私聊", "私聊消息复读失败, 错误码: " + to_string(e.code));
        }
    });

    /*
    on_message([](const MessageEvent &e) {
        logging::debug("消息", "收到消息: " + e.message + "\n实际类型: " + typeid(e).name());
    });
    */

    /**这是用来处理TRPG部分
	 * 目前已经包括了
	 * Call of Cthulhu
	 * Sword World
	 * Dungeons & Dragons
	 * */
    on_group_message([](const GroupMessageEvent &e) {
        auto message = e.message;//消息内容
        auto groupNumber = e.group_id;//群号	
        auto groupMemberInfo = get_group_member_info(groupNumber, e.user_id);
		string userNickname = groupMemberInfo.card == "" ? groupMemberInfo.nickname : groupMemberInfo.card;//使用者昵称
        if (message[0] != '.') return;

        //这里是今日人品
        /**
         * 这里的使用格式应该是：.jrrp
         * */
        if (message == ".jrrp")
        {
            string outstr = "* ";
            outstr += userNickname + "今天的人品值是：" + num2str(_random(100)) + "。";
            send_group_message(groupNumber, outstr);
			return;
        }


        //以下是有关Sword World有关的代码

        //首先是战斗伤害投掷
        /**
         * 这里的使用格式应该是：.sw XaY
         * */
        if (message.substr(1, 2) == "sw" && (message[3] == ' ' || isNumber(message[3])))
        {
            int tmp = 3, //当前字符串的位置
                ret = 0; //仅用于获取数字的时候
		    if (message[tmp] == ' ') tmp++;

		    int sw_x,      //伤害系数 
			    sw_b = 13; //暴击系数
		    for (ret = 0; isNumber(message[tmp]); tmp++)
			    ret++;

            string errorMessage = "* 好像……有哪里不太对劲";//用来返回错误的情况
		    if (!ret)
		    {
			    //返回X不为正确的数字的结果

			    send_group_message(groupNumber, errorMessage);
			    return;
		    }

		    if (ret) sw_x = str2num(message.substr(tmp - ret, ret));
		    if (sw_x < 0 || sw_x > 100)
		    {
			    //返回X在正确范围外的结果（0<=X<=100）

			    send_group_message(groupNumber, errorMessage);
			    return;
		    }

		    if (message[tmp] != 'a')
		    {
			    //返回没有正确输入a的结果

			    send_group_message(groupNumber, errorMessage);
			    return;
    		}
	    	tmp++; 

		    for (ret = 0; isNumber(message[tmp]); tmp++)
			    ret++;
	    	if (ret) sw_b = str2num(message.substr(tmp - ret, ret));
		    if (sw_b <= 6)
	    	{
		    	//返回Y在正确范围外的结果（7<=Y<=12）

			    send_group_message(groupNumber, errorMessage);
		    	return;
    		}

            //伤害表
		    int list[105][15] = {
    			{0,0,0,0,1,2,2,3,3,4,4},  //0
	    		{0,0,0,0,1,2,3,3,3,4,4},  //1
		    	{0,0,0,0,1,2,3,4,4,4,4},
			    {0,0,0,1,1,2,3,4,4,4,5},
    			{0,0,0,1,2,2,3,4,4,5,5},
	    		{0,0,1,1,2,2,3,4,5,5,5},  //5
		    	{0,0,1,1,2,3,3,4,5,5,5},
			    {0,0,1,1,2,3,4,4,5,5,6},
    			{0,0,1,2,2,3,4,4,5,5,6},
	    		{0,0,1,2,3,3,4,4,5,6,7},
		    	{0,1,1,2,3,3,4,5,5,6,7},  //10
			    {0,1,2,2,3,3,4,5,6,6,7},
			    {0,1,2,2,3,4,4,5,6,6,7},
    			{0,1,2,3,3,4,4,5,6,7,7},
	    		{0,1,2,3,4,4,4,5,6,7,8},
		    	{0,1,2,3,4,4,5,5,6,7,8},  //15
			    {0,1,2,3,4,4,5,6,7,7,8},
    			{0,1,2,3,4,5,5,6,7,7,8},
	    		{0,1,2,3,4,5,6,6,7,7,8},
		    	{0,1,2,3,4,5,6,7,7,8,9},
			    {0,1,2,3,4,5,6,7,8,9,10},  //20
    			{0,1,2,3,4,6,6,7,8,9,10},
    			{0,1,2,3,5,6,6,7,8,9,10},
	    		{0,2,2,3,5,6,7,7,8,9,10},
	    		{0,2,3,4,5,6,7,7,8,9,10},
		    	{0,2,3,4,5,6,7,8,8,9,10},  //25
			    {0,2,3,4,5,6,8,8,9,9,10},
    			{0,2,3,4,6,6,8,8,9,9,10},
	    		{0,2,3,4,6,6,8,9,9,10,10},
		    	{0,2,3,4,6,7,8,9,9,10,10},
			    {0,2,4,4,6,7,8,9,10,10,10},  //30
    			{0,2,4,5,6,7,8,9,10,10,11},
	    		{0,3,4,5,6,7,8,10,10,10,11},
		    	{0,3,4,5,6,8,8,10,10,10,11},
			    {0,3,4,5,6,8,9,10,10,11,11},
    			{0,3,4,5,7,8,9,10,10,11,12},  //35
	    		{0,3,5,5,7,8,9,10,11,11,12},
		    	{0,3,5,6,7,8,9,10,11,12,12},
			    {0,3,5,6,7,8,10,10,11,12,13},
    			{0,4,5,6,7,8,10,11,11,12,13},
	    		{0,4,5,6,7,9,10,11,11,12,13},  //40
		    	{0,4,6,6,7,9,10,11,12,12,13},
			    {0,4,6,7,7,9,10,11,12,13,13},
    			{0,4,6,7,8,9,10,11,12,13,14},
	    		{0,4,6,7,8,10,10,11,12,13,14},
		    	{0,4,6,7,9,10,10,11,12,13,14},  //45
			    {0,4,6,7,9,10,10,12,13,13,14},
			    {0,4,6,7,9,10,11,12,13,13,15},
    			{0,4,6,7,9,10,12,12,13,13,15},
	    		{0,4,6,7,10,10,12,12,13,14,15},
		    	{0,4,6,8,10,10,12,12,13,15,15},  //50
			    {0,5,7,8,10,10,12,12,13,15,15},
    			{0,5,7,8,10,11,12,12,13,15,15},
	    		{0,5,7,9,10,11,12,12,14,15,15},
		    	{0,5,7,9,10,11,12,13,14,15,16},
			    {0,5,7,10,10,11,12,13,14,16,16},  //55
    			{0,5,8,10,10,11,12,13,15,16,16},
	    		{0,5,8,10,11,11,12,13,15,16,17},
		    	{0,5,8,10,11,12,12,13,15,16,17},
			    {0,5,9,10,11,12,12,14,15,16,17},
    			{0,5,9,10,11,12,13,14,15,16,18},  //60
	    		{0,5,9,10,11,12,13,14,16,17,18},
		    	{0,5,9,10,11,13,13,14,16,17,18},
			    {0,5,9,10,11,13,13,15,17,17,18},
    			{0,5,9,10,11,13,14,15,17,17,18},
	    		{0,5,9,10,12,13,14,15,17,18,18},  //65
		    	{0,5,9,10,12,13,15,15,17,18,19},
			    {0,5,9,10,12,13,15,16,17,19,19},
    			{0,5,9,10,12,14,15,16,17,19,19},
	    		{0,5,9,10,12,14,16,16,17,19,19},
		    	{0,5,9,10,12,14,16,17,18,19,19},  //70
			    {0,5,9,10,13,14,16,17,18,19,20},
    			{0,5,9,10,13,15,16,17,18,19,20},
	    		{0,5,9,10,13,15,16,17,19,20,21},
		    	{0,6,9,10,13,15,16,18,19,20,21},
			    {0,6,9,10,13,16,16,18,19,20,21},  //75
    			{0,6,9,10,13,16,17,18,19,20,21},
	    		{0,6,9,10,13,16,17,18,20,21,22},
		    	{0,6,9,10,13,16,17,19,20,22,23},
			    {0,6,9,10,13,16,18,19,20,22,23},
    			{0,6,9,10,13,16,18,20,21,22,23},  //80
	    		{0,6,9,10,13,17,18,20,21,22,23},
		    	{0,6,9,10,14,17,18,20,21,22,24},
			    {0,6,9,11,14,17,18,20,21,23,24},
			    {0,6,9,11,14,17,19,20,21,23,24},
    			{0,6,9,11,14,17,19,21,22,23,24},  //85
	    		{0,7,10,11,14,17,19,21,22,23,25},
		    	{0,7,10,12,14,17,19,21,22,24,25},
			    {0,7,10,12,14,18,19,21,22,24,25},
    			{0,7,10,12,15,18,19,21,22,24,26},
	    		{0,7,10,12,15,18,19,21,23,25,26},  //90
		    	{0,7,11,13,15,18,19,21,23,25,26},
			    {0,7,11,13,15,18,20,21,23,25,27},
    			{0,8,11,13,15,18,20,22,23,25,27},
	    		{0,8,11,13,16,18,20,22,23,25,28},
		    	{0,8,11,14,16,18,20,22,23,26,28},  //95
			    {0,8,11,14,16,19,20,22,23,26,28},
    			{0,8,12,14,16,19,20,22,24,26,28},
	    		{0,8,12,15,16,19,20,22,24,27,28},
		    	{0,8,12,15,17,19,20,22,24,27,29},
			    {0,8,12,15,18,19,20,22,24,27,30}   //100
		    };

		    int diceres = 15,   //上一次骰子的结果
	    		damage = 0,     //伤害
	    	    dicer[2];       //两个骰子
            string outstr = "* ";
            outstr += userNickname + "投掷" + num2str(sw_x) + "a";
            outstr += sw_b == 15 ? "" : num2str(sw_b);
            outstr += "结果为：";

            while (diceres >= sw_b)
            {
                outstr += "\n";
                dicer[1] = _random(6);
                dicer[0] = _random(6);
                diceres = dicer[1] + dicer[0];
                damage += list[sw_x][diceres - 2];
                outstr += "2D6 = " + num2str(dicer[1]) + " + " + num2str(dicer[0]) + " = " + num2str(diceres) + " ";
                outstr += diceres >= sw_b ? "暴击，" : "没有暴击，";
                outstr += "伤害为：" + num2str(list[sw_x][diceres - 2]);
            }

            //判断是否有伤害加值
		    if (message[tmp] == ' ') tmp++;
	       	int bonus = 0;//伤害加值
	    	if (message[tmp] == '+' || message[tmp] == '-')
	    	{
	    		ret = 0;
	    		if (message[tmp] == '+') bonus = 1;
	    		else if (message[tmp] == '-') bonus = -1;
	    		tmp++;

	    		if (message[tmp] == ' ') tmp++;
		    	for (ret = 0; isNumber(message[tmp]); tmp++)
			    	ret++;
	    		if (ret) bonus *= str2num(message.substr(tmp - ret, ret));
	    	}
            
            outstr += "\n总伤害为：" + num2str(damage);
            if (!bonus) send_group_message(groupNumber, outstr);
            else
            {
                outstr += bonus > 0 ? " + " : " - ";
                outstr += num2str(abs(bonus)) + " = " + num2str(damage + bonus);
                send_group_message(groupNumber, outstr);
            }
            return;
        }

		//种族列表及天赋
        /**
         * 这里的使用格式应该是：.race
         * */
		if (message == ".race")
		{
			send_group_message(groupNumber, "https://docs.qq.com/sheet/DRFRqa1dSZnRyaHhi?c=M13A0E0");
			return;
		}

		//查询种族对应的出身和技能表
        /**
         * 这里的使用格式应该是：.job
         * */
		if (message == ".job")
		{
			send_group_message(groupNumber, "https://docs.qq.com/sheet/DRHplQWlmRWhiYnpw?c=I10A0E0");
			return;
		}

		//车卡投点
        /**
         * 这里的使用格式应该是：.sword [种族名] [出身职业]
         * */
		if (message == ".sword")
		{
			send_group_message(groupNumber, "* 输入：.sword [种族名] [出身职业] 完成投点");
			return;
		}
		if (message.substr(1, 5) == "sword")
		{
			int tmp = 6;
			if (message[tmp] == ' ') tmp++;
			string outstr = "* ";
			outstr += userNickname + " 的人物作成\n";

			//这是人类
			if (message.substr(tmp, 6) == "人类")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 8,   //技 
						body = 4,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;    //剩余经验值
					string job = "人类 魔动机师的初始职业是 魔动机师。\n";
					
					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";
					
					send_group_message(groupNumber, outstr);
					return;
				}
			
				//这是魔法师
				if (message.substr(tmp, 9) == "魔法师")
				{
					int skill = 7,   //技 
						body = 4,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 魔法师的初始职业是 魔术师or操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";
					
					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士
				if (message.substr(tmp, 9) == "轻战士")
				{
					int skill = 10,   //技 
						body = 7,    //体
						heart = 4;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 轻战士的初始职业是 斥候&轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是一般人
				if (message.substr(tmp, 9) == "一般人")
				{
					int skill = 7,   //技 
						body = 7,    //体
						heart = 7;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 3000;
					string job = "人类 一般人的初始职业是 无。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是佣兵（一）
				if (message.substr(tmp, 9) == "佣兵一")
				{
					int skill = 7,   //技 
						body = 10,    //体
						heart = 4;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 佣兵的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是神官（一）
				if (message.substr(tmp, 9) == "神官一")
				{
					int skill = 4,   //技 
						body = 8,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是妖精使
				if (message.substr(tmp, 9) == "妖精使")
				{
					int skill = 6,   //技 
						body = 6,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 妖精使的初始职业是 妖精使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是冒险者
				if (message.substr(tmp, 9) == "冒险者")
				{
					int skill = _random(6) + _random(6),   //技 
						body = _random(6) + _random(6),    //体
						heart = _random(6) + _random(6);   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 3000;
					string job = "人类 冒险者的初始职业是 无。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是斥候
				if (message.substr(tmp, 6) == "斥候")
				{
					int skill = 10,   //技 
						body = 6,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 斥候的初始职业是 斥候&轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是猎人
				if (message.substr(tmp, 6) == "猎人")
				{
					int skill = 9,   //技 
						body = 7,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 猎人的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是练体士
				if (message.substr(tmp, 9) == "练体士")
				{
					int skill = 8,   //技 
						body = 8,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 练体士的初始职业是 练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师
				if (message.substr(tmp, 9) == "魔术师")
				{
					int skill = 6,   //技 
						body = 5,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是拳斗士
				if (message.substr(tmp, 9) == "拳斗士")
				{
					int skill = 8,   //技 
						body = 9,    //体
						heart = 4;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 拳斗士的初始职业是 拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是佣兵（二）
				if (message.substr(tmp, 9) == "佣兵二")
				{
					int skill = 7,   //技 
						body = 10,    //体
						heart = 4;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 佣兵的初始职业是 战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是剑士
				if (message.substr(tmp, 6) == "剑士")
				{
					int skill = 9,   //技 
						body = 8,    //体
						heart = 4;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 剑士的初始职业是 轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是神官（二）
				if (message.substr(tmp, 9) == "神官二")
				{
					int skill = 5,   //技 
						body = 6,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是学者
				if (message.substr(tmp, 6) == "学者")
				{
					int skill = 5,   //技 
						body = 7,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 学者的初始职业是 学者。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";
					send_group_message(groupNumber, outstr);
					return;
				}

				//这是药师
				if (message.substr(tmp, 6) == "药师")
				{
					int skill = 9,   //技 
						body = 4,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 药师的初始职业是 游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";
					
					send_group_message(groupNumber, outstr);
					return;
				}

				//这是吟游诗人
				if (message.substr(tmp, 12) == "吟游诗人")
				{
					int skill = 7,   //技 
						body = 5,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 吟游诗人的初始职业是 吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是嗜好家
				if (message.substr(tmp, 9) == "嗜好家")
				{
					int skill = 8,   //技 
						body = 5,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 嗜好家的初始职业是 游侠&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金术师")
				{
					int skill = 6,   //技 
						body = 7,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 炼金术师的初始职业是 炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是军师
				if (message.substr(tmp, 6) == "军事")
				{
					int skill = 7,   //技 
						body = 8,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 军师的初始职业是 军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔法战士
				if (message.substr(tmp, 12) == "魔法战士")
				{
					int skill = 5,   //技 
						body = 8,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 1000;
					string job = "人类 魔法战士的初始职业是 战士&魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是占卜师
				if (message.substr(tmp, 9) == "占卜师")
				{
					int skill = 9,   //技 
						body = 3,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "人类 占卜师的初始职业是 占术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是参谋
				if (message.substr(tmp, 6) == "参谋")
				{
					int skill = 6,   //技 
						body = 5,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 参谋的初始职业是 学者&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔神使
				if (message.substr(tmp, 9) == "魔神使")
				{
					int skill = 3,   //技 
						body = 8,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "人类 魔神使的初始职业是 魔神使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是精灵
			else if (message.substr(tmp, 6) == "精灵")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是剑士
				if (message.substr(tmp, 6) == "剑士")
				{
					int skill = 12,   //技 
						body = 5,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "精灵 剑士的初始职业是 轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是射手
				if (message.substr(tmp, 6) == "射手")
				{
					int skill = 13,   //技 
						body = 5,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "精灵 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是妖精使
				if (message.substr(tmp, 9) == "妖精使")
				{
					int skill = 10,   //技 
						body = 4,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "精灵 妖精使的初始职业是 妖精使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师（一）
				if (message.substr(tmp, 12) == "魔术师一")
				{
					int skill = 10,   //技 
						body = 3,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是操灵术师
				if (message.substr(tmp, 12) == "操灵术师")
				{
					int skill = 9,   //技 
						body = 4,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 操灵术师的初始职业是 操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
				
				//这是神官
				if (message.substr(tmp, 6) == "神官")
				{
					int skill = 9,   //技 
						body = 5,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是药师
				if (message.substr(tmp, 6) == "药师")
				{
					int skill = 10,   //技 
						body = 5,    //体
						heart = 11;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 药师的初始职业是 学者&游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是练体士
				if (message.substr(tmp, 9) == "练体士")
				{
					int skill = 11,   //技 
						body = 5,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2500;
						string job = "精灵 练体士的初始职业是 练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是巡游者
				if (message.substr(tmp, 9) == "巡游者")
				{
					int skill = 12,   //技 
						body = 3,    //体
						heart = 11;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 巡游者的初始职业是 射手&游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师（二）
				if (message.substr(tmp, 12) == "魔术师二")
				{
					int skill = 10,   //技 
						body = 2,    //体
						heart = 14;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士
				if (message.substr(tmp, 6) == "战士")
				{
					int skill = 12,   //技 
						body = 6,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 战士的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是吟游诗人
				if (message.substr(tmp, 12) == "吟游诗人")
				{
					int skill = 11,   //技 
						body = 4,    //体
						heart = 11;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 吟游诗人的初始职业是 游侠&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是骑乘者
				if (message.substr(tmp, 9) == "骑乘者")
				{
					int skill = 12,   //技 
						body = 5,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2500;
						string job = "精灵 骑乘者的初始职业是 骑手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战奏者
				if (message.substr(tmp, 9) == "战奏者")
				{
					int skill = 10,   //技 
						body = 3,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 战奏者的初始职业是 吟游诗人&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是占卜师
				if (message.substr(tmp, 9) == "占卜师")
				{
					int skill = 12,   //技 
						body = 2,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2500;
						string job = "精灵 占卜师的初始职业是 占术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战军师
				if (message.substr(tmp, 9) == "战军师")
				{
					int skill = 12,   //技 
						body = 4,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 2000;
						string job = "精灵 战军师的初始职业是 妖精使&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔导师
				if (message.substr(tmp, 9) == "魔导师")
				{
					int skill = 9,   //技 
						body = 2,    //体
						heart = 15;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
						int EXP = 1000;
						string job = "精灵 魔导师的初始职业是 魔术师&操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是矮人
			else if (message.substr(tmp, 6) == "矮人")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是射手
				if (message.substr(tmp, 6) == "射手")
				{
					int skill = 6,   //技 
						body = 8,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2500;
					string job = "矮人 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士（一）
				if (message.substr(tmp, 9) == "战士一")
				{
					int skill = 4,   //技 
						body = 11,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 战士的初始职业是 战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是拳斗士
				if (message.substr(tmp, 9) == "拳斗士")
				{
					int skill = 5,   //技 
						body = 10,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 拳斗士的初始职业是 拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是神官（一）
				if (message.substr(tmp, 9) == "神官一")
				{
					int skill = 4,   //技 
						body = 7,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 6,   //技 
						body = 7,    //体
						heart = 7;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 魔动机师的初始职业是 魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是练体士
				if (message.substr(tmp, 9) == "练体士")
				{
					int skill = 5,   //技 
						body = 9,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2500;
					string job = "矮人 练体士的初始职业是 练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是吟游诗人
				if (message.substr(tmp, 12) == "吟游诗人")
				{
					int skill = 4,   //技 
						body = 8,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2500;
					string job = "矮人 吟游诗人的初始职业是 吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士（二）
				if (message.substr(tmp, 9) == "战士二")
				{
					int skill = 3,   //技 
						body = 12,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 战士的初始职业是 战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是神官（二）
				if (message.substr(tmp, 9) == "神官二")
				{
					int skill = 5,   //技 
						body = 6,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士
				if (message.substr(tmp, 9) == "轻战士")
				{
					int skill = 7,   //技 
						body = 8,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 轻战士的初始职业是 轻战士&斥候。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是占星术师
				if (message.substr(tmp, 12) == "占星术师")
				{
					int skill = 6,   //技 
						body = 7,    //体
						heart = 7;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2500;
					string job = "矮人 占星术师的初始职业是 占术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是枪械使
				if (message.substr(tmp, 9) == "枪械使")
				{
					int skill = 7,   //技 
						body = 6,    //体
						heart = 7;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 1500;
					string job = "矮人 枪械使的初始职业是 魔动机师&射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是嗜好家
				if (message.substr(tmp, 9) == "嗜好家")
				{
					int skill = 4,   //技 
						body = 8,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2000;
					string job = "矮人 嗜好家的初始职业是 学者&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是军师
				if (message.substr(tmp, 6) == "军师")
				{
					int skill = 7,   //技 
						body = 7,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 2500;
					string job = "矮人 军师的初始职业是 军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金战士")
				{
					int skill = 8,   //技 
						body = 7,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6) + 6,    //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6) + _random(6),        //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6),                     //知力
						MEN = _random(6) + _random(6) + 6;    //精神
					int EXP = 1500;
					string job = "矮人 炼金战士的初始职业是 战士&炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6+6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：1D6  =" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6+6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是塔比特
			else if (message.substr(tmp, 9) == "塔比特")
			{
				tmp += 9;
				if (message[tmp] == ' ') tmp++;

				//这是妖精使（一）
				if (message.substr(tmp, 12) == "妖精使一")
				{
					int skill = 7,   //技 
						body = 5,    //体
						heart = 10;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 妖精使的初始职业是 妖精使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是操灵术师
				if (message.substr(tmp, 12) == "操灵术师")
				{
					int skill = 6,   //技 
						body = 6,    //体
						heart = 10;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 操灵术师的初始职业是 操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师（一）
				if (message.substr(tmp, 12) == "魔术师一")
				{
					int skill = 5,   //技 
						body = 7,    //体
						heart = 10;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是学者
				if (message.substr(tmp, 6) == "学者") 
				{
					int skill = 5,   //技 
						body = 8,    //体
						heart = 9;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2500;
					string job = "塔比特 学者的初始职业是 学者。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 8,   //技 
						body = 5,    //体
						heart = 9;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 魔动机师的初始职业是 魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是吟游诗人
				if (message.substr(tmp, 12) == "吟游诗人")
				{
					int skill = 7,   //技 
						body = 7,    //体
						heart = 8;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2500;
					string job = "塔比特 吟游诗人的初始职业是 吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是药师
				if (message.substr(tmp, 6) == "药师")
				{
					int skill = 8,   //技 
						body = 6,    //体
						heart = 8;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2500;
					string job = "塔比特 药师的初始职业是 游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师（二）
				if (message.substr(tmp, 12) == "魔术师二")
				{
					int skill = 6,   //技 
						body = 5,    //体
						heart = 11;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是妖精使（二）
				if (message.substr(tmp, 12) == "妖精使二")
				{
					int skill = 4,   //技 
						body = 7,    //体
						heart = 11;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 妖精使的初始职业是 妖精使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是射手
				if (message.substr(tmp, 6) == "射手")
				{
					int skill = 8,   //技 
						body = 5,    //体
						heart = 9;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2500;
					string job = "塔比特 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔神使
				if (message.substr(tmp, 9) == "魔神使")
				{
					int skill = 7,   //技 
						body = 4,    //体
						heart = 1;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 魔神使的初始职业是 魔神使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是占卜师
				if (message.substr(tmp, 9) == "占卜师")
				{
					int skill = 9,   //技 
						body = 4,    //体
						heart = 9;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2500;
					string job = "塔比特 占卜师的初始职业是 占术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔导师
				if (message.substr(tmp, 9) == "魔导师")
				{
					int skill = 4,   //技 
						body = 6,    //体
						heart = 12;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 1000;
					string job = "塔比特 魔导师的初始职业是 魔术师&操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金术师")
				{
					int skill = 5,   //技 
						body = 6,    //体
						heart = 11;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 炼金术师的初始职业是 学者&炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是参谋
				if (message.substr(tmp, 6) == "参谋")
				{
					int skill = 9,   //技 
						body = 5,    //体
						heart = 8;   //心
					int DEX = _random(6),                     //灵巧
						AGI = _random(6),                     //敏捷
						STR = _random(6),                     //力量
						CON = _random(6) + _random(6),        //生命
						WIS = _random(6) + _random(6) + 6,    //知力
						MEN = _random(6) + _random(6);        //精神
					int EXP = 2000;
					string job = "塔比特 参谋的初始职业是 学者&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6  =" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6  =" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6  =" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6  =" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6+6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6  =" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是符民
			else if (message.substr(tmp, 6) == "符民")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是学者
				if (message.substr(tmp, 6) == "学者")
				{
					int skill = 8,   //技 
						body = 10,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 学者的初始职业是 学者。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是射手
				if (message.substr(tmp, 6) == "射手")
				{
					int skill = 12,   //技 
						body = 8,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士
				if (message.substr(tmp, 6) == "战士")
				{
					int skill = 9,   //技 
						body = 12,    //体
						heart = 5;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 战士的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 12,   //技 
						body = 8,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 魔动机师的初始职业是 魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师
				if (message.substr(tmp, 9) == "魔术师")
				{
					int skill = 9,   //技 
						body = 8,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是嗜好家
				if (message.substr(tmp, 9) == "嗜好家")
				{
					int skill = 8,   //技 
						body = 8,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 嗜好家的初始职业是 学者&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是练体士。
				if (message.substr(tmp, 9) == "练体士")
				{
					int skill = 10,   //技 
						body = 6,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 练体士的初始职业是 练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士
				if (message.substr(tmp, 6) == "战士")
				{
					int skill = 7,   //技 
						body = 11,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 战士的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士
				if (message.substr(tmp, 9) == "轻战士") 
				{
					int skill = 11,   //技 
						body = 9,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 轻战士的初始职业是 轻战士&斥候。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔法师
				if (message.substr(tmp, 9) == "魔法师")
				{
					int skill = 7,   //技 
						body = 9,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 魔法师的初始职业是 操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金术师")
				{
					int skill = 8,   //技 
						body = 9,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 炼金术师的初始职业是 炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是密探
				if (message.substr(tmp, 6) == "密探")
				{
					int skill = 12,   //技 
						body = 6,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2500;
					string job = "符民 密探的初始职业是 斥候&轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动战士
				if (message.substr(tmp, 12) == "魔动战士")
				{
					int skill = 7,   //技 
						body = 10,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 1000;
					string job = "符民 魔动战士的初始职业是 战士&魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是猎人
				if (message.substr(tmp, 6) == "猎人")
				{
					int skill = 13,   //技 
						body = 7,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 猎人的初始职业是 射手&游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔神使
				if (message.substr(tmp, 9) == "魔神使")
				{
					int skill = 7,   //技 
						body = 7,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6);                 //精神
					int EXP = 2000;
					string job = "符民 魔神使的初始职业是 魔神使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：1D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是梦魇
			else if (message.substr(tmp, 6) == "梦魇")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是魔术师
				if (message.substr(tmp, 9) == "魔术师")
				{
					int skill = 5,   //技 
						body = 13,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士（一）
				if (message.substr(tmp, 9) == "战士一")
				{
					int skill = 7,   //技 
						body = 15,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 战士的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士（一）
				if (message.substr(tmp, 12) == "轻战士一")
				{
					int skill = 11,   //技 
						body = 13,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 轻战士的初始职业是 轻战士or斥候。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是神官
				if (message.substr(tmp, 6) == "神官")
				{
					int skill = 6,   //技 
						body = 14,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 神官的初始职业是 神官。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 9,   //技 
						body = 9,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 魔动机师的初始职业是 魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是操灵术师
				if (message.substr(tmp, 12) == "操灵术师")
				{
					int skill = 5,   //技 
						body = 12,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 操灵术师的初始职业是 操灵术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是嗜好家
				if (message.substr(tmp, 9) == "嗜好家")
				{
					int skill = 10,   //技 
						body = 10,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 嗜好家的初始职业是 学者&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士（二）
				if (message.substr(tmp, 9) == "战士二")
				{
					int skill = 8,   //技 
						body = 13,    //体
						heart = 9;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 战士的初始职业是 战士or拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔术师
				if (message.substr(tmp, 9) == "魔术师")
				{
					int skill = 7,   //技 
						body = 10,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 魔术师的初始职业是 魔术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士（二）
				if (message.substr(tmp, 12) == "轻战士二")
				{
					int skill = 12,   //技 
						body = 12,    //体
						heart = 6;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 轻战士的初始职业是 轻战士&练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是参谋
				if (message.substr(tmp, 6) == "参谋")
				{
					int skill = 10,   //技 
						body = 7,    //体
						heart = 13;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 参谋的初始职业是 学者&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔神使
				if (message.substr(tmp, 9) == "魔神使")
				{
					int skill = 11,   //技 
						body = 8,    //体
						heart = 11;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 魔神使的初始职业是 魔神使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金术师")
				{
					int skill = 12,   //技 
						body = 8,    //体
						heart = 10;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 炼金术师的初始职业是 炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是猎人
				if (message.substr(tmp, 6) == "猎人")
				{
					int skill = 13,   //技 
						body = 9,    //体
						heart = 8;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "梦魇 猎人的初始职业是 射手&游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是占卜师
				if (message.substr(tmp, 9) == "占卜师")
				{
					int skill = 12,   //技 
						body = 6,    //体
						heart = 12;   //心
					int DEX = _random(6) + _random(6),    //灵巧
						AGI = _random(6) + _random(6),    //敏捷
						STR = _random(6),                 //力量
						CON = _random(6),                 //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "梦魇 占卜师的初始职业是 占术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：2D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：2D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：1D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：1D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			//这是暗影
			else if (message.substr(tmp, 6) == "暗影")
			{
				tmp += 6;
				if (message[tmp] == ' ') tmp++;

				//这是巡游者
				if (message.substr(tmp, 9) == "巡游者")
				{
					int skill = 15,   //技 
						body = 6,    //体
						heart = 6;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 巡游者的初始职业是 游侠。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是剑士
				if (message.substr(tmp, 6) == "剑士")
				{
					int skill = 17,   //技 
						body = 7,    //体
						heart = 3;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 剑士的初始职业是 轻战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是密探
				if (message.substr(tmp, 6) == "密探")
				{
					int skill = 16,   //技 
						body = 7,    //体
						heart = 4;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 密探的初始职业是 斥候。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是拳斗士
				if (message.substr(tmp, 9) == "拳斗士")
				{
					int skill = 14,   //技 
						body = 8,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 拳斗士的初始职业是 拳斗士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战士
				if (message.substr(tmp, 6) == "战士")
				{
					int skill = 15,   //技 
						body = 9,    //体
						heart = 3;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 战士的初始职业是 战士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是魔动机师
				if (message.substr(tmp, 12) == "魔动机师")
				{
					int skill = 17,   //技 
						body = 5,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 魔动机师的初始职业是 魔动机师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是射手（一）
				if (message.substr(tmp, 9) == "射手一")
				{
					int skill = 18,   //技 
						body = 6,    //体
						heart = 3;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是练体士
				if (message.substr(tmp, 9) == "练体士")
				{
					int skill = 16,   //技 
						body = 6,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 练体士的初始职业是 练体士。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是嗜好家
				if (message.substr(tmp, 9) == "嗜好家")
				{
					int skill = 14,   //技 
						body = 8,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 嗜好家的初始职业是 学者&吟游诗人。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是妖精使
				if (message.substr(tmp, 9) == "妖精使")
				{
					int skill = 13,   //技 
						body = 7,    //体
						heart = 7;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 妖精使的初始职业是 妖精使。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是射手（二）
				if (message.substr(tmp, 9) == "射手二")
				{
					int skill = 17,   //技 
						body = 6,    //体
						heart = 4;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 射手的初始职业是 射手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是战军师
				if (message.substr(tmp, 9) == "战军师")
				{
					int skill = 16,   //技 
						body = 8,    //体
						heart = 3;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 1500;
					string job = "暗影 战军师的初始职业是 战士&军师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是轻战士
				if (message.substr(tmp, 9) == "轻战士")
				{
					int skill = 17,   //技 
						body = 6,    //体
						heart = 4;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2000;
					string job = "暗影 轻战士的初始职业是 轻战士&斥候。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是骑兵
				if (message.substr(tmp, 6) == "骑兵")
				{
					int skill = 14,   //技 
						body = 8,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 骑兵的初始职业是 骑手。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}

				//这是炼金术师
				if (message.substr(tmp, 12) == "炼金术师")
				{
					int skill = 15,   //技 
						body = 7,    //体
						heart = 5;   //心
					int DEX = _random(6),                 //灵巧
						AGI = _random(6),                 //敏捷
						STR = _random(6) + _random(6),    //力量
						CON = _random(6) + _random(6),    //生命
						WIS = _random(6) + _random(6),    //知力
						MEN = _random(6) + _random(6);    //精神
					int EXP = 2500;
					string job = "暗影 炼金术师的初始职业是 炼金术师。\n";

					outstr += "技：" + num2str(skill) + "\n"
						   + "体：" + num2str(body) + "\n"
						   + "心：" + num2str(heart) + "\n"
						   + "灵巧：1D6=" + num2str(DEX) + "+" + num2str(skill) + "=" + num2str(DEX + skill) + "，加值为" + num2str((DEX + skill) / 6) + "\n"
						   + "敏捷：1D6=" + num2str(AGI) + "+" + num2str(skill) + "=" + num2str(AGI + skill) + "，加值为" + num2str((AGI + skill) / 6) + "\n"
						   + "力量：2D6=" + num2str(STR) + "+" + num2str(body) + "=" + num2str(STR + body) + "，加值为" + num2str((STR + body) / 6) + "\n"
						   + "生命：2D6=" + num2str(CON) + "+" + num2str(body) + "=" + num2str(CON + body) + "，加值为" + num2str((CON + body) / 6) + "\n"
						   + "知力：2D6=" + num2str(WIS) + "+" + num2str(heart) + "=" + num2str(WIS + heart) + "，加值为" + num2str((WIS + heart) / 6) + "\n"
						   + "精神：2D6=" + num2str(MEN) + "+" + num2str(heart) + "=" + num2str(MEN + heart) + "，加值为" + num2str((MEN + heart) / 6) + "\n"
						   + job + "剩余经验值" + num2str(EXP) + "点。";

					send_group_message(groupNumber, outstr);
					return;
				}
			}

			return;
		}


		//以下是普通骰子部分
		/**
		 * 这里使用的格式应该是：
		 * .r[投掷表达式](+ or - or * [投掷表达式])
		 * .rs[投掷表达式]
		 * .rh[投掷表达式]
		 * .r[投掷表达式]kN
		 * .r[投掷表达式]bN
		 * .r[投掷表达式]pN
		 * */
		if (message[1] == 'r' && (message[2] == 's' || message[2] == 'h' || isNumber(message[2]) || message[2] == 'd'))
		{
			int kind = 0;//投点的种类, rs为1, rh为2
			int tmp = 2, //当前字符串的位置
                ret = 0; //仅用于获取数字的时候
			string hiddenDiceMessage = "* ";
			hiddenDiceMessage += userNickname + "扔了一把隐藏的骰子，你们这些愚蠢的凡人是看不到的！";
			if (message[tmp] == ' ') tmp++;
			else if (message[tmp] == 's') tmp++, kind = 1;
			else if (message[tmp] == 'h') 
			{
				tmp++, kind = 2;
				send_group_message(groupNumber, hiddenDiceMessage);
			}

			if (message[tmp] == ' ') tmp++;

			int roll = 1,    //骰子个数
				dice = 100;  //骰子面数
			string errorMessage1 = "* 没有那么多骰子可以用了QAQ",  //骰子个数过多的错误信息
				   errorMessage2 = "* 这根本就是个球啊（摔）";     //骰子面数太多的错误信息
		    for (ret = 0; isNumber(message[tmp]); tmp++)
			    ret++;
	    	if (ret) roll = str2num(message.substr(tmp - ret, ret));
			if(roll > 50)
			{
				send_group_message(groupNumber, errorMessage1);

				return;
			}
			//取得了骰子的个数

			if (message[tmp] == 'd') tmp++;
			else return;
			
		    for (ret = 0; isNumber(message[tmp]); tmp++)
			    ret++;
	    	if (ret) dice = str2num(message.substr(tmp - ret, ret));
			if (dice > 1000)
			{
				send_group_message(groupNumber, errorMessage2);

				return;
			}
			//取得了骰子的面数

			int result[505];            //result[i]表示第i次投掷的最终结果
			vector<int> diceres[505];	//diceres[i][j]表示第i次投掷指令投掷的第j个骰子的结果
			bool tmpt[505];				//tmpt[i]用来标记第i个是否为投掷指令
			::memset(tmpt, 0, sizeof(tmpt));
			::memset(result, 0, sizeof(result));
			for (int i = 1; i <= roll; i++)
			{
				int resi = _random(dice);
				result[1] += resi;
				diceres[1].push_back(resi);
			}
			tmpt[1] = true;

			if (message[tmp] == 'k')
			{
				tmp++;
				for (ret = 0; isNumber(message[tmp]); tmp++)
					ret++;
				int kdice = roll;//实际要取的骰子的个数
				if (ret) kdice = min(roll, str2num(message.substr(tmp - ret, ret)));

				vector<int> tmp1 = diceres[1]; //用来临时储存第一次投掷结果

				int maxn = 0,      //当前能取得的骰子的最大值
					nowpos = 0,    //当前能取得的骰子的最大值的位置
					ans = 0;       //最终结果
				int pos[5005];     //最终取的骰子的位置
				for (int i = 1; i <= kdice; i++)
				{
					maxn = 0, nowpos = 0;
					for (unsigned int j = 0; j < tmp1.size(); j++)
						if (tmp1[j] > maxn)
						{
							maxn = tmp1[j];
							nowpos = j;
						}
					tmp1[nowpos] = 0;
					pos[i] = nowpos;
					ans += maxn;
				}

				if (kind == 1)
				{
					if (message[tmp] == ' ') tmp++;
					string tmpstr = message.substr(tmp, message.length() - tmp);
					string outstr = "* ";
					outstr += userNickname + "投掷" + tmpstr + "结果为：" + num2str(ans);
					send_group_message(groupNumber, outstr);

					return;
				}

				if (message[tmp] == ' ') tmp++;
				string tmpstr = message.substr(tmp, message.length());
				string outstr = "* ";
				outstr += userNickname + "投掷" + tmpstr + "结果为：[";
				for (unsigned int i = 0; i < tmp1.size() - 1; i++)
				{
					if (!tmp1[i])
						outstr += "(" + num2str(diceres[1][i]) + ")";
					else outstr += num2str(diceres[1][i]);
					outstr += " ";
				}
				if (!tmp1[diceres[1].size() - 1])
					outstr += "(" + num2str(diceres[1][diceres[1].size() - 1]) + ")";
				else outstr += num2str(diceres[1][diceres[1].size() - 1]);
				outstr += "] = " + num2str(ans);
				if(kind == 2)
				{
					send_private_message(e.user_id, outstr);

					return;
				}
				send_group_message(groupNumber, outstr);

				return;
			}

			if ((message[tmp] == 'p' || message[tmp] == 'b') && dice == 100)
			{
				int punishment = 0, bonus = 0;
				for (; message[tmp] == 'p' || message[tmp] == 'b'; )
				{
					if (message[tmp] == 'p')
					{
						tmp++;
						for (ret = 0; isNumber(message[tmp]); tmp++)
							ret++;
						if (ret) punishment += str2num(message.substr(tmp - ret, ret));
					}
					else if (message[tmp] == 'b')
					{
						tmp++;
						for (ret = 0; isNumber(message[tmp]); tmp++)
							ret++;
						if (ret) bonus += str2num(message.substr(tmp - ret, ret));
					}
				}

				if (message[tmp] == ' ') tmp++;
				string tmpstr = message.substr(tmp, message.length() - tmp);
				string outstr = "* ";
				outstr += userNickname + "投掷" + tmpstr + "结果为：";
				for (int i = 1; i <= roll; i++)
				{
					if (roll > 1) outstr += "\n";
					int units = _random(100);
					int tens  = units / 10;
					if (tens  == 10) tens  = 0;
					if (bonus == punishment)
					{
						outstr += num2str(units);

						continue;
					}

					int linshi[5005];     //存储临时的十位
					int linshires[5005];  //存储临时的结果
					for (int i = 1; i <= abs(bonus - punishment); i++)
					{
						linshi[i] = _random(10) - 1;
						linshires[i] = linshi[i] * 10 + units % 10;
						if (linshires[i] == 0) linshires[i] = 100;
					}

					linshires[abs(bonus - punishment) + 1] = units;
					linshi[abs(bonus - punishment) + 1] = tens;
					int ans = 0,  //最终结果
						now = 0;  //表示现在结果的位置
					if (bonus > punishment)
					{
						ans = 101;
						for (int ti = 1; ti <= abs(bonus - punishment) + 1; ti++)
							if (linshires[ti] < ans)
							{
								ans = linshires[ti];
								now = ti;
							}
					}
					if (bonus < punishment)
					{
						ans = 0;
						for (int i = 1; i <= abs(bonus - punishment) + 1; i++)
							if (linshires[i] > ans)
							{
								ans = linshires[i];
								now = i;
							}
					}
					if (kind == 1)
					{
						outstr += num2str(ans);

						continue;
					}

					outstr += "[";
					for (int i = 1; i <= abs(bonus - punishment); i++)
						if (i != now)
							outstr += num2str(linshi[i]) + " ";
						else if (i == now)
							outstr += "(" + num2str(linshi[i]) + ") ";

					if (now == abs(bonus - punishment) + 1)
						outstr += "(" + num2str(linshi[abs(bonus - punishment) + 1]) + ")";
					if (now != abs(bonus - punishment) + 1)
						outstr += num2str(linshi[abs(bonus - punishment) + 1]);
					outstr += "]*10 + " + num2str(units % 10) + " = [";

					for (int ti = 1; ti <= abs(bonus - punishment); ti++)
						if (ti != now)
							outstr += num2str(linshires[ti]) + " ";
						else if (ti == now)
							outstr += "(" + num2str(linshires[ti]) + ") ";

					if (now == abs(bonus - punishment) + 1)
						outstr += "(" + num2str(linshires[abs(bonus - punishment) + 1]) + ")";
					if (now != abs(bonus - punishment) + 1)
						outstr += num2str(linshires[abs(bonus - punishment) + 1]);
					outstr += "] = " + num2str(ans);
				}

				if(kind == 2) send_private_message(e.user_id, outstr);
				else send_group_message(groupNumber, outstr);

				return;
			}

			char tmpc[505];  //用来存储+-*符号
			for (int i = 1; i <= 500; i++)
			{
				if (message[tmp] == ' ') tmp++;
				if (message[tmp] == '+' || message[tmp] == '-' || message[tmp] == '*')
				{
					tmpc[i] = message[tmp];
					tmp++;
					if (message[tmp] == ' ') tmp++;
					for (ret = 0; isNumber(message[tmp]); tmp++)
						ret++;
					roll = 1;
					if (ret) roll = str2num(message.substr(tmp - ret, ret));
					if (message[tmp] != 'd')
					{
						result[i + 1] = roll;
						continue;
					}
					else
					{
						tmp++;
						for (ret = 0; isNumber(message[tmp]); tmp++)
							ret++;
						dice = 100;
						if (ret) dice = str2num(message.substr(tmp - ret, ret));
						result[i + 1] = 0;
						for (int j = 1; j <= roll; j++)
						{
							int ttt = _random(dice);
							result[i + 1] += ttt;
							diceres[i + 1].push_back(ttt);
						}
						tmpt[i + 1] = true;
					}
				}
				else
				{
					string tmpstr = message.substr(tmp, message.length() - tmp);
					string outstr = "* ";
					outstr += userNickname + "投掷" + tmpstr + "结果为：";
					for (int j = 1; j <= i && i != 1; j++)
					{
						if (tmpt[j])
						{
							outstr += "[";
							for (unsigned int k = 0; k < diceres[j].size() - 1; k++)
								outstr += num2str(diceres[j][k]) + "+";
							outstr += num2str(diceres[j][diceres[j].size() - 1]) + "](" + num2str(result[j]) + ")";
						}
						else
							outstr += num2str(result[j]);
						if (j != i)
							outstr += " " + cha2str(tmpc[j]) + " ";
					}
					if (i != 1)
						outstr += " = ";
					if (diceres[i].size() - 1 && i == 1)
						{
							for (unsigned int k = 0; k < diceres[i].size() - 1; k++)
								outstr += num2str(diceres[i][k]) + "+";
							outstr += num2str(diceres[i][diceres[i].size() - 1]);
							outstr += " = ";
						}

					if (kind == 1)
					{
						outstr = "* ";
						outstr += userNickname + "投掷" + tmpstr + "结果为：";
					}

					for (int j = 1; j < i; j++)
						if (tmpc[j] == '*')
						{
							tmpc[j] = '+';
							result[j] *= result[j + 1];
							result[j + 1] = 0;
						}
					int ans = result[1];
					for (int j = 1; j <= i; j++)
					{
						if (tmpc[j] == '+')
							ans += result[j + 1];
						if (tmpc[j] == '-')
							ans -= result[j + 1];
					}
					outstr += num2str(ans);
					if (kind == 2) send_private_message(e.user_id, outstr);
					else send_group_message(groupNumber, outstr);

					return;
				}
			}
		}


		//以下是Call of Cthulhu有关的代码

		//首先是技能检定
		/**
		 * 这里使用的格式是：.ra N [技能名称]
		 * */
		if (message.substr(1, 2) == "ra")
		{
			int tmp = 3, //当前字符串的位置
                ret = 0; //仅用于获取数字的时候
			string errorMessage = "* 这里的使用格式应该是 .ra [成功率] [技能名称]！";//格式使用错误的返回信息
			if (message[tmp] == ' ') tmp++;
			
			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			if (!ret) 
			{
				send_group_message(groupNumber, errorMessage);
				return;
			}

			int skill = 0; //技能成功率
			skill = str2num(message.substr(tmp - ret, ret));
			if (!skill)
			{
				send_group_message(groupNumber, errorMessage);

				return;
			}

			int rollDice = _random(100);  //投掷D100的结果
			
			string outstr = "* ";
			outstr += userNickname + "投掷D100结果为：" + num2str(rollDice) + "\n";
			
			if (message[tmp] == ' ') tmp++;
			string tmpstr = message.substr(tmp, message.length() - tmp);
			if (rollDice <= skill)
			{
				if (rollDice <= skill / 5)
					outstr += "技能 " + tmpstr + " 极难成功。";
				else if (rollDice <= skill / 2)
					outstr += "技能 " + tmpstr + " 困难成功。";
				else 
					outstr += "技能 " + tmpstr + " 通过检定。";
			}
			else outstr += "技能 " + tmpstr + " 未通过检定。";

			send_group_message(groupNumber, outstr);
			return;
		}

		//幕间/技能成长
		/**
		 * 这里的使用格式是：.en N [技能名称]
		 * */
		if (message.substr(1, 2) == "en")
		{
			int tmp = 3, //当前字符串的位置
                ret = 0; //仅用于获取数字的时候
			string errorMessage = "* 这里的使用格式应该是 .en [成功率] [技能名称]！";//格式使用错误的返回信息
			if (message[tmp] == ' ') tmp++;

			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			if (!ret)
			{
				send_group_message(groupNumber, errorMessage);
				return;
			}

			int skill = str2num(message.substr(tmp - ret, ret)); //成长前技能成功率
			int rollDice = _random(100);  //投掷D100结果
			if (message[tmp] == ' ') tmp++;
			string tmpstr = message.substr(tmp, message.length());

			string outstr = "* ";
			outstr += userNickname + "投掷D100结果为：" + num2str(rollDice) + "\n";
			if (rollDice <= skill && rollDice <= 95)
				outstr += "技能 " + tmpstr + " 未通过成长检定。";
			else
			{
				outstr += "技能 " + tmpstr += "通过成长检定。\n";
				int en = _random(10); //技能成长的点数
				outstr += "成长后点数为：" + num2str(skill) + " + " + num2str(en) + " = " + num2str(skill + en);
			}

			send_group_message(groupNumber, outstr);
			return;
		}
		
		//SanCheck, SanCheck!
		/**
		 * 这里的使用格式是：.sc [SanCheck表达式] N
		 * */
		if (message.substr(1, 2) == "sc")
		{
			int tmp = 3, //当前字符串的位置
                ret = 0, //仅用于获取数字的时候
				san = 0; //SanCheck前的SAN值
			string errorMessage1 = "* 这里的使用格式应该是 .sc [SanCheck表达式] [理智值]！",//格式使用错误的返回信息
			       errorMessage2 = "* SacCheck表达式的格式应该是 [投掷表达式(数字)]/[投掷表达式(数字)]！";//SanCheck表达式错误返回的信息
			if (message[tmp] == ' ') tmp++;

			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			if (!ret)
			{
				send_group_message(groupNumber, errorMessage1);
				return;
			}

			int roll1 = 1,   //理智检定成功时
				dice1 = 1;   //理智检定成功时
			roll1 = str2num(message.substr(tmp - ret, ret));
			if (message[tmp] == '/') tmp++;
			else if(message[tmp] == 'd')
			{
				tmp++;
				for (ret = 0; isNumber(message[tmp]); tmp++)
					ret++;
				if (!ret)
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}
				dice1 = str2num(message.substr(tmp - ret, ret));
			}
			else
			{
				send_group_message(groupNumber, errorMessage1);
				return;
			}
			//send_group_message(groupNumber, num2str(roll1) + " " + num2str(dice1));

			int roll2 = 1,   //理智检定失败时
				dice2 = 1;   //理智检定失败时
			tmp++;
			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			if (ret) roll2 = str2num(message.substr(tmp - ret, ret));
			if (message[tmp] == 'd')
			{
				tmp++;
				for (ret = 0; isNumber(message[tmp]); tmp++)
					ret++;
				if (!ret)
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}
				dice2 = str2num(message.substr(tmp - ret, ret));
			}
			//send_group_message(groupNumber, num2str(roll2) + " " + num2str(dice2));
			if(message[tmp] == ' ') tmp++;

			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			if (!ret)
			{
				send_group_message(groupNumber, errorMessage1);
				return;
			}
			san = str2num(message.substr(tmp - ret, ret));

			int sancheck = _random(100);//SanCheck投掷D100值
			string outstr = "* ";
			outstr += userNickname + "投掷D100结果为：" + num2str(sancheck) + "，";
			if (sancheck <= san)
			{
				outstr += "通过检定。\n";
				int res = 0;
				for (int i = 1; i <= roll1; i++)
					res += _random(dice1);
				outstr += "损失SAN值：" + num2str(res) + "点，剩余" + num2str(san - res) + "点";
			}
			else
			{
				outstr += "未通过检定。\n";
				int res = 0;
				for (int i = 1; i <= roll2; i++)
					res += _random(dice2);
				outstr += "损失SAN值：" + num2str(res) + "点，剩余" + num2str(san - res) + "点";
			}
			
			send_group_message(groupNumber, outstr);
			return;
		}

		//快捷投掷任务卡（不包含幸运）
		/**
		 * 这里的使用格式是：.coc7 (N)
		 * */
		if (message.substr(1, 4) == "coc7")
		{
			int tmp = 5,  //当前字符串的位置
				ret = 0;  //仅用于获取数字的时候
			if (message[tmp] == ' ') tmp++;
			string errorMessage = "* 啊好多人物卡欸……已经拿不下了！";//投掷人物卡过多的返回值

			for (ret = 0; isNumber(message[tmp]); tmp++)
				ret++;
			int number = 1; //投掷人物卡的数目
			if (ret) number = str2num(message.substr(tmp - ret, ret));
			if (number > 10)
			{
				send_group_message(groupNumber, errorMessage);
				return;
			}

			int STR, //力量
				CON, //体质
				DEX, //体型
				APP, //外貌
				POW, //意志
			    SIZ, //体型
				EDU, //教育
				INT; //智力
			string outstr = "* ";
			outstr += userNickname + "进行投掷\n";
			if (number > 1)
			{
				for (int i = 1; i <= number; i++)
				{
					STR = _random(6) + _random(6) + _random(6), STR *= 5;
					CON = _random(6) + _random(6) + _random(6), CON *= 5;
					DEX = _random(6) + _random(6) + _random(6), DEX *= 5;
					APP = _random(6) + _random(6) + _random(6), APP *= 5;
					POW = _random(6) + _random(6) + _random(6), POW *= 5;
					SIZ = _random(6) + _random(6) + 6, SIZ *= 5;
					EDU = _random(6) + _random(6) + 6, EDU *= 5;
					INT = _random(6) + _random(6) + 6, INT *= 5;
					outstr += "力量：" + num2str(STR) + "  敏捷：" + num2str(DEX) + "  智力：" + num2str(INT);
					outstr += "  体质：" + num2str(CON) + "  外貌：" + num2str(APP) + "  意志：" + num2str(POW);
					outstr += "  体型：" + num2str(SIZ) + "  教育：" + num2str(EDU) + "\n";
					outstr += "八大属性总和：" + num2str(STR + CON + DEX + APP + POW + SIZ + EDU + INT) + "\n";
					outstr += "- - - - - - - - - -\n";
				}
			}
			else if (number == 1)
			{
				STR = _random(6) + _random(6) + _random(6), STR *= 5;
				CON = _random(6) + _random(6) + _random(6), CON *= 5;
				DEX = _random(6) + _random(6) + _random(6), DEX *= 5;
				APP = _random(6) + _random(6) + _random(6), APP *= 5;
				POW = _random(6) + _random(6) + _random(6), POW *= 5;
				SIZ = _random(6) + _random(6) + 6, SIZ *= 5;
				EDU = _random(6) + _random(6) + 6, EDU *= 5;
				INT = _random(6) + _random(6) + 6, INT *= 5;
				outstr += "力量：" + num2str(STR) + "  敏捷：" + num2str(DEX) + "  智力：" + num2str(INT);
				outstr += "  体质：" + num2str(CON) + "  外貌：" + num2str(APP) + "  意志：" + num2str(POW);
				outstr += "  体型：" + num2str(SIZ) + "  教育：" + num2str(EDU) + "\n";
				outstr += "八大属性总和：" + num2str(STR + CON + DEX + APP + POW + SIZ + EDU + INT);
			}

			send_group_message(groupNumber, outstr);
			return;
		}

        e.block(); // 阻止当前事件传递到下一个插件
        /*
        static const set<int64_t> ENABLED_GROUPS = {123456, 123457};
        if (ENABLED_GROUPS.count(e.group_id) == 0) return; // 不在启用的群中, 忽略

        try {
            send_message(e.target, e.message); // 复读
            auto mem_list = get_group_member_list(e.group_id); // 获取群成员列表
            string msg;
            for (auto i = 0; i < min(10, static_cast<int>(mem_list.size())); i++) {
                msg += "昵称: " + mem_list[i].nickname + "\n"; // 拼接前十个成员的昵称
            }
            send_group_message(e.group_id, msg); // 发送群消息
        } catch (ApiError &) { // 忽略发送失败
        }
        if (e.is_anonymous()) {
            logging::info("群聊", "消息是匿名消息, 匿名昵称: " + e.anonymous.name);
        }
        */
    });

	/**
	 * 这是用来处理目前的群游戏企划
	 * 目前已经包括了
	 * Sand Car (沙车)
	 * */
	on_group_message([](const GroupMessageEvent &e) {
        auto message = e.message;//消息内容
        auto groupNumber = e.group_id;//群号	
        auto groupMemberInfo = get_group_member_info(groupNumber, e.user_id);
		string userNickname = groupMemberInfo.card == "" ? groupMemberInfo.nickname : groupMemberInfo.card;//使用者昵称
        if (message[0] != '.') return;

        if (message.substr(1, 4) == "init")
		{
			int tmp = 5;//当前字符串的位置
			if(message[tmp] == ' ') tmp++;
			if(message.substr(tmp, 15) != "沙车阶段一")
				return;

			string licensePlateNumber;
			ifstream inFile;
			string errorMessage1 = "一群人围坐在";//错误信息前半段（一个群只能有一辆车）
			string errorMessage2 = "的旁边，无暇再寻找新的车辆。";//错误信息后半段
			string lineStr;
			vector<string> strArray[2];

			rad:for (int i = 1; i <= 8; i++)
			{
				int t = _random(26 + 26 + 10);
				if (t <= 26)
					licensePlateNumber += cha2str('a' + t - 1);
				else if (t <= 52)
					licensePlateNumber += cha2str('A' + t - 27);
				else 
					licensePlateNumber += cha2str('0' + t - 53);
			}

			strArray[0].clear(), strArray[1].clear();
			inFile.open(licenseFile, ios::in);
			bool flag = 0;
			while(getline(inFile, lineStr))
			{
				stringstream ss(lineStr);
				string str;
				vector<string> lineArray;

				while(getline(ss,str,','))
					lineArray.push_back(str);
				if (lineArray[0] == licensePlateNumber)	
				{
					flag = 1;
					break;
				}

				if (lineArray[1] == num2str(groupNumber))
				{
					send_group_message(groupNumber, errorMessage1 + lineArray[0] + errorMessage2);
					return;
				}

				strArray[0].push_back(lineArray[0]);
				strArray[1].push_back(lineArray[1]);
			}
			if (flag) goto rad;

			ofstream outFile, outLicense;
			outLicense.open(licenseFile, ios::out);
			for (unsigned int i = 0; i < strArray[0].size(); i++)
				outLicense << strArray[0][i] << "," << strArray[1][i] << endl;
			outLicense << licensePlateNumber << "," << groupNumber;

			outFile.open(path + licensePlateNumber + ".csv", ios::out);
			outFile << "车的修复度" << "," << "0" << endl;
			outFile << "发动机" << "," << "0" << endl;
			outFile << "燃料" << "," << "0" << endl;
			outFile << "车轮1" << "," << "0" << endl;
			outFile << "车轮2" << "," << "0" << endl;
			outFile << "车轮3" << "," << "0" << endl;
			outFile << "车轮4" << "," << "0" << endl;
			outFile << "离合器" << "," << "0" << endl;
			outFile << "变速箱" << "," << "0" << endl;
			outFile << "传动轴" << "," << "0" << endl;
			outFile << "驱动桥" << "," << "0" << endl;
			outFile << "车架" << "," << "0" << endl;
			outFile << "前轴" << "," << "0" << endl;
			outFile << "悬架" << "," << "0" << endl;
			outFile << "转向操纵机构" << "," << "0" << endl;
			outFile << "转向器" << "," << "0" << endl;
			outFile << "转向传动机构" << "," << "0" << endl;
			outFile << "驾驶室" << "," << "0" << endl;
			outFile << "车厢" << "," << "0" << endl;
			outFile << "蓄电池" << "," << "0" << endl;
			outFile << "照明设备" << "," << "0" << endl;
			outFile << "仪表" << "," << "0" << endl;
			outFile << "治安状况" << "," << "50" << endl;
			outFile << "据点等级" << "," << "0" << endl;
			outFile << "科研等级" << "," << "0" << endl;
			outFile << "入侵者" << "," << "0" << "," << "0" << endl;
			outFile << "物品" << "," << "0" << "," << "0" << endl;
			outFile << "车上的乘客" << "," << "0";

			string backMessage = "";
			backMessage += "你们在废墟中，发现了一辆车的残骸。";
			backMessage += "虽然是残骸，但或许有些东西还可以继续使用，你们需要的就是修好它。";
			backMessage	+= userNickname + "擦了擦车牌，上面露出了";
			backMessage	+= licensePlateNumber + "的字样。";
            send_group_message(groupNumber, backMessage);
			return;
		}

		if (message.substr(1, 8) == "getincar")
		{
			int tmp = 9;//当前字符串的位置
			if(message[tmp] == ' ') tmp++;
			
			string path = ".\\data\\SandCar\\";
			string licenseFile = path + "licenses.csv";
			string licensePlateNumber = getLicensePlateNumber(groupNumber);
			string errorMessage1 = userNickname + "已经在车上了！";//如果已经在车上的情况
			string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

			if (licensePlateNumber == "")
			{
				send_group_message(groupNumber, errorMessage2);
				return;
			}

			ifstream inFile;
			inFile.open(path + licensePlateNumber + ".csv", ios::in);
			vector<string> strArray;
			string lineStr;
			while (getline(inFile, lineStr))
			{
				if (lineStr.substr(0, 15) == "车上的乘客")
				{
					if (lineStr[17] == 0)
						lineStr = lineStr.substr(0, 16) + num2str(e.user_id);
					else 
					{
						stringstream ss(lineStr);
						string str;
						vector<string> lineArray;

						while (getline(ss, str, ','))
							if (str == num2str(e.user_id))
							{
								send_group_message(groupNumber, errorMessage1);
								return;
							}
						lineStr += "," + num2str(e.user_id);
					}
				}
				strArray.push_back(lineStr);
			}
			ofstream outFile, outMember;
			outFile.open(path + licensePlateNumber + ".csv", ios::out);
			for (unsigned int i = 0; i < strArray.size() - 1; i++)
				outFile << strArray[i] << endl;
			outFile << strArray[strArray.size() - 1];

			string backMessage = "";
			backMessage += userNickname +  "走上车，一种陌生而熟悉的感觉油然而生。他填写了乘员表：\n";
			backMessage += "力量STR：40   灵巧DEX：40   知力INT：40   精神SAN：40\n";
 			backMessage += "感到一种神秘的力量附着在身上，此时，" + userNickname + "抓住了命运。";
			send_group_message(groupNumber, backMessage);
			outMember.open(path + licensePlateNumber + num2str(e.user_id) + ".csv", ios::out);
			outMember << "STR" << "," << "40" << "," << "0" << endl;
			outMember << "DEX" << "," << "40" << "," << "0" << endl;
			outMember << "INT" << "," << "40" << "," << "0" << endl;
			outMember << "SAN" << "," << "40" << "," << "0" << endl;
			outMember << "FATE" << "," << "2" << endl;
			outMember << "CP" << "," << "0" << endl;
			outMember << "物品" << "," << "0" << endl;
			return;
		}

		if (message.substr(1, 5) == "state")
		{
			int tmp = 6;//当前字符串的位置
			if (message[tmp] == ' ') tmp++;
			if (message == ".state" || message.substr(tmp, 6) == "人物")
			{
				Members memberInfo;
				string backMessage = userNickname + "\n";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}
				
				memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}
				backMessage += "力量STR：" + num2str(memberInfo.STR) + "（" + num2str(memberInfo.strEXP) + "）\n";
				backMessage += "灵巧DEX：" + num2str(memberInfo.DEX) + "（" + num2str(memberInfo.dexEXP) + "）\n";
				backMessage += "智力INT：" + num2str(memberInfo.INT) + "（" + num2str(memberInfo.intEXP) + "）\n";
				backMessage += "理智SAN：" + num2str(memberInfo.SAN) + "（" + num2str(memberInfo.sanEXP) + "）\n";
				backMessage += "命运FATE：" + num2str(memberInfo.FATE);

				send_group_message(groupNumber, backMessage);
				return;
			}
			if (message.substr(tmp, 6) == "物品")
			{
				Members memberInfo;
				string backMessage = "";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}
				
				memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}
				backMessage += userNickname + "随身携带的物品：";
				if (memberInfo.bag[0].id == 0)
					backMessage += "\n无";
				else
					for (unsigned int i = 0; i < memberInfo.bag.size(); i++)
						backMessage += "\n" + memberInfo.bag[i].name + "（" + memberInfo.bag[i].function + "）";

				send_group_message(groupNumber, backMessage);
				return;
			}
			if (message.substr(tmp, 12) == "据点资源" || message.substr(tmp, 6) == "资源")
			{
				Centers centerInfo;
				string backMessage = "";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况
				string emptyMessage = "据点里什么物资都没有了……";//如果据点中没有物资

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}

				Members memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}

				centerInfo = getCenterInfo(licensePlateNumber);
				if (centerInfo.bag[0].first.id == 0)
					send_group_message(groupNumber, emptyMessage);
				else
				{
					string backMessage = "物资列表：\n";
					for (unsigned int i = 0; i < centerInfo.bag.size() - 1; i++)
						backMessage += centerInfo.bag[i].first.name + "（" + num2str(centerInfo.bag[i].second) + "）\n";
					backMessage += centerInfo.bag[centerInfo.bag.size() - 1].first.name + "（" + num2str(centerInfo.bag[centerInfo.bag.size() - 1].second) + "）";
					send_group_message(groupNumber, backMessage);
				}
				return;
			}

			if (message.substr(tmp, 3) == "车")
			{
				SandCar carInfo;
				string backMessage = "";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}

				Members memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}

				carInfo = getCarInfo(licensePlateNumber);

				backMessage += "总体修复（" + dou2str(carInfo.repair) + "%）\n";
				backMessage += "发动机（" + dou2str(carInfo.engine) + "%）\n";
				backMessage += "底盘（" + dou2str(carInfo.chassisRepair) + "%）\n";
				backMessage += "车身（" + dou2str(carInfo.carBodyRepair) + "%）\n";
				backMessage += "电气设备（" + dou2str(carInfo.electricAccessoryRepair) + "%）\n";
				backMessage += "燃料（" + dou2str(carInfo.fuel) + "%）";
				send_group_message(groupNumber, backMessage);
				return;
			}

			if (message.substr(tmp, 6) == "据点")
			{
				Centers centerInfo;
				string backMessage = "";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}

				Members memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}

				centerInfo = getCenterInfo(licensePlateNumber);
				
				backMessage += "据点等级：" + num2str(centerInfo.level) + "（" + num2str(centerInfo.MemberNumber) + "/" +num2str(centerInfo.levelUpLimit) + "）\n";
				backMessage += "科研等级：" + num2str(centerInfo.scientificResearch) + "\n";
				backMessage += "治安状况：" + num2str(centerInfo.publicSecurity) + "\n";
				if (!centerInfo.invaders) 
					backMessage += "入侵者：无";
				else
					backMessage += "入侵者：" + centerInfo.inv.name + "（" + num2str(centerInfo.inv.HP) + "）";
				send_group_message(groupNumber, backMessage);
				return;
			}

		}

		if (message.substr(1, 4) == "list")
		{
			int tmp = 5;//当前字符串的位置
			if (message[tmp] == ' ') tmp++;
			if (message.substr(tmp, 6) == "冒险")
			{
				string backMessage = "";
				string licensePlateNumber = getLicensePlateNumber(groupNumber);
				string errorMessage1 = "没时间解释了了，快上车！\n（请使用 .getincar";//如果没有上车的情况
				string errorMessage2 = "诶？这里好像还没有车……\n（请使用 .init 沙车阶段一 ";//如果没有车的情况

				if (licensePlateNumber == "")
				{
					send_group_message(groupNumber, errorMessage2);
					return;
				}
				
				Members memberInfo = getMemberInfo(licensePlateNumber, e.user_id);
				if (!memberInfo.flag)
				{
					send_group_message(groupNumber, errorMessage1);
					return;
				}

				backMessage += "1、废料的垃圾场\n";
				backMessage += "2、杂草丛生的荒地\n";
				backMessage += "3、墙外沙漠\n";
				backMessage += "4、小学旧址\n";
				backMessage += "5、围墙\n";
				backMessage += "6、鬼屋";
				send_group_message(groupNumber, backMessage);
				return;
			}
		}

		if (message.substr(1, 3) == "adv")
		{
			int tmp = 4;//当前字符串的位置
			if (message[tmp] == ' ') tmp++;
			if (message[tmp] == '1' || message.substr(tmp, 18) == "废料的垃圾场")
			{

			}
		}

		e.block();

    });

    /*
    on_group_upload([](const auto &e) { // 可以使用 auto 自动推断类型
        stringstream ss;
        ss << "您上传了一个文件, 文件名: " << e.file.name << ", 大小(字节): " << e.file.size;
        try {
            send_message(e.target, ss.str());
        } catch (ApiError &) {
        }
    });
    */
}


/*
CQ_MENU(menu_demo_1) {
    logging::info("菜单", "点击菜单1");
}

CQ_MENU(menu_demo_2) {
    send_private_message(10000, "测试");
}
*/