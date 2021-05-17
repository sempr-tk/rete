#ifndef RETE_REASONER_CALLBACKEFFECTBUILDER_HPP_
#define RETE_REASONER_CALLBACKEFFECTBUILDER_HPP_

#include "NodeBuilder.hpp"
#include "../rete-core/CallbackEffect.hpp"
#include <utility>

namespace rete {

namespace {
}

/**
    Builds nodes required to trigger callback functions as effects of a rule.
*/
template <class... Args>
class CallbackEffectBuilder : public NodeBuilder {

    /**
     * Returns a PersistentInterpretation<T> with T being the I'th type in Args.
    */
    template <size_t I>
    static auto makePI(ArgumentList& args)
    {
        return util::requireInterpretation<
                    typename util::index_type_pack<I, Args...>::type
                >(args, I);
    }

    /**
     * Returns a tuple of PersistentInterpretation<Args>...
    */
    template <size_t... Is>
    static auto makePIs(ArgumentList& args, std::index_sequence<Is...>)
    {
        return std::make_tuple(makePI<Is>(args)...);
    }

public:
    typedef CallbackEffect<Args...> node_t;

    CallbackEffectBuilder(const std::string& name, typename node_t::fn_t fn)
        : rete::NodeBuilder(name, rete::NodeBuilder::BuilderType::EFFECT),
          callback_(fn), name_(name)
    {
    }

    Production::Ptr buildEffect(ArgumentList& args) const
    {
        // check number of arguments
        util::requireNumberOfArgs(args, sizeof...(Args));

        // create a tuple of PersistentInterpretation<Args>...
        auto persistentInterps = makePIs(args, std::make_index_sequence<sizeof...(Args)>{});

        // a helper that constructs the node from a callback and all the arguments
        auto nodeBuilder = 
        [&](PersistentInterpretation<Args>&... args)
        {
            return std::make_shared<CallbackEffect<Args...>>(
                this->name_, this->callback_, std::move(args)...
            );
        };
        
        // construct the node
        return util::apply(persistentInterps, nodeBuilder);
    }

private:
    typename node_t::fn_t callback_;
    std::string name_;
};


// helper specialization to handle argument types packed in a tuple type
template <class... Args>
class CallbackEffectBuilder<std::tuple<PropagationFlag, Args...>> : public CallbackEffectBuilder<Args...> {
public:
    CallbackEffectBuilder(const std::string& name, typename CallbackEffectBuilder<Args...>::node_t::fn_t fn)
        : CallbackEffectBuilder<Args...>(name, fn)
    {
    }
};

/**
 * Helper function to automatically infer template arguments for the required
 * NodeBuilder class. Simply give it the name for the effect and a function or
 * functor, the rest is template magic. :)
 */
template <class T>
auto makeCallbackBuilder(const std::string& name, T fn)
{
    // note - this function is only necessary as template arguments cannot be
    // deduced in a call to the constructor.
    
    typedef CallbackEffectBuilder<typename util::function_traits<T>::arg_types> builder_t;

    std::unique_ptr<builder_t> ptr(
        new builder_t(name, fn)
    );

    return ptr;

}

} /* rete */

#endif /* end of include guard: RETE_REASONER_CALLBACKEFFECTBUILDER_HPP_ */
