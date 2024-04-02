include "elements"
include "tools"

-- Generate the element list
local element_defines = {}
for id, name in pairs(elements) do
    table.insert(element_defines, "ELEMENT_DEFINE(" .. name .. "," .. id .. ");\n")
end
element_defines = table.concat(element_defines)

defines { "__GLOBAL_ELEMENT_COUNT=" .. (#elements) }

-- Get previous hash of element list
local f = io.open("ElementNumbers.h")
local oldhash = f:read()
local newhash = "// " .. string.hash(element_defines)
f:close()

-- Write result to ElementNumbers.h if changed
if oldhash ~= newhash then
    local element_number_h_out = {}
    table.insert(element_number_h_out, newhash .. "\n") -- Hash for updates

    for line in io.lines("ElementNumbers.h.template") do
        if line == "@element_defs@" then
            line = element_defines
        end

        element_number_h_out[#element_number_h_out + 1] = line .. "\n"
    end

    local f = io.open("ElementNumbers.h", "w")
    f:write(table.concat(element_number_h_out))
    f:close()
end


-- Generate the tool list
local tool_defines = {}
for id, name in pairs(tools) do
    table.insert(tool_defines, "TOOL_DEFINE(" .. name .. "," .. id .. ");\n")
end
tool_defines = table.concat(tool_defines)

defines { "__GLOBAL_TOOL_COUNT=" .. (#tools) }

-- Get previous hash of tool list
local f = io.open("ToolNumbers.h")
local oldhash = f:read()
local newhash = "// " .. string.hash(tool_defines)
f:close()

-- Write result to ToolNumbers.h if changed
if oldhash ~= newhash then
    local tool_number_h_out = {}
    table.insert(tool_number_h_out, newhash .. "\n") -- Hash for updates

    for line in io.lines("ToolNumbers.h.template") do
        if line == "@tool_defs@" then
            line = tool_defines
        end

        tool_number_h_out[#tool_number_h_out + 1] = line .. "\n"
    end

    local f = io.open("ToolNumbers.h", "w")
    f:write(table.concat(tool_number_h_out))
    f:close()
end


-- The rest
files {"**.c", "**.cpp", "**.h", "**.hpp"}