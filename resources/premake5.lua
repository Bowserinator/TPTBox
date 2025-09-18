-- Shaders: compiled to new folder
local function concatArray(a, b)
    local result = {table.unpack(a)}
    table.move(b, 1, #b, #result + 1, result)
    return result
end

local function minifyShader(shader_code)
    return shader_code
        :gsub("\r\n", "\n")
        :gsub("//[^\n\r]*", "") -- Remove comments
        -- :gsub("[\n\r]", "") -- Condense all code to 1 line, removed for easier shader macro support
        :gsub("\n%s*\n", "\n")
        :gsub("  ", " ")
        :gsub("%;[ ]+", ";") -- naive minification
end

local function importBlock(fpath, subblock)
    local f = io.open(fpath, "r")
    local shader_code = f:read("*all")
    local expected = "#startblock " .. subblock
    local in_block = false
    local out = ""

    for line in string.gmatch(shader_code,'[^\r\n]+') do
        if line:sub(1, #expected) == expected then
            in_block = true
        elseif in_block and line ~= "#endblock" then
            out = out .. line .. "\n"
        elseif in_block and line == "#endblock" then
            return minifyShader(out:gsub('[ \t]+%f[\r\n%z]', ''))
        end
    end
    error("Unable to find " .. fpath .. ":" .. subblock)
end

local function generateEmbeddedShader(fpath)
    if string.match(fpath, "/imports/") then -- imports, not standalone shaders
        return
    end

    local outpath = "shaders/generated/" .. path.getname(fpath) .. ".h"
    local var_name = path.getname(fpath):gsub("%.", "_") .. "_source"

    local f = io.open(fpath, "r")
    local shader_code = f:read("*all")

    -- -- Get previous hash of element list
    f = io.open(outpath, "r")
    local oldhash = "//";
    if f ~= nil then
        oldhash = f:read()
        f:close()
    end

    local newhash = "// " .. string.hash(shader_code)

    -- Regenerate shader header if changed
    if oldhash ~= newhash then
        print("Regenerating shader", fpath)
        shader_code = minifyShader(shader_code)

        symbols = { "{", "}", "(", ")", ",", "=", "<", ">", "<=", ">=", "!=", "==", "*", "/", "+", "-", "&", "<<", ">>", ">>=", "<<=", "^", "?" }
        for _, symbol in ipairs(symbols) do
            shader_code = shader_code:gsub("[ ]*%" .. symbol .. "[ ]*", symbol)
        end
        -- Version directive requires newline, fortunately it's always at the top
        for version = 100,600,10 do
            shader_code = shader_code:gsub("%#version " .. version, "#version " .. version .. "\n")
        end

        -- Macro replacement
        local to_replace = {}
        local prefix = "#import "
        for line in string.gmatch(shader_code,'[^\r\n]+') do
            if line:sub(1, #prefix) == prefix then
                to_replace[#to_replace + 1] = line:sub(#prefix + 1);
            end
        end

        for _, import in ipairs(to_replace) do
            local tokens = {}
            for i in string.gmatch(import, "[^:]+") do
                tokens[#tokens + 1] = i
            end
            imported = importBlock("shaders/imports/" .. tokens[1], tokens[2])
            shader_code = shader_code:gsub("\n#import " .. import .. "\n", "\n" .. imported)
        end

        shader_code = newhash .. "\n" ..
            "// This file is auto-generated! Your edits will not be saved\n" ..
            "#ifdef EMBED_SHADERS\n" ..
            "util::TPBShaderSourceCode " .. var_name .. "{\"" .. fpath .. "\", R\"(\n" ..
            shader_code ..
            ")\"};\n" ..
            "#else\n" ..
            "util::TPBShaderSourceCode " .. var_name .. "{\"" .. fpath .. "\", util::load_text_file(\"resources/" .. fpath .. "\")};\n" ..
            "#endif\n"

        local f = io.open(outpath, "w")
        f:write(shader_code)
        f:close()
    end

end

local function generateEmbeddedImage(filename)
    local outpath = "generated/" .. path.getname(filename) .. ".h"
    local bytes = {}
    local byte_count = 0
    file = assert(io.open(filename, "rb"))

    while true do
        local byte = file:read(1)
        if byte == nil then
            break
        else
            bytes[#bytes + 1] = string.format("0x%x", string.byte(byte))
            byte_count = byte_count + 1
        end
    end
    file:close()
    local data_block = table.concat(bytes, ",")

    -- -- Get previous hash of element list
    f = io.open(outpath, "r")
    local oldhash = "//";
    if f ~= nil then
        oldhash = f:read()
        f:close()
    end

    local newhash = "// " .. string.hash(data_block)

    -- Regenerate shader header if changed
    if oldhash ~= newhash then
        print("Regenerating image", filename)

        local var_name1 = path.getname(filename):gsub("%.", "_") .. "_data"
        local var_name2 = path.getname(filename):gsub("%.", "_") .. "_size"

        data_block = newhash .. "\n" ..
            "// This file is auto-generated! Your edits will not be saved\n" ..
            "const unsigned char " .. var_name1 .. "[] = {" .. data_block .. "};\n"
                .. "const unsigned int " .. var_name2 .. " = " .. tostring(byte_count) .. ";\n"

        local f = io.open(outpath, "w")
        f:write(data_block)
        f:close()
    end
end

local shaders = concatArray(
    os.matchfiles("shaders/**.vs"),
    concatArray(
        os.matchfiles("shaders/**.fs"),
        os.matchfiles("shaders/**.comp")
    )
)

for _, filepath in pairs(shaders) do
    generateEmbeddedShader(filepath)
end

generateEmbeddedImage("icons.png")