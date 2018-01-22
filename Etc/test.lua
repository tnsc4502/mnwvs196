selfx = Self.get(selfPtr)
selfx:pushArray(30030) 
selfx:pushArray(30020) 
selfx:pushArray(30000) 
selfx:pushArray(30480) 
selfx:pushArray(30310) 
selfx:pushArray(30330) 
selfx:pushArray(30060) 
selfx:pushArray(30150) 
selfx:pushArray(30410) 
selfx:pushArray(30210) 
selfx:pushArray(30140) 
selfx:pushArray(30120) 
selfx:pushArray(30200) 
selfx:askAvatar("Select : ", 4031249)

s = selfx:askMenu("#L0#Test0#l\r\n#L1#Test1#l\r\n#L2#Test2#l\r\n#L3#Test3#l")
s = selfx:sayNext("選擇了選項:" .. s, 0, 0)

s = selfx:askYesNo("測試 Yes No")
if(s == 1) then
    selfx:say("Press Yes")
else
    selfx:say("Press No")
end

s = selfx:askText("輸入某些文字:", "文字", 0, 5)
selfx:sayNext("輸入的文字 : " .. s, 0, 0)

s = selfx:askNumber("輸入某些數值:", 100, 0, 10000)
selfx:sayNext("輸入的數值 : " .. s, 0, 0)

local page = 0
while true do
    print("Run Script , page : "..page)
    if(page == 0) then
        page = selfx:sayNext("#r第 1 頁#k", 0, 1)  
    elseif(page == 1) then
        page = selfx:sayNext("#b第 2 頁#k", 1, 2)  
    elseif(page == 2) then
        page = selfx:sayNext("#g第 3 頁", 2, 3)  
    elseif(page == 3) then
        page = selfx:sayNext("第 4 頁", 3, 4)  
    elseif(page == 4) then
        page = selfx:sayNext("第 5 頁", 4, 5)  
    elseif(page >= 5) then break 
    else break end
end

s = selfx:sayNext("結束", 0, 0)