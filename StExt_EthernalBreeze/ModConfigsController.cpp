#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    constexpr const char* ExportConfigsFileNameTemplate = "EthernalBreeze_ExportedConfig_";

    inline zSTRING GetConfigsDirPath() { return zoptions->GetDirString(zTOptionPaths::DIR_EXECUTABLE) + "Autorun\\EthernalBreezeConfigs\\"; }

    inline bool WriteFile(const zSTRING& path, const zSTRING& content, bool rewriteIfExist = false)
    {
        zFILE_FILE* configsFile = new zFILE_FILE(path);
        if (configsFile->Exists())
        {
            if (!rewriteIfExist)
            {
                DEBUG_MSG("WriteFile - fail to save content to '" + path + "' - file already exist!");
                SAFE_DELETE(configsFile);
                return false;
            }

            if (configsFile->IsOpened()) configsFile->Close();
            configsFile->FileDelete();            
        }

        configsFile->Create(path);        
        configsFile->Open(path, true);
        configsFile->Write(content);
        configsFile->Close();

        //configsFile->s_physPathString = Z path;
        //configsFile->s_virtPathString = Z("\\Autorun\\EthernalBreezeConfigs\\");

        SAFE_DELETE(configsFile);
        return true;
    }

    // The. Dumbest. Way. Ever!
    // But it's only one working way to do this.
    inline void SearchConfigsPresets(Array<zSTRING>& filesList)
    {
        filesList.Clear();
        const zSTRING configsDirPath = GetConfigsDirPath();
        const zSTRING configNameTemplate = Z(ExportConfigsFileNameTemplate);

        for (int i = 1; i < 100; ++i)
        {
            zSTRING configName = configNameTemplate + FormatNumberPad(i, 3) + ".d";
            zSTRING path = configsDirPath + configName;
            zFILE_FILE* configsFile = new zFILE_FILE(path);
            if (configsFile->Exists())
                filesList.Insert(configName);
            SAFE_DELETE(configsFile);
        }
    }

    void UpdateExportConfigsLoadingList()
    {
        zSTRING configsListPath = GetConfigsDirPath() + "ExportedConfigs.src";
        zSTRING configsListContent;
        Array<zSTRING> configsList;
        SearchConfigsPresets(configsList);

        if (configsList.IsEmpty())
        {
            DEBUG_MSG("UpdateExportConfigsLoadingList: configs list is empty!");
            return;
        }

        for (uint i = 0; i < configsList.GetNum(); ++i)
            configsListContent += "EthernalBreezeConfigs\\" + configsList[i] + "\n";
        WriteFile(configsListPath, configsListContent, true);
    }

    void BuildCurrentConfigsFileContent(zSTRING& contentName, zSTRING& content)
    {
        const zSTRING configNameTemplate = Z(ExportConfigsFileNameTemplate);
        const zSTRING configTextTemplate = "ExportedConfigs_";
        zSTRING configName, configText, configApplyFunc, configsList;

        int configId = 0;
        do
        {
            ++configId;
            configName = configNameTemplate + FormatNumberPad(configId, 3);
        } 
        while (GetConfigPreset(configName));

        configText = configTextTemplate + FormatNumberPad(configId, 3);
        configApplyFunc = configName + "_OnApply";

        for (auto& data : ExtraConfigsData)
        {
            if (!data.IsExportable) continue;

            zCPar_Symbol* sym = parser->GetSymbol(data.ValueSymbol);
            if (!sym)
            {
                DEBUG_MSG("BuildCurrentConfigsFileContent - symbol '" + data.ValueSymbol + "' Not Found!");
                continue;
            }

            zSTRING configLine = data.ValueSymbol + " = ";
            if (sym->type == zPAR_TYPE_INT) configLine += Z(sym->single_intdata);
            else if (sym->type == zPAR_TYPE_FLOAT) configLine += Z(sym->single_floatdata);
            else if (sym->type == zPAR_TYPE_STRING) configLine += "\"" + sym->stringdata[0] + "\"";
            else
            {
                DEBUG_MSG("BuildCurrentConfigsFileContent -> symbol '" + data.ValueSymbol + "' Has unknown type!");
                continue;
            }
            configsList += "\r\n\t" + configLine + ";";
        }

        contentName = configName;
        content = ConfigsExportTemplate;
        content.Replace("[ConfigName]", configName);
        content.Replace("[ConfigText]", configText);
        content.Replace("[ConfigApplyFunc]", configApplyFunc);
        content.Replace("[ConfigsList]", configsList);
    }

	bool ExportCurrentConfigs(zSTRING& configsName)
	{
        DEBUG_MSG("ExportCurrentConfigs - export started...");
        zSTRING fileContent, fileName;
        BuildCurrentConfigsFileContent(fileName, fileContent);

        if (fileName.IsEmpty() || fileContent.IsEmpty())
        {
            DEBUG_MSG("ExportCurrentConfigs - fail to build file content or name!");
            return false;
        }

        const zSTRING path = GetConfigsDirPath() + fileName + ".d";
        DEBUG_MSG("ExportCurrentConfigs - start save configs to '" + path + "' ...");
        if (!WriteFile(path, fileContent, false))
        {
            DEBUG_MSG("ExportCurrentConfigs - fail write to file!");
            return false;
        }

        configsName = fileName;
        UpdateExportConfigsLoadingList();
        DEBUG_MSG("ExportCurrentConfigs - configs [" + fileName + "] successfully exported!");
        return true;
	}
}