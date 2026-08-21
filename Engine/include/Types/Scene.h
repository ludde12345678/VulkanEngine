#pragma once
#include <Types/Object.h>
#include <string>

struct ObjectHandle
{
	uint32_t index = 0;
	bool isValid = false;
};

struct Scene
{
	std::string name;
	std::vector<Object> objects;

	ObjectHandle findObjectByName(const std::string name) {
		ObjectHandle handle{};
		uint32_t index = 0;
		for (auto& obj : objects) 
		{
			if (obj.name == name) 
			{
				handle.index = index;
				handle.isValid = true;
				return handle;
			}
			index++;
		}
		return handle;

	}
	Object& getObject(ObjectHandle handle) {
		return objects[handle.index];
	}

};