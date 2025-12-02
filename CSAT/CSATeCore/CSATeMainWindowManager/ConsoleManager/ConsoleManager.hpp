#include"ConsoleStrings.hpp"

#include<Windows.h>
#include<mutex>

class ConsoleManager {
public:
	ConsoleStrings ConsoleStrings{};

    HWND ConsolehWnd;
    //cls_LanguageManager* LM;
    std::mutex ConsoleMutex;

    void Init();

    void RedirectIO();
    bool SetTitle(const std::string& Title);

    void Print(const std::string& Msg);
    void PrintError(const std::string& Msg);
    void PrintWarning(const std::string& Msg);
    void PrintInfor(const std::string& Msg);

    // 可选：添加更多便利方法
    void PrintWithCout(const std::string& Msg);
    void PrintLine(const std::string& Msg);
    void PrintErrorLine(const std::string& Msg);
    void PrintWarningLine(const std::string& Msg);
    void PrintInforLine(const std::string& Msg);
};