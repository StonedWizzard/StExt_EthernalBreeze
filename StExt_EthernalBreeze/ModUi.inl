#pragma once

template<typename Func>
inline void Gothic_II_Addon::BaseMenuPanel::ForEachItem(Func&& func)
{
    uint count = Items.GetNum();
    for (uint i = 0; i < count; ++i) {
        func(i, Items[i]);
    }
}

template<typename T, typename... Args>
inline void Gothic_II_Addon::MenuItem::SetOwnedData(Args&&... args)
{
    ClearData();
    Data = new T(std::forward<Args>(args)...);
    OwnsData = true;
}

template<typename T>
inline void Gothic_II_Addon::MenuItem::SetPtrData(T* ptr)
{
    ClearData();
    Data = ptr;
    OwnsData = false;
}

template<typename T>
inline T* Gothic_II_Addon::MenuItem::GetData() const { return static_cast<T*>(Data); }
