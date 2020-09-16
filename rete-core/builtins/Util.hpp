#ifndef RETE_BUILTIN_UTIL_HPP_
#define RETE_BUILTIN_UTIL_HPP_

#include "../Builtin.hpp"
#include "../Production.hpp"
#include "../Accessors.hpp"
#include "../TokenGroupAccessor.hpp"

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
    PersistentInterpretation<float> leftNum_, rightNum_;
    PersistentInterpretation<std::string> leftStr_, rightStr_;

    Mode mode_;
    bool compareNumbers_;
public:
    using Ptr = std::shared_ptr<Compare>;
    Compare(Mode mode, PersistentInterpretation<float>&& left,
                       PersistentInterpretation<float>&& right);
    Compare(Mode mode, PersistentInterpretation<std::string>&& left,
                       PersistentInterpretation<std::string>&& right);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;

};


/**
    Print stuff to cout, comma separated, with newline at the end
*/
class Print : public Builtin {
    std::vector<PersistentInterpretation<std::string>> values_;
public:
    using Ptr = std::shared_ptr<Print>;
    Print();
    void add(PersistentInterpretation<std::string>&&);
    void add(const std::string&);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;
};

/**
    Prints the contents of a group. All given group accessors must refer to
    the same token group, as their respective values will be printed together
    for every entry in the group.
*/
class PrintGroup : public Builtin {
    std::vector<std::unique_ptr<TokenGroupAccessor>> groupAccessors_;
public:
    using Ptr = std::shared_ptr<PrintGroup>;
    PrintGroup();
    void add(std::unique_ptr<TokenGroupAccessor>);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;
};

/**
    Similar to "Print", but as a rule effect that also adds a
    ADD/REMOVE/UPDATE prefix given on what actually happened.
*/
class PrintEffect : public Production {
    std::vector<PersistentInterpretation<std::string>> values_;
public:
    using Ptr = std::shared_ptr<PrintEffect>;
    PrintEffect();
    void add(PersistentInterpretation<std::string>&&);
    void add(const std::string&);

    void execute(Token::Ptr, PropagationFlag, std::vector<WME::Ptr>&) override;
};



/**
    Counts the number of entries inside the token group that the given
    accessor points to.
*/
class CountEntriesInGroup : public Builtin {
    PersistentInterpretation<TokenGroup::Ptr> group_;
public:
    using Ptr = std::shared_ptr<CountEntriesInGroup>;
    CountEntriesInGroup(PersistentInterpretation<TokenGroup::Ptr>);

    WME::Ptr process(Token::Ptr) override;
    bool operator == (const BetaNode& other) const override;
    std::string getDOTAttr() const override;
};


} /* builtin */
} /* rete */


#endif /* end of include guard: RETE_BUILTIN_UTIL_HPP_ */
