#pragma once

class CSATiManager;

class EffectorManager {
private:
	CSATiManager* CSATi{};
public:
	void Init(CSATiManager* CSATi);
};