#pragma once
class CConfig
{
public:
	void Load();
	void Save();

	struct ConfigData {
		float windowScale = 1.0f;
	};

	ConfigData& GetConfigData() { return m_data; };

private:
	ConfigData m_data;
};

