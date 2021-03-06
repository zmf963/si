#include "stdafx.h"
#include "SystemInfo/include/systeminfo/GetComputerName.h"
#include "siutility/PFileSiUtils.h"

using namespace filesystem;
namespace macbasic {

GetComputerName::GetComputerName()
{

}

GetComputerName::~GetComputerName()
{

}

bool GetComputerName::analyze(CPRecord* rootRecord)
{

    std::vector<std::wstring> computerNames;
    std::vector<PSIHANDLE> pVolumes;
    CFileSystem::GetAllVolumes(pVolumes);
    for(std::vector<PSIHANDLE>::iterator vit = pVolumes.begin(); vit != pVolumes.end(); ++vit)
    {
        PSIHANDLE pVolume = *vit;
        PSIHANDLE handle = CFileSystem::GetItemHandleByRawPath(pVolume, L"/Library/Preferences/SystemConfiguration/preferences.plist", true);
        if(handle != NULL)
        {
            siutility::CHandleExport che;
            std::wstring plistPath = che.Export(handle);
            std::wstring computerName;
            if(AnalyzerPlist(plistPath, computerName))
            {
                computerNames.push_back(computerName);
            }
        }
    }
    std::wstring tempstr; //need the value to show
    int i = 0;
    for (std::vector<std::wstring>::iterator iter = computerNames.begin();iter != computerNames.end(); ++iter,i++)
    {
        if(i > 0)  //若挂载两个系统则以 XX&&XX形式输出
        {
            tempstr +=L"&&";
        }
        tempstr += *iter;
    }
    CPRecordType* osType = CPRecordTypeFactory::get()->getType(L"table.osidentifyparam");
        CPRecord* osRootRecord = osType->newRecord();
        rootRecord->addChild(osRootRecord);
        CPRecord* osRecord = osRootRecord->addChild();
        osRecord->at(L"ComputerName") = tempstr;
        //osRecord->at(L"SystemRoot") = L"/munt";
       // osRecord->at(L"InstallDate") = L"10:12:13";
        return true;
}

bool GetComputerName::AnalyzerPlist(const std::wstring& plistpath, std::wstring& computerName)
{
    std::string strFilePath = CPCodeChanger::UnicodeToUTF8(plistpath.c_str());
    plistcpp::dictionary_type dict;
    try
    {
        plistcpp::CPlistCpp::ReadPlist(strFilePath.c_str(), dict);
    }
    catch(...)
    {
        return false;
    }
    plistcpp::dictionary_type::iterator itrDict = dict.find("System");
    if(itrDict == dict.end())
    {
        return false;
    }

    plistcpp::dictionary_type dictSys = boost::any_cast<plistcpp::dictionary_type>(itrDict->second);
    plistcpp::dictionary_type::iterator itrDictSys = dictSys.find("System");
    if(itrDictSys == dictSys.end())
    {
        return false;
    }
    plistcpp::dictionary_type ThirdSecond = boost::any_cast<plistcpp::dictionary_type>(itrDictSys->second);
    plistcpp::dictionary_type::iterator itrDictThird = ThirdSecond.find("ComputerName");
    if(itrDictThird == ThirdSecond.end())
    {
        return false;
    }

    std::string strName;
    strName= boost::any_cast<plistcpp::string_type>(itrDictThird->second);
    computerName = CPCodeChanger::UTF8ToUnicode(strName.c_str());

    return true;
}

}
