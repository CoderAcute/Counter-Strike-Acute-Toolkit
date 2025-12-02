#include"../../ThirdParty/All_ImGui.hpp"

#include"HomeManager.hpp"
#include"../CSATiManager.hpp"

#include"../GlobalVars/GlobalVars.hpp"
bool HomeManager::Init() {
	return true;
}
void HomeManager::Menu() {	
 	ImGui::Text("欢迎使用 A宝工具包！(女大自用99新，持续更新中~)");
	ImGui::Text("软件全称：%s", GlobalVarsOnlyRead::FullName);
	ImGui::Text("开发者ID：%s", GlobalVarsOnlyRead::DeveloperName);
	ImGui::Text("当前版本：%s", GlobalVarsOnlyRead::CSATiVersion);
	ImGui::Text("时间戳：%s", GlobalVarsOnlyRead::TimeStamp);
	ImGui::Text("本软件除使用必要第三方开源库外，完全由我个人独立研发哦～");
	ImGui::Text("我对项目架构有稳定理解，具备持续开发和维护的能力呢！");

	ImGui::Separator();

	ImGui::Text("感谢使用我的小工具包～");
	ImGui::Text("本软件正在积极开发中，部分功能还在完善中，我在努力优化稳定性！");
	ImGui::Text("如果遇到问题请多多包涵，我会及时修复的呀～");

	ImGui::Separator();

	ImGui::Text("关于配置文件兼容性的说明：");
	ImGui::BulletText("主配置文件：软件更新后可能需要调整，遇到问题我会帮你修复哦！");
	ImGui::BulletText("元素配置与解决方案文件：这两类文件结构相对独立，将长期保持稳定～");

	ImGui::Separator();
	ImGui::Text("感谢支持！一起进步吧～");
}

void HomeManager::VirtualMain() {

}