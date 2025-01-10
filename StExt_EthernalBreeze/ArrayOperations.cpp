#include <UnionAfx.h>
#include <vector>
#include <algorithm>

namespace Gothic_II_Addon
{
    void StExt_PushInt(const int& value)
    {
        parser->datastack.Push(value);
        parser->datastack.Push(zPAR_TOK_PUSHINT);
    }

    void StExt_PushString(const zSTRING& value)
    {
        parser->datastack.Push(reinterpret_cast<int>(&value));
        parser->datastack.Push(zPAR_TOK_PUSHVAR);
    }

    void StExt_PushFloat(const float& value)
    {
        parser->datastack.Push(reinterpret_cast<const int&>(value));
        parser->datastack.Push(zPAR_TOK_PUSHINT);
    }

    void StExt_Call(const int& funcIndex) { parser->DoStack(parser->GetSymbol(funcIndex)->single_intdata); }

    int StExt_GetFuncAddress(zCPar_Symbol* sym)
    {
        if (!sym->HasFlag(zPAR_FLAG_CONST) || sym->HasFlag(zPAR_FLAG_EXTERNAL) || sym->HasFlag(zPAR_FLAG_MERGED)) {
            return -1;
        }
        if (sym->type != zPAR_TYPE_FUNC && sym->type != zPAR_TYPE_INSTANCE && sym->type != zPAR_TYPE_PROTOTYPE) {
            return -1;
        }

        const char* name = sym->name.ToChar();
        for (int i = 0; name[i] != '\0' && name[i] != '\n'; i++)
        {
            if (name[i] == '.') { return -1; }
        }
        return sym->single_intdata;
    }

    struct StExt_FuncEntity
    {
        int index;
        int address;

        StExt_FuncEntity(int index, int address) :
            index(index),
            address(address)
        {

        }
    };

    std::vector<StExt_FuncEntity> StExt_Functions;
    bool StExt_ArrayOperationsInitialized = false;

    void StExt_InitFunctions()
    {
        if (StExt_ArrayOperationsInitialized) { return; }

        for (int i = 0; i < parser->symtab.GetNumInList(); i++)
        {
            int address = StExt_GetFuncAddress(parser->GetSymbol(i));
            if (address == -1) { continue; }
            StExt_Functions.push_back(StExt_FuncEntity(i, address));
        }

        std::sort(StExt_Functions.begin(), StExt_Functions.end(), [](const StExt_FuncEntity& x, const StExt_FuncEntity& y) { return x.address < y.address; });
        StExt_ArrayOperationsInitialized = true;
    }

    const zSTRING& StExt_GetFuncNameByAddress(int address)
    {
        auto it = std::lower_bound(StExt_Functions.begin(), StExt_Functions.end(), StExt_FuncEntity(-1, address), [](const StExt_FuncEntity& x, const StExt_FuncEntity& y) { return x.address <= y.address; });
        it--;
        return parser->GetSymbol(it->index)->name;
    }

    void StExt_TerminateGame() { gameMan->ExitGame(); }

    void StExt_ExtractSegments(string name, zCArray<string>& segments)
    {
        int nextStart = 0;
        for (size_t i = 0; i <= name.Length(); i++)
        {
            if (i == name.Length() || name[i] == '.')
            {
                segments.InsertEnd(name.Copy(nextStart, i - nextStart));
                nextStart = i + 1;
            }
        }
    }

    bool StExt_ValidateSegments(const zCArray<string>& segments)
    {
        if (segments.GetNum() < 1 || segments.GetNum() > 2) { return false; }
        for (int i = 0; i < segments.GetNum(); i++)
        {
            if (segments[i].IsEmpty()) { return false; }
        }
        return true;
    }

    void StExt_ShowParserError(string data)
    {
        const zSTRING& func = StExt_GetFuncNameByAddress(parser->stack.stackptr - parser->stack.stack);
        Message::Error((string)"Error in function " + func.ToChar() + ".\n" + data + ".", "Loops Error");
        StExt_TerminateGame();
    }

    bool StExt_ValidateArrayType(int type)
    {
        switch (type)
        {
        case zPAR_TYPE_INT:
        case zPAR_TYPE_FLOAT:
        case zPAR_TYPE_FUNC:
        case zPAR_TYPE_STRING:
            return true;
        default:
            return false;
        }
    }

    zSTRING& StExt_GetInstanceClassName(zCPar_Symbol* instance)
    {
        while (instance->GetParent()) { instance = instance->GetParent(); }
        return instance->name;
    }

