#pragma once

#include <string>

using namespace std;

struct AppException : public std::exception {
    string message;

    AppException(string component, string error, string description = "");

    const char* what() const noexcept override;
};