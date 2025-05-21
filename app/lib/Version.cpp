#include "Version.hpp"
#include <algorithm>
#include <sstream>
#include <string>


/**
 * Constructs a Version object from a brace-enclosed list of version segments.
 *
 * Example: Version{1, 2, 3} creates a Version object with the version string "1.2.3".
 *
 * @param version_digits A list of integers representing the version segments.
 */
Version::Version(std::initializer_list<int> version_digits)
    : digits(version_digits) {}



/**
 * Constructs a Version object from a vector of integers representing version segments.
 *
 * @param version_digits A vector of integers representing the version segments.
 */

Version::Version(const std::vector<int>& version_digits)
    : digits(version_digits) {}

/**
 * Compares two versions for equality or greater.
 *
 * @param other The version to compare with.
 * @return true if this version is greater than or equal to the other version, false otherwise.
 */

bool Version::operator>=(const Version& other) const {
    for (size_t i = 0; i < std::max(digits.size(), other.digits.size()); ++i) {
        int lhs = (i < digits.size()) ? digits[i] : 0;
        int rhs = (i < other.digits.size()) ? other.digits[i] : 0;

        if (lhs > rhs) return true;
        if (lhs < rhs) return false;
    }
    return true;
}


/**
 * Compares two versions for greater than.
 *
 * @param other The version to compare with.
 * @return true if this version is greater than the other version, false otherwise.
 */
bool Version::operator>(const Version& other) const
{
    for (size_t i = 0; i < std::max(digits.size(), other.digits.size()); ++i) {
        int lhs = (i < digits.size()) ? digits[i] : 0;
        int rhs = (i < other.digits.size()) ? other.digits[i] : 0;

        if (lhs > rhs) return true;
        if (lhs < rhs) return false;
    }
    return false;
}


/**
 * Compares two versions for less than or equal.
 *
 * @param other The version to compare with.
 * @return true if this version is less than or equal to the other version, false otherwise.
 */
bool Version::operator<=(const Version& other) const
{
    for (size_t i = 0; i < std::max(digits.size(), other.digits.size()); ++i) {
        int lhs = (i < digits.size()) ? digits[i] : 0;
        int rhs = (i < other.digits.size()) ? other.digits[i] : 0;

        if (lhs > rhs) return false;
        if (lhs < rhs) return true;
    }
    return true;
}


/**
 * Converts the version to a string representation.
 *
 * The returned string is a period-separated list of the version segments.
 * If the version is empty, the returned string is "0".
 *
 * @return A string representation of the version.
 */
std::string Version::to_string() const
{
    if (digits.empty()) return "0";
    std::ostringstream oss;
    for (size_t i = 0; i < digits.size(); ++i) {
        if (i > 0) oss << '.';
        oss << digits[i];
    }
    return oss.str();
}