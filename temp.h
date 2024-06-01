std::string GetArtist(std::string beatmap);
std::string GetDifficultyName(std::string beatmap);
std::vector<std::string> GetBeatmapHitObjects(std::string beatmap);
int CalcSpinBonus(int length, double od, float adjustTime, int difficultyModifier);
std::string CalcAmount(int rotations, int rotReq);
int CalcRotReq(int length, double od, int difficultyModifier);
double CalcLeeway(int length, float adjustTime, double od, int difficultyModifier);
float CalcRotations(int length, float adjustTime);
#pragma once

std::vector<std::string> GetMods(std::string mods);

int CalculateMaxScore(std::string beatmap, std::vector<std::string> mods);

int GetBeatmapVersion(std::string beatmap);

int GetDifficultyModifier(std::vector<std::string> mods);

float GetAdjustTime(std::vector<std::string> mods);

float GetHP(std::string beatmap);

float GetCS(std::string beatmap);

float GetOD(std::string beatmap);

double GetSliderMult(std::string beatmap);

double GetSliderTRate(std::string beatmap);

std::string GetTitle(std::string beatmap);

std::vector<std::vector<double>> GetTimingPoints(std::string beatmap);

int CalculateDrainTime(std::string beatmap, int startTime, int endTime);

int CalculateTickCount(double length, int slides, double sliderMult, double sliderTRate, double beatLength, double sliderVMult, int beatmapVersion);

std::string GetModsString(std::vector<std::string> mods);

std::vector<std::vector<int>> GetSpinners(std::string beatmap);

float Clamp(float value, float min, float max);
double Clamp(double value, double min, double max);

int GetObjectType(int id);

std::vector<double> GetBeatLengthAt(int time, std::vector<std::vector<double>> timingPoints);

std::string GetBeatmapPage(int mapID);

int GetBestScore(std::vector<std::vector<std::string>> scores, std::vector<std::string> mods);

bool IsSameMods(std::vector<std::string> mods1, std::vector<std::string> mods2);

std::vector<std::vector<std::string>> GetTop50(std::string beatmapPage);

double CalculateModMultiplier(std::vector<std::string> mods);

std::string RemoveUselessMods(std::string mods);
