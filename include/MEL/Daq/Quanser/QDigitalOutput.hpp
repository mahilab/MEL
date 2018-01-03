#pragma once

#include <MEL/Daq/OutputModule.hpp>
#include <MEL/Utility/NonCopyable.hpp>

namespace mel {

    //==============================================================================
    // FORWARD DECLARATIONS
    //==============================================================================

    class QDaq;

    //==============================================================================
    // CLASS DECLARATION
    //==============================================================================

    class QDigitalOutput : public OutputModule<logic>, NonCopyable {

    public:

        QDigitalOutput(QDaq& daq, const std::vector<uint32>& channel_numbers);

        ~QDigitalOutput();

        bool enable() override;

        bool disable() override;

        bool update() override;

        bool update_channel(uint32 channel_number) override;

        bool set_expire_values(const std::vector<logic>& expire_values) override;

        bool set_expire_value(uint32 channel_number, logic expire_value) override;

    private:

        QDaq& daq_;  ///< Reference to parent QDaq

    };

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================