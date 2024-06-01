#include <algorithm> // For std::max and std::min
#include <iostream>
#include <cmath>

#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <iomanip>
#include "temp.h"
#include <iterator>
#include <cpr/cpr.h>

const float DT = 1.5f;
const float HT = 0.75f;
const int HR = 16;
const int EZ = 2;
const int CIRCLE = 0;
const int SLIDER = 1;
const int SPINNER = 3;

std::vector<std::string> split(const std::string& s, char delimiter)
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

std::vector<std::string> GetMods(std::string mods) {
    if (mods.empty() || mods.length() < 2 || mods.length() % 2 != 0)
        return std::vector<std::string>{"HD", "NC", "HR", "FL"};

    std::vector<std::string> mods1(mods.length() / 2);
    for (int index = 0; index < mods1.size(); ++index)
        mods1[index] = mods.substr(index * 2, 2);

    return mods1;
}

//
int CalculateMaxScore(std::string beatmap, std::vector<std::string> mods) {
    double hp = GetHP(beatmap);
    double cs = GetCS(beatmap);
    double od = GetOD(beatmap);
    int beatmapVersion = GetBeatmapVersion(beatmap);
    float adjustTime = GetAdjustTime(mods);
    int difficultyModifier = GetDifficultyModifier(mods);
    double sliderMult = GetSliderMult(beatmap);
    double sliderTrate = GetSliderTRate(beatmap);
    std::vector<std::string> beatmapHitObjects = GetBeatmapHitObjects(beatmap);

 
    int startTime = std::stoi(split(beatmapHitObjects[0], ',')[2]);
    

    int endTime = std::stoi(split(beatmapHitObjects[beatmapHitObjects.size()-1], ',')[2]);


    std::vector<std::vector<double>> timingPoints = GetTimingPoints(beatmap);
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = CalculateDrainTime(beatmap, startTime, endTime) / 1000;
    double num5 = round((hp + od + cs + std::clamp(static_cast<float>(beatmapHitObjects.size() / num4 * 8.0), 0.0f, 16.0f)) / 38.0 * 5.0) * CalculateModMultiplier(mods);
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

std::vector<std::string> CalculateBeatmapMaxScore(std::string beatmap)
{
    int maxScore = CalculateMaxScore(beatmap, GetMods("HDNCHRFL"));
    std::string str = "HDNCHRFL";
    std::vector<std::string> mods = { "HDNCFL", "HDHRFL", "HDFL", "EZHDNCFL", "EZHDFL", "EZHDHTFL", "HDHTFL", "HDHTHRFL" };

    for (const auto& mod : mods)
    {
        int score = CalculateMaxScore(beatmap, GetMods(mod));
        if (score > maxScore)
        {
            maxScore = score;
            str = mod;
        }
    }

    return { std::to_string(maxScore), str };
}

void PrintTable(int beatmapID, std::string beatmap, std::vector<std::string> mods)
{
    std::string beatmapPage;
    if (beatmapID > 0)
        beatmapPage = GetBeatmapPage(beatmapID);

    bool flag = false;
    int maxScore;
    if (mods.empty())
    {
        std::vector<std::string> beatmapMaxScore = CalculateBeatmapMaxScore(beatmap);
        maxScore = std::stoi(beatmapMaxScore[0]);
        mods = GetMods(beatmapMaxScore[1]); // Split mods string into array
        // Check if mods are same as "HDNCHRFL"
        if (!IsSameMods(mods, GetMods("HDNCHRFL")))
            flag = true;
    }
    else
    {
        // Calculate max score
        maxScore = CalculateMaxScore(beatmap, mods);
    }

    std::vector<std::vector<int>> spinners = GetSpinners(beatmap);
    float adjustTime = GetAdjustTime(mods);
    float od = GetOD(beatmap);
    std::string modsString = GetModsString(mods);
    int difficultyModifier = GetDifficultyModifier(mods);
    // Print beatmap info
    std::cout << " " << GetArtist(beatmap) << " - " << GetTitle(beatmap) << " (" << GetDifficultyName(beatmap) << ")" << std::endl;
    std::cout << " Max Score: " << std::fixed << std::setprecision(0) << maxScore << " (";
    if (flag)
    {
        std::cout << "\033[35m"; // Set the text color to magenta
        std::cout << modsString;
        std::cout << "\033[0m"; // Reset the text color
    }
    else
        std::cout << modsString;
    std::cout << ")" << std::endl;

    // Check if beatmap has leaderboard
    if (!beatmapPage.empty())
    {
        // Get best score from leaderboard
        int bestScore = GetBestScore(GetTop50(beatmapPage), mods);
        std::cout << bestScore << "\n";
        if (bestScore > 0)
        {
            // Print leaderboard information
            if (bestScore == maxScore)
            {
                std::cout << "\033[31m"; // Set the text color to red
                std::cout << " Capped";
                std::cout << "\033[0m"; // Reset the text color
                std::cout << ": (" << modsString << ")" << std::endl;
            }
            else if (bestScore < maxScore)
            {
                std::cout << "\033[32m"; // Set the text color to green
                std::cout << " Not Capped";
                std::cout << "\033[0m"; // Reset the text color
                std::cout << ": +" << maxScore - bestScore << " (" << modsString << ")" << std::endl;
            }
            else
            {
                std::cout << "\033[33m"; // Set the text color to yellow
                std::cout << " Over Cap";
                std::cout << "\033[0m"; // Reset the text color
                std::cout << ": +" << bestScore - maxScore << " (" << modsString << ")" << std::endl;
            }
        }
    }
    else {
        std::cout << "NoLB\n";
    }

    // Print table header
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "| # | Length | Combo | Amount (+100) | Rotations | Leeway  |" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    // Print table rows
    for (int index = 0; index < spinners.size(); ++index)
    {
        int length = spinners[index][1];
        int combo = spinners[index][0];
        float rotations = CalcRotations(length, adjustTime);

        int rotReq = CalcRotReq(length, od, difficultyModifier);
        std::string amount = CalcAmount(rotations, rotReq);
        double leeway = CalcLeeway(length, adjustTime, od, difficultyModifier);

        std::cout << "| " << std::setw(2) << index + 1 << " | " << std::setw(6) << length << " | "
            << std::setw(5) << combo << " | " << std::setw(13) << std::setprecision(0) << amount << " | "
            << std::setw(10) << std::fixed << std::setprecision(5) << rotations << " | "
            << std::setw(7) << std::fixed << std::setprecision(5) << leeway << " |" << std::endl;
    }

    // Print table footer
    std::cout << "------------------------------------------------------------" << std::endl;
}


std::string GetBeatmapPage(int mapID)
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

int GetBestScore(std::vector<std::vector<std::string>> scores, std::vector<std::string> mods) {
    for (int index = 0; index < scores.size(); ++index) {
        if (IsSameMods(mods, GetMods(scores[index][2])))
            return std::stoi(scores[index][0]);
    }
    return -1;
}

bool IsSameMods(std::vector<std::string> mods1, std::vector<std::string> mods2) {
    if (mods1.size() != mods2.size())
        return false;
    for (int index = 0; index < mods1.size(); ++index) {
        if (mods1[index] == "DT" || mods1[index] == "NC") {
            if (std::find(mods2.begin(), mods2.end(), "DT") == mods2.end() &&
                std::find(mods2.begin(), mods2.end(), "NC") == mods2.end())
                return false;
        }
        else if (std::find(mods2.begin(), mods2.end(), mods1[index]) == mods2.end())
            return false;
    }
    return true;
}

std::vector<std::vector<std::string>> GetTop50(std::string beatmapPage) {
    std::regex r("<tr class='row[1-2]p'>.*?<td>(?:<b>)?((?:\\d{1,3},)?(?:(?:\\d{1,3},)+)?(?:\\d{1,3}))(?:</b>)?</td>.*?<td>(?:<b>)?(.*?)%(?:</b>)?</td>.*?<td>(None|(?:(?:[A-Z]{2},)+)?[A-Z]{2})</td>.*?</tr>\\r?\\n");
    std::smatch match;
    std::vector<std::vector<std::string>> top50;

    std::istringstream stream(beatmapPage);
    std::string line;
    while (std::getline(stream, line)) {
        if (std::regex_search(line, match, r)) {
            std::vector<std::string> row;
            row.push_back(match.str(1));
            row.push_back(match.str(2));
            row.push_back(RemoveUselessMods(match.str(3))); // Assuming you have this function defined somewhere
            top50.push_back(row);
        }
    }
    return top50;
}

double CalculateModMultiplier(std::vector<std::string> mods) {
    double modMultiplier = 1.0;
    for (auto& mod : mods) {
        if (mod == "NF" || mod == "EZ") {
            modMultiplier *= 0.5;
        }
        else {
            int num;
            if (mod == "HT") {
                modMultiplier *= 0.3;
                continue;
            }
            else if (mod == "HD") {
                num = 1;
            }
            else {
                num = (mod == "HR") ? 1 : 0;
            }
            if (num != 0) {
                modMultiplier *= 1.06;
            }
            else if (mod == "DT" || mod == "NC" || mod == "FL") {
                modMultiplier *= 1.12;
            }
            else if (mod == "SO") {
                modMultiplier *= 0.9;
            }
            else {
                modMultiplier *= 0.0;
            }
        }
    }
    return modMultiplier;
}

bool HasLeaderboard(std::string beatmapPage)
{
    std::regex pattern("<td><b>(.*?)</b></td>\\r?\\n");
    return std::regex_search(beatmapPage, pattern);
}

std::string RemoveUselessMods(std::string mods) {
    if (mods == "None" || mods == "SD" || mods == "PF" || mods == "TD" || mods == "SO")
        return "NM";
    if (mods.length() == 2)
        return mods;
    std::string result = "";
    std::string temp = mods;
    std::replace(temp.begin(), temp.end(), ',', ' ');  // replace ',' by ' '
    std::istringstream iss(temp);
    std::string mod;
    while (iss >> mod) {
        if (mod == "HD" || mod == "DT" || mod == "NC" || mod == "HR" || mod == "FL" || mod == "EZ" || mod == "HT")
            result += mod;
    }
    return result.empty() ? "NM" : result;
}

bool IsValidMod(std::string mod)
{
    return mod == "NF" || mod == "EZ" || mod == "HT" || mod == "HD" || mod == "HR" || mod == "DT" || mod == "NC" || mod == "FL" || mod == "NM";
}

bool IsValidModCombo(std::vector<std::string> mods) {
    int num;
    std::vector<std::string> validMods1 = { "HD", "DT", "HR", "FL" };
    std::vector<std::string> validMods2 = { "HD", "NC", "HR", "FL" };

    if (mods != validMods1)
        num = (mods == validMods2) ? 1 : 0;
    else
        num = 1;

    if (num != 0)
        return true;

    for (int index1 = 0; index1 < mods.size(); ++index1) {
        if (!IsValidMod(mods[index1]) || (mods[index1] == "NM" && mods.size() > 2))
            return false;

        if (index1 + 1 < mods.size()) {
            if (mods[index1] == "DT") {
                for (int index2 = index1 + 1; index2 < mods.size(); ++index2) {
                    if (mods[index2] == "NC" || mods[index2] == "HT")
                        return false;
                }
            }
            else if (mods[index1] == "NC") {
                for (int index3 = index1 + 1; index3 < mods.size(); ++index3) {
                    if (mods[index3] == "DT" || mods[index3] == "HT")
                        return false;
                }
            }
            else if (mods[index1] == "HT") {
                for (int index4 = index1 + 1; index4 < mods.size(); ++index4) {
                    if (mods[index4] == "DT" || mods[index4] == "NC")
                        return false;
                }
            }
            else if (mods[index1] == "HR") {
                for (int index5 = index1 + 1; index5 < mods.size(); ++index5) {
                    if (mods[index5] == "EZ")
                        return false;
                }
            }
            else if (mods[index1] == "EZ") {
                for (int index6 = index1 + 1; index6 < mods.size(); ++index6) {
                    if (mods[index6] == "HR")
                        return false;
                }
            }
        }
    }
    return true;
}

double Clamp(double value, double min, double max)
{
    if (value < min) {
        return min;
    }
    return value > max ? max : value;
   
}

float Clamp(float value, float min, float max)
{
    if ((double)value < (double)min)
        return min;
    return (double)value > (double) max ? max : value;
}

int GetObjectType(int id) {
    std::string str1 = "00000000" + std::bitset<32>(id).to_string();
    std::string str2 = str1.substr(str1.length() - 8, 8);
    if (str2[4] == '1')
        return 3;
    return str2[6] == '1' ? 1 : 0;
}
std::vector<double> GetBeatLengthAt(int time, std::vector<std::vector<double>> timingPoints) {
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

int GetBeatmapID(const std::string& beatmap)
{
    std::regex pattern("BeatmapID:([0-9]+)\\r?\\n");
    std::smatch match;
    if (std::regex_search(beatmap, match, pattern))
    {
        return std::stoi(match[1]);
    }
    return -1;
}

std::vector<std::vector<double>> GetTimingPoints(std::string beatmap) {
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
    for (auto& numArray : timingPoints) {
        if (numArray[1] > 0.0) {
            timingPoints.insert(timingPoints.begin(), { 0.0, numArray[1] });
            break;
        }
    }
    return timingPoints;
}

int CalculateDrainTime(std::string beatmap, int startTime, int endTime) {
    std::vector<std::string> strArray1;
    std::istringstream f(beatmap);
    std::string s;
    while (getline(f, s, '\n')) {
        strArray1.push_back(s);
    }

    std::vector<int> intList;
    for (int index1 = 0; index1 < strArray1.size(); ++index1) {
        if (strArray1[index1].find("Break Periods") != std::string::npos) {
            for (int index2 = index1 + 1; index2 < strArray1.size(); ++index2) {
                std::vector<std::string> strArray2;
                std::istringstream f(strArray1[index2]);
                std::string s;
                while (getline(f, s, ',')) {
                    strArray2.push_back(s);
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

//
int CalculateTickCount(double length, int slides, double sliderMult, double sliderTRate, double beatLength, double sliderVMult, int beatmapVersion)
{
    double num1 = ((Clamp(std::abs(sliderVMult), 10.0, 1000.0) * length) * beatLength) / (sliderMult * 10000.0);
    double num2 = beatLength / sliderTRate;
    if (beatmapVersion < 8) {
      num2 *= (Clamp(std::abs(sliderVMult), 10.0, 1000.0)) / 100.0;
    }
      
    double num3 = num1 - num2;
    int num4 = 0;
    for (; num3 >= 10.0; num3 -= num2) {
        ++num4;
    }
        
    
    return (num4 + (num4 * (slides - 1)));
}

std::string GetModsString(std::vector<std::string> mods)
{
    std::string modsString = "";
    for (std::string mod : mods)
    {
        modsString += mod;
    }
    return modsString;
}

//
std::vector<std::vector<int>> GetSpinners(std::string beatmap) {
    std::vector<std::string> beatmapHitObjects = GetBeatmapHitObjects(beatmap);
    std::vector<std::vector<double>> timingPoints = GetTimingPoints(beatmap);
    int beatmapVersion = GetBeatmapVersion(beatmap);
    double sliderMult = GetSliderMult(beatmap);
    double sliderTrate = GetSliderTRate(beatmap);
    std::vector<std::vector<int>> spinners;
    int num = 0;
    for (std::string str : beatmapHitObjects) {
    
        std::vector<std::string> strArray = split(str,',');

        switch (GetObjectType(std::stoi(strArray[3]))) {
        case 0:
            num++;
            break;
        case 1:{
            double length = std::stod(strArray[7]);
            int slides = std::stoi(strArray[6]);
            std::vector<double> beatLengthAt = GetBeatLengthAt(std::stoi(strArray[2]), timingPoints);
            int tickCount = CalculateTickCount(length, slides, sliderMult, sliderTrate, beatLengthAt[0], beatLengthAt[1], beatmapVersion);
            num += (tickCount) + (slides);
            break;}
        case 3:
            std::vector<int> temp;
            temp.push_back(num);
            temp.push_back(std::stoi(strArray[5]) - std::stoi(strArray[2]));
            spinners.push_back(temp);
            num++;
            break;
        }
    }
    return spinners;
}

int GetBeatmapVersion(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("osu file format v([0-9]+)\\r?\\n")))
    {
        std::cout << std::stoi(match[1]) << "\n";
        return std::stoi(match[1]);
    }
    return 0;
}

int GetDifficultyModifier(std::vector<std::string> mods) {
    for (std::string mod : mods) {
        if (mod == "HR") {
            return 16;
        }
        else if (mod == "EZ") {
            return 2;
        }
    }
    return 0;
}

float GetAdjustTime(std::vector<std::string> mods) {
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

float GetHP(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("HPDrainRate:(.*?)\\r?\\n")))
    {
        std::cout << "HP:" << std::stof(match[1]) << "\n";
        return std::stof(match[1]);
    }
    return 0.0f;
}

float GetCS(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("CircleSize:(.*?)\\r?\\n")))
    {
        return std::stof(match[1]);
    }
    return 0.0f;
}

float GetOD(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("OverallDifficulty:(.*?)\\r?\\n")))
    {
        return std::stof(match[1]);
    }
    return 0.0f;
}