    void StExt_GetArray(zSTRING& arrayName, zCPar_Symbol*& arrayDesc, void*& pointer, int& size)
    {
        StExt_InitFunctions();
        arrayDesc = nullptr;
        pointer = nullptr;
        size = 0;

        string name = arrayName.ToChar();
        name.Upper();
        zCArray<string> segments;
        StExt_ExtractSegments(name, segments);

        if (!StExt_ValidateSegments(segments)) {
            return StExt_ShowParserError((string)"Invalid array name: " + arrayName.ToChar());
        }

        string funcName = StExt_GetFuncNameByAddress(parser->stack.stackptr - parser->stack.stack).ToChar();
        zCPar_Symbol* symbol = parser->GetSymbol((funcName + "." + segments[0]).GetVector());

        if (!symbol) {
            symbol = parser->GetSymbol(segments[0].GetVector());
        }

        if (!symbol) {
            return StExt_ShowParserError((string)"Variable not found: " + segments[0]);
        }

        if (segments.GetNum() == 1)
        {
            if (!StExt_ValidateArrayType(symbol->type)) {
                return StExt_ShowParserError((string)"Invalid array type: " + symbol->type);
            }

            arrayDesc = symbol;
            size = arrayDesc->ele;
            switch (arrayDesc->type)
            {
                case zPAR_TYPE_INT:
                case zPAR_TYPE_FUNC:
                    pointer = (size == 1) ? &arrayDesc->single_intdata : arrayDesc->intdata;
                    break;
                case zPAR_TYPE_FLOAT:
                    pointer = (size == 1) ? &arrayDesc->single_floatdata : arrayDesc->floatdata;
                    break;
                case zPAR_TYPE_STRING:
                    pointer = arrayDesc->stringdata;
                    break;
            }
            return;
        }

        if (symbol->type != zPAR_TYPE_INSTANCE) {
            return StExt_ShowParserError(segments[0] + " is not instance");
        }

        string className = StExt_GetInstanceClassName(symbol).ToChar();
        string classVarName = className + "." + segments[1];
        zCPar_Symbol* classVarSymbol = parser->GetSymbol(classVarName.GetVector());

        if (!classVarSymbol) {
            return StExt_ShowParserError((string)"Can't find class member: " + classVarName);
        }
        if (!StExt_ValidateArrayType(classVarSymbol->type)) {
            return StExt_ShowParserError((string)"Invalid array type: " + classVarSymbol->type);
        }

        arrayDesc = classVarSymbol;
        pointer = (byte*)symbol->GetInstanceAdr() + arrayDesc->GetOffset();
        size = arrayDesc->ele;
    }

    void StExt_GetIntArray(zSTRING& arrayName, zCPar_Symbol*& arrayDesc, int*& pointer, int& size)
    {
        void* ptr;
        StExt_GetArray(arrayName, arrayDesc, ptr, size);
        pointer = (int*)ptr;

        if (!pointer)
        {
            StExt_ShowParserError("GetArray failed");
            return;
        }

        if (arrayDesc->type != zPAR_TYPE_INT && arrayDesc->type != zPAR_TYPE_FUNC)
        {
            StExt_ShowParserError((string)"Array type mistmatch.\nExpected: " + zPAR_TYPE_INT + " or " + zPAR_TYPE_FUNC +
                ".\nGot: " + arrayDesc->type);
            return;
        }
    }

    void StExt_GetFloatArray(zSTRING& arrayName, zCPar_Symbol*& arrayDesc, float*& pointer, int& size)
    {
        void* ptr;
        StExt_GetArray(arrayName, arrayDesc, ptr, size);
        pointer = (float*)ptr;

        if (!pointer)
        {
            StExt_ShowParserError("GetArray failed");
            return;
        }

        if (arrayDesc->type != zPAR_TYPE_FLOAT)
        {
            StExt_ShowParserError((string)"Array type mistmatch.\nExpected: " + zPAR_TYPE_FLOAT +
                ".\nGot: " + arrayDesc->type);
            return;
        }
    }

    void StExt_GetStringArray(zSTRING& arrayName, zCPar_Symbol*& arrayDesc, zSTRING*& pointer, int& size)
    {
        void* ptr;
        StExt_GetArray(arrayName, arrayDesc, ptr, size);
        pointer = (zSTRING*)ptr;

        if (!pointer)
        {
            StExt_ShowParserError("GetArray failed");
            return;
        }

        if (arrayDesc->type != zPAR_TYPE_STRING)
        {
            StExt_ShowParserError((string)"Array type mistmatch.\nExpected: " + zPAR_TYPE_STRING +
                ".\nGot: " + arrayDesc->type);
            return;
        }
    }

    void StExt_ValidateIndex(int index, int size)
    {
        if (index < 0 || index >= size)
        {
            StExt_ShowParserError((string)"Index out of range: " + index + ".\nArray size is " + size);
            ogame->Done();
        }
    }

    int __cdecl StExt_GetArrInt()
    {
        int index;
        zSTRING arrayName;

        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        int* pointer;
        int size;

        StExt_GetIntArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        parser->SetReturn(pointer[index]);
        return 0;
    }

    int __cdecl StExt_SetArrInt()
    {
        int value;
        int index;
        zSTRING arrayName;

        parser->GetParameter(value);
        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        int* pointer;
        int size;

        StExt_GetIntArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        pointer[index] = value;
        return 0;
    }

    int __cdecl StExt_GetArrFloat()
    {
        int index;
        zSTRING arrayName;

        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        float* pointer;
        int size;

        StExt_GetFloatArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        parser->SetReturn(pointer[index]);
        return 0;
    }

