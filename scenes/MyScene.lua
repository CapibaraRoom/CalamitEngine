extends("Node2D")

function onload(self)
    local r1 = loadNode("Rectangle.lua")
    r1.x, r1.y, r1.width, r1.height = 100, 100, 80, 80
    self:addChild(r1)

    local r2 = loadNode("Rectangle.lua")
    r2.x, r2.y, r2.width, r2.height = 300, 100, 80, 80
    self:addChild(r2)
end
