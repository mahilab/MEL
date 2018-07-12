#include <MEL/Experimental/Engine/Static/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Experimental/Engine/Static/Engine.hpp>
#include <MEL/Logging/Log.hpp>
#include <algorithm>

namespace mel {

//=============================================================================
// INTERFACE FUNCTIONS
//=============================================================================

Component::~Component() {}

void Component::start() {}

void Component::update() {}

void Component::late_update() {}

void Component::stop() {}

void Component::reset() {}

//=============================================================================
// PRIVATE FUNCTIONS
//=============================================================================



} // namespace mel
