-- Creating a helper class which carries a number of strings
-- We haven't managed to pass Lua tables to C++ methods, so we are using
-- this workaround of creating a C++ class and adding the strings to it
--
-- Available in: ASC, mapeditor

s = asc.StringArray()
s:add('male')
s:add('female')

-- selectString lets the user select on entry from a list of Strings

result = asc.selectString('TTTT', s )
asc.infoMessage(result)

-- it returns the index, or -1 if cancel was pressed      
      
if result < 0 then
return
end

      
-------------------------------------------      
-- Demonstrates the usage of the PropertyEditor
-------------------------------------------

-- creating an instance of the PropertyDialog class      
dlg = asc.PropertyDialog('This is the title')
      
-- adding various properties      
dlg:addString('Name', '' )
dlg:addInteger('Age', 106 )
dlg:addBool('Alive', true )


dlg:addIntDropdown( 'sex', s, 0 )

result = dlg:run()

if result == false then
   -- user pressed Cancel
   return
end

if dlg:getBool('Alive') == false then
   asc.warningMessage("I don't believe you!")
   return
end

asc.infoMessage('You entered ' .. dlg:getString('Name') .. ' and ' .. dlg:getInteger('Age') .. ' and ' .. dlg:getInteger('sex') )

