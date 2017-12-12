//
// Created by yuwenyong on 17-12-4.
//

#ifndef NET4CXX_COMMON_CONFIGURATION_JSON_H
#define NET4CXX_COMMON_CONFIGURATION_JSON_H

#include "net4cxx/common/common.h"
#include <boost/variant.hpp>

NS_BEGIN


enum class JSONType {
    JSON_NULL = 0,
    JSON_INTEGER,
    JSON_REAL,
    JSON_STRING,
    JSON_BOOL,
    JSON_ARRAY,
    JSON_OBJECT,
};


class NET4CXX_COMMON_API JSONValue {
public:
    struct NullValue {

    };

    using ObjectType = std::map<std::string, JSONValue>;
    using ArrayType = std::vector<JSONValue>;
    using ValueType = boost::variant<NullValue, long, double, std::string, bool, ArrayType, ObjectType>;

    struct GetTypeVisitor: public boost::static_visitor<JSONType> {
        JSONType operator()(NullValue v) const {
            return JSONType::JSON_NULL;
        }

        JSONType operator()(long v) const {
            return JSONType::JSON_INTEGER;
        }

        JSONType operator()(double v) const {
            return JSONType::JSON_REAL;
        }

        JSONType operator()(const std::string &v) const {
            return JSONType::JSON_STRING;
        }

        JSONType operator()(bool v) const {
            return JSONType::JSON_BOOL;
        }

        JSONType operator()(const ArrayType &v) const {
            return JSONType::JSON_ARRAY;
        }

        JSONType operator()(const ObjectType &v) const {
            return JSONType::JSON_OBJECT;
        }
    };

    struct IsIntegerVisitor: public boost::static_visitor<bool> {
        bool operator()(NullValue v) const {
            return false;
        }

        bool operator()(long v) const {
            return true;
        }

        bool operator()(double v) const {
            return v >= LONG_MIN && v <= LONG_MAX && isIntegral(v);
        }

        bool operator()(const std::string &v) const {
            return false;
        }

        bool operator()(bool v) const {
            return false;
        }

        bool operator()(const ArrayType &v) const {
            return false;
        }

        bool operator()(const ObjectType &v) const {
            return false;
        }
    };

    struct AsIntVisitor: public boost::static_visitor<int> {
        int operator()(NullValue v) const {
            return 0;
        }

        int operator()(long v) const;

        int operator()(double v) const;

        int operator()(const std::string &v) const;

        int operator()(bool v) const {
            return v ? 1 : 0;
        }

        int operator()(const ArrayType &v) const;

        int operator()(const ObjectType &v) const;
    };

    struct AsLongVisitor: public boost::static_visitor<long> {
        long operator()(NullValue v) const {
            return 0;
        }

        long operator()(long v) const {
            return v;
        }

        long operator()(double v) const;

        long operator()(const std::string &v) const;

        long operator()(bool v) const {
            return v ? 1 : 0;
        }

        long operator()(const ArrayType &v) const;

        long operator()(const ObjectType &v) const;
    };

    struct AsFloatVisitor: public boost::static_visitor<float> {
        float operator()(NullValue v) const {
            return 0.0f;
        }

        float operator()(long v) const {
            return static_cast<float>(v);
        }

        float operator()(double v) const {
            return static_cast<float>(v);
        }

        float operator()(const std::string &v) const;

        float operator()(bool v) const {
            return v ? 1.0f : 0.0f;
        }

        float operator()(const ArrayType &v) const;

        float operator()(const ObjectType &v) const;
    };

    struct AsDoubleVisitor: public boost::static_visitor<double> {
        double operator()(NullValue v) const {
            return 0.0;
        }

        double operator()(long v) const {
            return static_cast<double >(v);
        }

        double operator()(double v) const {
            return v;
        }

        double operator()(const std::string &v) const;

        double operator()(bool v) const {
            return v ? 1.0 : 0.0;
        }

        double operator()(const ArrayType &v) const;

        double operator()(const ObjectType &v) const;
    };

    struct AsBoolVisitor: public boost::static_visitor<bool> {
        bool operator()(NullValue v) const {
            return false;
        }

        bool operator()(long v) const {
            return v != 0;
        }

        bool operator()(double v) const {
            return v != 0.0;
        }

        bool operator()(const std::string &v) const;

        bool operator()(bool v) const {
            return v;
        }

        bool operator()(const ArrayType &v) const;

        bool operator()(const ObjectType &v) const;
    };

    struct AsStringVisitor: public boost::static_visitor<std::string> {
        std::string operator()(NullValue v) const {
            return "";
        }

        std::string operator()(long v) const {
            return std::to_string(v);
        }

        std::string operator()(double v) const {
            return valueToString(v);
        }

        std::string operator()(const std::string &v) const {
            return v;
        }

        std::string operator()(bool v) const {
            return v ? "true" : "false";
        }

        std::string operator()(const ArrayType &v) const;

        std::string operator()(const ObjectType &v) const;
    };

