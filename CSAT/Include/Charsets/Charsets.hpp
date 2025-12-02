#pragma once

#include<Windows.h>
#include<string>

std::wstring U8ToW(const std::string& u8String);
std::string WToU8(const std::wstring& wString);