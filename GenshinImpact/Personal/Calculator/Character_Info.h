#ifndef _CHARACTOR_INFO_H_
#define _CHARACTOR_INFO_H_

typedef struct
{
	//生命
	float HP;
	//攻击
	float ATK;
	//防御
	float DEF;
	//精通
	float EM;
	//暴击率
	float CtR;
	//暴击伤害
	float CtD;
	//治疗加成
	float Heal;
	//受治疗加成
	float Healed;
	//充能效率
	float EC;
	//护盾强度
	float Shield_Strength;
	//元素伤害
	struct
	{
		//风
		float Anemo;
		//岩
		float Geo;
		//火
		float Pyro;
		//冰
		float Cyro;
		//水
		float Hydro;
		//雷
		float Electro;
		//草
		float Dendro;
	}EMD;
	//元素抗性
	struct
	{
		//风
		float Anemo;
		//岩
		float Geo;
		//火
		float Pyro;
		//冰
		float Cyro;
		//水
		float Hydro;
		//雷
		float Electro;
		//草
		float Dendro;
	}EDD;
}Character_Info_Typedef;

#endif