    friend bool operator<(const JSONValue &lhs, const JSONValue &rhs);
    friend bool operator==(const JSONValue &lhs, const JSONValue &rhs);

    JSONValue() = default;

    explicit JSONValue(nullptr_t) {

    }

    explicit JSONValue(int value): _value(long{value}) {

    }

    explicit JSONValue(long value): _value(value) {

    }

    explicit JSONValue(float value): _value(double{value}) {

    }

    explicit JSONValue(double value): _value(value) {

    }

    explicit JSONValue(const char *value): _value(std::string{value}) {

    }

    JSONValue(const char *value, size_t length): _value(std::string{value, value + length}) {

    }

    explicit JSONValue(const std::string &value): _value(value) {

    }

    explicit JSONValue(bool value): _value(value) {

    }

    JSONValue& operator=(nullptr_t) {
        _value = {};
        return *this;
    }

    JSONValue& operator=(int rhs) {
        _value = long{rhs};
        return *this;
    }

    JSONValue& operator=(long rhs) {
        _value = rhs;
        return *this;
    }

    JSONValue& operator=(float rhs) {
        _value = double{rhs};
        return *this;
    }

    JSONValue& operator=(double rhs) {
        _value = rhs;
        return *this;
    }

    JSONValue& operator=(const char *rhs) {
        _value = std::string{rhs};
        return *this;
    }

    JSONValue& operator=(const std::string &rhs) {
        _value = rhs;
        return *this;
    }

    JSONValue& operator=(bool rhs) {
        _value = rhs;
        return *this;
    }

    void setNull() {
        if (!isNull()) {
            _value = NullValue{};
        }
    }

    void setArray() {
        if (_value.type() != typeid(ArrayType)) {
            _value = ArrayType{};
        }
    }

    void setObject() {
        if (_value.type() != typeid(ObjectType)) {
            _value = ObjectType{};
        }
    }

    JSONType getType() const {
        return boost::apply_visitor(GetTypeVisitor(), _value);
    }

    bool isNull() const {
        return _value.type() == typeid(NullValue);
    }

    bool isBool() const {
        return _value.type() == typeid(bool);
    }

    bool isInteger() const {
        return boost::apply_visitor(IsIntegerVisitor(), _value);
    }

    bool isReal() const {
        return _value.type() == typeid(long) || _value.type() == typeid(double);
    }

    bool isNumeric() const {
        return isReal();
    }

    bool isString() const {
        return _value.type() == typeid(std::string);
    }

    bool isArray() const {
        return _value.type() == typeid(ArrayType);
    }

    bool isObject() const {
        return _value.type() == typeid(ObjectType);
    }

    int asInt() const {
        return boost::apply_visitor(AsIntVisitor(), _value);
    }

    long asLong() const {
        return boost::apply_visitor(AsLongVisitor(), _value);
    }

    float asFloat() const {
        return boost::apply_visitor(AsFloatVisitor(), _value);
    }

    double asDouble() const {
        return boost::apply_visitor(AsDoubleVisitor(), _value);
    }

    bool asBool() const {
        return boost::apply_visitor(AsBoolVisitor(), _value);
    }

    std::string asString() const {
        return boost::apply_visitor(AsStringVisitor(), _value);
    }

    static bool isIntegral(double d) {
        double integralPart;
        return modf(d, &integralPart) == 0.0;
    }

    static std::string valueToString(double value) {
        return valueToString(value, false, 17);
    }

    static std::string valueToString(double value, bool useSpecialFloats, unsigned int precision);
private:
    enum CommentPlacement {
        COMMENT_BEFORE = 0,
        COMMENT_ON_SAME_LINE,
        COMMENT_AFTER,
        COMMENT_COUNT,
    };

    ValueType _value;
    std::array<std::string, COMMENT_COUNT> _comments;
};


inline bool operator<(const JSONValue &lhs, const JSONValue &rhs) {
    return lhs._value < rhs._value;
}

inline bool operator<=(const JSONValue &lhs, const JSONValue &rhs) {
    return !(rhs < lhs);
}

inline bool operator>(const JSONValue &lhs, const JSONValue &rhs) {
    return rhs < lhs;
}

inline bool operator>=(const JSONValue &lhs, const JSONValue &rhs) {
    return !(lhs < rhs);
}

inline bool operator==(const JSONValue &lhs, const JSONValue &rhs) {
    return lhs._value == rhs._value;
}

inline bool operator!=(const JSONValue &lhs, const JSONValue &rhs) {
    return !(lhs == rhs);
}

inline bool operator<(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return false;
}

inline bool operator<=(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return !(rhs < lhs);
}

inline bool operator>(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return rhs < lhs;
}

inline bool operator>=(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return !(lhs < rhs);
}

inline bool operator==(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return true;
}

inline bool operator!=(const JSONValue::NullValue &lhs, const JSONValue::NullValue &rhs) {
    return !(lhs == rhs);
}

NS_END

#endif //NET4CXX_COMMON_CONFIGURATION_JSON_H