double GetSliderMult(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("SliderMultiplier:(.*?)\\r?\\n")))
    {
        return std::stod(match[1]);
    }
    return 0.0;
}

double GetSliderTRate(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("SliderTickRate:(.*?)\\r?\\n")))
    {
        return std::stod(match[1]);
    }
    return 0.0;
}

std::string GetTitle(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("Title:(.*?)\\r?\\n")))
    {
        return match[1];
    }
    return "Title";
}

std::string GetArtist(std::string beatmap)
{
    std::regex rgx("Artist:(.*?)\\r?\\n");
    std::smatch match;

    if (std::regex_search(beatmap, match, rgx)) {
        return match[1].str();
    }

    return "Artist";
}

std::string GetDifficultyName(std::string beatmap)
{
    std::smatch match;
    if (std::regex_search(beatmap, match, std::regex("Version:(.*?)\\r?\\n")))
    {
        return match[1];
    }
    return "DiffName";
}

std::vector<std::string> GetBeatmapHitObjects(std::string beatmap) {
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

std::string GetBeatmap(int id)
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

//
int CalcSpinBonus(int length, double od, float adjustTime, int difficultyModifier)
{
    int num1 = static_cast<int>(CalcRotations(length, adjustTime));
    int num2 = CalcRotReq(length, od, difficultyModifier);
    return (num2 % 2 != 0 ? (num2 + 3) / 2 * 100 : static_cast<int>(std::floor(static_cast<double>(num1) / 2.0)) * 100) + static_cast<int>(std::floor(static_cast<double>(num1 - (num2 + 3)) / 2.0)) * 1100;
}
//
std::string CalcAmount(int rotations, int rotReq) {
    double num = std::max<double>(0, rotations - (rotReq + 3));
    if (rotReq % 2 != 0)
        return std::to_string(std::floor(num / 2.0)) + "k (F)";
    return fmod(num, 2.0) == 0.0 ? std::to_string(std::round(num / 2.0)) + "k (T)" : std::to_string(std::round(std::floor(num / 2.0))) + "k+100 (T)";
}

//
int CalcRotReq(int length, double od, int difficultyModifier)
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
    return static_cast<int>(static_cast<double>(length) / 1000.0 * num);
}
//
double CalcLeeway(int length, float adjustTime, double od, int difficultyModifier)
{
    int num = CalcRotReq(length, od, difficultyModifier);
    float d = CalcRotations(length, adjustTime);
    return (num % 2 != 0 && std::floor(d) != 0) ? (d - std::floor(d) + 1.0) : (d - std::floor(d));
}
//
float CalcRotations(int length, float adjustTime)
{
    float num1 = 0.0f;
    float num2 = (8E-05f + std::max<float>(0.0f, 5000.0f - static_cast<float>(length)) / 1000.0f / 2000.0f) / adjustTime;
    float val1 = 0.0f;
    int num3 = static_cast<int>(length - std::floor(50.0 / 3.0 * adjustTime));
    for (int index = 0; index < num3; ++index)
    {
        val1 += num2;
        num1 += std::min<float>(val1, 0.05f) / 3.14159265358979323846;
    }
    return num1;
}

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
            num = GetBeatmapID(beatmap);
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
        beatmap = GetBeatmap(num);

        std::vector<std::string> mods1;
        std::regex modsRegex("([A-Za-z]+)$");
        if (std::regex_search(input, match, modsRegex))
        {
            std::vector<std::string> mods2 = GetMods(match[1].str());
            if (IsValidModCombo(mods2))
            {
                mods1 = mods2;
            }
        }


        PrintTable(num, beatmap, mods1);
    }
    catch (...)
    {
        // Handle exceptions (e.g., invalid input)
    }

    return 0;
}