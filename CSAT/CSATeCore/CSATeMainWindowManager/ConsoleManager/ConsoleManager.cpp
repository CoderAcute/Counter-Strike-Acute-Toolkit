#include"ConsoleManager.hpp"

#include"../../../Include/Charsets/Charsets.hpp"

#include <io.h>
#include <fcntl.h>
#include <iostream>

void ConsoleManager::Init() {
    if (!AllocConsole()) {
        MessageBoxA(NULL, "控制台创建失败！", "错误！", MB_ICONERROR | MB_OK);
        return;
    }
    
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    RedirectIO();
    SetTitle(this->ConsoleStrings.Title);
}

void ConsoleManager::RedirectIO() {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    FILE* f;

    // 重定向标准输入输出
    freopen_s(&f, "CONIN$", "r", stdin);
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

    // 清除任何错误状态
    std::cin.clear();
    std::cout.clear();
    std::cerr.clear();
}

bool ConsoleManager::SetTitle(const std::string& Title) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    if (Title.empty()) {
        return false;
    }
    
    return SetConsoleTitleW(U8ToW(Title).c_str());
}

void ConsoleManager::Print(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s", Msg.c_str());
    fflush(stdout);  // 确保立即输出
}

void ConsoleManager::PrintError(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s", this->ConsoleStrings.Error.c_str(), Msg.c_str());
    fflush(stdout);
}

void ConsoleManager::PrintWarning(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s", this->ConsoleStrings.Warning.c_str(), Msg.c_str());
    fflush(stdout);
}

void ConsoleManager::PrintInfor(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s", this->ConsoleStrings.Infor.c_str(), Msg.c_str());
    fflush(stdout);
}

// 如果需要支持流式输出，可以添加这些方法
void ConsoleManager::PrintWithCout(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    std::cout << Msg << std::flush;
}

void ConsoleManager::PrintLine(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s\n", Msg.c_str());
    fflush(stdout);
}

void ConsoleManager::PrintErrorLine(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s\n", this->ConsoleStrings.Error.c_str(), Msg.c_str());
    fflush(stdout);
}

void ConsoleManager::PrintWarningLine(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s\n", this->ConsoleStrings.Warning.c_str(), Msg.c_str());
    fflush(stdout);
}

void ConsoleManager::PrintInforLine(const std::string& Msg) {
    std::lock_guard<std::mutex> lock(ConsoleMutex);
    printf("%s%s\n", this->ConsoleStrings.Infor.c_str(), Msg.c_str());
    fflush(stdout);
}