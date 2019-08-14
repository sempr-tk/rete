" Vim syntax file
" Language: Rete reasoner rules

if exists("b:current_syntax")
    finish
endif

syn match ruleVariable "?[a-zA-Z]\+" contained
syn match ruleFunctionName "[a-zA-Z]\+(\@=" contained
syn match ruleRDFResource "<.\{-}>" contained
syn match ruleRDFPrefixed "[a-zA-Z]\{-}:\w\+" contained
syn match ruleString "\".\{-}[^\\]\"" contained

syn keyword ruleKeyword noValue contained
syn match ruleOperator "->" contained
syn match ruleSquareBraces "\[\|\]"
syn match ruleRoundBraces "\((\|)\)" contained
syn match ruleComment "^#.*$"
syn match ruleName "[a-zA-Z]\{-}:" contained
syn match rulePrefix "@PREFIX.*$" contains=ruleName,ruleRDFResource
syn region ruleCondition start="[a-zA-Z]\{-}(" end=")" transparent contains=ruleVariable,ruleFunctionName,ruleRDFResource,ruleRDFPrefixed,ruleString
syn region rule start="\[" end="\]" fold transparent contains=ruleName,ruleKeyword,ruleOperator,ruleCondition,ruleComment,ruleSquareBraces



let b:current_syntax = "rules"

hi def link ruleName            Label
hi def link ruleFunctionName    Function
hi def link ruleRDFResource     Constant
hi def link ruleRDFPrefixed     Constant
hi def link ruleString          String
hi def link ruleComment         Comment
hi def link rulePrefix          PreProc
hi def link ruleKeyword         Keyword
hi def link ruleOperator        Special
hi def link ruleRoundBraces     Statement
hi def link ruleSquareBraces    Delimiter
hi def link ruleVariable        Identifier
hi def link ruleCondition       Statement
