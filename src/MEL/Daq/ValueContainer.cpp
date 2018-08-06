#include <MEL/Daq/ValueContainer.hpp>  
#include <MEL/Daq/Module.hpp>

namespace mel {

    ValueContainerBase::ValueContainerBase(ModuleBase* module) :
        module_(module) 
    {
        module_->add_container(this);
    }

    std::size_t ValueContainerBase::index(uint32 channel_number) const {
        return module_->index(channel_number);
    }


} // namespace mel