#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    TimedEffect::TimedEffect()
    {
        Self = Null;
        Other = Null;        
        FunctionName = zSTRING();
        Function = Invalid;
        
        ElapsedTime = 0U;
        ZeroMemory(&ScriptInstance, sizeof(ScriptInstance));
        ScriptInstance.Enabled = False;
    }

    void TimedEffect::Archive(zCArchiver& arc)
    {        
        arc.WriteString("OnTick", FunctionName);
        arc.WriteRaw("ElapsedTime", &ElapsedTime, sizeof(ElapsedTime));
        arc.WriteRaw("ScriptInstance", &ScriptInstance, sizeof(ScriptInstance));
        arc.WriteInt("SelfUId", ScriptInstance.SelfUId);
        arc.WriteInt("OtherUId", ScriptInstance.OtherUId);
    }

    void TimedEffect::UnArchive(zCArchiver& arc)
    {
        arc.ReadString("OnTick", FunctionName);
        arc.ReadRaw("ElapsedTime", &ElapsedTime, sizeof(ElapsedTime));
        arc.ReadRaw("ScriptInstance", &ScriptInstance, sizeof(ScriptInstance));
        arc.ReadInt("SelfUId", ScriptInstance.SelfUId);
        arc.ReadInt("OtherUId", ScriptInstance.OtherUId);
    }

    TimedEffect::~TimedEffect()
    {
        Self = Null;
        Other = Null;
        FunctionName.Clear();
        Function = Invalid;
    }
}