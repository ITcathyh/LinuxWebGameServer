#pragma once
#include <string>
#include "baseFightUnit.h"
using namespace std;

class Monster : public BaseFightUnit
{
public:
	long monsterid;
	string monstername;
};