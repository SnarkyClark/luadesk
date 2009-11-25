title = "LuaDesk"
welcome = [[
Welcome to LuaDesk!
Since no application scripts where found,
]]

function main(args)
	iup.Message(title, welcome)

    local infile = inputfiles()
    if not infile then return 1 end

    local outfile = outputfile()
    if not outfile then return 1 end

    local fo = io.open(outfile, 'wb')

    -- copy self
    local fi = io.open(args[1], 'rb')
    fo:write(fi:read('*a'))
    fi:close()
    -- process scripts
    local toc = {}
    for i, v in ipairs(infile) do
        local fi = io.open(v, 'rb')
        if fi then
            -- read in file
            local buf = fi:read('*a')
            -- register toc entry
            toc[i] = {
                name = v,
                offset = fo:seek(),
                length = #buf
            }
            -- append output
            fo:write(buf)
            fi:close()
        end
    end
    -- finish with TOC
    local offset = fo:seek()
    fo:write("LuaDesk PQ" .. ld.htons(#infile))
    for i, v in ipairs(toc) do
        fo:write(table.concat {
            ld.htons(#v.name),
            ld.htonl(v.length),
            ld.htonl(v.offset),
            v.name
        })
    end
    fo:write(ld.htonl(offset) .. "LuaDesk PQ")
    fo:close()

    return 0
end

function explode(p, d)
    local t, ll
    t = {}
    ll = 0
    if(#p == 1) then return p end
    while true do
        l = string.find(p, d, ll+1, true) -- find the next d in the string
        if l ~= nil then -- if "not not" found then..
            table.insert(t, string.sub(p, ll, l-1)) -- Save it in our array.
            ll = l+1 -- save just after where we found it for searching next time.
        else
            table.insert(t, string.sub(p, ll)) -- Save what's left in our array.
            break -- Break at end, as it should be, according to the lua manual.
        end
    end
    return t
end

function inputfiles()
    local infile = {}
    local dlg = iup.filedlg {
        dialogtype = 'OPEN',
        title = title .. " Input File(s)",
        extfilter = "Lua Scripts|*.lua;Lua Objects|*.lo;All Files|*.*",
        multiplefiles = 'YES',
        directory = "./"
    }

    -- Shows file dialog in the center of the screen
    dlg:popup(iup.ANYWHERE, iup.ANYWHERE)

    if tonumber(dlg.status) ~= -1 then
        infile = explode(dlg.value, '|')
        if #infile > 1 then
            local t = {}
            local dir = nil
            for _, v in ipairs(infile) do
                if not dir then
                    dir = v
                else
                    if #v > 0 then table.insert(t, dir .. '\\' .. v) end
                end
            end
            infile = t
        end
        return infile
    else
        return
    end
end

function outputfile()
    local dlg = iup.filedlg {
        dialogtype = 'SAVE',
        title = title .. " Output file",
        extfilter = "Executable|*.exe;All Files|*.*",
        multiplefiles = 'YES',
        directory = "./"
    }

    -- Shows file dialog in the center of the screen
    dlg:popup(iup.ANYWHERE, iup.ANYWHERE)

    if tonumber(dlg.status) ~= -1 then
        return dlg.value
    else
        return
    end

end
