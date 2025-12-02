#include"../../ThirdParty/All_ImGui.hpp"

#include"GameSettingsManager.hpp"

#include"../CSATiManager.hpp"
#include"../CSController/CSController.hpp"

bool GameSettingsManager::Init() {
	this->OldGameSettings = this->GameSettings;
	this->pNow = reinterpret_cast<uintptr_t>(&this->GameSettings);
	this->pOld = reinterpret_cast<uintptr_t>(&this->OldGameSettings);

	return true;
}


bool GameSettingsManager::CommandExecute(const char* cmd) {
	this->CSATi->CS().Execute(cmd);
	return true;
}

bool GameSettingsManager::SafeCmdUse() {
	return !ImGui::GetIO().MouseDown[0];
}

//应用设置
void GameSettingsManager::Override() {
	//基本设置
	this->CommandOverride("cl_drawhud", &this->GameSettings.HUD);
	this->CommandOverride("cl_draw_only_deathnotices", &this->GameSettings.OnlyDeath);
	this->CommandOverride("cl_showfps", &this->GameSettings.FPSShowMode);
	this->CommandOverride("cl_showtick", &this->GameSettings.TickShowMode);
	this->CommandOverride("host_timescale", &this->GameSettings.GameSpeed);
	this->CommandOverride("fps_max", &this->GameSettings.FPSSettings.MaxFPS);
	

	//画面设置
	this->CommandOverride("spec_show_xray", &this->GameSettings.ScreenSettings.XRay);

	//声音设置
	this->CommandOverride("snd_mute_losefocus", &this->GameSettings.SoundSettings.snd_mute_losefocus);

	//声音设置-音乐设置
	this->CommandOverrideSquare("snd_menumusic_volume", &this->GameSettings.SoundSettings.snd_menumusic_volume);
	this->CommandOverrideSquare("snd_roundstart_volume", &this->GameSettings.SoundSettings.snd_roundstart_volume);
	this->CommandOverrideSquare("snd_roundaction_volume", &this->GameSettings.SoundSettings.snd_roundaction_volume);
	this->CommandOverrideSquare("snd_roundend_volume", &this->GameSettings.SoundSettings.snd_roundend_volume);
	this->CommandOverrideSquare("snd_mvp_volume", &this->GameSettings.SoundSettings.snd_mvp_volume);
	this->CommandOverrideSquare("snd_mapobjective_volume", &this->GameSettings.SoundSettings.snd_mapobjective_volume);
	this->CommandOverrideSquare("snd_tensecondwarning_volume", &this->GameSettings.SoundSettings.snd_tensecondwarning_volume);
	this->CommandOverrideSquare("snd_deathcamera_volume", &this->GameSettings.SoundSettings.snd_deathcamera_volume);
	this->CommandOverride("snd_mute_mvp_music_live_players", &this->GameSettings.SoundSettings.snd_mute_mvp_music_live_players);

	//声音设置-音效设置


	return;
}

void GameSettingsManager::SettingGraphFloat(const char* Label, float* V, const float& min, const float& max) {
	ImGui::SliderFloat(Label, V, min, max, "%.3f");
	if (ImGui::Button("-1"))*V -= 1; ImGui::SameLine();
	if (ImGui::Button("-0.1"))*V -= 0.1; ImGui::SameLine();
	if (ImGui::Button("-0.01"))*V -= 0.01; ImGui::SameLine();
	if (ImGui::Button("-0.001"))*V -= 0.001; ImGui::SameLine();
	if (ImGui::Button("+0.001"))*V += 0.001; ImGui::SameLine();
	if (ImGui::Button("+0.01"))*V += 0.01; ImGui::SameLine();
	if (ImGui::Button("+0.1"))*V += 0.1; ImGui::SameLine();
	if (ImGui::Button("+1"))*V += 1;

	if (ImGui::Button("0.1"))*V = 0.1; ImGui::SameLine();
	if (ImGui::Button("0.25"))*V = 0.25; ImGui::SameLine();
	if (ImGui::Button("0.5"))*V = 0.5; ImGui::SameLine();
	if (ImGui::Button("1"))*V = 1; ImGui::SameLine();
	if (ImGui::Button("2"))*V = 2; ImGui::SameLine();
	if (ImGui::Button("3"))*V = 3;
}

void GameSettingsManager::SettingGraphInt(const char* Label, int* V, const int& min, const int& max) {
	ImGui::SliderInt(Label, V, min, max);
	if (ImGui::Button("-10"))*V -= 10; ImGui::SameLine();
	if (ImGui::Button("-5"))*V -= 5; ImGui::SameLine();
	if (ImGui::Button("-1"))*V -= 1; ImGui::SameLine();
	if (ImGui::Button("+1"))*V += 1; ImGui::SameLine();
	if (ImGui::Button("+5"))*V += 5; ImGui::SameLine();
	if (ImGui::Button("+10"))*V += 10;

	if (ImGui::Button("0"))*V = 0; ImGui::SameLine();
	if (ImGui::Button("30"))*V = 30; ImGui::SameLine();
	if (ImGui::Button("60"))*V = 60; ImGui::SameLine();
	if (ImGui::Button("90"))*V = 90; ImGui::SameLine();
	if (ImGui::Button("120"))*V = 120; ImGui::SameLine();
	if (ImGui::Button("144"))*V = 144; ImGui::SameLine();
	if (ImGui::Button("200"))*V = 200; ImGui::SameLine();
	if (ImGui::Button("240"))*V = 240;
}

