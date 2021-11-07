" Vim syntax file
" Language: Bambi

" Usage Instructions
" Put this file in .vim/syntax/bambi.vim
" and add in your .vimrc file the next line:
" autocmd BufRead,BufNewFile *.bambi set filetype=bambi

if exists("b:current_syntax")
  finish
endif

syntax keyword bambiTodos TODO FIXME NOTE

" Language keywords
syntax keyword bambiKeywords if else elseif while do const return for fn

" Comments
syntax region bambiCommentLine start="//" end="$" contains=bambiTodos

" Strings
syntax region bambiString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region bambiString start=/\v'/ skip=/\v\\./ end=/\v'/

" Numbers
syntax match bambiNumber "\v<\d+>"
syntax match bambiNumber "\v<\d+\.\d+>"
syntax match bambiNumber "\v<\d*\.?\d+([Ee]-?)?\d+>"
syntax match bambiNumber "\v<0x\x+([Pp]-?)?\x+>"
syntax match bambiNumber "\v<0b[01]+>"
syntax match bambiNumber "\v<0o\o+>"

" Boolean
syntax keyword bambiBoolean true false nil

" Builin Types
syntax keyword bambiBuiltInTypes bool int float nil void struct enum

" Set highlights
highlight default link bambiTodos Todo
highlight default link bambiKeywords Keyword
highlight default link bambiCommentLine Comment
highlight default link bambiString String
highlight default link bambiNumber Number
highlight default link bambiBoolean Boolean
highlight default link bambiBuiltInTypes Type

let b:current_syntax = "bambi"
