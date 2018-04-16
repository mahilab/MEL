#include <MEL/Engine/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Engine.hpp>

namespace mel {

    Component::~Component() {}

    void Component::on_start() {}

    void Component::on_update() {}

    void Component::on_late_update() {}

    void Component::on_stop() {}

} // namespace mel
