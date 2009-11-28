function idle_cb()
  local value = gauge.value
  value = value + 0.01;
  ldesk.msleep(10)
  if value > 1.0 then
    value = 0.0
	ldesk.sleep(1)
  end
  gauge.value = value
  return iup.DEFAULT
end

function main(args)
	local value = 0
	gauge = iup.progressbar {max = 100}
	label = iup.label { title = string.format("value = %f...", value) }
	dlg = iup.dialog{iup.vbox{gauge, label}; title = "IupGauge"}
	dlg.size = "QUARTERxEIGHTH"
	
	local value = 0.0
	dlg:showxy(iup.CENTER, iup.CENTER)
	for i = 1, 1000, 1 do
		value = value + 1.0
		if value > 100.0 then
			value = 0.0
			ldesk.sleep(1)
		end
		gauge.value = value * 1.20
		label.title = string.format("value = %f...", value)
		ldesk.msleep(50)
		iup.LoopStep()
	end
	dlg:hide()
	dlg:destroy()
	
--[[
	-- Registers idle callback
	iup.SetIdle(idle_cb)
	dlg:showxy(iup.CENTER, iup.CENTER)

	if (not iup.MainLoopLevel or iup.MainLoopLevel()==0) then
		iup.MainLoop()
	end
]]

end
