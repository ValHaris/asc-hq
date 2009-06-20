-- This example lets the user select a position on the map
-- 
-- Available in: mapeditor

m = asc.selectPosition()
      
-- the result (assigned to m in this example) is of type MapCoordinate     
-- if the user pressed Cancel in the dialog, an invalid map coordinate 
-- will be returned
      
if  m:valid() then
   asc.infoMessage('valid')
else
   asc.infoMessage('invalid')
end
  