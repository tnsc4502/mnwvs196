selfx = Self.get(selfPtr)

selfx:sayNextGroup(
    "#r第 1 頁#k", 
    "#b第 2 頁#k", 
    "#g第 3 頁#k", 
    "第 4 頁", 
    "第 5 頁"
    );


selfx:askAvatar("Select : ", 4031249
    ,30030
    ,30020
    ,30000
    ,30480
    ,30310
    ,30330
    ,30060
    ,30150
    ,30410
    ,30210
    ,30140
    ,30120
    ,30200
)

s = selfx:askMenu("#L0#Test0#l\r\n#L1#Test1#l\r\n#L2#Test2#l\r\n#L3#Test3#l")
s = selfx:sayNext("選擇了選項:" .. s)

s = selfx:askYesNo("測試 Yes No")
if(s == 1) then
    selfx:say("Press Yes")
else
    selfx:say("Press No")
end

s = selfx:askText("輸入某些文字:", "文字", 0, 5)
selfx:sayNext("輸入的文字 : " .. s)

s = selfx:askNumber("輸入某些數值:", 100, 0, 10000)
selfx:sayNext("輸入的數值 : " .. s)

s = selfx:sayNext("結束")