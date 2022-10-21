#include <stdio.h>
#include <conio.h>
/*
1567.3
979
0.606
1.229
0.0
*/

/*
1461.9
829
60.6
122.9
0.0
*/

// 怪物减伤总比
#define MON_DES ((1.0 - 0.15) * MON_DDS)
// 怪物防御力
#define MON_DEF (100.0*5.0 + 500.0)
// 怪物防御力减伤比例
#define MON_DDS (MON_DEF / (MON_DEF + 90.0 * 5.0 + 500.0))

//伤害期望计算 Base Damage , CtR , CtD
//formula : Expact Damage = Base Damage·(1-CtR) + Base Damage·(1+CtD)·CtR
//                        = Base Damage·(1+CtR·CtD)
float Expact_Damage(float BD , float CtR , float CtD)
{
	return (BD*(1 + CtR * CtD));
}

typedef struct
{
	unsigned char Double_Pyro;
	unsigned char Double_Dendro;
}Team_Buff_Typedef;

typedef struct
{
	//更新标志
	unsigned char update;
	//激化率
	float Activation_Rate;
	//超激化基础值
	float Aggravate_Base;
	//蔓激化基础值
	float Spread_Base;
}Reaction_Parm_Typedef;

//激化倍率数值计算
float Activation_Rate(float EM)
{
	float AR;//Activation Rate
	
	AR  = (5 * EM) / (EM + 1200);
	
	return AR;
}

//蔓激化基础数值计算
float Spread_Damage(float AR)
{
	float SD;//Spread Damage
	
	SD   = 1477 * 1.25 * (AR + 1);
	
	return SD;
}

//超激化基础数值计算
float Aggravate_Damage(float AR)
{
	float AD;//Aggravate Damage
	
	AD   = 1477 * 1.15 * (AR + 1);
	
	return AD;
}

void Reaction_Parm_Update(Reaction_Parm_Typedef * RP , float EM)
{
	RP->Activation_Rate = Activation_Rate(EM);
	
	RP->Aggravate_Base = Aggravate_Damage(RP->Activation_Rate);
	
	RP->Spread_Base = Spread_Damage(RP->Activation_Rate);
	
	
	
	RP->update = 1;
}

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
	//元素伤害
	float EMD;
	//充能效率
	float EC;
}chara_info;

typedef struct
{
//基础面板
	chara_info Info;
//伤害参数[3] [0]基础 [1]暴击 [2]期望
  //普通攻击伤害参数-无反应
	//普通攻击 [x] 第x段
	float Atk_N[3][4];
	//蓄力攻击
	float Atk_C[3];
	//下落期间伤害
	float Atk_D[3];
	//下落攻击 [0]低空 [1]高空
	float Atk_P[3][2];
	
  //普通攻击伤害参数-蔓激化
	//普通攻击 [x] 第x段
	float Atk_N_SP[3][4];
	//蓄力攻击
	float Atk_C_SP[3];
	//下落期间伤害
	float Atk_D_SP[3];
	//下落攻击 [0]低空 [1]高空
	float Atk_P_SP[3][2];
	
  //元素战技伤害参数-无反应
	//短按
	float ES_N[3];
	//长按
	float ES_C[3];
	//三业净灭 Tri-Karma Purification
	//[0] 基础伤害 [1] 加入天赋增幅 [2] 加入元素爆发增幅-后台 [3] 加入元素爆发增幅-站场
	float ES_TKP[3][4];
	
  //元素战技伤害参数-蔓激化
	//短按
	float ES_N_SP[3];
	//长按
	float ES_C_SP[3];
	//三业净灭 Tri-Karma Purification
	//[0] 基础伤害 [1] 加入天赋增幅 [2] 加入元素爆发增幅-后台 [3] 加入元素爆发增幅-站场
	float ES_TKP_SP[3][4];

//其他参数
  //元素精通三业净灭增益
	//元素精通转三业净灭伤害增幅
	float EM2TKP_DMG_Buff;
	//元素精通转三业净灭暴击率增幅
	float EM2TKP_CtR_Buff;
	
//元素爆发增益
  //元素爆发配队增益
	struct
	{
		unsigned char update;
		//配队 [0]火元素数量 [1]雷元素数量 [2]水元素数量 [3]备用
		unsigned char Team[4];
	  //元素爆发增益
		//火元素-三业净灭伤害增幅
		float P_DMG_Bouns;
		//雷元素-三业净灭释放间隔冷缩
		float E_CD_Decrease;
		//水元素-摩耶之殿持续时间增幅
		float H_TL_Extension;
	}EB_Team_Buff;
  //元素爆发前台元素精通增益
	float EB_F_EM_Buff;
	
	//元素共鸣增益
	Team_Buff_Typedef Team_Buff;
	
//元素战技参数
	//三业净灭间隔
	float ES_TKP_CD;
	//摩耶之殿持续时间 Shrine of Maya
	float EB_SOM_TL;
	
//反应伤害参数
	Reaction_Parm_Typedef RP;
	
//临时参数
	float base_atk;
}Nahida_Parm_Typedef;

