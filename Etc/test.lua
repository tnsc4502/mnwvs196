--[[inv = inventory
--self.say(inventory.exchange(100, 1372007, 1))

self.askAvatar("Select : ", 4031249, {30030
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
,30200})

s = self.askMenu("#L0#Test0#l\r\n#L1#Test1#l\r\n#L2#Test2#l\r\n#L3#Test3#l")

s = self.askYesNo("測試 Yes No")
if(s == 1) then
    self.say("Press Yes")
else
    self.say("Press No")
end

s = self.askText("輸入某些文字:", "文字", 0, 5)
self.sayNext("輸入的文字 : " .. s)

s = self.askNumber("輸入某些數值:", 100, 0, 10000)
self.sayNext("輸入的數值 : " .. s)

s = self.sayNext("結束")--]]
fieldSet.enter("TestFieldSet")