關於此專案
About this project
----
此專案為台版楓之谷v196.3對應版本使用伺服器，且遵循官方架構，為高效能且具高可靠度的伺服器。


---

The purpose of this project is to build a high performance/reliable and official-like C++ game server for version 196.3 (or higher[Taiwan-Client]).

下載此專案前須注意的事情
Before cloning this project
----
這不是一個可玩版本，目前的進度[2017/11/30]只能進到遊戲中切換地圖/取得怪物控制權，但此專案已經建立可行的仿官方架構基礎伺服器，並且整合了boost::asio網路、基於Poco Library的資料庫存取、高效能的Wz讀取等除此之外，伺服器已經具備基礎功能，伺服器間的溝通架構也已經編寫完畢，對於想參與開發的人來說已經不需要在重新編寫底層功能。

由於我有點忙碌加上一個人開發的效率實在太慢，因此希望能有好心人一起加入開發行列，另外在專案中可能會看到不同的變數命名習慣，主要是因為這個專案已經擱置一段時間，當我後來決定完全仿造官方命名法則時已經來不及了，但會在下次全數修正(盡力)。

---

This is not a playble server but have merged many basic functions like boost::asio networking, Mysql based on Poco, high speed Wz library and the basic server has been done[Including WvsGame, WvsLogin, WvsCenter and local server communications]

Maybe you will see different variable-naming in the project becuase when I decided following official-naming style the server fundmentals have been done, this will be improved next time.

此專案的相依套件
Dependencies of this project
----
大部分所需的套件已經包含在專案中，但有些較為龐大的必須額外下載或者需要對應平台支援:
1. 此專案只有在Visual Studio 2015上編譯成功，其他版本尚未測試。
2. 此專案需要 boost 1.63.0 版本(boost::asio, boost::singleton_pool需要使用)
3. 此專案需要 poco 1.7.8(Mysql功能)
4. 此專案需要微軟 Parallel Patterns Library(ppltask.h)作為異步Scheduler的基底，通常已經內建在Visual Studio了


---

Most of the libraries the projected used had been included in the project but here still some larage and platform-dependent lib you should notice:
1. Compilation of this project only tried(and sure, successed) on Visual Studio 2015, other compilers not tried yet.
2. Need boost 1.63.0 for networking and thread-safe memory pool.
3. Need poco 1.7.8 for mysql.
4. Need Microsoft Parallel Patterns Library(ppltask.h) for async task scheduler (usually built-in Visual Studio 2015).

關於某些套件
About some libraries in the project
----
專案中有些部分是取自於其他來源，其用途如下:
**Common\AES:**
此資料夾內的檔案用於處理AES加/解密，取自於TitanMS(或者其他專案...忘記了).

**Common\Memory:**
此專案內包含目前已經沒有使用到的Memory Pool檔案，目前僅使用MemoryPoolMan.cpp/hpp，使用Memory Pool的原因如下:
1. stWzMemoryPoolMan 用於Wz Lib的記憶體配置，如果伺服器在一開始就讀取所有Wz數值，之後可從WzResMan釋放所有記憶體，原始的Wz Lib透過new 或 malloc並不允許這樣的操作，如果Wz讀取採取Lazy-init這個真的不實用。
2. stMemoryPoolMan 用於封包的陣列記憶體配置，為了減少new造成的記憶體碎片問題，使用此pool作為陣列專用的配置器，但注意不可配置一般物件，因為配置後的物件沒有呼叫Contructor，除非你自己仿造placment new的作法。
3. Memory Pool配置出來的記憶體請使用對應的DestructArray函式，請勿使用delete或free。
4. 如果覺得不需要Memory Pool (因為效能實際改進多少我並不知道)，可以移除。

**Common\Task:**
AsnycScheduler是為了要建立Timer用，原本要使用boost的timer library但是好像ppltask的更為方便。

**Common\Wz:**
Wz Lib，使用方法可參考Field.cpp。


---

Here are some libraries from other projects(and used in this project):
**Common\AES:**
As the name shown, for aes encrypt/decrypt.

**Common\Memory:**
Including memory pool manager (some files are not used currently, I will delete them in the furture.)
The only used are MemoryPoolMan.cpp/hpp, and you should take care:
1. stWzMemoryPoolMan is the allocator for wz library, the reason I use memory pool instead of new or malloc is for one-time deallocating all wz resources in memory after your server loads all wz infomations[Like Odinms did, load all data when server started], but the server currently used lazy-init not sure the practicality in this situation.
2. stMemoryPoolMan is the allocator for packet allocation, purpose for reducing memory fragnments(Not sure whether the OS or compiler this time will optimize these problems or not) and **must not** used for allocating variable object because  there is no constructor-calling in memory pool. 
3. Don't use delete or free to deallocate the resource that allocated from memory pool.
4. Maybe you don't need memory pool at all then you can remove it and replaced by new/delete.

**Common\Task:**
Like Java TimerTask, Can easily be re-implemented by boost timer library if you dont want use ppltask.

**Common\Wz:**
Wz lib here, the example usage can be found in WvsGame/FieldMan.cpp.


最後
The last
----
As you see, there are lots of warnings (you know, treating char like int ... etc...) when trying to build the project and maybe lots of bugs or memory leaking in the project, ya, I hope we can solve them together. LOL.