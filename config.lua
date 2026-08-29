Settings.include("./scenes")
Settings.include("./lua")
Settings.include("./lua/nodes")
Settings.import("settings.lua")

local scene = loadNode("MyScene.lua")
Tree.load(scene)
