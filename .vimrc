"mkdir -p ~/.vim/bundle
"git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim

set encoding=utf-8 fileencodings=ucs-bom,utf-8,cp936
syntax enable
syntax on

set hlsearch   "搜索语法高亮
set tabstop=4 "tab键个格数

set number
se relativenumber

set tags=tags;
set autochdir
set autoindent
set expandtab

colorscheme morning

"----------------------------"
"----------Vundle------------"
"----------------------------"

set nocompatible
filetype off
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

Plugin 'VundleVim/Vundle.vim'

Plugin 'The-NERD-tree'
map <F2> :NERDTreeToggle<CR>
let g:NERDTreeShowLineNumbers=1 "显示目录树窗口行号

Plugin 'The-NERD-Commenter'
let mapleader=","

Plugin 'vim-airline/vim-airline'
set laststatus=2
set t_Co=256
let g:airline#extensions#tabline#enabled = 1
"按Ctrl+h 向左移动一个buffer
nnoremap <C-h> :bp<CR>
""按Ctrl+l 向右移动一个buffer
nnoremap <C-l> :bn<CR>
"按Ctrl+x 关闭当前buffer
nnoremap <C-x> :bd<CR>

Plugin 'ctrlpvim/ctrlp.vim'
set wildignore+=*/tmp/*,*.so,*.swp,*.zip
let g:ctrlp_custom_ignore = {
                         \ 'dir':  '\v[\/]\.(git)$',
                         \ 'file': '\v\.(log|jpg|png|jpeg)$',
                         \ }

call vundle#end() 
"filetype plugin indent on 
filetype on

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
