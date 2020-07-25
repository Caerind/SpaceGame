#pragma once

#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/Application/AudioSystem.hpp>
#include <Enlivengine/System/Time.hpp>

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#endif // ENLIVE_ENABLE_IMGUI

struct TrackData
{
	TrackData() : musicIdentifier(""), volumeDefault(1.0f) {}

	std::string musicIdentifier;
	en::F32 volumeDefault;
};

ENLIVE_META_CLASS_BEGIN(TrackData)
	ENLIVE_META_CLASS_MEMBER("musicIdentifier", &TrackData::musicIdentifier),
	ENLIVE_META_CLASS_MEMBER("volumeDefault", &TrackData::volumeDefault)
ENLIVE_META_CLASS_END()

class AudioTrackMixer3000
{
	ENLIVE_SINGLETON(AudioTrackMixer3000);
	~AudioTrackMixer3000() {}

public:
	enum class SecondTrackType
	{
		Nav,
		Safe,
		Fight,

		Count
	};

	en::U32 tempoBpm;
	TrackData baseTrack;
	std::array<TrackData, static_cast<en::U32>(SecondTrackType::Count)> tracks;
	en::F32 distanceFromSolarMax;

public:
	bool IsPlaying() const { return mPlaying; }
	void PlayAllTracks();
	void StopAllTracks();

	void UpdateTempo(en::Time dt);
	void UpdateVolumes();
	void ChangeTrack(SecondTrackType newTrack);

	en::F32 GetDistanceFromSolarFactor() const;
	en::F32 GetDistanceFromSolar() const;
	void SetDistanceFromSolar(en::F32 distanceFromSolar);

	SecondTrackType GetSecondTrack() const;

private:
	void StartTrack();

private:
	en::F32 mDistanceFromSolar;
	SecondTrackType mSecondTrack;
	en::MusicPtr mMusicBase;
	en::MusicPtr mMusicOther;
	en::Time mTimeAcc;
	bool mPlaying;
};

ENLIVE_META_CLASS_BEGIN(AudioTrackMixer3000)
	ENLIVE_META_CLASS_MEMBER("tempoBpm", &AudioTrackMixer3000::tempoBpm),
	ENLIVE_META_CLASS_MEMBER("baseTrack", &AudioTrackMixer3000::baseTrack),
	ENLIVE_META_CLASS_MEMBER("tracks", &AudioTrackMixer3000::tracks),
	ENLIVE_META_CLASS_MEMBER("distanceFromSolarMax", &AudioTrackMixer3000::distanceFromSolarMax)
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

		en::ObjectEditor::ImGuiEditor(audioMixer, "StaticStats");
		if (audioMixer.distanceFromSolarMax <= 0.0f)
		{
			audioMixer.distanceFromSolarMax = 0.01f;
		}

		ImGui::Separator();

		en::F32 distanceFromSolar = audioMixer.GetDistanceFromSolar();
		if (en::ObjectEditor::ImGuiEditor(distanceFromSolar, "distanceFromSolar"))
		{
			if (distanceFromSolar <= 0.0f)
			{
				distanceFromSolar = 0.01f;
			}
			audioMixer.SetDistanceFromSolar(distanceFromSolar);
		}
		ImGui::Text("Factor: %f", audioMixer.GetDistanceFromSolarFactor());

		AudioTrackMixer3000::SecondTrackType secondTrack = audioMixer.GetSecondTrack();
		if (en::ObjectEditor::ImGuiEditor(secondTrack, "secondTrack"))
		{
			audioMixer.ChangeTrack(secondTrack);
		}

		if (!audioMixer.IsPlaying())
		{
			if (ImGui::Button("Play"))
			{
				audioMixer.PlayAllTracks();
			}
		}
		else
		{
			if (ImGui::Button("Stop"))
			{
				audioMixer.StopAllTracks();
			}
		}
	}
};
#endif // ENLIVE_ENABLE_IMGUI