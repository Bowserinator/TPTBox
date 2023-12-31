include "elements"

-- Generate the element list
local element_defines = {}
for id, name in pairs(elements) do
    table.insert(element_defines, "ELEMENT_DEFINE(" .. name .. "," .. id .. ");\n")
end
element_defines = table.concat(element_defines)

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

-- The rest
files {"**.c", "**.cpp", "**.h", "**.hpp"}