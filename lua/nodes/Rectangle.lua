extends("Node2D")
width = 0
height = 0

function draw(self)
    draw_rectangle(self.x, self.y, self.width, self.height)
end

function ondraw(self)
    self.draw(self)
end
