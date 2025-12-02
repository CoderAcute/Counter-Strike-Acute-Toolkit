#include"../../ThirdParty/All_ImGui.hpp"

#include"ConsoleManager.hpp"

#include"../CSATiManager.hpp"
#include"../Debugger/IDebugger.hpp"
#include"../GameSettingsManager/GameSettingsManager.hpp"
#include"../IPCer/IPCer.hpp"
#include"../KeyTracker/KeyTracker.hpp"
#include"../GlobalVars/GlobalVars.hpp"

bool ConsoleManager::Init() {
	return true;
}

void ConsoleManager::Menu() {
	if (ImGui::CollapsingHeader("CSAT设置")) {
		//调试功能设置
		//调试模式下提供更多功能，但可能影响性能和稳定性
		//调试设置
		if (ImGui::TreeNode("调试")) {
			ImGui::Text("调试模式（Debug Mode），提供更多功能，但可能影响性能和稳定性");
			static bool debugMode = this->CSATi->GlobalVars().DebugMode;
			if (ImGui::Checkbox("启用调试功能", &debugMode)) {
				this->CSATi->GlobalVars().DebugMode = debugMode;
			}
			ImGui::TreePop();
		}
		//监测设置
		if (ImGui::TreeNode("监测")) {
			if (ImGui::Button("打开监测")) {
				this->CSATi->IDebugger().ShowWindow();
			}
			ImGui::SameLine();
			if (ImGui::Button("关闭监测")) {
				this->CSATi->IDebugger().HideWindow();
			}
			ImGui::Checkbox("当有错误信息时弹出监测", &this->CSATi->IDebugger().ShowWhenError);
			ImGui::Checkbox("自动滚动到最新消息", &this->CSATi->IDebugger().AutoScroll);
			if (ImGui::Button("测试")) {
				this->CSATi->IDebugger().AddError("这是一个测试！");
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("初始化")) {
		

		if (ImGui::TreeNode("游戏初始化")) {
			if (ImGui::Button("初始化游戏设置")) {
				this->CSATi->IDebugger().AddInfo("正在尝试初始化游戏设置");
				this->CSATi->GameSettingsManager().Override();
				this->CSATi->IDebugger().AddInfo("尝试完毕，请自行检验效果");
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("CSAT初始化")) {
			if (ImGui::Button("初始化IPCer")) {
				this->CSATi->IDebugger().AddInfo("正在尝试初始化IPCer");
				this->CSATi->IPCer().Init();
			}
			if (ImGui::Button("查看IPCer结果")) {
				this->ShowFilePath();
			}
			if (ImGui::Button("初始化CSATi")) {

			}
			ImGui::TreePop();
		}
	}


	ImGui::Separator();
	ImGui::Separator();

	//static bool aimbot = false;
	ImGui::Checkbox("自瞄", &this->CSATi->CS().AimbotEnable);

	/*ImGui::InputInt("要瞄准的实体索引", &this->CSATi->CS().AimTargetIndex);
	constexpr KeyCheckPack pack1 = { VK_F1,true,false,false,2 };
	constexpr KeyCheckPack pack2 = { VK_F2,true,false,false,2 };
	constexpr KeyCheckPack pack3 = { VK_F3,true,false,false,2 };
	constexpr KeyCheckPack pack4 = { VK_F4,true,false,false,2 };
	constexpr KeyCheckPack pack5 = { VK_F5,true,false,false,2 };
	constexpr KeyCheckPack pack6 = { VK_F6,true,false,false,2 };
	constexpr KeyCheckPack pack7 = { VK_F7,true,false,false,2 };
	constexpr KeyCheckPack pack8 = { VK_F8,true,false,false,2 };
	constexpr KeyCheckPack pack9 = { VK_F9,true,false,false,2 };
	constexpr KeyCheckPack pack10 = { VK_F10,true,false,false,2 };

	if (this->CSATi->KT().IsKeyPressed(VK_RBUTTON)) {
		this->CSATi->CS().AimbotMain();
		if (this->CSATi->KT().CheckWithPack(pack1)) {
			this->CSATi->CS().AimTargetIndex = 1;
		}

		if (this->CSATi->KT().CheckWithPack(pack2)) {
			this->CSATi->CS().AimTargetIndex = 2;
		}

		if (this->CSATi->KT().CheckWithPack(pack3)) {
			this->CSATi->CS().AimTargetIndex = 3;
		}

		if (this->CSATi->KT().CheckWithPack(pack4)) {
			this->CSATi->CS().AimTargetIndex = 4;
		}

		if (this->CSATi->KT().CheckWithPack(pack5)) {
			this->CSATi->CS().AimTargetIndex = 5;
		}

		if (this->CSATi->KT().CheckWithPack(pack6)) {
			this->CSATi->CS().AimTargetIndex = 6;
		}

		if (this->CSATi->KT().CheckWithPack(pack7)) {
			this->CSATi->CS().AimTargetIndex = 7;
		}

		if (this->CSATi->KT().CheckWithPack(pack8)) {
			this->CSATi->CS().AimTargetIndex = 8;
		}

		if (this->CSATi->KT().CheckWithPack(pack9)) {
			this->CSATi->CS().AimTargetIndex = 9;
		}

		if (this->CSATi->KT().CheckWithPack(pack10)) {
			this->CSATi->CS().AimTargetIndex = 10;
		}
	}*/
}

void ConsoleManager::VirtualMain() {

}

void ConsoleManager::ShowFilePath() {
	if (this->CSATi->IPCer().Inited) {
		this->CSATi->IDebugger().AddInfo("---------------------------------------------------------------------------------");
		this->CSATi->IDebugger().AddInfo(this->CSATi->IPCer().GetAllPathMsg());
		this->CSATi->IDebugger().AddInfo("---------------------------------------------------------------------------------");
	}
	else {
		this->CSATi->IDebugger().AddError("IPCer尚未初始化成功！");
	}
}