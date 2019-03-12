#ifndef RETE_BUILTIN_UTIL_HPP_
#define RETE_BUILTIN_UTIL_HPP_

#include "../Builtin.hpp"
#include "../Accessors.hpp"

namespace rete {
namespace builtin {

/**
    Comparisons: lt, le, eq, neq, ge, gt
*/
class Compare : public Builtin {
public:
    enum Mode {
        LT = 0, LE, EQ, NEQ, GE, GT
    };
    static std::string ModeName(Mode m);
private:
    std::unique_ptr<Accessor> left_, right_;
    Mode mode_;
    bool compareNumbers_;
public:
    using Ptr = std::shared_ptr<Compare>;
    Compare(Mode mode, std::unique_ptr<Accessor> left, std::unique_ptr<Accessor> right);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;

};


/**
    Print stuff to cout, comma separated, with newline at the end
*/
class Print : public Builtin {
    std::vector<std::unique_ptr<StringAccessor>> values_;
public:
    using Ptr = std::shared_ptr<Print>;
    Print();
    void add(std::unique_ptr<StringAccessor>);
    void add(const std::string&);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;
};


} /* builtin */
} /* rete */


#endif /* end of include guard: RETE_BUILTIN_UTIL_HPP_ */
