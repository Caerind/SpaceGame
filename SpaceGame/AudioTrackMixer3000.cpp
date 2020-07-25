#include "AudioTrackMixer3000.hpp"

AudioTrackMixer3000::AudioTrackMixer3000()
	: tempoBpm(71)
	, baseTrack()
	, tracks()
	, distanceFromSolarMax(1000.0f)
	, mDistanceFromSolar(500.0f)
	, mSecondTrack(SecondTrackType::Nav)
	, mMusicBase()
	, mMusicOther()
	, mTimeAcc()
	, mPlaying(false)
{
}

void AudioTrackMixer3000::PlayAllTracks()
{
	mPlaying = true;
	if (mMusicBase.IsValid())
	{
		mMusicBase.Play();
	}
	else
	{
		mMusicBase = en::AudioSystem::GetInstance().PlayMusic(baseTrack.musicIdentifier.c_str());
	}
	if (mMusicOther.IsValid())
	{
		mMusicOther.Play();
	}
	else
	{
		StartTrack();
	}
}

void AudioTrackMixer3000::StopAllTracks()
{
	mPlaying = false;
	if (mMusicBase.IsValid())
	{
		mMusicBase.Stop();
		mMusicBase = en::MusicPtr();
	}
	if (mMusicOther.IsValid())
	{
		mMusicOther.Stop();
		mMusicOther = en::MusicPtr();
	}
}

void AudioTrackMixer3000::UpdateTempo(en::Time dt)
{
	// TODO
}

void AudioTrackMixer3000::UpdateVolumes()
{
	const en::F32 factor = GetDistanceFromSolarFactor();
	if (mMusicBase.IsValid())
	{
		mMusicBase.SetVolume(baseTrack.volumeDefault * factor);
	}
	if (mMusicOther.IsValid())
	{
		mMusicOther.SetVolume(tracks[static_cast<en::U32>(mSecondTrack)].volumeDefault * factor);
	}
}

void AudioTrackMixer3000::ChangeTrack(SecondTrackType newTrack)
{
	if (newTrack != mSecondTrack)
	{
		mSecondTrack = newTrack;
		StartTrack();
	}
}

en::F32 AudioTrackMixer3000::GetDistanceFromSolarFactor() const
{
	if (distanceFromSolarMax > 0.0f)
	{
		return en::Math::Clamp(mDistanceFromSolar, 0.0f, distanceFromSolarMax) / distanceFromSolarMax;
	}
	else
	{
		return 1.0f;
	}
}

en::F32 AudioTrackMixer3000::GetDistanceFromSolar() const
{
	return mDistanceFromSolar;
}

void AudioTrackMixer3000::SetDistanceFromSolar(en::F32 distanceFromSolar)
{
	mDistanceFromSolar = distanceFromSolar;
}

AudioTrackMixer3000::SecondTrackType AudioTrackMixer3000::GetSecondTrack() const
{
	return mSecondTrack;
}

void AudioTrackMixer3000::StartTrack()
{
	mMusicOther = en::AudioSystem::GetInstance().PlayMusic(tracks[static_cast<en::U32>(mSecondTrack)].musicIdentifier.c_str());
}

#ifdef ENLIVE_ENABLE_IMGUI
AudioTrackMixer3000Tool::AudioTrackMixer3000Tool() : en::ImGuiTool() {}
#endif