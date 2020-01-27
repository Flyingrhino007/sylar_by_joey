" =======================
" 基本设置
" =======================
syntax enable           " 语法高亮
syntax on               " 语法高亮

filetype on             " 检测文件类型
filetype plugin on      " 不同文件采用不同缩进方式
filetype indent on      " 允许插件
filetype plugin indent on " 启动自动补全
set autoindent

colorscheme desert

set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

Plugin 'VundleVim/Vundle.vim'
Plugin 'a.vim'

call vundle#end()

set nu                              " display row number
set ruler                           " 显示当前行号
set showmode                        " 显示当前vim模式
set showcmd                         " display input command
set shiftwidth=4                    " shift width = 4
set tabstop=4                       " tabstop width = 4
set expandtab                       " Automatically convert tabs into spaces when typing
set smarttab                        " backspace 自动删除 tab
" set mouse=a                         " 允许鼠标操作
set nobackup                        " Cancel automatic backup and generate swp file
set nowb                            
set noswapfile                      " 取消临时文件
set encoding=utf8
set completeopt=longest,menu        " 自动补全设置
set nocompatible 
set backspace=eol,start,indent      " set backspace deal with indent, eol, start
set magic                           
set showmatch                       " highlight matched 
set ai                              " Make vi automatically align
set si                              " Smart indentation indent
set cindent                         " Indented inline code blocks
set wildmenu                        " Command-line auto-completion in enhanced mode
set history=400
set fileencodings=utf8,gb2312,gb18030,ucs-bom,latin1

" =======================
" 状态栏设置
" =======================
set laststatus=2
highlight StatusLine cterm=bold ctermfg=yellow ctermbg=blue
set statusline=%F%m%r%h%w\[POS=%l,%v][%p%%]\[%{&fileformat}]\[%{&encoding}]\[%{strftime(\"%Y-%m-%d\ %H:%M:%S\")}] 

" =======================
" TagList插件设置
" =======================
autocmd FileType cpp set tags+=~/.vim/tags/cpp_src/tags     " 设置tags文件包含
let Tlist_Show_One_File=1                                   " 只展示一个文件的taglist
let Tlist_Exit_OnlyWindow=1                                 " 当taglist是最后一个窗口时自动退出
let Tlist_Use_Right_Window=1                                " 在右边显示taglist窗口
let Tlist_Sort_Type="name"                                  " tag按名字排序


" =======================
" C++ 补全插件设置
" =======================
autocmd FileType c setlocal omnifunc=cppcomplete#Complete
autocmd FileType cpp setlocal omnifunc=cppcomplete#Complete

let OmniCpp_NamespaceSearch=1
let OmniCpp_GlobalScopeSearch=2
let OmniCpp_ShowAccess=1
let OmniCpp_MayCompleteDot=1
let OmniCpp_MayCompleteArrow=1
let OmniCpp_MayCompleteScope=1
let OmniCpp_DefaultNamespaces=["std","_GLIBCXX_STD"]

set tags+=tags
set tags+=~/.vim/tags/cpp_src/tags
"set tags+=~/.vim/tags/libc.tags
"set tags+=~/.vim/tags/cpp.tags
"set tags+=~/.vim/tags/glib.tags
"set tags+=~/.vim/tags/susv2.tags
set tags+=~/workspace/sylar/tags

"============================
""  自动添加文件头
"============================
""新建.c,.h,.sh,.java文件，自动插入文件头 
autocmd BufNewFile *.cpp,*.[ch],*.sh,*.rb,*.java,*.py exec ":call SetTitle()" 
""定义函数SetTitle，自动插入文件头 
func SetTitle() 
    "如果文件类型为.sh文件 
    if &filetype == 'sh' 
        call setline(1,"\#!/bin/bash") 
        call append(line("."), "") 
    elseif &filetype == 'python'
        call setline(1,"#!/usr/bin/env python")
        call append(line("."),"# coding=utf-8")
        call append(line(".")+1, "") 
    else 
        call setline(1, "/*************************************************************************") 
        call append(line("."), "   > File Name: ".expand("%")) 
        call append(line(".")+1, "   > Author: Joey/Cao") 
        call append(line(".")+2, "   > Mail: flyingrhino@163.com") 
        call append(line(".")+3, "   > Created Time: ".strftime("%c")) 
        call append(line(".")+4, " ************************************************************************/") 
        call append(line(".")+5, "")
    endif
    if expand("%:e") == 'cpp'
        call append(line(".")+6, "")
    endif
    if &filetype == 'c'
        call append(line(".")+6, "#include<stdio.h>")
        call append(line(".")+7, "")
    endif
    if expand("%:e") == 'h'
        call append(line(".")+6, "#ifndef _".toupper(expand("%:r"))."_H")
        call append(line(".")+7, "#define _".toupper(expand("%:r"))."_H")
        call append(line(".")+8, "#endif")
    endif
    if &filetype == 'java'
        call append(line(".")+6,"public class ".expand("%:r"))
        call append(line(".")+7,"")
    endif
    "新建文件后，自动定位到文件末尾
endfunc 
autocmd BufNewFile * normal G

"============================
""  快捷键设置
"============================
""Ctrl + n 打开目录树
" map <C-n> :NERDTree<CR>         
"Ctrl + t 打开 taglist
map <F7> :Tlist<CR>
"ctags 生成 cpp 文件的 tags
" map <F12> :call CreateTags()<CR>
"<F5> 编译运行
" map <F5> :call ComplieAndRun() <CR>
""<C-F5> 调试
" map <C-F5> :call Rungdb() <CR>

if &term == "xterm"
    set t_Co=8
    set t_Sb=^[[4%dm
    set t_Sf=^[[3%dm
endif

let g:SuperTabRetainCompletionType=2
let g:SuperTabDefaultCompletionType="<C-X><C-O>"
