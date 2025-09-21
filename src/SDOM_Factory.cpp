// SDOM_Factory.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Stage.hpp>

namespace SDOM
{

    Factory::Factory() : IDataObject()
    {
        // Register built-in types
        registerType("Stage", Stage::Creator);
    }


    void Factory::registerType(const std::string& typeName, Creator creator)
    {
        creators_[typeName] = std::move(creator);
    }



}