    int __cdecl StExt_SetArrFloat()
    {
        float value;
        int index;
        zSTRING arrayName;

        parser->GetParameter(value);
        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        float* pointer;
        int size;

        StExt_GetFloatArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        pointer[index] = value;
        return 0;
    }

    int __cdecl StExt_GetArrStr()
    {
        int index;
        zSTRING arrayName;

        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        zSTRING* pointer;
        int size;

        StExt_GetStringArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        parser->SetReturn(pointer[index]);
        return 0;
    }

    int __cdecl StExt_SetArrStr()
    {
        zSTRING value;
        int index;
        zSTRING arrayName;

        parser->GetParameter(value);
        parser->GetParameter(index);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        zSTRING* pointer;
        int size;

        StExt_GetStringArray(arrayName, arrayDesc, pointer, size);
        StExt_ValidateIndex(index, size);

        pointer[index] = value;
        return 0;
    }

    // While(LoopBody_Func);
    int __cdecl StExt_While()
    {
        int funcIndex;
        parser->GetParameter(funcIndex);
        int index = 0;
        while (true)
        {
            int result = *(int*)parser->CallFunc(funcIndex, index++);
            if (!result) { return 0; }
        }
    }

    int __cdecl StExt_FillArrInt()
    {
        int value;
        zSTRING arrayName;
        parser->GetParameter(value);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        int* pointer;
        int size;
        StExt_GetIntArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            pointer[i] = value;
        }
        return 0;
    }

    int __cdecl StExt_FillArrFloat()
    {
        float value;
        zSTRING arrayName;
        parser->GetParameter(value);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        float* pointer;
        int size;
        StExt_GetFloatArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            pointer[i] = value;
        }
        return 0;
    }

    int __cdecl StExt_FillArrStr()
    {
        zSTRING value;
        zSTRING arrayName;
        parser->GetParameter(value);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        zSTRING* pointer;
        int size;
        StExt_GetStringArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            pointer[i] = value;
        }
        return 0;
    }

    int __cdecl StExt_ForEachInt()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        int* pointer;
        int size;
        StExt_GetIntArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushInt(pointer[i]);
            StExt_Call(funcIndex);
        }
        return 0;
    }

    int __cdecl StExt_ForEachFloat()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        float* pointer;
        int size;
        StExt_GetFloatArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushFloat(pointer[i]);
            StExt_Call(funcIndex);
        }
        return 0;
    }

    int __cdecl StExt_ForEachStr()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        zSTRING* pointer;
        int size;
        StExt_GetStringArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushString(pointer[i]);
            StExt_Call(funcIndex);
        }
        return 0;
    }

    int __cdecl StExt_AlterEachInt()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        int* pointer;
        int size;
        StExt_GetIntArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushInt(pointer[i]);
            StExt_Call(funcIndex);
            parser->GetParameter(pointer[i]);
        }
        return 0;
    }

    int __cdecl StExt_AlterEachFloat()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        float* pointer;
        int size;
        StExt_GetFloatArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushFloat(pointer[i]);
            StExt_Call(funcIndex);
            parser->GetParameter(pointer[i]);
        }
        return 0;
    }

    int __cdecl StExt_AlterEachStr()
    {
        int funcIndex;
        zSTRING arrayName;
        parser->GetParameter(funcIndex);
        parser->GetParameter(arrayName);

        zCPar_Symbol* arrayDesc;
        zSTRING* pointer;
        int size;
        StExt_GetStringArray(arrayName, arrayDesc, pointer, size);

        for (int i = 0; i < size; i++)
        {
            StExt_PushInt(i);
            StExt_PushString(pointer[i]);
            StExt_Call(funcIndex);
            parser->GetParameter(pointer[i]);
        }
        return 0;
    }

    void ArraysOperations_DefineExternals()
    {
        // имя функции, указатель на реализацию, тип возвращаемого значения, тип первого аргумента, тип второго аргумента, метка конца аргументов (обязательно)
        parser->DefineExternal("StExt_Array_SetInt", StExt_SetArrInt, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_SetFloat", StExt_SetArrFloat, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_SetString", StExt_SetArrStr, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_Array_GetInt", StExt_GetArrInt, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_GetFloat", StExt_GetArrFloat, zPAR_TYPE_FLOAT, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_GetString", StExt_GetArrStr, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_Array_FillInt", StExt_FillArrInt, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_FillFloat", StExt_FillArrFloat, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_FillString", StExt_FillArrStr, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_While", StExt_While, zPAR_TYPE_VOID, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_Array_ForEachInt", StExt_ForEachInt, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_ForEachFloat", StExt_ForEachFloat, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_ForEachStr", StExt_ForEachStr, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_Array_AlterEachInt", StExt_AlterEachInt, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_AlterEachFloat", StExt_AlterEachFloat, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Array_AlterEachStr", StExt_AlterEachStr, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
    }
}