#pragma once

namespace mel
{

//==============================================================================
// CLASS DECLARATIOIN
//==============================================================================

/// Utility class that makes any derived class non-copyable
class NonCopyable {

protected:

    /// Default constructor
    ///
    /// Because this class has a copy constructor, the compiler
    /// will not automatically generate the default constructor.
    /// That's why we must define it explicitly.
    NonCopyable() {}

    /// Default destructor
    ///
    /// By declaring a protected destructor it's impossible to
    /// call delete on a pointer of mel::NonCopyable, thus
    /// preventing possible resource leaks.
    ~NonCopyable() {}

private:


    /// Disabled copy constructor
    ///
    /// By making the copy constructor private, the compiler will
    /// trigger an error if anyone outside tries to use it.
    /// To prevent NonCopyable or friend classes from using it,
    /// we also give no definition, so that the linker will
    /// produce an error if the first protection was inefficient.
    NonCopyable(const NonCopyable&);

    /// Disabled assignment operator
    ///
    /// By making the assignment operator private, the compiler will
    /// trigger an error if anyone outside tries to use it.
    /// To prevent NonCopyable or friend classes from using it,
    /// we also give no definition, so that the linker will
    /// produce an error if the first protection was inefficient.
    NonCopyable& operator =(const NonCopyable&);
};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::NonCopyable
/// \ingroup system
///
/// This class makes its instances non-copyable, by explicitly
/// disabling its copy constructor and its assignment operator.
///
/// To create a non-copyable class, simply inherit from
/// mel::NonCopyable.
///
/// The type of inheritance (public or private) doesn't matter,
/// the copy constructor and assignment operator are declared private
/// in mel::NonCopyable so they will end up being inaccessible in both
/// cases. Thus you can use a shorter syntax for inheriting from it
/// (see below).
///
/// Usage example:
/// \code
/// class MyNonCopyableClass : mel::NonCopyable
/// {
///     ...
/// };
/// \endcode
///
/// Deciding whether the instances of a class can be copied
/// or not is a very important design choice. You are strongly
/// encouraged to think about it before writing a class,
/// and to use mel::NonCopyable when necessary to prevent
/// many potential future errors when using it. This is also
/// a very important indication to users of your class.
