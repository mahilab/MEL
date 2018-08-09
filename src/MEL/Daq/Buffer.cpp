#include <MEL/Daq/Buffer.hpp>  
#include <MEL/Daq/Module.hpp>

namespace mel {

    BufferBase::BufferBase(ModuleBase* module) :
        module_(module) 
    {
        module_->add_container(this);
    }

    std::size_t BufferBase::index(uint32 channel_number) const {
        return module_->index(channel_number);
    }


} // namespace mel