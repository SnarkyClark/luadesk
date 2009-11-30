title = "Library Test"
s = "test"

function main(args)
	iup.Message(title, string.format("ldesk.crc32(s) = '%s'", str2hex(ldesk.crc32(s))))
	iup.Message(title, string.format("ldesk.md5(s) = '%s'", str2hex(ldesk.md5(s))))
end

function str2hex(s)
	local t = {}
	for i = 1, #s do
		table.insert(t, string.format("%02x", s:byte(i)))
	end
	return table.concat(t)
end