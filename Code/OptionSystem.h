#pragma once
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <filesystem>

struct OptionSystemResource
{
	float bgmVolume{ 0.0f };
	float seVolume{ 0.0f };
	int directingSetting{ 0 };

	template<class T>
	void serialize(T& archive)
	{
		archive(bgmVolume, seVolume, directingSetting);
	}
};

class OptionSystem
{
	OptionSystem() {}
	~OptionSystem() = default;

public:
	static OptionSystem& Instance()
	{
		static OptionSystem instance;
		return instance;
	}

	void Initialize() { Load(optionFilename); }
	void Finalize() { Save(optionFilename); }

	//-----Getter, Setter-----//
	[[nodiscard]] float GetBgmVolume() const { return systemResource.bgmVolume; }
	[[nodiscard]] float GetSeVolume() const { return systemResource.seVolume; }
	[[nodiscard]] int GetDirectingSetting() const { return systemResource.directingSetting; }
	[[nodiscard]] OptionSystemResource GetSystem() const { return systemResource; }
	void SetSystemResources(OptionSystemResource resource) { systemResource = resource; }
	//void SetBgmVolume(const float volume) { optionSystem.bgmVolume = volume; }
	//void SetSeVolume(const float volume) { optionSystem.seVolume = volume; }
	//void SetDirectingSetting(const int setting) { optionSystem.directingSetting = setting; }

private:
	inline void Load(const wchar_t* FilePath)
	{
		std::filesystem::path cereal_filename(FilePath);
		cereal_filename.replace_extension("dat");
		if (std::filesystem::exists(cereal_filename.c_str()))
		{
			std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
			cereal::BinaryInputArchive deserialization(ifs);
			deserialization(systemResource);
		}
		else
		{
			systemResource.bgmVolume = 0.5f;
			systemResource.seVolume = 0.5f;
			systemResource.directingSetting = 0;

			std::filesystem::path cereal_filename(FilePath);
			cereal_filename.replace_extension("dat");
			if (!std::filesystem::exists(cereal_filename.c_str()))
			{
				std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
				cereal::BinaryOutputArchive serialization(ofs);
				serialization(systemResource);
			}

			//assert("OptionSystemFile Can Not Open");
		}
	}
	inline void Save(const wchar_t* FilePath)
	{
		std::filesystem::path cereal_filename(FilePath);
		cereal_filename.replace_extension("dat");
		if (std::filesystem::exists(cereal_filename.c_str()))
		{
			std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
			cereal::BinaryOutputArchive serialization(ofs);
			serialization(systemResource);
		}
		else
		{
			assert("OptionSystemFile Can Not Open");
		}
	}

private:
	OptionSystemResource systemResource{};

	const wchar_t* optionFilename = { L"./resources/Files/option.dat" };
};