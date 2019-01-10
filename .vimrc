set encoding=utf-8 fileencodings=ucs-bom,utf-8,cp936
syntax enable
syntax on
set tabstop=2
set expandtab
set nu
set autoindent
hi Normal ctermbg=Black ctermfg=white
set tags=tags;
set autochdir
se relativenumber
colorscheme morning

inoremap jk <esc>
inoremap ( ()<ESC>i
inoremap ) <c-r>=ClosePair(')')<CR>
function! ClosePair(char)
    if getline('.')[col('.') - 1] == a:char
        return "\<Right>"
    else
        return a:char
    endif
endfunction

function! s:insert_gates()
let gatename = substitute(toupper(expand("%:t")), "\\.", "_", "g")
	execute "normal! i#ifndef _" . gatename 
	execute "normal! o#define _" . gatename
	execute "normal! o\r\n"
	execute "normal! Go#endif // _" . gatename
	normal! kk
endfunction
autocmd BufNewFile *.{h,hpp,H} call s:insert_gates()

set rtp+=~/.vim/bundle/vundle/
