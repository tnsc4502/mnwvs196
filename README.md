MapleNight Wvs Project
=
About this project
----
This is an official-like server emulator of TWMS ver. 196. 

This server is based on C++ language and uses lua as script language(the official server use a lua-like language for the script system also), [here](https://hackmd.io/-iQnMU8eSieu4s-QraftrA) to understand the difference between our implementation and OdinMS. 

We (do our best)follow official-like function naming, coding styles, core implementations and script implementations, for descriptions of script functions, please refer to [here](https://hackmd.io/EkoGL7yFTnutUS8gNiXcKw).

Furthermore, the server maintains a memory allocator to prevent memory fragmentation. We also provide a manual which instruct developer to allocate/deallocate objects in this project correctly, please refer to [this](https://hackmd.io/mMCvnLFRTx2VrZTuot0OpQ).

How to build
----
This project is currently able to build on Windows OS only because we use ppltask(which is provided by Windows as their unique concurrent C++ API.) to implement timer and event task driver.

This project is working under Visual Studio 2015. To be compatible with newer version of VS, you need to rebuild boost library by that VS.

The dependencies are included in the "Library" folder, which has been compressed as a rar file. All you need to do is to extract "boost_1_63_0" and "poco-1.7.8-all" folders under "Library" folder. The former is used for our network core, and the later one is to provide access to the MySQL server respectively.

The progress
---
So far we had finished:
* WvsGame, WvsLogin, WvsCenter and WvsShop are now basically working.
* [Script system](https://hackmd.io/EkoGL7yFTnutUS8gNiXcKw) had been implemented, it's easy to expand just by following extant samples.
* When transferring between channels(said, WvsGame) or between channel and shop(said, WvsShop), the status(like the remained time of buffers, pets, summoneds) are able to transfer correctly too.

Maybe you will see different variable-naming in the project becuase when I decided following official-naming style the server fundmentals have been done, this will be improved next time.

