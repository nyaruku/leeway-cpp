// C++ port of C# Leeway

#include <algorithm>
#include <iostream>
#include <cmath>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <iomanip>
#include <iterator>
#include <cctype>
#include <cpr/cpr.h>

class LeewayCalculator {
public:

	// works
	static float CalcRotations(int length, float adjustTime)
	{
		float num1 = 0.0f;
		float num2 = static_cast<float>(8E-05f + std::max<double>(0.0f, 5000.0f - length) / 1000.0f / 2000.0f) / adjustTime;
		float val1 = 0.0f;
		int num3 = static_cast<int>(length - std::floor(50.0 / 3.0 * static_cast<double>(adjustTime)));
		for (int index = 0; index < num3; ++index)
		{
			val1 += num2;
			num1 += std::min<float>(static_cast<double>(val1), 0.05f) / 3.14159265358979323846f;
		}
		return num1;
	}
	// works
	static double CalcLeeway(int length, float adjustTime, double od, int difficultyModifier)
	{
		int num = CalcRotReq(length, od, difficultyModifier);
		float d = CalcRotations(length, adjustTime);
		return num % 2 != 0 && std::floor(d) != 0.0 ? d - std::floor(d) + 1.0 : d - std::floor(d);
	}
	// works
	static int CalcRotReq(int length, double od, int difficultyModifier)
	{
		switch (difficultyModifier)
		{
		case 2:
			od /= 2.0;
			break;
		case 16:
			od = std::min<double>(10.0, od * 1.4);
			break;
		}
		double num = (od <= 5.0) ? (3.0 + 0.4 * od) : (2.5 + 0.5 * od);
		return static_cast<int>(length / 1000.0 * num);
	}
	// works
	static std::string CalcAmount(int rotations, int rotReq) {
		double num = std::max<double>(0.0, rotations - (rotReq + 3));
		if (rotReq % 2 != 0)
			return std::to_string(std::floor(num / 2.0)) + "k (F)";
		return fmod(num, 2.0) == 0.0 ? std::to_string(num / 2.0) + "k (T)" : std::to_string(std::floor(num / 2.0)) + "k+100 (T)";
	}
	// works
	static int CalcSpinBonus(int length, double od, float adjustTime, int difficultyModifier)
	{
		int num1 = static_cast<int>(CalcRotations(length, adjustTime));
		int num2 = CalcRotReq(length, od, difficultyModifier);
		return (num2 % 2 != 0 ? (num2 + 3) / 2 * 100 : (int)std::floor(num1 / 2.0) * 100) + (int)std::floor((num1 - (num2 + 3)) / 2.0) * 1100;
	}
	// works
	static std::string GetBeatmap(int id)
	{
		std::string url = "https://old.ppy.sh/osu/" + std::to_string(id);
		cpr::Response response = cpr::Get(cpr::Url{ url });
		if (response.status_code != 200) {
			std::cerr << "Error downloading file: " << response.status_code << std::endl;
			return "Error";
		}

		// Get the content as a string
		std::string fileContent = response.text;
		return fileContent;
	}
	// works
	static std::vector<std::string> GetBeatmapHitObjects(std::string beatmap) {
		std::vector<std::string> strArray;
		std::string token;
		std::istringstream tokenStream(beatmap);
		while (std::getline(tokenStream, token)) {
			strArray.push_back(token);
		}

		std::vector<std::string> beatmapHitObjects;
		for (size_t index1 = 0; index1 < strArray.size(); ++index1) {
			if (strArray[index1].find("HitObjects") != std::string::npos) {
				for (size_t index2 = index1 + 1; index2 < strArray.size() && strArray[index2].length() > 1; ++index2)
					beatmapHitObjects.push_back(strArray[index2]);
				break;
			}
		}
		return beatmapHitObjects;
	}
	// works
	static float GetHP(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("HPDrainRate:(.*?)\\r?\\n")))
		{
			std::cout << "HP:" << std::stof(match[1]) << "\n";
			return std::stof(match[1]);
		}
		return 0.0f;
	}
	// works
	static float GetCS(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("CircleSize:(.*?)\\r?\\n")))
		{
			return std::stof(match[1]);
		}
		return 0.0f;
	}
	// works
	static float GetOD(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("OverallDifficulty:(.*?)\\r?\\n")))
		{
			return std::stof(match[1]);
		}
		return 0.0f;
	}
	// works
	static double GetSliderMult(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("SliderMultiplier:(.*?)\\r?\\n")))
		{
			return std::stod(match[1]);
		}
		return 0.0;
	}
	// works
	static double GetSliderTRate(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("SliderTickRate:(.*?)\\r?\\n")))
		{
			return std::stod(match[1]);
		}
		return 0.0;
	}
	// works
	static std::string GetTitle(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("Title:(.*?)\\r?\\n")))
		{
			return match[1];
		}
		return "Title";
	}
	// works
	static std::string GetArtist(std::string beatmap)
	{
		std::regex rgx("Artist:(.*?)\\r?\\n");
		std::smatch match;

		if (std::regex_search(beatmap, match, rgx)) {
			return match[1].str();
		}

		return "Artist";
	}
	// works
	static std::string GetDifficultyName(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("Version:(.*?)\\r?\\n")))
		{
			return match[1];
		}
		return "DiffName";
	}
	// works
	static float GetAdjustTime(std::vector<std::string> mods) {
		for (std::string mod : mods) {
			if (mod == "DT" || mod == "NC") {
				return 1.5f;
			}
			else if (mod == "HT") {
				return 0.75f;
			}
		}
		return 1.0f;
	}
	// works
	static int GetDifficultyModifier(std::vector<std::string> mods) {
		for (std::string mod : mods) {
			if (mod == "HR") {
				return 16;
			}
			if (mod == "EZ") {
				return 2;
			}
		}
		return 0;
	}
	// works
	static int GetBeatmapVersion(std::string beatmap)
	{
		std::smatch match;
		if (std::regex_search(beatmap, match, std::regex("osu file format v([0-9]+)\\r?\\n")))
		{
			std::cout << std::stoi(match[1]) << "<<fileformat\n";
			return std::stoi(match[1]);
		}
		return 0;
	}
	// should works
	static std::vector<std::vector<int>> GetSpinners(std::string beatmap) {
		std::vector<std::string> beatmapHitObjects = GetBeatmapHitObjects(beatmap);
		std::vector<std::vector<double>> timingPoints = GetTimingPoints(beatmap);
		int beatmapVersion = GetBeatmapVersion(beatmap);
		double sliderMult = GetSliderMult(beatmap);
		double sliderTrate = GetSliderTRate(beatmap);
		std::vector<std::vector<int>> spinners;
		int num = 0;
		for (std::string str : beatmapHitObjects) {
			std::stringstream ss(str);
			std::string item;
			std::vector<std::string> strArray;
			while (std::getline(ss, item, ',')) {
				strArray.push_back(item);
			}
			switch (GetObjectType(std::stoi(strArray[3]))) {
			case 0:
				++num;
				break;

			case 1:{
				double length = std::stod(strArray[7]);
				int slides = std::stoi(strArray[6]);
				std::vector<double> beatLengthAt = GetBeatLengthAt(std::stoi(strArray[2]), timingPoints);
				int tickCount = CalculateTickCount(length, slides, sliderMult, sliderTrate, beatLengthAt[0], beatLengthAt[1], beatmapVersion);
				num += tickCount + slides + 1; }
				break;

			case 3:
				spinners.push_back(std::vector<int>{num, std::stoi(strArray[5]) - std::stoi(strArray[2])});
				++num;
				break;
			}
		}
		return spinners;
	}
	// works
	static std::string GetModsString(std::vector<std::string> mods)
	{
		std::string modsString = "";
		for (std::string mod : mods)
		{
			modsString += mod;
		}
		return modsString;
	}

	//works
	static std::string BestModCombination(std::string beatmap) {
		std::vector<std::vector<std::string>> allMods = {
			{"HD", "DT", "HR", "FL"},
			{"HD", "DT", "FL"},
			{"HD", "FL", "EZ"},
			{"HD", "HT", "FL"},
			{"HD", "FL"}
		};
		std::string bestMods = "";
		int highestScore = 0;
		for (std::vector<std::string> mods : allMods) {
			int score = CalculateMaxScore(beatmap, mods);
			if (score > highestScore) {
				highestScore = score;
				bestMods = GetModsString(mods);
			}
		}
		return std::to_string(highestScore) + " (" + ReorderMods(bestMods) + ")";
	}
	//works
	static int CalculateMaxScore(std::string beatmap, std::vector<std::string> mods) {
		double hp = (double)GetHP(beatmap);
		double cs = (double)GetCS(beatmap);
		double od = (double)GetOD(beatmap);

		int beatmapVersion = GetBeatmapVersion(beatmap);
		float adjustTime = GetAdjustTime(mods);
		int difficultyModifier = GetDifficultyModifier(mods);

		double sliderMult = GetSliderMult(beatmap);
		double sliderTrate = GetSliderTRate(beatmap);

		std::vector<std::string> beatmapHitObjects = GetBeatmapHitObjects(beatmap);

		int startTime = std::stoi(split(beatmapHitObjects[0], ',')[2]);

		int endTime = std::stoi(split(beatmapHitObjects[beatmapHitObjects.size() - 1], ',')[2]);

		std::vector<std::vector<double>> timingPoints = GetTimingPoints(beatmap);
		int num1 = 0;
		int num2 = 0;
		int num3 = 0;
		int num4 = CalculateDrainTime(beatmap, startTime, endTime) / 1000;
		double num5 = static_cast<int>(round((hp + od + cs + static_cast<double>(std::clamp(static_cast<float>(beatmapHitObjects.size() / num4 * 8.0), 0.0f, 16.0f))) / 38.0 * 5.0) * CalculateModMultiplier(mods));
		int num6 = 0;
		for (std::string str : beatmapHitObjects) {
			std::vector<std::string> strArray = split(str, ',');
			int objectType = GetObjectType(std::stoi(strArray[3]));
			if (objectType == 0) {
				++num6;
				num1 += 300 + static_cast<int>(std::max<int>(0, num2 - 1) * (12.0 * num5));

				++num2;
			}
			if (objectType == 1) {
				setlocale(LC_ALL, "en_US.UTF-8");
				double length = std::stod(strArray[7]);
				int slides = std::stoi(strArray[6]);
				std::vector<double> beatLengthAt = GetBeatLengthAt(std::stoi(strArray[2]), timingPoints);
				int tickCount = CalculateTickCount(length, slides, sliderMult, sliderTrate, beatLengthAt[0], beatLengthAt[1], beatmapVersion);
				num3 += tickCount * 10 + (slides + 1) * 30;
				num2 += tickCount + slides + 1;
				num1 += 300 + static_cast<int>(std::max<int>(0, num2 - 1) * (12.0 * num5));
			}
			else if (objectType == 3) {
				num1 += 300 + static_cast<int>(std::max<int>(0, num2 - 1) * (12.0 * num5));
				int length = std::stoi(strArray[5]) - std::stoi(strArray[2]);
				num3 += CalcSpinBonus(length, od, adjustTime, difficultyModifier);
				++num2;
			}
		}
		return num1 + num3;
	}
	//works
	static int CalculateTickCount(double length, int slides, double sliderMult, double sliderTRate, double beatLength, double sliderVMult, int beatmapVersion)
	{
		double num1 = Clamp(std::abs(sliderVMult), 10.0, 1000.0) * length * beatLength / (sliderMult * 10000.0);
		double num2 = beatLength / sliderTRate;
		if (beatmapVersion < 8) {
			num2 *= Clamp(std::abs(sliderVMult), 10.0, 1000.0) / 100.0;
		}

		double num3 = num1 - num2;
		int num4 = 0;
		for (; num3 >= 10.0; num3 -= num2) {
			++num4;
		}

		return num4 + num4 * (slides - 1);
	}
	//works
	static int CalculateDrainTime(std::string beatmap, int startTime, int endTime) {
		std::vector<std::string> strArray1;
		std::stringstream ss(beatmap);
		std::string token;

		while (std::getline(ss, token, '\n')) {
			strArray1.push_back(token);
		}

		std::vector<int> intList;
		for (size_t index1 = 0; index1 < strArray1.size(); ++index1) {
			if (strArray1[index1].find("Break Periods") != std::string::npos) {
				for (size_t index2 = index1 + 1; index2 < strArray1.size(); ++index2) {
					std::stringstream ss2(strArray1[index2]);
					std::vector<std::string> strArray2;
					while (std::getline(ss2, token, ',')) {
						strArray2.push_back(token);
					}
					if (strArray2.size() == 3)
						intList.push_back(std::stoi(strArray2[2]) - std::stoi(strArray2[1]));
					else
						break;
				}
				break;
			}
		}
		int drainTime = endTime - startTime;
		for (int num : intList)
			drainTime -= num;
		return drainTime;
	}

	static std::vector<std::vector<double>> GetTimingPoints(std::string beatmap) {
		std::istringstream iss(beatmap);
		std::vector<std::string> strArray1((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
		std::vector<std::vector<double>> timingPoints;
		for (int index1 = 0; index1 < strArray1.size(); ++index1) {
			if (strArray1[index1].find("TimingPoints") != std::string::npos) {
				for (int index2 = index1 + 1; index2 < strArray1.size(); ++index2) {
					std::istringstream iss(strArray1[index2]);
					std::vector<std::string> strArray2((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
					if (strArray2.size() > 1) {
						double num1 = std::stod(strArray2[0]);
						double num2 = std::stod(strArray2[1]);
						timingPoints.push_back({ num1, num2 });
					}
					else {
						break;
					}
				}
				break;
			}
		}
		for (std::vector<double> numArray : timingPoints) {
			if (numArray[1] > 0.0) {
				timingPoints.insert(timingPoints.begin(), { 0.0, numArray[1] });
				break;
			}
		}
		return timingPoints;
	}

	static int GetBeatmapID(const std::string& beatmap)
	{
		std::regex pattern("BeatmapID:([0-9]+)\\r?\\n");
		std::smatch match;
		if (std::regex_search(beatmap, match, pattern))
		{
			return std::stoi(match[1]);
		}
		return -1;
	}

	static std::vector<double> GetBeatLengthAt(int time, std::vector<std::vector<double>> timingPoints) {
		double num1 = 0.0;
		double num2 = -100.0;
		for (int index = 0; index < (timingPoints.size()); ++index) {
			if (time >= timingPoints[index][0]) {
				if (timingPoints[index][1] > 0.0) {
					num1 = timingPoints[index][1];
					num2 = -100.0;
				}
				else {
					num2 = timingPoints[index][1];
				}
			}
		}
		return std::vector<double> { num1, num2 };
	}

	static double Clamp(double value, double min, double max)
	{
		if (value < min) {
			return min;
		}
		return value > max ? max : value;
	}

	static float Clamp(float value, float min, float max)
	{
		if ((double)value < (double)min)
			return min;
		return (double)value > (double) max ? max : value;
	}

	static int GetObjectType(int id) {
		std::string str1 = "00000000" + std::bitset<32>(id).to_string();
		std::string str2 = str1.substr(str1.length() - 8, 8);
		if (str2[4] == '1')
			return 3;
		return str2[6] == '1' ? 1 : 0;
	}

	static std::vector<std::string> GetMods(std::string mods) {
		if (mods.empty() || mods.size() < 2 || mods.size() % 2 != 0)
			return std::vector<std::string>{"HD", "NC", "HR", "FL"};

		std::vector<std::string> mods1(mods.size() / 2);
		for (int index = 0; index < mods1.size(); ++index)
			mods1[index] = mods.substr(index * 2, 2);

		return mods1;
	}

	static bool IsValidModCombo(std::vector<std::string> mods) {
		if (mods.empty())
			return false;

		// Check for specific valid combinations
		std::vector<std::string> validCombo1 = { "HD", "DT", "HR", "FL" };
		std::vector<std::string> validCombo2 = { "HD", "NC", "HR", "FL" };

		if (mods == validCombo1 || mods == validCombo2)
			return true;

		// Check individual mods and their combinations
		for (int i = 0; i < mods.size(); ++i) {
			if (!IsValidMod(mods[i]) || (mods[i] == "NM" && mods.size() > 2))
				return false;

			for (int j = i + 1; j < mods.size(); ++j) {
				if (!IsValidCombination(mods[i], mods[j]))
					return false;
			}
		}

		return true;
	}

	static bool IsValidCombination(std::string mod1, std::string mod2)
	{
		if (mod1 == "DT") {
			return !((mod2 == "NC") || (mod2 == "HT"));
		}
		if (mod1 == "NC") {
			return !((mod2 == "DT") || (mod2 == "HT"));
		}
		if (mod1 == "HT") {
			return !((mod2 == "DT") || (mod2 == "NC"));
		}
		if (mod1 == "HR") {
			return !(mod2 == "EZ");
		}
		if (mod1 == "EZ") {
			return !(mod2 == "HR");
		}

		return true;
	}

	static bool IsValidMod(std::string mod)
	{
		return mod == "HD" || mod == "DT" || mod == "HR" || mod == "FL" || mod == "NC" || mod == "HT" || mod == "EZ";
	}

	static std::string RemoveUselessMods(std::string mods) {
		if (mods == "None" || mods == "SD" || mods == "PF" || mods == "TD" || mods == "SO") {
			return "NM";
		}

		if (mods.size() == 2) {
			return mods;
		}

		std::string usefulMods = "";
		std::vector<std::string> modArray = split(mods, ',');

		for (std::string mod : modArray) {
			if (mod == "HD" || mod == "DT" || mod == "NC" || mod == "HR" || mod == "FL" || mod == "EZ" || mod == "HT") {
				usefulMods += mod;
			}
		}
		return usefulMods.empty() ? "NM" : usefulMods;
	}

	static std::string ReorderMods(std::string mods)
	{
		//mods = toUpper(mods);
		if (mods.size() <= 2)
			return mods;

		std::string reorderedMods = "";

		// Check for HD
		for (int i = 0; i < mods.size() - 1; i += 2)
		{
			if (mods.substr(i, 2) == ("HD"))
			{
				reorderedMods += mods.substr(i, 2);
				break;
			}
		}

		// Check for DT, NC, HT
		for (int i = 0; i < mods.size() - 1; i += 2)
		{
			if (mods.substr(i, 2) == ("DT") || mods.substr(i, 2) == ("NC") || mods.substr(i, 2) == ("HT"))
			{
				reorderedMods += mods.substr(i, 2);
				break;
			}
		}

		// Check for HR, EZ
		for (int i = 0; i < mods.size() - 1; i += 2)
		{
			if (mods.substr(i, 2) == ("HR") || mods.substr(i, 2) == ("EZ"))
			{
				reorderedMods += mods.substr(i, 2);
				break;
			}
		}

		// Check for FL
		for (int i = 0; i < mods.size() - 1; i += 2)
		{
			if (mods.substr(i, 2) == ("FL"))
			{
				reorderedMods += mods.substr(i, 2);
				break;
			}
		}

		return reorderedMods;
	}

	static bool HasLeaderboard(std::string beatmapPage) {
		std::regex pattern("<td><b>(.*?)</b></td>", std::regex_constants::ECMAScript);
		return !beatmapPage.empty() && std::regex_search(beatmapPage, pattern);
	}

	static double CalculateModMultiplier(std::vector<std::string> mods) {
		double modMultiplier = 1.0;
		for (std::string mod : mods) {
			if (mod == "NF" || mod == "EZ") {
				modMultiplier *= 0.5;
				break;
			}
			if (mod == "HT") {
				modMultiplier *= 0.3;
				break;
			}
			if (mod == "HD") {
				modMultiplier *= 1.06;
				break;
			}
			if (mod == "HR") {
				modMultiplier *= 1.06;
				break;
			}
			if (mod == "DT" || mod == "NC" || mod == "FL") {
				modMultiplier *= 1.12;
				break;
			}
			if (mod == "SO") {
				modMultiplier *= 0.9;
				break;
			}
			modMultiplier *= 0.0;
			break;
		}
		return modMultiplier;
	}

	static std::vector<std::vector<std::string>> GetTop50(std::string beatmapPage) {
		std::regex expression("<tr class='row[1-2]p'>.*?c=(\\d+)&.*?<td>(?:<b>)?((?:\\d{1,3},)?(?:(?:\\d{1,3},)+)?(?:\\d{1,3}))(?:</b>)?</td>.*?<td>(?:<b>)?(.*?)%(?:</b>)?</td>.*?<td>(None|(?:(?:[A-Z]{2},)+)?[A-Z]{2})</td>.*?</tr>", std::regex_constants::ECMAScript);
		std::sregex_iterator it(beatmapPage.begin(), beatmapPage.end(), expression);
		std::sregex_iterator end;

		std::vector<std::vector<std::string>> top50;
		for (; it != end; ++it) {
			std::vector<std::string> row;
			row.push_back(it->str(2));
			row.push_back(it->str(3));
			row.push_back(ReorderMods(RemoveUselessMods(it->str(4))));
			row.push_back(it->str(1));
			top50.push_back(row);
		}
		return top50;
	}

	static int GetBestScoreIndex(std::vector<std::vector<std::string>> scores, std::vector<std::string> mods) {
		for (int bestScoreIndex = 0; bestScoreIndex < scores.size(); ++bestScoreIndex) {
			if (IsSameMods(mods, GetMods(scores[bestScoreIndex][2])))
				return bestScoreIndex;
		}
		return -1;
	}

	static bool IsSameMods(std::vector<std::string> mods1, std::vector<std::string> mods2) {
		if (mods1.size() != mods2.size())
			return false;
		for (int index = 0; index < mods1.size(); ++index)
		{
			if (mods1[index] == "DT" || mods1[index] == "NC")
			{
				if (std::find(mods2.begin(), mods2.end(), "DT") == mods2.end() &&
					std::find(mods2.begin(), mods2.end(), "NC") == mods2.end())
					return false;
			}
			else if (std::find(mods2.begin(), mods2.end(), mods1[index]) == mods2.end())
				return false;
		}
		return true;
	}

	static std::string GetBeatmapPage(int mapID)
	{
		// URL for the GET request
		std::string url = "https://old.ppy.sh/b/" + std::to_string(mapID);

		cpr::Response response = cpr::Get(cpr::Url{ url });
		if (response.status_code != 200) {
			std::cout << "Error downloading file: " << response.status_code << std::endl;
			return "";
		}

		// Get the content as a string
		std::string fileContent = response.text;
		return fileContent;
	}

	static std::string GetNumberOneOnBeatmapPage(std::string beatmapPage)
	{
		std::string foundUsername = "";
		std::string searchString = "is in the lead";
		std::vector<std::string> lines = split(beatmapPage, '\n');

		for (std::string line : lines)
		{
			if (contains(line, searchString))
			{
				char stopChar = '<';
				int startIndex = line.find("'>") + 2;
				while (line[startIndex] != stopChar)
				{
					foundUsername += line[startIndex];
					startIndex++;
				}
				break;
			}
		}
		return foundUsername;
	}

	static std::vector<std::string> CalculateBeatmapMaxScore(std::string beatmap)
	{
		std::vector<std::string> modCombinations;
		modCombinations.push_back("HDNCHRFL");
		modCombinations.push_back("HDNCFL");
		modCombinations.push_back("HDHRFL");
		modCombinations.push_back("HDFL");
		modCombinations.push_back("EZHDNCFL");
		modCombinations.push_back("EZHDFL");
		modCombinations.push_back("EZHDHTFL");
		modCombinations.push_back("HDHTFL");
		modCombinations.push_back("HDHTHRFL");
	

		int maxScore = 0;
		std::string maxScoreMods = "";

		for (std::string mods : modCombinations)
		{
			int currentScore = CalculateMaxScore(beatmap, GetMods(mods));
			
			if (currentScore > maxScore)
			{
				maxScore = currentScore;
				maxScoreMods = mods;
			}
		}
		std::vector<std::string> result;
		result.push_back(std::to_string(maxScore));
		result.push_back(maxScoreMods);
		return result;
	}

	static void PrintLeaderboardInfo(int num3, int maxScore, std::string str, std::string numberOne, double num4)
	{
		if (num3 == maxScore)
		{
			PrintCappedInfo(str, numberOne);
		}
		else if (num3 < maxScore)
		{
			PrintNotCappedInfo(str, numberOne, maxScore - num3);
		}
		else
		{
			PrintOverCapInfo(num3, maxScore, str, numberOne, num3 - maxScore, num4);
		}
	}

	static void PrintCappedInfo(std::string str, std::string numberOne)
	{
		std::cout << "\033[31m" << " Capped" << "\033[0m" << std::endl;

		std::cout << ": (" << str << ") #1: " << numberOne;
	}

	static void PrintNotCappedInfo(std::string str, std::string numberOne, int scoreDifference)
	{
		std::cout << "\033[31m" << " Not Capped" << "\033[0m" << std::endl;

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << scoreDifference;

		std::cout << ": +" << ss.str() << " (" << str << ") #1: " << numberOne << std::endl;
	}

	static void PrintOverCapInfo(int num3, int maxScore, std::string str, std::string numberOne, int scoreDifference, double num4)
	{
		std::cout << "\033[31m" << " Over Capped" << "\033[0m" << std::endl;

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << scoreDifference;

		std::cout << ": +" << ss.str() << " (" << str << ") #1: " << numberOne << std::endl;

		if (num3 % 100 == maxScore % 100)
		{
			PrintReworkInfo(num4);
		}

		std::cout << "\n";
	}

	static void PrintReworkInfo(double num4)
	{
		if (num4 < 2801770131.0)
		{
			std::cout << "\033[31m" << " Pre-Spin Rework" << "\033[0m" << std::endl;
		}
		else
		{
			std::cout << "\033[31m" << " Post-Spin Rework" << "\033[0m" << std::endl;
		}
	}

	static void PrintTable(int beatmapID, std::string beatmap, std::vector<std::string> mods, bool leaderboardLookups = true)
	{
		std::string beatmapPage;
		if (beatmapID > 0 && leaderboardLookups) {
			beatmapPage = GetBeatmapPage(beatmapID);
		}
		
		bool flag = false;
		int maxScore;
		if (mods.empty())
		{
			std::vector<std::string> beatmapMaxScore = CalculateBeatmapMaxScore(beatmap);
			maxScore = std::stoi(beatmapMaxScore[0]);
			mods = GetMods(beatmapMaxScore[1]); // Split mods string into array
			// Check if mods are same as "HDNCHRFL"
			if (!IsSameMods(mods, GetMods("HDNCHRFL"))) {
				flag = true;
			}
				
		}
		else
		{
			// Calculate max score
			maxScore = CalculateMaxScore(beatmap, mods);
		}
		std::vector<std::vector<int>> spinners = GetSpinners(beatmap);

		float adjustTime = GetAdjustTime(mods);
		float od = GetOD(beatmap);
		int difficultyModifier = GetDifficultyModifier(mods);
		std::string result;
		for (int index = 0; index < spinners.size(); ++index)
		{
			int length = spinners[index][1];
			int num1 = spinners[index][0];
			float rotations = CalcRotations(length, adjustTime);
			int rotReq = CalcRotReq(length, (double)od, difficultyModifier);
			std::string str = CalcAmount((int)rotations, rotReq);
			double num2 = CalcLeeway(length, adjustTime, (double)od, difficultyModifier);
			
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(5);

			ss << index + 1 << ", " << length << ", " << num1 << ", " << str << ", ";
			ss << rotations << ", " << num2 << "\n";

			result += ss.str();
		}
		
		
		// Print beatmap info
		std::cout << " " << GetArtist(beatmap) << " - " << GetTitle(beatmap) << " (" << GetDifficultyName(beatmap) << ")" << std::endl;
		std::cout << " Max Score: " << std::fixed << std::setprecision(0) << maxScore << " (";
		
		if (flag)
		{
			std::cout << "\033[35m"; // Set the text color to magenta
			std::cout << GetModsString(mods);
			std::cout << "\033[0m"; // Reset the text color
		}
		else {
			std::cout << GetModsString(mods);
		}			
		std::cout << ")" << std::endl;
		std::cout << " Best Mods: " << BestModCombination(beatmap) << std::endl;
		
		// Check if beatmap has leaderboard
		if (HasLeaderboard(beatmapPage) && leaderboardLookups)
		{
			std::string numberOne = GetNumberOneOnBeatmapPage(beatmapPage);
			int firstPlaceScore = -1;
			std::string modsFromTop50;
			double beatmapUnixTimestamp = 2801770131.0;

			if (flag)
			{
				std::vector<std::vector<std::string>> top50 = GetTop50(beatmapPage);
				firstPlaceScore = std::stoi(top50[0][0]);
				modsFromTop50 = top50[0][2];
				beatmapUnixTimestamp = std::stod(top50[0][3]);
			}
			else
			{
				std::vector<std::vector<std::string>> top50 = GetTop50(beatmapPage);
				int bestScoreIndex = GetBestScoreIndex(top50, mods);

				if (bestScoreIndex >= 0)
				{
					firstPlaceScore = std::stoi(top50[bestScoreIndex][0]);
					modsFromTop50 = top50[bestScoreIndex][2];
					beatmapUnixTimestamp =  std::stod(top50[bestScoreIndex][3]);
				}
			}

			if (firstPlaceScore >= 0)
			{
				PrintLeaderboardInfo(firstPlaceScore, maxScore, modsFromTop50, numberOne, beatmapUnixTimestamp);
			}
		}
		else {
			std::cout << "NoLB\n";
		}
		std::cout << result;
		// Print table footer
		std::cout << "------------------------------------------------------------" << std::endl;
	}

	// helper functions

	static std::vector<std::string> split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}

	static std::string toUpper(std::string input) {
		std::transform(input.begin(), input.end(), input.begin(), ::toupper);
		std::cout << input << "<<Upper\n";
		return input;
	}

	static bool contains(std::string input, std::string key) {
		if (input.find(key) != std::string::npos) {
			return true;
		}
		return false;
	}
};
int main()
{
	std::string input;
	std::cout << "Enter Beatmap ID (+Mods): ";
	std::getline(std::cin, input);
	try
	{
		std::string beatmap;
		int num;
		std::regex fileRegex("\"(.*?)\"");
		std::smatch match;
		if (std::regex_search(input, match, fileRegex) && std::ifstream(match[1].str()))
		{
			std::ifstream file(match[1].str());
			std::stringstream buffer;
			buffer << file.rdbuf();
			beatmap = buffer.str();
			num = LeewayCalculator::GetBeatmapID(beatmap);
		}
		else
		{
			std::regex idRegex1("^([0-9]+)");
			std::regex idRegex2("osu/([0-9]+)");
			if (std::regex_search(input, match, idRegex1))
			{
				num = std::stoi(match[1].str());
			}
			else if (std::regex_search(input, match, idRegex2))
			{
				num = std::stoi(match[1].str());
			}
			else
			{
				throw std::invalid_argument("Invalid input format");
			}
		}
		std::cout << "tried to get id:" << num << "\n";
		beatmap = LeewayCalculator::GetBeatmap(num);

		std::vector<std::string> mods1;
		std::regex modsRegex("([A-Za-z]+)$");
		if (std::regex_search(input, match, modsRegex))
		{
			std::vector<std::string> mods2 = LeewayCalculator::GetMods(match[1].str());
			if (LeewayCalculator::IsValidModCombo(mods2))
			{
				mods1 = mods2;
			}
			else {
				mods1.clear();
			}
		}

		mods1 = LeewayCalculator::GetMods(LeewayCalculator::ReorderMods(LeewayCalculator::GetModsString(mods1)));
		LeewayCalculator::PrintTable(num, beatmap, mods1);
	}
	catch (...)
	{
		// Handle exceptions (e.g., invalid input)
	}

	return 0;
}