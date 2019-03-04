#define USE_RTTI
#include <pegmatite/pegmatite.hh>

namespace rete {

using namespace pegmatite;

/**
    The rule-parsers grammar
*/
class RuleGrammar {
    RuleGrammar();

public:
    static const RuleGrammar& get()
    {
        static RuleGrammar g;
        return g;
    }

    #define TRACE_LEVEL 1
    #if TRACE_LEVEL > 0
        #define rtrace(name, expression)    (trace((name), (expression)))
    #else
        #define rtrace(name, expression)    (expression)
    #endif
    #if TRACE_LEVEL > 1
        #define r1trace(name, expression)    (trace((name), (expression)))
    #else
        #define r1trace(name, expression)    (expression)
    #endif

    Rule endl   = "\r\n"_E | "\n\r" | "\n" | "\r";
    Rule ws     = *(" \t"_S | endl);
    Rule alpha  = "[a-zA-Z]"_R;
    Rule num    = "[0-9]"_R;
    Rule alphanum = alpha | num;
    Rule echar  = '\\' >> "tbnrf\"\'\\"_S;

    // while num is just a single digit,
    // number is a complete, terminal value.
    Rule number = term( -("+-"_S) >> +num >> // some digits, plus optionally...
                  -('.'_E >> +num >>  // dot and numbers, plus optionally...
                    -("eE"_S >> -("+-"_S) >> +num) // an optionally signed exponent
                    ));
    // to allow whitespaces etc as a single argument, define a quoted string.
    // starts with a ", ends with a ", and everything inbetween is either \" or just not "
    Rule quotedString = "\"" >> *("\\\""_E | (!"\""_E >> any())) >> "\"";

    Rule iriref = rtrace("iriref",
        (
            r1trace("open-bracket", '<'_E) >>
            r1trace("any-amount-of-not-stuff", *(
                r1trace("not", !
                    r1trace("stuff", ("<>\"{}|^`\\"_S))) >>
                    r1trace("followed by any (not doesnt move cursor)", any())
                      )
                  ) >>
            r1trace("close-bracket", '>'))
        );

    // e.g.: @PREFIX envire: <http://envire.semantic/>
    Rule prefixname = rtrace("prefixname", (+alphanum));
    Rule prefixuri = rtrace("prefixuri", iriref);
    Rule prefixdef = rtrace("prefixdef", "@PREFIX"_E >> prefixname >> ':' >> prefixuri);

    Rule prefixedURI = rtrace("prefixedURI", term(+alphanum >> ':' >> +alphanum));

    Rule stringliteralquote = '\"'_E >> *((!("\x22\x5C\xA\xD"_S)) | echar) >> '\"';
    Rule langtag = '@'_E >> +alpha >> *('-' >> +alphanum);

    Rule literal =          (stringliteralquote >> -("^^" >> iriref | langtag));
    Rule blank_node_label = ("_:"_E >> +alphanum);
    Rule variable =         ("?"_E >> +alphanum);
    Rule uri =              prefixedURI | iriref;

    Rule subject    = rtrace("subject",   term(variable | iriref | prefixedURI | blank_node_label));
    Rule predicate  = rtrace("predicate", term(variable | iriref | prefixedURI));
    Rule object     = rtrace("object",    term(variable | iriref | prefixedURI | blank_node_label | literal));

    Rule triple = rtrace("triple",   ('('_E >> subject >> predicate >> object >> ')'));
    // cannot reuse triple for infertriple, would lead to triple being constructed before infertriple, and the infertriple being empty...
    Rule inferTriple = rtrace("inferTriple", '('_E >> subject >> predicate >> object >> ')');

    // [name: (precondition1), (precondition2) --> (effect1), (effect2)]

    Rule argument = rtrace("argument", term(quotedString | number | variable));
    Rule builtinName = rtrace("builtinName", +alphanum >> -(":"_E >> +alphanum));
    Rule builtin = rtrace("builtin", builtinName >> "(" >> *argument >> ")");
    Rule precondition = rtrace("precondition", triple | builtin);
    Rule effect = rtrace("effect", inferTriple);

    Rule rulename = rtrace("rulename", +alphanum);
    Rule rule = rtrace("rule", ('['_E >> -(rulename >> ':') >> precondition >> *(',' >> precondition) >> "->" >> effect >> *(',' >> effect) >> ']'));
    Rule rules = rtrace("rules", *prefixdef >> +rule);

    // TODO: Overhaul for builtins, other WMEs than triples, ...
};


} /* rete */
