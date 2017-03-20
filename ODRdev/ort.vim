" Vim syntax file
" Language: Ortus  Development Rules
" Maintainer: Andrew W.E. McDonald
" Latest Revision: 3/11/2017
"
" NOTE: run:
" :so $VIMRUNTIME/syntax/hitest.vim 
" to see the colors vim uses for different highlight groups

if exists("b:current_syntax")
    finish
endif

" keywords for attributes block (the '{ ... }' section to the right of the
" rule specification
"
" lit => less important than
" mit => more important than
"
syn keyword attributeKeys lit mit bad good type

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" if referring to a neuron's negative change, it's a '-' followed by captial
" letters or numbers
syn match neuronNeg '-[A-Z0-9]\+'
" if referring to a neuron, regardless of change, it's captial
" letters or numbers
syn match neuronNeutral '[A-Z0-9]\+'
" if referring to a neuron's postivie change, it's a '+' followed by captial
" letters or numbers
syn match neuronPos '+[A-Z0-9]\+'
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" the rule ID (e.g., R1:, R2:, R100:, etc.). Simply 'R', followed by a number,
" and a colon, ':'. 
syn match ruleNum 'R\d\+:'

" this is the wrong approach, need to do it in a block, but for now, just
" highlight the braces
syn match attributeBlock /{/
syn match attributeBlock /}/


""""""""""""""""""""""""""""""""""""""""""""
" Gap Junction denoted by ==
syn match odrOper /==/
" Chemical Synapse denoted by >>
syn match odrOper />>/
syn keyword odrOper causes is dominates increases decreases
""""""""""""""""""""""""""""""""""""""""""""


hi neuronNeg guifg=#af0000 guibg=NONE guisp=NONE gui=NONE ctermfg=124 ctermbg=NONE cterm=NONE
hi neuronNeutral guifg=#af00af guibg=NONE guisp=NONE gui=NONE ctermfg=127 ctermbg=NONE cterm=NONE
hi neuronPos guifg=#00af00 guibg=NONE guisp=NONE gui=NONE ctermfg=34 ctermbg=NONE cterm=NONE

" want something for it to fall back on, i think... don't know if this is the
" way to do it or not.
"hi link neuronNeg String
"hi link neuronNeutral String
"hi link neuronPos String


syn keyword odrTodo contained TODO FIXME XXX NOTE
syn match odrComment "//.*$" contains=odrTodo

hi link odrTodo Todo
hi link odrComment Comment

hi link ruleNum Statement


hi link odrOper Operator 


hi link attributeKeys Type

hi link attributeBlock String

let b:current_syntax = "ort"

"hi def link ConnectionTypes    Operator 