void Nahida_Pram_Init(Nahida_Parm_Typedef * Nahida_Parm)
{
	unsigned int i , j;
	
	Nahida_Parm->Info.HP  = 10360;
	Nahida_Parm->Info.ATK = 298.97;
	Nahida_Parm->Info.DEF = 630.21;
	Nahida_Parm->Info.CtR = 0.05;
	Nahida_Parm->Info.CtD = 0.5;
	Nahida_Parm->Info.EMD = 0;
	Nahida_Parm->Info.EC  = 1;
	
	Nahida_Parm->base_atk = 298.97;
	
	for(i = 0 ; i < 3 ; i++)
	{
		for(j = 0; j < 4; j++)
		{
			Nahida_Parm->Atk_N[i][j] = 0;
			Nahida_Parm->Atk_N_SP[i][j] = 0;
			
			Nahida_Parm->ES_TKP[i][j] = 0;
			Nahida_Parm->ES_TKP_SP[i][j] = 0;
		}
		
		Nahida_Parm->Atk_C[i] = 0;
		Nahida_Parm->Atk_D[i] = 0;
		Nahida_Parm->Atk_C_SP[i] = 0;
		Nahida_Parm->Atk_D_SP[i] = 0;
		
		for(j = 0; j < 2; j++)
		{
			Nahida_Parm->Atk_P[i][j] = 0;
			Nahida_Parm->Atk_P_SP[i][j] = 0;
		}
		
		Nahida_Parm->ES_N[i] = 0;
		Nahida_Parm->ES_C[i] = 0;
		
		Nahida_Parm->ES_N_SP[i] = 0;
		Nahida_Parm->ES_C_SP[i] = 0;
		
		Nahida_Parm->EB_Team_Buff.Team[i] = 0;
	}
	
	Nahida_Parm->EM2TKP_DMG_Buff = 0;
	Nahida_Parm->EM2TKP_CtR_Buff = 0;
	
	Nahida_Parm->EB_Team_Buff.update = 0;
	
	Nahida_Parm->EB_F_EM_Buff = 850 * 0.25;
	
	Nahida_Parm->Team_Buff.Double_Pyro = 0;
	Nahida_Parm->Team_Buff.Double_Dendro = 0;
	
	Nahida_Parm->ES_TKP_CD = 2.5;
	Nahida_Parm->EB_SOM_TL = 15;
	
	Nahida_Parm->RP.update = 0;
	Nahida_Parm->RP.Activation_Rate = 0;
	Nahida_Parm->RP.Aggravate_Base = 0;
	Nahida_Parm->RP.Spread_Base = 0;
}

void Nahida_Check_RP_Update(Nahida_Parm_Typedef * Nahida_Parm)
{
	if(!(Nahida_Parm->RP.update))
	{
		if(Nahida_Parm->Info.EM)
		{
			Reaction_Parm_Update(&(Nahida_Parm->RP) , Nahida_Parm->Info.EM);
		}
		else
		{}
	}
}

