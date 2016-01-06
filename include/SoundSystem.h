#pragma once

#include <string>

struct SoundSystem {
	SoundSystem();
	~SoundSystem();

	bool initialize();

	void setSndNumber(size_t count);
	bool load(size_t n, const std::string &filename);
	void play(size_t n, float volume);

	bool play(const std::string &filename, float volume);
	bool playBackground(const std::string &filename, float volume);

	void update();

protected:
	struct SoundSystemImpl;
	SoundSystemImpl *pimpl;
};

