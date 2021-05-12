#ifndef RETE_CORE_CALLBACK_EFFECT_HPP_
#define RETE_CORE_CALLBACK_EFFECT_HPP_

#include <functional>

#include "Production.hpp"
#include "Accessors.hpp"
#include "Util.hpp"

namespace rete {

/**
 * Does not infer new data, but instead triggers a previously configured
 * callback function/functor.
 */
template <class... Args>
class CallbackEffect : public Production {
public:
    typedef std::function<void(PropagationFlag, Args...)> fn_t;

    CallbackEffect(const std::string& name, fn_t c, PersistentInterpretation<Args>... interpretations)
        : callback_(c), interpretations_(std::move(interpretations)...), name_(name)
    {
    }

    std::string toString() const
    {
        return "callback: " + name_;
    }

    void execute(Token::Ptr token, PropagationFlag flag, std::vector<WME::Ptr>&)
    {
        //std::cout << "Triggering Effect: " << name_ << std::endl
        //          << util::beautified_typename<decltype(*this)>().value
        //          << std::endl;
        
        // helper to extract one value given an index.
        auto fetchOne = [&](auto I)
        {
            typename util::index_type_pack<I, Args...>::type value;
            // get value using persistent interpretation
            std::get<I>(interpretations_).interpretation->getValue(token, value);
            return value;
        };

        // helper collect the results from multiple calls to fetchOne in a tuple
        auto fetchAll = [&](auto... Is)
        {
            return std::make_tuple<Args...>(
                fetchOne(Is)...
            );
        };

        // fetch all values we need by applying indices 0-(size-1) to fetchAll
        auto values = util::index_apply<sizeof...(Args)>(fetchAll);
        
        // helper to add the propagation flag to the call
        auto applyCallback = [&](auto... args)
        {
            callback_(flag, args...);
        };

        // apply all collected values and the propagation flag to the
        // configured callback function/functor
        util::apply(values, applyCallback);
    }

private:
    fn_t callback_;
    std::tuple<PersistentInterpretation<Args>...> interpretations_;
    std::string name_;
};


/**
 * Specialization in case the callback needs no arguments
 * (besides the propagation flag)
 */
template <>
class CallbackEffect<> : public Production {
public:
    typedef std::function<void(PropagationFlag)> fn_t;

    CallbackEffect(const std::string& name, fn_t c)
        : callback_(c), name_(name)
    {
    }

    std::string toString() const
    {
        return "callback: " + name_;
    }

    void execute(Token::Ptr /*token*/, PropagationFlag flag, std::vector<WME::Ptr>&)
    {
        //std::cout << "Triggering Effect: " << name_ << std::endl
        //          << util::beautified_typename<decltype(*this)>().value
        //          << std::endl;

        callback_(flag);
    }

private:
    fn_t callback_;
    std::string name_;
};

}

#endif /* include guard: RETE_CORE_CALLBACK_EFFECT_HPP_ */
