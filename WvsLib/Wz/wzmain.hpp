///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <codecvt>
#include <sstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>
using namespace std;
using namespace std::tr1;
using namespace std::tr2;
using namespace std::tr2::sys;
#include "WzAES.hpp"
#include "mapfile.hpp"
#include "wz.hpp"
namespace WZ {
extern bool Lazy;
extern void Init(bool);
extern Node Base;
extern void AddPath(const string &path);
}
#include "wzimg.hpp"
#include "wzcrypto.hpp"

static_assert(sizeof(wchar_t) == 2, "");
inline void die() { throw(273); }