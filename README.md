# A Colorful Version of **UNIX Like** systems'
*[ls](https://git.savannah.gnu.org/cgit/coreutils.git/tree/src/ls.c)* command, inspired by *[colorls](https://github.com/athityakumar/colorls)* using *[NerdFonts](https://nerdfonts.com/)*

___

## Requierements

1. _**gcc --version >= 8.0 or another compiler that supports C++17**_
2. **Linux**
3. **Terminal with 24-bit / true-color Support**
4. **Nerd Fonts**

___

## Installation

### TODO

- [x] Fix the one row bug
- [x] Put the long list of icons in another file
- [x] Add usage (Flags)
- [x] Add more screenshots
- [ ] Add proper symlink support
- [ ] Change the File class to use `std::filesystem` classes

### Build Only
**Run `make` in the _`src`_ directory**

**_This will create a directory called `bin` with the binary in it without installing it_**
(Doesn't write to root, so we don't need root privileges)

___
### Install To /usr/bin/
**Run `sudo make list install` in the `src` directory, if you want to change the installation location edit the `Makefile`**

**_To change or add icons modify `icons.cpp` and recompile_**
 *(Note that some icons need a space after them)*

## Screenshots
![My Home Directory](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-26-43.png  "Home")
___
![Single Row](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-25-40.png  "One Row Printing")
___
![Multi Rows Single Column](https://github.com/OnurKader/list/blob/master/imgs/Screenshot%20from%202019-07-15%2016-25-08.png  "Multiple Rows")
___
![New Home Listing](https://raw.githubusercontent.com/OnurKader/list/refactor/imgs/2020-03-03%2014-36-57.png)
___
![Long Listing](https://raw.githubusercontent.com/OnurKader/list/refactor/imgs/2020-03-03%2014-37-26.png)
