
local s = 0

function addX(a)
    s = s + a
    self.debug("add called" .. s)
    return a + a
end

function onTimeout()
    self.debug("Timed out" .. s)
end

function onUserEnter()
    self.debug("On User Enter" .. s)
end

self.debug("none called")