### A Colorful Version of **UNIX Like** systems' *[ls](https://git.savannah.gnu.org/cgit/coreutils.git/tree/src/ls.c)* command, inspired by *[colorls](https://github.com/athityakumar/colorls)* using *[NerdFonts](https://nerdfonts.com/)*

___

##### Requierements
1. _**gcc --version >= 8.0**_
2. **C++17**
3. [*Linux*](*Linux*)

___

##### Installation
**TODO**
- [x] Fix the one row bug
- [x] Fix the case when list -a a doesnt return 'Directory Not Found', but list a -a does...
- [x] Fix Floating Point Exception when `term_width < file_width`
- [ ] Add A Makefile
- [ ] Put the long list of icons in another file

**Run the following command in the _'list'_ directory**

`g++ -o bin/list src/list.cpp -O3 -std=c++17 -lstdc++fs`
	(Doesn't write to root, so we don't need root privileges)

##### Screenshots
![My Home Directory](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-26-43.png  "Home")
___
![Single Row](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-25-40.png  "One Row Printing")
___
![Multi Rows Single Column](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-25-08.png  "Multiple Rows")
