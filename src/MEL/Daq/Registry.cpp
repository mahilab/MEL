#include <MEL/Daq/Registry.hpp>  
#include <MEL/Daq/Module.hpp>

namespace mel {

    RegistryBase::RegistryBase(ModuleBase* module) :
        module_(module) 
    {
        module_->add_registry(this);
    }

    std::size_t RegistryBase::index(ChanNum channel_number) const {
        return module_->index(channel_number);
    }


} // namespace mel