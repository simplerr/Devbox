/******************************************************************************************//**
* Taken from Game Coding Complete 4th Edition.

* A generic object factory
* Contains a map with GenericObjectCreationFunction functions for different types of objects.
* New creation objects are registered with Register<DerivedClassName>(IdType id)
*
* Really smart class!
*********************************************************************************************/

#include <map>
using namespace std;

template <class BaseType, class SubType>
BaseType* GenericObjectCreationFunction(void) { return new SubType; }

template <class BaseClass, class IdType>
class GenericObjectFactory
{
	typedef BaseClass* (*ObjectCreationFunction)(void);
	std::map<IdType, ObjectCreationFunction> m_creationFunctions;

public:
	template <class SubClass>
	bool Register(IdType id)
	{
		auto findIt = m_creationFunctions.find(id);
		if (findIt == m_creationFunctions.end())
		{
			m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>; 
			return true;
		}

		return false;
	}

	BaseClass* Create(IdType id)
	{
		auto findIt = m_creationFunctions.find(id);
		if (findIt != m_creationFunctions.end())
		{
			ObjectCreationFunction pFunc = findIt->second;
			return pFunc();
		}

		return NULL;
	}
};