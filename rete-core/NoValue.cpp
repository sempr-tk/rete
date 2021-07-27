#include "NoValue.hpp"
#include "TupleWME.hpp"

namespace rete {

NoValue::NoValue(size_t diff)
    : tokenSizeDiff_(diff)
{
}

std::string NoValue::getDOTAttr() const
{
    return "[label=\"NoValue[diff: " + std::to_string(tokenSizeDiff_) + "]\"]";
}

std::string NoValue::toString() const
{
    return "NoValue[diff: " + std::to_string(tokenSizeDiff_) + "]";
}

Token::Ptr NoValue::getCorresponding(Token::Ptr token)
{
    for (size_t i = 0; i < tokenSizeDiff_; i++)
    {
        if (!token->parent) throw std::exception();
        token = token->parent;
    }

    return token;
}

void NoValue::leftActivate(Token::Ptr token, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception(); // what did you doooo?!

    // let the memory handle the retract.
    if (flag == PropagationFlag::RETRACT)
    {
        bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
        return;
    }

    /*
        Iterate the items in the right memory. If any of those has a token that
        is based on the token we got as input the "noValue" is violated and the
        token must not be forwarded.
    */
    auto pRight = getRightParent();
    for (auto rightToken : *pRight)
    {
        if(getCorresponding(rightToken) == token)
        {
            // there is a match for this token in the right memory.
            // --> noValue violated.
            if (flag == PropagationFlag::ASSERT)
            {
                // nothing to do. just dont propagate the assert.
                return;
            }
            else if (flag == PropagationFlag::UPDATE)
            {
                // well... there might have been an assert that was okay
                // before. to be sure, issue a RETRACT.
                bmem->leftActivate(token, nullptr, PropagationFlag::RETRACT);
                return;
            }
        }
    }

    // if we get here, there has been no match in the right memory.
    // so, there is "noValue".
    auto empty = std::make_shared<EmptyWME>();
    empty->description_ = "noValue";
    bmem->leftActivate(token, empty, flag);
}


void NoValue::rightActivate(Token::Ptr token, PropagationFlag flag)
{
    auto bmem = bmem_.lock();
    if (!bmem) throw std::exception();

    if (flag == PropagationFlag::ASSERT)
    {
        // a new entry in the exclude-list -- check the output memory, we
        // might need to retract something from there
        auto corresponding = getCorresponding(token);

        // if corresponding is present in the left memory (well it is, else it
        // could not be in the right memory, which is a descendent of left)
        // and it exists in the output, we need to retract it from there.
        // Nothing easier than that: just let the memory handle it.
        bmem->leftActivate(corresponding, nullptr, PropagationFlag::RETRACT);
    }
    else if (flag == PropagationFlag::UPDATE)
    {
        // what to do on update? An existing match changed, but it still exists,
        // which is all we are checking anyways. Nothing to do then.
    }
    else if (flag == PropagationFlag::RETRACT)
    {
        // step 1: check if there is another entry in right memory with the
        // same corresponding token. If so, we can just stop as the other will
        // already violate the noValue condition
        auto corresponding = getCorresponding(token);
        auto pRight = getRightParent();
        for (auto rightToken : *pRight)
        {
            if (getCorresponding(rightToken) == corresponding)
            {
                return; // yup. this will do.
            }
        }

        // step 2: well, there was no such token... so if it still is in the
        // left memory we know that only the "noValue" part was retracted,
        // making the "noValue" condition satisfied.
        auto pLeft = getLeftParent();
        for (auto leftToken : *pLeft)
        {
            if (leftToken == corresponding)
            {
                // assert it now.
                auto empty = std::make_shared<EmptyWME>();
                empty->description_ = "noValue";
                bmem->leftActivate(leftToken, empty, PropagationFlag::ASSERT);
                return; // there can be only one such token, as we check by ptr
            }
        }
    }
}

}
