#pragma once

#include <string>

using namespace std;

struct AppException : public std::exception {
    string message;

    AppException(const string& component, const string& error, const string& description = "");

    const char* what() const noexcept override;
};