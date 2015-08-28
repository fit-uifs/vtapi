#pragma once

#include <string>
#include <exception>

namespace vtapi {


class Exception : public std::exception
{
public:
    explicit Exception(const std::string &message)
        : _message(message){}
    Exception(const std::string &type, const std::string &message)
        : _message(type + ": " + message) {}
    virtual ~Exception() noexcept {}

    const char* what() const noexcept override
    {
        return _message.c_str();
    }
    const std::string & message() const noexcept
    {
        return _message;
    }

protected:
    std::string _message;

private:
    Exception() = delete;
};

class RuntimeException : public Exception
{
public:
    RuntimeException(const std::string &error)
        : Exception("RuntimeException", error) {}
    virtual ~RuntimeException() noexcept {}
};

class BadConfigurationException : public Exception
{
public:
    BadConfigurationException(const std::string &error)
        : Exception("BadConfigurationException", error) {}
    virtual ~BadConfigurationException() noexcept {}
};

class DatabaseConnectionException : public Exception
{
public:
    DatabaseConnectionException(const std::string &connstring, const std::string &error)
        : Exception("DatabaseConnectionException",
                    "connection \'" + connstring + "\': " + error) {}
    virtual ~DatabaseConnectionException() noexcept {}
};

class BackendException : public Exception
{
public:
    BackendException(const std::string &backend, const std::string &error)
        : Exception("BackendException",
                    "backend \'" + backend + "\': " + error) {}
    virtual ~BackendException() noexcept {}
};

class RuntimeBackendException : public Exception
{
public:
    RuntimeBackendException(const std::string &error)
        : Exception("RuntimeBackendException", error) {}
    virtual ~RuntimeBackendException() noexcept {}
};

class ModuleException : public Exception
{
public:
    ModuleException(const std::string &module, const std::string &error)
        : Exception("ModuleException",
                    "module \'" + module + "\': " + error) {}
    virtual ~ModuleException() noexcept {}
};

class RuntimeModuleException : public Exception
{
public:
    RuntimeModuleException(const std::string &error)
        : Exception("RuntimeModuleException", error) {}
    virtual ~RuntimeModuleException() noexcept {}
};

}