void Nahida_Normal_Attak(Nahida_Parm_Typedef * Nahida_Parm)
{
	const float ATK_N_Rate[4] = {0.6852 , 0.6286 , 0.7799 , 0.9929};
	const float ATK_C_Rate = 2.244;
	const float ATK_D_Rate = 1.0441;
	const float ATK_P_Rate[2] = {2.0877 , 2.6076};
	
	float atk_buff = 0;
	float atk_update = 0;
	float em_buff = 0;
	float em_update = 0;
	float em_temp = 0;
	unsigned int i;
	
	Nahida_Check_RP_Update(Nahida_Parm);
	
	if(Nahida_Parm->Team_Buff.Double_Pyro)
	{
		atk_buff = 1.15;
	}
	else
	{
		atk_buff = 1;
	}
	
	switch(Nahida_Parm->Team_Buff.Double_Dendro)
	{
		case 0:
			em_buff = 0;
		break;
		
		case 1:
			em_buff = 50;
		break;
		
		case 2:
			em_buff = 70;
		break;
		
		case 3:
			em_buff = 80;
		break;
		
		case 4:
			em_buff = 100;
		break;
	}
	
	atk_update = (Nahida_Parm->base_atk*atk_buff) + (Nahida_Parm->Info.ATK - Nahida_Parm->base_atk);
	em_update  = Nahida_Parm->Info.EM + em_buff;
	em_temp    = Nahida_Parm->Info.EM;
	Nahida_Parm->Info.EM = em_update;
	Nahida_Parm->RP.update = 0;
	Nahida_Check_RP_Update(Nahida_Parm);
	
	Nahida_Parm->Info.EM = em_temp;
	
	//普攻无反应伤害计算
	//角色攻击力×释放倍率×（1+元素伤害加成+造成的伤害加成）×（1+暴击伤害）×怪物等级即防御减伤×（1-怪物抗性）=最终伤害。
	for(i = 0; i < 4; i++)
	{
		//普攻4段基础值计算
		Nahida_Parm->Atk_N[0][i] = atk_update * ATK_N_Rate[i] * (1 + Nahida_Parm->Info.EMD) * MON_DES;
		//普攻4段暴击值计算
		Nahida_Parm->Atk_N[1][i] = Nahida_Parm->Atk_N[0][i] * (1 + Nahida_Parm->Info.CtD);
		//普攻4段期望值计算
		Nahida_Parm->Atk_N[2][i] = Expact_Damage(Nahida_Parm->Atk_N[0][i] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	}
	
	//重击基础值计算
	Nahida_Parm->Atk_C[0] = atk_update * ATK_C_Rate * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//重击暴击值计算
	Nahida_Parm->Atk_C[1] = Nahida_Parm->Atk_C[0] * (1 + Nahida_Parm->Info.CtD);
	//重击期望值计算
	Nahida_Parm->Atk_C[2] = Expact_Damage(Nahida_Parm->Atk_C[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);

	//下落期间伤害基础值计算
	Nahida_Parm->Atk_D[0] = atk_update * ATK_D_Rate * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//下落期间伤害暴击值计算
	Nahida_Parm->Atk_D[1] = Nahida_Parm->Atk_D[0] * (1 + Nahida_Parm->Info.CtD);
	//下落期间伤害期望值计算
	Nahida_Parm->Atk_D[2] = Expact_Damage(Nahida_Parm->Atk_D[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
	for(i = 0; i < 2; i++)
	{
		//下落伤害基础值计算
		Nahida_Parm->Atk_P[0][i] = atk_update * ATK_P_Rate[i] * (1 + Nahida_Parm->Info.EMD) * MON_DES;
		//下落伤害暴击值计算
		Nahida_Parm->Atk_P[1][i] = Nahida_Parm->Atk_P[0][i] * (1 + Nahida_Parm->Info.CtD);
		//下落伤害期望值计算
		Nahida_Parm->Atk_P[2][i] = Expact_Damage(Nahida_Parm->Atk_P[0][i] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	}
	
	//普攻4段蔓激化伤害计算
	for(i = 0; i < 4; i++)
	{
		//普攻4段基础值计算
		Nahida_Parm->Atk_N_SP[0][i] = (atk_update * ATK_N_Rate[i] + Nahida_Parm->RP.Spread_Base)* (1 + Nahida_Parm->Info.EMD) * MON_DES;
		//普攻4段暴击值计算
		Nahida_Parm->Atk_N_SP[1][i] = Nahida_Parm->Atk_N_SP[0][i] * (1 + Nahida_Parm->Info.CtD);
		//普攻4段期望值计算
		Nahida_Parm->Atk_N_SP[2][i] = Expact_Damage(Nahida_Parm->Atk_N_SP[0][i] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	}
	
	//重击蔓激化基础值计算
	Nahida_Parm->Atk_C_SP[0] = (atk_update * ATK_C_Rate  + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//重击蔓激化暴击值计算
	Nahida_Parm->Atk_C_SP[1] = Nahida_Parm->Atk_C_SP[0] * (1 + Nahida_Parm->Info.CtD);
	//重击蔓激化期望值计算
	Nahida_Parm->Atk_C_SP[2] = Expact_Damage(Nahida_Parm->Atk_C_SP[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);

	//下落期间蔓激化伤害基础值计算
	Nahida_Parm->Atk_D_SP[0] = ((Nahida_Parm->Info.ATK * atk_buff * ATK_D_Rate) + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//下落期间蔓激化伤害暴击值计算
	Nahida_Parm->Atk_D_SP[1] = Nahida_Parm->Atk_D_SP[0] * (1 + Nahida_Parm->Info.CtD);
	//下落期间蔓激化伤害期望值计算
	Nahida_Parm->Atk_D_SP[2] = Expact_Damage(Nahida_Parm->Atk_D_SP[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
	for(i = 0; i < 2; i++)
	{
		//下落蔓激化伤害基础值计算
		Nahida_Parm->Atk_P_SP[0][i] = ((Nahida_Parm->Info.ATK * atk_buff * ATK_P_Rate[i]) + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
		//下落蔓激化伤害暴击值计算
		Nahida_Parm->Atk_P_SP[1][i] = Nahida_Parm->Atk_P_SP[0][i] * (1 + Nahida_Parm->Info.CtD);
		//下落蔓激化伤害期望值计算
		Nahida_Parm->Atk_P_SP[2][i] = Expact_Damage(Nahida_Parm->Atk_P_SP[0][i] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	}
}

void Nahida_Show_Normal_Attack(Nahida_Parm_Typedef * Nahida_Parm)
{
	unsigned int i;
	printf("\n");
	printf("[普通攻击伤害]\n");
	printf("|\t\t|无反应\t\t\t\t\t\t|蔓激化\n");
	printf("|项目\t\t|基础\t\t|期望\t\t|暴击\t\t|基础\t\t|期望\t\t|暴击\n");
	for(i = 0; i < 4; i++)
	{
		printf("|普攻第 %d段:\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n", i + 1,
		Nahida_Parm->Atk_N[0][i] , Nahida_Parm->Atk_N[2][i] , Nahida_Parm->Atk_N[1][i] , 
		Nahida_Parm->Atk_N_SP[0][i] , Nahida_Parm->Atk_N_SP[2][i] , Nahida_Parm->Atk_N_SP[1][i]);
	}
	
	printf("|重击\t\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->Atk_C[0] , Nahida_Parm->Atk_C[2] , Nahida_Parm->Atk_C[1] , 
	Nahida_Parm->Atk_C_SP[0] , Nahida_Parm->Atk_C_SP[2] , Nahida_Parm->Atk_C_SP[1]);
	
	printf("|下落期间\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->Atk_D[0] , Nahida_Parm->Atk_D[2] , Nahida_Parm->Atk_D[1] , 
	Nahida_Parm->Atk_D_SP[0] , Nahida_Parm->Atk_D_SP[2] , Nahida_Parm->Atk_D_SP[1]);
	
	for(i = 0; i < 2; i++)
	{
		printf("|下落%s伤害\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n", (i?"高空" : "低空"),
		Nahida_Parm->Atk_P[0][i] , Nahida_Parm->Atk_P[2][i] , Nahida_Parm->Atk_P[1][i] , 
		Nahida_Parm->Atk_P_SP[0][i] , Nahida_Parm->Atk_P_SP[2][i] , Nahida_Parm->Atk_P_SP[1][i]);
	}
}

void Nahida_EB_Buff_Update(Nahida_Parm_Typedef * Nahida_Parm)
{
	switch(Nahida_Parm->EB_Team_Buff.Team[0])
	{
		case 0:
			Nahida_Parm->EB_Team_Buff.P_DMG_Bouns = 0;
		break;
		
		case 1:
			Nahida_Parm->EB_Team_Buff.P_DMG_Bouns = 0.2678;
		break;
		
		case 2:
		default:
			Nahida_Parm->EB_Team_Buff.P_DMG_Bouns = 0.4018;
		break;
	}
	
	switch(Nahida_Parm->EB_Team_Buff.Team[1])
	{
		case 0:
			Nahida_Parm->EB_Team_Buff.E_CD_Decrease = 0;
		break;
		
		case 1:
			Nahida_Parm->EB_Team_Buff.E_CD_Decrease = -0.45;
		break;
		
		case 2:
		default:
			Nahida_Parm->EB_Team_Buff.E_CD_Decrease = -0.67;
		break;
	}
	
	switch(Nahida_Parm->EB_Team_Buff.Team[2])
	{
		case 0:
			Nahida_Parm->EB_Team_Buff.H_TL_Extension = 0;
		break;
		
		case 1:
			Nahida_Parm->EB_Team_Buff.H_TL_Extension = 6.02;
		break;
		
		case 2:
		default:
			Nahida_Parm->EB_Team_Buff.H_TL_Extension = 9.03;
		break;
	}
	
	Nahida_Parm->EB_Team_Buff.update = 1;
}

void Nahida_Check_EB_Buff_Update(Nahida_Parm_Typedef * Nahida_Parm)
{
	if(!(Nahida_Parm->EB_Team_Buff.update))
	{
		if((Nahida_Parm->EB_Team_Buff.Team[0] >= 0) && (Nahida_Parm->EB_Team_Buff.Team[1] >= 0) && (Nahida_Parm->EB_Team_Buff.Team[2] >= 0))
		{
			Nahida_EB_Buff_Update(Nahida_Parm);
		}
		else
		{}
	}
}

void Nahida_Elemental_Skill(Nahida_Parm_Typedef * Nahida_Parm)
{
	const float ES_N_Rate = 1.7712;
	const float ES_C_Rate = 2.3472;
	const float ES_TKP_ATK_Rate = 1.8576;
	const float ES_TKP_EM_Rate = 3.7152;
	
	float atk_buff = 0;
	float atk_update = 0;
	float em_buff = 0;
	float em_update = 0;
	float em_temp = 0;
	
	float TKP_CtR_Update = 0;
	
	if(Nahida_Parm->Team_Buff.Double_Pyro)
	{
		atk_buff = 1.15;
	}
	else
	{
		atk_buff = 1;
	}
	
	switch(Nahida_Parm->Team_Buff.Double_Dendro)
	{
		case 0:
			em_buff = 0;
		break;
		
		case 1:
			em_buff = 50;
		break;
		
		case 2:
			em_buff = 70;
		break;
		
		case 3:
			em_buff = 80;
		break;
		
		case 4:
			em_buff = 100;
		break;
	}
	
	atk_update = (Nahida_Parm->base_atk*atk_buff) + (Nahida_Parm->Info.ATK - Nahida_Parm->base_atk);
	em_update  = Nahida_Parm->Info.EM + em_buff;
	em_temp    = Nahida_Parm->Info.EM;
	Nahida_Parm->Info.EM = em_update;
	Nahida_Parm->RP.update = 0;
	Nahida_Check_RP_Update(Nahida_Parm);
	
	Nahida_Parm->Info.EM = em_temp;
	
	Nahida_Check_EB_Buff_Update(Nahida_Parm);
	
	if(Nahida_Parm->Info.EM < 1000)
	{
		Nahida_Parm->EM2TKP_DMG_Buff = (Nahida_Parm->Info.EM - 200) * 0.001;
		Nahida_Parm->EM2TKP_CtR_Buff = (Nahida_Parm->Info.EM - 200) * 0.0003;
	}
	else
	{
		Nahida_Parm->EM2TKP_DMG_Buff = 0.8;
		Nahida_Parm->EM2TKP_CtR_Buff = 0.24;
	}
	
	TKP_CtR_Update = Nahida_Parm->Info.CtR + Nahida_Parm->EM2TKP_CtR_Buff;
	
//元素战技短按，无反应
	//元素战技短按基础伤害
	Nahida_Parm->ES_N[0] = (Nahida_Parm->Info.ATK * ES_N_Rate) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素战技短按暴击伤害
	Nahida_Parm->ES_N[1] = Nahida_Parm->ES_N[0] * (1 + Nahida_Parm->Info.CtD);
	//元素战技短按期望
	Nahida_Parm->ES_N[2] = Expact_Damage(Nahida_Parm->ES_N[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
//元素战技长按，无反应
	//元素战技短按基础伤害
	Nahida_Parm->ES_C[0] = (Nahida_Parm->Info.ATK * ES_C_Rate) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素战技短按暴击伤害
	Nahida_Parm->ES_C[1] = Nahida_Parm->ES_C[0] * (1 + Nahida_Parm->Info.CtD);
	//元素战技短按期望
	Nahida_Parm->ES_C[2] = Expact_Damage(Nahida_Parm->ES_C[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
//元素战技短按，蔓激化
	//元素战技短按基础伤害
	Nahida_Parm->ES_N_SP[0] = (Nahida_Parm->Info.ATK * ES_N_Rate + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素战技短按暴击伤害
	Nahida_Parm->ES_N_SP[1] = Nahida_Parm->ES_N_SP[0] * (1 + Nahida_Parm->Info.CtD);
	//元素战技短按期望
	Nahida_Parm->ES_N_SP[2] = Expact_Damage(Nahida_Parm->ES_N_SP[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
//元素战技长按，蔓激化
	//元素战技短按基础伤害
	Nahida_Parm->ES_C_SP[0] = (Nahida_Parm->Info.ATK * ES_C_Rate + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素战技短按暴击伤害
	Nahida_Parm->ES_C_SP[1] = Nahida_Parm->ES_C_SP[0] * (1 + Nahida_Parm->Info.CtD);
	//元素战技短按期望
	Nahida_Parm->ES_C_SP[2] = Expact_Damage(Nahida_Parm->ES_C_SP[0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
	
//三业净灭,无反应
  //基础伤害
	//三业净灭基础伤害-基础伤害
	Nahida_Parm->ES_TKP[0][0] = (Nahida_Parm->Info.ATK * ES_TKP_ATK_Rate + Nahida_Parm->Info.EM * ES_TKP_EM_Rate) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//三业净灭基础伤害-暴击伤害
	Nahida_Parm->ES_TKP[1][0] = Nahida_Parm->ES_TKP[0][0] * (1 + Nahida_Parm->Info.CtD);
	//三业净灭基础伤害-期望
	Nahida_Parm->ES_TKP[2][0] = Expact_Damage(Nahida_Parm->ES_TKP[0][0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
  //加入天赋增幅
	//三业净灭天赋增幅-基础伤害
	Nahida_Parm->ES_TKP[0][1] = Nahida_Parm->ES_TKP[0][0] * (1 + Nahida_Parm->EM2TKP_DMG_Buff);
	//三业净灭天赋增幅-暴击伤害
	Nahida_Parm->ES_TKP[1][1] = Nahida_Parm->ES_TKP[0][1] * (1 + Nahida_Parm->Info.CtD);
	//三业净灭天赋增幅-期望
	Nahida_Parm->ES_TKP[2][1] = Expact_Damage(Nahida_Parm->ES_TKP[0][1] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
  //加入元素爆发增幅-后台
	//元素爆发增幅-后台-基础伤害
	Nahida_Parm->ES_TKP[0][2] = Nahida_Parm->ES_TKP[0][1] * (1 + Nahida_Parm->EB_Team_Buff.P_DMG_Bouns);
	//元素爆发增幅-后台-暴击伤害
	Nahida_Parm->ES_TKP[1][2] = Nahida_Parm->ES_TKP[0][2] * (1 + Nahida_Parm->Info.CtD);
	//元素爆发增幅-后台-期望
	Nahida_Parm->ES_TKP[2][2] = Expact_Damage(Nahida_Parm->ES_TKP[0][2] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
	
	em_temp = Nahida_Parm->Info.EM;
	Nahida_Parm->Info.EM += Nahida_Parm->EB_F_EM_Buff + em_buff;
	
  //加入元素爆发增幅-前台
	//元素爆发增幅-前台-基础伤害
	Nahida_Parm->ES_TKP[0][3] = (Nahida_Parm->Info.ATK * ES_TKP_ATK_Rate + Nahida_Parm->Info.EM * ES_TKP_EM_Rate) * (1 + Nahida_Parm->EB_Team_Buff.P_DMG_Bouns) * (1 + Nahida_Parm->EM2TKP_DMG_Buff) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素爆发增幅-前台-暴击伤害
	Nahida_Parm->ES_TKP[1][3] = Nahida_Parm->ES_TKP[0][3] * (1 + Nahida_Parm->Info.CtD);
	//元素爆发增幅-前台-期望
	Nahida_Parm->ES_TKP[2][3] = Expact_Damage(Nahida_Parm->ES_TKP[0][3] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
	
	Nahida_Parm->Info.EM = em_temp;
	
//三业净灭,蔓激化
  //基础伤害
	//三业净灭基础伤害-基础伤害
	Nahida_Parm->ES_TKP_SP[0][0] = (Nahida_Parm->Info.ATK * ES_TKP_ATK_Rate + Nahida_Parm->Info.EM * ES_TKP_EM_Rate + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//三业净灭基础伤害-暴击伤害
	Nahida_Parm->ES_TKP_SP[1][0] = Nahida_Parm->ES_TKP_SP[0][0] * (1 + Nahida_Parm->Info.CtD);
	//三业净灭基础伤害-期望
	Nahida_Parm->ES_TKP_SP[2][0] = Expact_Damage(Nahida_Parm->ES_TKP_SP[0][0] , Nahida_Parm->Info.CtR , Nahida_Parm->Info.CtD);
  //加入天赋增幅
	//三业净灭天赋增幅-基础伤害
	Nahida_Parm->ES_TKP_SP[0][1] = Nahida_Parm->ES_TKP_SP[0][0] * (1 + Nahida_Parm->EM2TKP_DMG_Buff);
	//三业净灭天赋增幅-暴击伤害
	Nahida_Parm->ES_TKP_SP[1][1] = Nahida_Parm->ES_TKP_SP[0][1] * (1 + Nahida_Parm->Info.CtD);
	//三业净灭天赋增幅-期望
	Nahida_Parm->ES_TKP_SP[2][1] = Expact_Damage(Nahida_Parm->ES_TKP_SP[0][1] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
  //加入元素爆发增幅-后台
	//元素爆发增幅-后台-基础伤害
	Nahida_Parm->ES_TKP_SP[0][2] = Nahida_Parm->ES_TKP_SP[0][1] * (1 + Nahida_Parm->EB_Team_Buff.P_DMG_Bouns);
	//元素爆发增幅-后台-暴击伤害
	Nahida_Parm->ES_TKP_SP[1][2] = Nahida_Parm->ES_TKP_SP[0][2] * (1 + Nahida_Parm->Info.CtD);
	//元素爆发增幅-后台-期望
	Nahida_Parm->ES_TKP_SP[2][2] = Expact_Damage(Nahida_Parm->ES_TKP_SP[0][2] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
	
	em_temp = Nahida_Parm->Info.EM;
	Nahida_Parm->Info.EM += Nahida_Parm->EB_F_EM_Buff + em_buff;
	Nahida_Parm->RP.update = 0;
	Nahida_Check_RP_Update(Nahida_Parm);
	
  //加入元素爆发增幅-前台
	//元素爆发增幅-前台-基础伤害
	Nahida_Parm->ES_TKP_SP[0][3] = (Nahida_Parm->Info.ATK * ES_TKP_ATK_Rate + Nahida_Parm->Info.EM * ES_TKP_EM_Rate + Nahida_Parm->RP.Spread_Base) * (1 + Nahida_Parm->EB_Team_Buff.P_DMG_Bouns) * (1 + Nahida_Parm->EM2TKP_DMG_Buff) * (1 + Nahida_Parm->Info.EMD) * MON_DES;
	//元素爆发增幅-前台-暴击伤害
	Nahida_Parm->ES_TKP_SP[1][3] = Nahida_Parm->ES_TKP_SP[0][3] * (1 + Nahida_Parm->Info.CtD);
	//元素爆发增幅-前台-期望
	Nahida_Parm->ES_TKP_SP[2][3] = Expact_Damage(Nahida_Parm->ES_TKP_SP[0][3] , TKP_CtR_Update , Nahida_Parm->Info.CtD);
	
	Nahida_Parm->Info.EM = em_temp;
}

void Nahida_Show_Elemental_Skill(Nahida_Parm_Typedef * Nahida_Parm)
{
	printf("\n");
	printf("[元素战技伤害]\n");
	printf("|\t\t|无反应\t\t\t\t\t\t|蔓激化\n");
	printf("|项目\t\t|基础\t\t|期望\t\t|暴击\t\t|基础\t\t|期望\t\t|暴击\n");

	printf("|短按\t\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_N[0] , Nahida_Parm->ES_N[2] , Nahida_Parm->ES_N[1] , 
	Nahida_Parm->ES_N_SP[0] , Nahida_Parm->ES_N_SP[2] , Nahida_Parm->ES_N_SP[1]);
	
	printf("|长按\t\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_C[0] , Nahida_Parm->ES_C[2] , Nahida_Parm->ES_C[1] , 
	Nahida_Parm->ES_C_SP[0] , Nahida_Parm->ES_C_SP[2] , Nahida_Parm->ES_C_SP[1]);
	
	printf("[三业净灭伤害]\n");
	printf("|\t\t\t|无反应\t\t\t\t\t\t|蔓激化\n");
	printf("|项目\t\t\t|基础\t\t|期望\t\t|暴击\t\t|基础\t\t|期望\t\t|暴击\n");
	printf("|基础伤害\t\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_TKP[0][0] , Nahida_Parm->ES_TKP[2][0] , Nahida_Parm->ES_TKP[1][0] , 
	Nahida_Parm->ES_TKP_SP[0][0] , Nahida_Parm->ES_TKP_SP[2][0] , Nahida_Parm->ES_TKP_SP[1][0]);
	printf("|加入天赋增幅\t\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_TKP[0][1] , Nahida_Parm->ES_TKP[2][1] , Nahida_Parm->ES_TKP[1][1] , 
	Nahida_Parm->ES_TKP_SP[0][1] , Nahida_Parm->ES_TKP_SP[2][1] , Nahida_Parm->ES_TKP_SP[1][1]);
	printf("|加入元素爆发增幅-后台\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_TKP[0][2] , Nahida_Parm->ES_TKP[2][2] , Nahida_Parm->ES_TKP[1][2] , 
	Nahida_Parm->ES_TKP_SP[0][2] , Nahida_Parm->ES_TKP_SP[2][2] , Nahida_Parm->ES_TKP_SP[1][2]);
	printf("|加入元素爆发增幅-前台\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\t|%-9.6g\n",
	Nahida_Parm->ES_TKP[0][3] , Nahida_Parm->ES_TKP[2][3] , Nahida_Parm->ES_TKP[1][3] , 
	Nahida_Parm->ES_TKP_SP[0][3] , Nahida_Parm->ES_TKP_SP[2][3] , Nahida_Parm->ES_TKP_SP[1][3]);
}

void Nahida_Show_Talent_Info(Nahida_Parm_Typedef * Nahida_Parm)
{
	printf("\n");
	printf("[净善摄受明论]\n");
	printf("|摩耶之殿领域内元素精通加成  ：%g\n" , Nahida_Parm->EB_F_EM_Buff);
	printf("|该加成根据全队最高精通为850计算，有概率不符合实际\n");
	printf("[慧明缘觉智论]\n");
	printf("|元素精通转三业净灭伤害加成  ：%g%%\n" , Nahida_Parm->EM2TKP_DMG_Buff * 100);
	printf("|元素精通转三业净灭暴击率加成：%g%%\n" , Nahida_Parm->EM2TKP_CtR_Buff * 100);
}

void Nahida_Show_EB_Buff(Nahida_Parm_Typedef * Nahida_Parm)
{
	printf("[元素爆发增幅]\n");
	printf("三业净灭增幅:\n");
	printf("火元素数量：%d, 伤害加成：%5.4g%%\n" , Nahida_Parm->EB_Team_Buff.Team[0] , Nahida_Parm->EB_Team_Buff.P_DMG_Bouns * 100);
	printf("雷元素数量：%d, 释放CD减免:%5.4gs, 当前CD:%5.4gs\n" , Nahida_Parm->EB_Team_Buff.Team[1] , -Nahida_Parm->EB_Team_Buff.E_CD_Decrease , Nahida_Parm->ES_TKP_CD + Nahida_Parm->EB_Team_Buff.E_CD_Decrease);
	printf("摩耶之殿领域持续时间增幅:\n");
	printf("水元素数量：%d, 持续时间增幅:%5.4gs, 当前持续时间:%5.4gs\n" , Nahida_Parm->EB_Team_Buff.Team[2] , Nahida_Parm->EB_Team_Buff.H_TL_Extension , Nahida_Parm->EB_SOM_TL + Nahida_Parm->EB_Team_Buff.H_TL_Extension);
}

void start_show(void)
{
	printf(">Author : 二尾·凯茨莱茵\n");
	printf(">Note0  : 本程序用于计算纳西妲的伤害\n公式来源于网络，计算结果仅供参考，请以实机为准\n");
	printf(">Note1  : 本程序是以纳西妲90级，0命，普攻9级，元素战技10级，元素爆发10级\n在计算元素爆发领域内元素精通提升时，默认队伍中最高精通为850\n以及怪物设定为90级的丘丘人\n");
	printf(">Note2  : 本程序未添加武器选择功能，请直接输入计算好的面板数值\n\n");
}

#define test_type 0

void main(void)
{
	Nahida_Parm_Typedef Nahida_Parm;
	char temp;
	
	start_show();
	
	Nahida_Pram_Init(&Nahida_Parm);
	
	if(test_type)
	{
		printf("测试模式%d\n" , test_type);
	}
	
	printf("怪物参数\n");
	printf("怪物防御力减伤参数:%g%%\n" , MON_DDS * 100);
	printf("怪物元素抗性参数:10%%\n");
	printf("\n");
	
	printf("\n请输入纳西妲基础面板\n");
	
#if test_type == 0
	printf("攻击力  :");
	scanf("%f" , &(Nahida_Parm.Info.ATK)); getchar();
	
	printf("精通    :");
	scanf("%f" , &(Nahida_Parm.Info.EM));  getchar();
	
	printf("暴击率  :");
	scanf("%f" , &(Nahida_Parm.Info.CtR)); getchar(); Nahida_Parm.Info.CtR /= 100;
	
	printf("爆伤    :");
	scanf("%f" , &(Nahida_Parm.Info.CtD)); getchar(); Nahida_Parm.Info.CtD /= 100;
	
	printf("元素伤害:");
	scanf("%f" , &(Nahida_Parm.Info.EMD)); getchar(); Nahida_Parm.Info.EMD /= 100;
	printf("\n");
	
	printf("\n请输入队伍中对应元素的数量\n");
	printf("火元素:");
	scanf("%d" , &(Nahida_Parm.EB_Team_Buff.Team[0]) ); getchar();
	printf("雷元素:");
	scanf("%d" , &(Nahida_Parm.EB_Team_Buff.Team[1]) ); getchar();
	printf("水元素:");
	scanf("%d" , &(Nahida_Parm.EB_Team_Buff.Team[2]) ); getchar();	
	
	printf("\n元素共鸣\n");
	printf("双火 15%%攻击力(Y/N or y/n):");
	do
	{
		temp = _getch();
		if(temp != 'Y' && temp != 'y' && temp != 'N' && temp != 'n')
		{
			continue;
		}
		else
		{
			putchar(temp);
			printf("\n");
			if(temp == 'Y' || temp == 'y')
			{
				Nahida_Parm.Team_Buff.Double_Pyro = 1;
			}
			else
			{
				Nahida_Parm.Team_Buff.Double_Pyro = 0;
			}
			printf("\n");
			break;
		}
	}while(1);

	printf("双草 (Y/N or y/n):");
	do
	{
		temp = _getch();
		if(temp != 'Y' && temp != 'y' && temp != 'N' && temp != 'n')
		{
			continue;
		}
		else
		{
			putchar(temp);
			if(temp == 'Y' || temp == 'y')
			{
				printf("\n1.+50\n2.+70\n3.+80\n4+100\n请选择:");
				do
				{
					temp = _getch();
					if(temp < '1' || temp > '4')
					{
						continue;
					}
					else
					{
						putchar(temp);
						Nahida_Parm.Team_Buff.Double_Dendro = temp - '0';
					}
					break;
				}while(1);
			}
			else
			{
				Nahida_Parm.Team_Buff.Double_Dendro = 0;
			}
			printf("\n");
			break;
		}
	}while(1);
#elif test_type == 1
	printf("攻击力  :1461.9\t");
	Nahida_Parm.Info.ATK = 1461.9;
	
	printf("精通    :829\t");
	Nahida_Parm.Info.EM = 829;
	
#elif test_type == 2
	printf("攻击力  :1567.3\t");
	Nahida_Parm.Info.ATK = 1567.3;
	
	printf("精通    :979\t");
	Nahida_Parm.Info.EM = 829;
	
#endif
#if test_type
	
	printf("暴击率  :60.6\t");
	Nahida_Parm.Info.CtR = 0.606;
	
	printf("爆伤    :122.9\t");
	Nahida_Parm.Info.CtD = 1.229;
	
	printf("元素伤害:0.0\t");
	Nahida_Parm.Info.EMD = 0.0;

	printf("\n");
	
	printf("\n请输入队伍中对应元素的数量\n");
	printf("火元素:0");
	Nahida_Parm.EB_Team_Buff.Team[0] = 0;
	printf("雷元素:0");
	Nahida_Parm.EB_Team_Buff.Team[1] = 0;
	printf("水元素:0");
	Nahida_Parm.EB_Team_Buff.Team[2] = 0;	
#endif
	printf("\n\n");
	
	Nahida_Check_RP_Update(&Nahida_Parm);
	
	printf("激化倍率：%g%%\n" , Nahida_Parm.RP.Activation_Rate * 100);
	printf("蔓激化值：%g\n\n" , Nahida_Parm.RP.Spread_Base);
	
	Nahida_Normal_Attak(&Nahida_Parm);
	Nahida_Show_Normal_Attack(&Nahida_Parm);
	
	Nahida_Elemental_Skill(&Nahida_Parm);
	Nahida_Show_Elemental_Skill(&Nahida_Parm);
	
	Nahida_Show_Talent_Info(&Nahida_Parm);
	
	Nahida_Show_EB_Buff(&Nahida_Parm);
	
	printf("\n\nPress any key to continue");
	getchar();
}

/*
1567.3
979
0.606
1.229
0.0
*/

/*
1461.9
829
60.6
122.9
0.0
*/

/*
	printf("ES:\t%0.6g\t%0.6g\t%0.6g\n" , E_S_BASE_DAM , E_S_BASE_DAM * DAM_INC_RATE , E_S_BASE_DAM * DAM_INC_RATE * SPREAD_DAM_RATE);
	printf("EL:\t%0.6g\t%0.6g\t%0.6g\n" , E_L_BASE_DAM , E_L_BASE_DAM * DAM_INC_RATE , E_L_BASE_DAM * DAM_INC_RATE * SPREAD_DAM_RATE);
	printf("EA:\t%0.6g\t%0.6g\t%0.6g\n" , E_A_BASE_DAM , E_A_BASE_DAM * DAM_INC_RATE , E_A_BASE_DAM * DAM_INC_RATE * SPREAD_DAM_RATE);
*/
