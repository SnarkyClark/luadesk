-------------------
-- configuration --
-------------------
conf = {
	title = "LuaDesk",
	welcome = [[
Welcome to LuaDesk!

Since no application scripts were found,
the embedded linker is now starting...
]],
	step1 = [[
Step 1 is to select one or more Lua scripts
to link onto the host executable.

They will be linked in the order selected,
and one must contain a main() function. If
more then one script has a main() function,
the last one linked will be called.
]],
	step2 = [[
Step 2 is to specify the name and location
of the output executable.
]]
}

function main(args)
	iup.Message(conf.title, conf.welcome)

	------------
	-- step 1 --
	------------
	iup.Message(conf.title, conf.step1)
    local infile = inputfiles(conf.title .. " Input Files(s)", "Lua Scripts|*.lua|Lua Objects|*.lo|All Files|*.*")
    if not infile then return 1 end

	------------
	-- step 2 --
	------------
	iup.Message(conf.title, conf.step2)
    local outfile = outputfile(conf.title .. "Output File", "Executable|*.exe|All Files|*.*")
    if not outfile then return 1 end

    ------------
    -- linker --
    ------------
	-- progress popup
	local progress = iup.progressbar { max = #infile + 1, expand = 'HORIZONTAL' }
	local dlg = iup.dialog {
		title = conf.title, resize = 'NO', maxbox = 'NO',
		iup.vbox {
			margin = '16x16', alignment = 'ACENTER', cgap = 8,
			iup.label { title = string.format("Linking '%s'...", outfile) },
			progress
		}
	}
	dlg:showxy(iup.CENTER, iup.CENTER)
    local fo = io.open(outfile, 'wb')
    -- copy self
    local fi = io.open(args[1], 'rb')
    fo:write(fi:read('*a'))
    fi:close()
    -- process scripts
    local toc = {}
    for i, v in ipairs(infile) do
    	-- stay responsive
    	progress.value = i
		ldesk.msleep(10)
        local fi = io.open(v, 'rb')
        if fi then
            -- read in file
            local buf = fi:read('*a')
            -- register TOC entry
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
	progress.value = #infile + 1
	ldesk.msleep(10)
    -- cap file with TOC
    local offset = fo:seek()
    -- TOC header
    fo:write("LuaDesk PQ" .. ldesk.htons(#infile))
    for i, v in ipairs(toc) do
        -- TOC entry
        fo:write(table.concat {
            ldesk.htons(#v.name),
            ldesk.htonl(v.length),
            ldesk.htonl(v.offset),
            v.name
        })
    end
    -- TOC footer
    fo:write(ldesk.htonl(offset) .. "LuaDesk PQ")
    -- fini
    fo:close()

    ldesk.sleep(1)
    dlg:hide()
    dlg:destroy()

    iup.Message(conf.title, "All Done!")

    return 0
end

--------------------------------
-- explode a string by a char --
--------------------------------
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

-------------------------------------------
-- prompt for one or more existing files --
-------------------------------------------
function inputfiles(title, extfilter)
    local infile = {}
	-- create file selection dialog
    local dlg = iup.filedlg {
        dialogtype = 'OPEN',
        title = title,
        extfilter = extfilter,
        multiplefiles = 'YES',
        directory = "./" -- CWD
    }
    -- shows modal file dialog in the center of the screen
    dlg:popup(iup.ANYWHERE, iup.ANYWHERE)

    if tonumber(dlg.status) ~= -1 then
		-- parse filedlg output into sensible array
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

------------------------------------
-- prompt for one path + filename --
------------------------------------
function outputfile(title, extfilter)
	-- create file selection dialog
    local dlg = iup.filedlg {
        dialogtype = 'SAVE',
        title = title,
        extfilter = extfilter,
        multiplefiles = 'YES',
        directory = "./" -- CWD
    }
    -- shows modal file dialog in the center of the screen
    dlg:popup(iup.ANYWHERE, iup.ANYWHERE)

    if tonumber(dlg.status) ~= -1 then
        return dlg.value
    else
        return
    end
end
