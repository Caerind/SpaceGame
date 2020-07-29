#pragma once

#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/Application/AudioSystem.hpp>
#include <Enlivengine/System/Time.hpp>

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#endif // ENLIVE_ENABLE_IMGUI

#include "GameSingleton.hpp"

struct TrackData
{
	TrackData() : musicIdentifier(""), volumeDefault(1.0f), playing(false), music() {}

	std::string musicIdentifier;
	en::F32 volumeDefault;

	bool IsPlaying() const { return playing; }
	void Play()
	{
		playing = true;
		if (music.IsValid())
		{
			music.Play();
		}
		else
		{
			music = en::AudioSystem::GetInstance().PlayMusic(musicIdentifier.c_str());
		}
	}
	void FakeStop()
	{
		playing = false;
		music.SetVolume(0.0f);
	}
	void HardStop()
	{
		playing = false;
		if (music.IsValid())
		{
			music.Stop();
			music = en::MusicPtr();
		}
	}
	void UpdateVolume(en::F32 factor)
	{
		if (playing)
		{
			music.SetVolume(factor * volumeDefault);
		}
		else
		{
			music.SetVolume(0.0f);
		}
	}

	bool playing;
	en::MusicPtr music;

#ifdef ENLIVE_ENABLE_IMGUI
	float volumes[90];
	int volumesOffset;
	void UpdateVolumeTool()
	{
		volumes[volumesOffset] = music.GetFinalVolume();
		volumesOffset = (volumesOffset + 1) % IM_ARRAYSIZE(volumes);
	}
#endif // ENLIVE_ENABLE_IMGUI
};

ENLIVE_META_CLASS_BEGIN(TrackData)
	ENLIVE_META_CLASS_MEMBER("musicIdentifier", &TrackData::musicIdentifier),
	ENLIVE_META_CLASS_MEMBER("volumeDefault", &TrackData::volumeDefault)
ENLIVE_META_CLASS_END()

class AudioTrackMixer3000
{
	AudioTrackMixer3000()
		: mBaseTrack()
		, mTracks()
		, mSecondTrack(SecondTrackType::Nav)
		, mPlaying(false)
	{
	}

public:
	static AudioTrackMixer3000& GetInstance() 
	{
		static AudioTrackMixer3000 instance;
			return instance;
	}
	~AudioTrackMixer3000() {}

public:
	enum class SecondTrackType
	{
		Nav,
		Safe,
		Fight,

		Count
	};

	TrackData mBaseTrack;
	std::array<TrackData, static_cast<en::U32>(SecondTrackType::Count)> mTracks;

public:
	bool IsPlaying() const { return mPlaying; }
	void Play()
	{
		mPlaying = true;
		mBaseTrack.Play();
		mTracks[static_cast<en::U32>(mSecondTrack)].Play();
	}
	void Stop()
	{
		mPlaying = false;
		mBaseTrack.HardStop();
		for (auto& t : mTracks)
		{
			t.HardStop();
		}
	}

	void UpdateVolumes()
	{
		const en::F32 factor = GetDistanceFromSolarFactor();
		mBaseTrack.UpdateVolume(factor);
		for (auto& t : mTracks)
			t.UpdateVolume(factor);
	}
	void ChangeTrack(SecondTrackType newTrack)
	{
		if (newTrack != mSecondTrack)
		{
			mTracks[static_cast<en::U32>(mSecondTrack)].FakeStop();
			mSecondTrack = newTrack;
			mTracks[static_cast<en::U32>(mSecondTrack)].Play();
		}
	}

	en::F32 GetDistanceFromSolarFactor() const
	{
		auto& gameSing = GameSingleton::GetInstance();
		if (gameSing.distanceMax > 0.0f && gameSing.energyFactorEnv > 0.0f)
		{
			return en::Math::Clamp((gameSing.energyFactor / gameSing.energyFactorEnv) - 1.0f, 0.0f, 1.0f);
		}
		else
		{
			return 1.0f;
		}
	}

	SecondTrackType GetSecondTrack() const { return mSecondTrack; }

private:
	SecondTrackType mSecondTrack;
	bool mPlaying;
};

ENLIVE_META_CLASS_BEGIN(AudioTrackMixer3000)
	ENLIVE_META_CLASS_MEMBER("mBaseTrack", &AudioTrackMixer3000::mBaseTrack),
	ENLIVE_META_CLASS_MEMBER("mTracks", &AudioTrackMixer3000::mTracks)
ENLIVE_META_CLASS_END()


#ifdef ENLIVE_ENABLE_IMGUI
class AudioTrackMixer3000Tool : public en::ImGuiTool
{
	ENLIVE_SINGLETON(AudioTrackMixer3000Tool);
	~AudioTrackMixer3000Tool() {}

public:
	virtual en::ImGuiToolTab GetTab() const { return en::ImGuiToolTab::Game; }
	virtual const char* GetName() const { return "AudioTrackMixer3000"; }

	virtual void Display() 
	{
		AudioTrackMixer3000& audioMixer = AudioTrackMixer3000::GetInstance();
		en::ObjectEditor::ImGuiEditor(audioMixer, "Config");

		ImGui::Separator();


		static double refreshTime = 0.0f;
		if (refreshTime == 0.0)
			refreshTime = ImGui::GetTime();
		while (refreshTime < ImGui::GetTime()) // Create dummy data at fixed 60 Hz rate for the demo
		{
			refreshTime += 1.0f / 60.0f;
			audioMixer.mBaseTrack.UpdateVolumeTool();
			for (auto& track : audioMixer.mTracks)
			{
				track.UpdateVolumeTool();
			}
		}

		static constexpr const char* names[] = {
			"Nav",
			"Safe",
			"Fight"
		};
		const en::F32 factor = audioMixer.GetDistanceFromSolarFactor();
		ImGui::Text("BaseTrack: %f (%f, %f)", audioMixer.mBaseTrack.music.GetVolume(), audioMixer.mBaseTrack.volumeDefault, factor);
		ImGui::PlotLines("", audioMixer.mBaseTrack.volumes, IM_ARRAYSIZE(audioMixer.mBaseTrack.volumes), audioMixer.mBaseTrack.volumesOffset, nullptr, 0.0f, 1.0f);
		for (en::U32 i = 0; i < static_cast<en::U32>(AudioTrackMixer3000::SecondTrackType::Count); ++i)
		{
			std::string n = std::string(en::Meta::GetEnumName(static_cast<AudioTrackMixer3000::SecondTrackType>(i)));
			ImGui::Text("%s: %f (%f, %f)", n.c_str(), audioMixer.mTracks[i].music.GetVolume(), audioMixer.mTracks[i].volumeDefault, factor);
			ImGui::PlotLines("", audioMixer.mTracks[i].volumes, IM_ARRAYSIZE(audioMixer.mTracks[i].volumes), audioMixer.mTracks[i].volumesOffset, nullptr, 0.0f, 1.0f);
			if (audioMixer.mTracks[i].IsPlaying())
			{
				ImGui::SameLine();
				ImGui::Text(" -> Playing");
				/*if (ImGui::Button("Stop"))
				{

				}
				*/
			}
			else
			{
				ImGui::SameLine();
				ImGui::PushID(i);
				if (ImGui::Button("Play"))
				{
					audioMixer.ChangeTrack(static_cast<AudioTrackMixer3000::SecondTrackType>(i));
				}
				ImGui::PopID();
			}
		}
	}
};
#endif // ENLIVE_ENABLE_IMGUI