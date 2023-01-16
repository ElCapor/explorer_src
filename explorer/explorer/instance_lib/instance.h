#ifndef INSTANCE_H
#define INSTANCE_H
#include <Windows.h>
#include <string>
#include <vector>
#include "util.h"
class Instance {
protected:
DWORD addr;
private:
    std::string ReadInstanceName() // roblox messed up their engine again , use this only on instance
    {
        int lentgh = Read<int>(Read<DWORD>(this->addr + instance::name_offset) + 0x10);
        if (lentgh < 16u)
        {
            return ReadStringOfUnknownLength(Read<DWORD>(this->addr + instance::name_offset));
        }
        else {
            return ReadStringOfUnknownLength(Read<DWORD>(Read<DWORD>(this->addr + instance::name_offset)));
        }
    }
public:
    Instance(DWORD address)
    {
        this->addr = address;
    }
    Instance()
    {
        this->addr = 0;
    }
    DWORD GetAddress()
    {
        return this->addr;
    }
    std::string Name()
    {
        return ReadInstanceName();
    }
    std::vector<Instance> GetChildren();
    {
        std::vector<Instance> childs;
        for (DWORD child : GetInstanceTable(Read<DWORD>(addr+0x2C)+0xC))
        {
            childs.push_back(Instance(child));
        }
        return childs;
    }

    Instance FindFirstChild(std::string name)
    {
        // to do : add recursing
	    for (Instance child : GetChildren())
	    {
		    if (child.Name() == name)
			    return child;
	    }
	    return Instance(0);
    }

    Instance Parent()
    {
        return Instance(Read<DWORD>(addr + instance::parent_offset)); 
    }
    bool isValid() // != to 0
    {
        if (addr > 0)
		    return true;
	    return false;
    }
    std::string Class()
    {
        return readClassName(addr);
    }


};
#endif // !INSTANCE_H