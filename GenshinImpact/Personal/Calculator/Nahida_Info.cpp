#include "Nahida_Info.h"

Nahida_Info::Nahida_Info(void)
{
	//Nahida_info Initialization
	Nahida_info.HP	= 0.0;
	Nahida_info.ATK	= 0.0;
	Nahida_info.DEF	= 0.0;
	Nahida_info.EM	= 0.0;
	Nahida_info.CtR	= 0.05;
	Nahida_info.CtD	= 0.5;
	Nahida_info.Heal= 0.0;
	Nahida_info.Healed	= 0.0;
	Nahida_info.EC	= 0.0;
	Nahida_info.Shield_Strength = 0.0;
	
	Nahida_info.EMD.Anemo	= 0.0;
	Nahida_info.EMD.Geo		= 0.0;
	Nahida_info.EMD.Pyro	= 0.0;
	Nahida_info.EMD.Cyro	= 0.0;
	Nahida_info.EMD.Hydro	= 0.0;
	Nahida_info.EMD.Electro	= 0.0;
	Nahida_info.EMD.Dendro	= 0.0;

	Nahida_info.EDD.Anemo	= 0.0;
	Nahida_info.EDD.Geo		= 0.0;
	Nahida_info.EDD.Pyro	= 0.0;
	Nahida_info.EDD.Cyro	= 0.0;
	Nahida_info.EDD.Hydro	= 0.0;
	Nahida_info.EDD.Electro	= 0.0;
	Nahida_info.EDD.Dendro	= 0.0;
}

Nahida_Info::~Nahida_Info(void)
{}