void GameSettingsManager::Menu() {
	if (ImGui::Button("一键修改为默认设置")) {
		this->Override();
	}
	if (ImGui::Button("一键修复数字切人bug")) {
		this->CommandExecute("unbind 1");
		this->CommandExecute("unbind 2");
		this->CommandExecute("unbind 3");
		this->CommandExecute("unbind 4");
		this->CommandExecute("unbind 5");
		this->CommandExecute("unbind 6");
		this->CommandExecute("unbind 7");
		this->CommandExecute("unbind 8");
		this->CommandExecute("unbind 9");
		this->CommandExecute("unbind 0");


		this->CommandExecute("bind 1 spec_player 1");
		this->CommandExecute("bind 2 spec_player 2");
		this->CommandExecute("bind 3 spec_player 3");
		this->CommandExecute("bind 4 spec_player 4");
		this->CommandExecute("bind 5 spec_player 5");
		this->CommandExecute("bind 6 spec_player 6");
		this->CommandExecute("bind 7 spec_player 7");
		this->CommandExecute("bind 8 spec_player 8");
		this->CommandExecute("bind 9 spec_player 9");
		this->CommandExecute("bind 0 spec_player 10");
	}


	this->SettingGraphFloat("游戏速度", &this->GameSettings.GameSpeed, 0.001f, 10.000f);
	
	if (ImGui::CollapsingHeader("画面设置")) {
		this->SettingGraphInt("FPS上限", &this->GameSettings.FPSSettings.MaxFPS, 0, 1000);

		if (ImGui::TreeNode("UI设置")) {
			ImGui::Checkbox("显示HUD", &this->GameSettings.HUD);
			ImGui::Checkbox("只渲染击杀信息", &this->GameSettings.OnlyDeath);
			ImGui::SliderInt("展示FPS", &this->GameSettings.FPSShowMode, 0, 3, "%d");
			ImGui::SliderInt("展示Tick", &this->GameSettings.TickShowMode, 0, 3, "%d");
			if (ImGui::Button("切换Demo进度条UI显示")){
				this->CSATi->CS().Execute("demoui");
			}
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("渲染设置")) {
			ImGui::Checkbox("X光", &this->GameSettings.ScreenSettings.XRay);
			ImGui::Checkbox("方框透视", &this->GameSettings.ScreenSettings.ESPBox);

			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("声音设置")) {
		ImGui::Checkbox("游戏窗口失去焦点时静音", &this->GameSettings.SoundSettings.snd_mute_losefocus);

		if (ImGui::TreeNode("音乐设置")) {
			ImGui::SliderFloat("主菜单音量", &this->GameSettings.SoundSettings.snd_menumusic_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("回合开始音量", &this->GameSettings.SoundSettings.snd_roundstart_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("回合开始行动音量", &this->GameSettings.SoundSettings.snd_roundaction_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("回合结束音量", &this->GameSettings.SoundSettings.snd_roundend_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("MVP音量", &this->GameSettings.SoundSettings.snd_mvp_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("炸弹/人质音量", &this->GameSettings.SoundSettings.snd_mapobjective_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("十秒警告音量", &this->GameSettings.SoundSettings.snd_tensecondwarning_volume, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("死亡视角音量", &this->GameSettings.SoundSettings.snd_deathcamera_volume, 0.0f, 1.0f, "%.2f");
			ImGui::Checkbox("当双方团队成员都存活时关闭MVP音乐", &this->GameSettings.SoundSettings.snd_mute_mvp_music_live_players);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("音效设置")) {

			ImGui::TreePop();
		}
	}
	
	this->Override();
}

void GameSettingsManager::VirtualMain() {


	if (this->GameSettings.ScreenSettings.ESPBox) {
		this->ESPDraw();
	}
}


void GameSettingsManager::ESPDraw() {
	for (int i = 0;  i < 10; ++i) {
		C_Entity Entity = this->CSATi->CS().GetEntity(i);
		DirectX::XMFLOAT3 EyePos3D = Entity.Pawn.GetEyePos();
		const DirectX::XMFLOAT3& OriginPos3D = Entity.Pawn.m_vOldOrigin;

		DirectX::XMFLOAT2 EyePos2D{};
		DirectX::XMFLOAT2 OriginPos2D{};
		CSATMath::XMWorldToScreen(EyePos3D, EyePos2D, this->CSATi->CS().CSGetMatrix(), this->CSATi->CS().CSGetScreenWide(), this->CSATi->CS().CSGetScreenHigh());
		CSATMath::XMWorldToScreen(OriginPos3D, OriginPos2D, this->CSATi->CS().CSGetMatrix(), this->CSATi->CS().CSGetScreenWide(), this->CSATi->CS().CSGetScreenHigh());
		const float hight{ ::abs(EyePos2D.y - OriginPos2D.y) * 1.25f };
		const float width{ hight / 2.f };
		const float x = EyePos2D.x - (width / 2.f);
		const float y = EyePos2D.y - (width / 2.5f);
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + width, y + hight), ImColor(0, 255, 0, 255), 0.0f, 0, 1.5f);
	}
}