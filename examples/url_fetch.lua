title = "Fetch"

function main(args)
	local status, url = iup.GetParam(title, nil, "URL: %s\n", '')
	if not status then return 1 end
	url = string.gsub(url, '\n', '')
	if #url < 1 then iup.Message(title, "You must enter a URL!") return 1 end
	local s = fetch(url)
	--iup.Message(title, string.format("%d bytes retrieved", #s))
	display(s)
	iup.Message(title, "Done!")
end

function display(str)
	local dlg = iup.dialog {
		title = title,
		resize = 'NO',
		iup.vbox {
			margin = '8x8',
			alignment = 'ACENTER',
			iup.text {
				multiline = 'YES',
				visiblecolumns = 60,
				visiblelines = 20,
				readonly = 'YES',
				value = str
			},
			iup.button {
				title = "Ok",
				padding = '16x1',
				action = function(self) return iup.CLOSE end
			},
		}
	}
	dlg:popup()
	dlg:destroy()
end

function fetch(url, referrer)
	local c = curl.new()
	local t = {}
	local dlg = iup.dialog {
		title = title,
		iup.vbox {
			margin = '16x16',
			iup.label {
				title = string.format("Downloading '%s'...", url)
			}
		}
	}
	dlg:showxy(iup.CENTER, iup.CENTER)
	c:setopt(curl.OPT_WRITEFUNCTION,
		function(_, s)
			--iup.Message("notice", s)
			table.insert(t,s)
           	return #s
		end
	)
	if referrer then c:setopt(curl.OPT_HTTPHEADER, string.format("Referer: %s", referrer)) end
	c:setopt(curl.OPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0")
	c:setopt(curl.OPT_URL, url)
	c:perform()
	dlg:hide()
	dlg:destroy()
	return table.concat(t)
end
