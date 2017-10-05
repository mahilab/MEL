#pragma once

/*
Error messages must ALWAYS be added to the end of the list.  NEVER add an error message
to the middle of the list. Doing so could cause code that is already built using QUARC
to fail when the user upgrades MEL, or it could cause the wrong error message to be
displayed when an error occurs. Either situation would be very confusing to the user
who upgrades MEL. Even if error messages would more naturally be grouped together,
add the new error messages to the end of the list anyways. Do NOT insert an error
message into the middle of the list.
*/

bool MEL_DEBUG_MESSAGES = false;

enum mel_error {

};
