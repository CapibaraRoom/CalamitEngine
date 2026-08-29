local paths = {}
local classes = {}

local Object = {__name = "Object"}
Object.__index = Object
function Object:new(o)
    o = o or {}
    setmetatable(o, self)
    return o
end
function Object:addChild(child)
    if not self.children then self.children = {} end
    table.insert(self.children, child)
end
function Object:onload() end
function Object:ondraw() end
classes["Object"] = Object

local function find_file(name)
    for _, dir in ipairs(paths) do
        local full = dir .. "/" .. name
        local f = io.open(full, "r")
        if f then f:close(); return full end
    end
    return nil
end

function __engine_add_path(p)
    table.insert(paths, p)
end

local __parent = nil
function extends(name)
    __parent = name
end

local function class_name_from(filename)
    return filename:match("([^/\\]+)%.lua$")
end

local function load_class(filename)
    local name = class_name_from(filename)
    if classes[name] then return classes[name] end

    local path = find_file(filename)
    if not path then error("file not found: " .. filename) end

    __parent = nil
    local chunk = loadfile(path)
    if not chunk then error("failed to compile: " .. path) end

    local env = setmetatable({
        extends = extends,
        loadNode = loadNode,
        draw_rectangle = draw_rectangle,
    }, {__index = _G})
    setfenv(chunk, env)
    chunk()

    local cls = {}
    cls.__index = cls
    cls.__name = name
    for k, v in pairs(env) do
        if k ~= "extends" and k ~= "loadNode" and k ~= "draw_rectangle" then
            cls[k] = v
        end
    end

    if __parent then
        local parent = load_class(__parent .. ".lua")
        setmetatable(cls, {__index = parent})
        __parent = nil
    end

    classes[name] = cls
    return cls
end

function loadNode(filename)
    local cls = load_class(filename)
    return cls:new()
end

Tree = {roots = {}}

function Tree.load(node)
    table.insert(Tree.roots, node)
end

function Tree.update()
    local function rec(n)
        if n.onload then n:onload() end
        if n.children then
            for _, c in ipairs(n.children) do rec(c) end
        end
    end
    for _, n in ipairs(Tree.roots) do rec(n) end
end

function Tree.draw()
    local function rec(n)
        if n.ondraw then n:ondraw() end
        if n.children then
            for _, c in ipairs(n.children) do rec(c) end
        end
    end
    for _, n in ipairs(Tree.roots) do rec(n) end
end
