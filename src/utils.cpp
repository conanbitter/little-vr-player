#include "utils.hpp"

#include <sstream>

AppException::AppException(string component, string error, string description) {
    stringstream buf;
    buf << "[" << component << "] " << error;
    if (description.length() > 0) {
        buf << " : " << description;
    }
    message = buf.str();
};

const char* AppException::what() const noexcept {
    return message.c_str();
}